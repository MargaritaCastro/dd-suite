#include "IndependentSetProblem.h"
#include <numeric>

IndependentSetProblem::IndependentSetProblem(const IndependentSetInstance& params, bool sort)
        : AbstractProblem(params.initial_state, params.variables)
{
    _dict_node_neighbors = params.dict_node_neighbors;
    check_atributes(params.variables, params.dict_node_neighbors);

    // Identity default (overwritten by sort_variables when sort=true)
    dd_to_original_index.resize(params.variables.size());
    iota(dd_to_original_index.begin(), dd_to_original_index.end(), 0);

    if (sort) {
        apply_variable_order(sort_variables(params.variables));
    }

    int n = static_cast<int>(params.dict_node_neighbors.size());
    node_neighbors.resize(n);
    for (const auto& [key, neighbors] : params.dict_node_neighbors) {
        int var_index = this->name_to_index.at(key);
        node_neighbors[var_index] = dynamic_bitset<>(n);
        for (int neighbor_num : neighbors) {
            int neighbor_index = this->name_to_index.at("x_" + to_string(neighbor_num));
            node_neighbors[var_index].set(neighbor_index);
        }
    }
}

vector<string> IndependentSetProblem::sort_variables(const vector<pair<string, vector<int>>>& variables) {
    vector<string> sorted_names;
    sorted_names.reserve(variables.size());
    for (const auto& [name, domain] : variables)
        sorted_names.push_back(name);
    std::stable_sort(sorted_names.begin(), sorted_names.end(),
        [this](const string& a, const string& b) {
            return _dict_node_neighbors.at(a).size() > _dict_node_neighbors.at(b).size();
        });

    map<string, int> name_to_orig;
    for (int i = 0; i < (int)variables.size(); ++i)
        name_to_orig[variables[i].first] = i;
    dd_to_original_index.clear();
    for (const string& name : sorted_names)
        dd_to_original_index.push_back(name_to_orig.at(name));

    return sorted_names;
}

void IndependentSetProblem::check_atributes(const vector<pair<string, vector<int>>>& variables, const map<string, vector<int>>& dict_node_neighbors) {
    check_same_variables(variables, dict_node_neighbors);
    check_consistent_dictionary_of_neighbors(dict_node_neighbors);
}

void IndependentSetProblem::check_same_variables(const vector<pair<string, vector<int>>>& variables, const map<string, vector<int>>& dict_node_neighbors) {
    assert(keysOfVector(variables) == keysToVector(dict_node_neighbors) && "Variables must be the same between dictionaries");
}

unordered_set<std::string> IndependentSetProblem::keysToVector(const map<string, vector<int>>& dict_node_neighbors) {
    std::unordered_set<std::string> result;
    for (const auto& entry : dict_node_neighbors) {
        result.insert(entry.first);
    }
    return result;
}

unordered_set<std::string> IndependentSetProblem::keysOfVector(const vector<pair<string, vector<int>>>& variables) {
    std::unordered_set<std::string> result;
    for (const auto& entry : variables) {
        result.insert(entry.first);
    }
    return result;
}

void IndependentSetProblem::check_consistent_dictionary_of_neighbors(const map<string, vector<int>>& dict_node_neighbors) {
    for (const auto& entry : dict_node_neighbors) {
        for (int value : entry.second) {
            assert(isNeighborConsistent(entry.first, value, dict_node_neighbors) && "Dictionary of neighbors must be consistent");
        }
    }
}

bool IndependentSetProblem::isNeighborConsistent(const string& key, int value, const map<string, vector<int>>& dict_node_neighbors) {
    auto neighborEntry = dict_node_neighbors.find("x_" + to_string(value));
    return (neighborEntry != dict_node_neighbors.end()) &&
           (find(neighborEntry->second.begin(), neighborEntry->second.end(), stoi(key.substr(2))) != neighborEntry->second.end());
}

bool IndependentSetProblem::transition_function(const dynamic_bitset<>* previous_state, const int variable_index, int variable_value, dynamic_bitset<>* scratch_state) const {
    if (variable_value == 0) {
        *scratch_state = *previous_state;
        if (previous_state->test(variable_index))
            scratch_state->reset(variable_index);
        return true;
    }
    if (!previous_state->test(variable_index))
        return false;
    *scratch_state = *previous_state;
    *scratch_state -= node_neighbors[variable_index];
    scratch_state->reset(variable_index);
    return true;
}

int IndependentSetProblem::get_priority_for_discard_node(const dynamic_bitset<>* state) const {
    return int(state->count());
}

int IndependentSetProblem::get_priority_for_merge_nodes(const int node_id, const dynamic_bitset<>* state) const {
    return int(state->count());
}

dynamic_bitset<>* IndependentSetProblem::merge_operator(const dynamic_bitset<>* state_one, const dynamic_bitset<>* state_two) const {
    dynamic_bitset<>* merged_state = new dynamic_bitset<>(*state_one | *state_two);
    return merged_state;
}

string IndependentSetProblem::get_state_as_string(const dynamic_bitset<>* state) const {
    string state_string;
    boost::to_string(*state, state_string);
    return state_string;
}

dynamic_bitset<>* IndependentSetProblem::get_state_copy(const dynamic_bitset<>* state) const {
    dynamic_bitset<>* copy_bitset = new dynamic_bitset<>(*state);
    return copy_bitset;
}

dynamic_bitset<>* IndependentSetProblem::convert_to_bitset(const vector<int>& vec, int size) {

    dynamic_bitset<>* bs = new dynamic_bitset<>(size);

    for (int value : vec) {
        if (value > 0) {
            bs->set(value - 1);
        }
    }
    return bs;
}

dynamic_bitset<>* IndependentSetProblem::get_final_state() {
    vector<int> final_state(node_neighbors.size(), 0);
    return convert_to_bitset(final_state, node_neighbors.size());
}