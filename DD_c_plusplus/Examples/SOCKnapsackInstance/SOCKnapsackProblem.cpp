#include "SOCKnapsackProblem.h"

using namespace std;

// ============================================================
// Static helper: prune zero-weight variables, build name→index map
// ============================================================

SOCKnapsackProblem::PrunedData SOCKnapsackProblem::_prune(
    const vector<pair<string, vector<int>>>& variables,
    const vector<int>& linear_weights,
    const vector<int>& sqrt_weights)
{
    PrunedData pd;
    for (int i = 0; i < (int)variables.size(); ++i) {
        pd.original_name_to_index[variables[i].first] = i;
        if (linear_weights[i] != 0 || sqrt_weights[i] != 0) {
            pd.pruned_variables.push_back(variables[i]);
            pd.pruned_linear_weights.push_back(linear_weights[i]);
            pd.pruned_sqrt_weights.push_back(sqrt_weights[i]);
        }
    }
    return pd;
}

// ============================================================
// Delegating constructor (receives pruned data, applies sort)
// ============================================================

SOCKnapsackProblem::SOCKnapsackProblem(vector<double>* initial_state,
                                       PrunedData&& pd,
                                       int omega,
                                       int capacity,
                                       bool sort)
    : AbstractProblem<vector<double>>(initial_state, pd.pruned_variables),
      original_name_to_index(move(pd.original_name_to_index)),
      original_variable_names_domains(move(pd.pruned_variables)),
      _pruned_linear_weights(move(pd.pruned_linear_weights)),
      _pruned_sqrt_weights(move(pd.pruned_sqrt_weights)),
      omega(omega),
      capacity(capacity)
{
    if (sort) {
        apply_variable_order(sort_variables(original_variable_names_domains));
    } else {
        linear_weights = _pruned_linear_weights;
        sqrt_weights   = _pruned_sqrt_weights;
        // AbstractProblem initializes dd_to_original_index as identity over pruned variables,
        // but we need indices into the full original variable list.
        dd_to_original_index.clear();
        for (const auto& [name, _d] : original_variable_names_domains)
            dd_to_original_index.push_back(original_name_to_index.at(name));
    }

    sqrt_weights_2.resize(sqrt_weights.size());
    for (int i = 0; i < (int)sqrt_weights.size(); ++i)
        sqrt_weights_2[i] = sqrt_weights[i] * sqrt_weights[i];

    _compute_min_and_sum_for_pruning();

    assert(linear_weights.size() == ordered_variables.size() &&
           "linear_weights and variables must have the same length");
    assert(sqrt_weights.size() == ordered_variables.size() &&
           "sqrt_weights and variables must have the same length");
}

// ============================================================
// Public constructor — takes full instance + constraint index
// ============================================================

SOCKnapsackProblem::SOCKnapsackProblem(vector<double>* initial_state,
                                       const SOCKnapsackInstance& params,
                                       int constraint_index,
                                       bool sort)
    : SOCKnapsackProblem(initial_state,
                         _prune(params.variables,
                                params.linear_weights[constraint_index],
                                params.sqrt_weights[constraint_index]),
                         params.omega,
                         params.right_side_of_restrictions[constraint_index],
                         sort)
{}

// ============================================================
// sort_variables override
// ============================================================

vector<string> SOCKnapsackProblem::sort_variables(const vector<pair<string, vector<int>>>& variables) {
    struct Entry {
        pair<string, vector<int>> variable;
        int linear_w;
        int sqrt_w;
    };
    vector<Entry> entries;
    entries.reserve(variables.size());
    for (int i = 0; i < (int)variables.size(); ++i)
        entries.push_back({variables[i], _pruned_linear_weights[i], _pruned_sqrt_weights[i]});

    sort(entries.begin(), entries.end(), [this](const Entry& a, const Entry& b) {
        return (a.linear_w + omega * a.sqrt_w) > (b.linear_w + omega * b.sqrt_w);
    });

    dd_to_original_index.clear();
    dd_to_original_index.reserve(entries.size());
    for (const auto& e : entries)
        dd_to_original_index.push_back(original_name_to_index.at(e.variable.first));

    linear_weights.clear();
    sqrt_weights.clear();
    linear_weights.reserve(entries.size());
    sqrt_weights.reserve(entries.size());
    vector<pair<string, vector<int>>> sorted_domains;
    sorted_domains.reserve(entries.size());
    for (const auto& e : entries) {
        linear_weights.push_back(e.linear_w);
        sqrt_weights.push_back(e.sqrt_w);
        sorted_domains.push_back(e.variable);
    }
    original_variable_names_domains = move(sorted_domains);

    vector<string> names;
    names.reserve(entries.size());
    for (const auto& e : entries)
        names.push_back(e.variable.first);
    return names;
}

// ============================================================
// Precomputed pruning bounds
// ============================================================

void SOCKnapsackProblem::_compute_min_and_sum_for_pruning() {
    int n = (int)linear_weights.size();
    min_linear_weight.clear();
    min_sqrt_weight.clear();
    sum_linear_weight.clear();
    sum_sqrt_weight.clear();

    for (int i = 0; i < n - 1; ++i) {
        auto lw_tail = vector<int>(linear_weights.begin() + i + 1, linear_weights.end());
        auto sw_tail = vector<int>(sqrt_weights_2.begin()  + i + 1, sqrt_weights_2.end());

        min_linear_weight.push_back(*min_element(lw_tail.begin(), lw_tail.end()));
        min_sqrt_weight.push_back(  *min_element(sw_tail.begin(), sw_tail.end()));
        sum_linear_weight.push_back(accumulate(lw_tail.begin(), lw_tail.end(), 0));
        sum_sqrt_weight.push_back(  accumulate(sw_tail.begin(), sw_tail.end(), 0));
    }
}

// ============================================================
// Pruning predicates
// ============================================================

bool SOCKnapsackProblem::_is_only_zero_arcs_feasible_in_next_layers(
    const vector<double>* state, int variable_index) const
{
    double lin = (*state)[0] + min_linear_weight[variable_index];
    double sq  = (*state)[1] + min_sqrt_weight[variable_index];
    return lin + omega * sqrt(sq) > capacity;
}

bool SOCKnapsackProblem::_is_one_arc_always_feasible_in_next_layers(
    const vector<double>* state, int variable_index) const
{
    double lin = (*state)[0] + sum_linear_weight[variable_index];
    double sq  = (*state)[1] + sum_sqrt_weight[variable_index];
    return lin + omega * sqrt(sq) <= capacity;
}

// ============================================================
// Transition function (with state encoding and pruning)
// ============================================================

bool SOCKnapsackProblem::transition_function(
    const vector<double>* previous_state,
    int variable_index,
    int variable_value,
    vector<double>* scratch_state) const
{
    // Handle sentinel states: state[0] < 0 means outcome was pre-determined
    if ((*previous_state)[0] < 0) {
        (*scratch_state)[0] = (*previous_state)[0];
        (*scratch_state)[1] = (*previous_state)[1];
        return variable_value == 0 || (*previous_state)[0] == -2.0;
    }

    if (variable_value == 0) {
        (*scratch_state)[0] = (*previous_state)[0];
        (*scratch_state)[1] = (*previous_state)[1];
    } else {
        (*scratch_state)[0] = (*previous_state)[0] + linear_weights[variable_index] * variable_value;
        (*scratch_state)[1] = (*previous_state)[1] + sqrt_weights_2[variable_index] * variable_value;

        if ((*scratch_state)[0] + omega * sqrt((*scratch_state)[1]) > capacity)
            return false;
    }

    // Early termination: encode future pruning into the state
    if (variable_index < (int)min_linear_weight.size()) {
        if (_is_only_zero_arcs_feasible_in_next_layers(scratch_state, variable_index)) {
            (*scratch_state)[0] = -1.0;
            (*scratch_state)[1] = -1.0;
            ++count_1;
        } else if ((*scratch_state)[0] >= 0 &&
                   _is_one_arc_always_feasible_in_next_layers(scratch_state, variable_index)) {
            (*scratch_state)[0] = -2.0;
            (*scratch_state)[1] = -2.0;
            ++count_2;
        }
    }

    return true;
}

// ============================================================
// Remaining AbstractProblem interface
// ============================================================

int SOCKnapsackProblem::get_priority_for_discard_node(const vector<double>* state) const {
    if ((*state)[0] < 0) return (int)(*state)[0];
    return -(int)((*state)[0] + omega * sqrt((*state)[1]));
}

int SOCKnapsackProblem::get_priority_for_merge_nodes(const int /*node_id*/,
                                                      const vector<double>* state) const {
    if ((*state)[0] < 0) return (int)(*state)[0];
    return -(int)((*state)[0] + omega * sqrt((*state)[1]));
}

vector<double>* SOCKnapsackProblem::merge_operator(const vector<double>* state_one,
                                                    const vector<double>* state_two) const {
    // Sentinel -2 (always feasible) dominates: merging with anything stays always-feasible.
    // Sentinel -1 (only zero-arcs feasible) merged with a normal state (>=0) must yield the
    // normal state, because the normal state is more permissive. Taking min would produce -1
    // and incorrectly block 1-arcs that the normal-state node allowed.
    const bool s1_always   = (*state_one)[0] == -2.0;
    const bool s2_always   = (*state_two)[0] == -2.0;
    if (s1_always || s2_always)
        return new vector<double>{-2.0, -2.0};

    const bool s1_zero_only = (*state_one)[0] == -1.0;
    const bool s2_zero_only = (*state_two)[0] == -1.0;
    if (s1_zero_only && !s2_zero_only)
        return new vector<double>(*state_two);
    if (s2_zero_only && !s1_zero_only)
        return new vector<double>(*state_one);

    return new vector<double>{
        min((*state_one)[0], (*state_two)[0]),
        min((*state_one)[1], (*state_two)[1])
    };
}

string SOCKnapsackProblem::get_state_as_string(const vector<double>* state) const {
    return to_string((long long)(*state)[0]) + "," + to_string((long long)(*state)[1]);
}

vector<double>* SOCKnapsackProblem::get_state_copy(const vector<double>* state) const {
    return new vector<double>(*state);
}

vector<double>* SOCKnapsackProblem::get_final_state() {
    return new vector<double>{0.0, 0.0};
}
