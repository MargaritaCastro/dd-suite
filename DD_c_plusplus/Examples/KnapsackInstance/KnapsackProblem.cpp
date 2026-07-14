#include "KnapsackProblem.h"

using namespace std;

KnapsackProblem::KnapsackProblem(const KnapsackInstance& params, bool sort)
        : AbstractProblem(params.initial_state, params.variables),
          weights(params.weights),
          capacity(params.right_side_of_restrictions)
{
    assert(params.variables.size() == weights.size() && "weights and variables must have the same length");
}

bool KnapsackProblem::transition_function(const int* previous_state, const int variable_index, int variable_value, int* scratch_state) const {
    *scratch_state = *previous_state;
    if (variable_value != 0) {
        *scratch_state += weights[variable_index] * variable_value;
        return *scratch_state <= capacity;
    }
    return true;
}

int KnapsackProblem::get_priority_for_discard_node(const int* state) const {
    int priority = *state;
    return -priority;
}

int KnapsackProblem::get_priority_for_merge_nodes(const int node_id, const int* state) const {
    int priority = *state;
    return -priority;
}


int* KnapsackProblem::merge_operator(const int* state_one, const int* state_two) const {
    int* state = new int();
    *state = min(*state_one, *state_two);

    return state;
}

string KnapsackProblem::get_state_as_string(const int* state) const {
    return std::to_string(*state);
}

int* KnapsackProblem::get_state_copy(const int* state) const {
    int* copy_state = new int(*state);
    return copy_state;
}


int* KnapsackProblem::get_final_state() {
    int* final_state = new int(0);
    return final_state;
}
