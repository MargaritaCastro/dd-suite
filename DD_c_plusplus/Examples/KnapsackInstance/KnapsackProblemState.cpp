#include "KnapsackProblemState.h"

State::State(vector<int> value) : value(value) {}

bool State::operator==(const State& other) const {
    return value == other.value;
}


KnapsackProblemState::KnapsackProblemState(State* initial_state, const KnapsackInstance& params, bool sort)
        : AbstractProblem(initial_state, params.variables),
          weights(params.weights),
          capacity(params.right_side_of_restrictions)
{
    assert(params.variables.size() == weights.size() && "weights and variables must have the same length");
}

bool KnapsackProblemState::transition_function(const State* previous_state, const int variable_index, int variable_value, State* scratch_state) const {
    bool isFeasible = false;
    for (int i = 0; i < 2; i++) {
        scratch_state->value.at(i) = previous_state->value.at(i) + weights[variable_index] * variable_value;
        if (scratch_state->value.at(i) <= capacity) isFeasible = true;
    }
    return isFeasible;
}

int KnapsackProblemState::get_priority_for_discard_node(const State* state) const {
    int total = 0;
    for (int i = 0; i < state->value.size(); i++) {
        total += state->value.at(i);
    }
    return -total;
}

int KnapsackProblemState::get_priority_for_merge_nodes(const int node_id, const State* state) const {
    if (state->value.at(0) != state->value.at(1)) {
        return -numeric_limits<int>::infinity();
    }
    return -node_id;
}

State* KnapsackProblemState::merge_operator(const State* state_one, const State* state_two) const {
    vector<int>* state = new vector<int>(2);

    state->at(0) = min(state_one->value.at(0), state_two->value.at(0));
    state->at(1) = max(state_one->value.at(1), state_two->value.at(1));

    State* new_state = new State(*state);
    delete state;
    return new_state;
}

string KnapsackProblemState::get_state_as_string(const State* state) const {
    string result;
    for (int i = 0; i < state->value.size(); ++i) {
        result += std::to_string(state->value[i]);
        if (i != state->value.size() - 1) {
            result += ",";
        }
    }
    return result;
}

State* KnapsackProblemState::get_state_copy(const State* state) const {
    vector<int> copy_vector;
    copy_vector.reserve(state->value.size());
    copy(state->value.begin(), state->value.end(), back_inserter(copy_vector));

    State* new_state =  new State(copy_vector);
    return new_state;
}

State* KnapsackProblemState::get_final_state() {
    vector<int> state = vector<int>(2,0);
    State* new_state = new State(state);
    return new_state;
}