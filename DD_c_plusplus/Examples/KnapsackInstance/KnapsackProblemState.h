#ifndef TEMPLATETYPE_KNAPSACKPROBLEMSTATE_H
#define TEMPLATETYPE_KNAPSACKPROBLEMSTATE_H

#include "MyExceptions.h"
#include "AbstractProblemClass.h"
#include "KnapsackInstance.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cassert>
#include <set>
#include <limits>


using namespace std;

/**
 * @brief Class representing the state of the KnapsackInstance Problem.
 */
class State {
public:
    /**
     * @brief Constructor for the State class.
     * @param value Vector representing the state values.
     */
    State(vector<int> value);

    vector<int> value;/**< Vector representing the state values. */

    /**
     * @brief Equality operator for comparing two states.
     * @param other State to compare with.
     * @return bool true if the states are equal, false otherwise.
     */
    bool operator==(const State& other) const;
};

/**
 * @brief Class representing a KnapsackInstance Problem.
 * This class inherits from AbstractProblem and defines a knapsack problem,
 * where the goal is to maximize the total value of selected items,
 * subject to a maximum weight constraint.
 */
class KnapsackProblemState : public AbstractProblem<State> {
public:
    /**
     * @brief Constructor for the KnapsackProblemState class.
     * @param initial_state Initial state of the problem (caller owns and manages this pointer).
     * @param params KnapsackInstance containing variables, weights and capacity.
     * @param sort If true, apply sort_variables() ordering heuristic.
     */
    KnapsackProblemState(State* initial_state, const KnapsackInstance& params, bool sort = true);


    /**
     * @brief Transition function of the problem.
     * @param previous_state Previous state of the problem.
     * @param variable_id Identifier of the variable.
     * @param variable_value Value of the variable.
     * @return pair<vector<int>, bool> New state of the problem and a boolean indicating if the state is valid.
     */
    bool transition_function(const State* previous_state, const int variable_index, int variable_value, State* scratch_state) const override;
    /**
     * @brief Get the priority for discarding a node.
     * @param state State of the problem.
     * @return int Priority for discarding a node.
     */
    int get_priority_for_discard_node(const State* state) const override;
    /**
     * @brief Get the priority for merging nodes.
     * @param node_id Identifier of the node.
     * @param state State of the problem.
     * @return int Priority for merging nodes.
     */
    int get_priority_for_merge_nodes(const int node_id, const State* state) const override;
    /**
     * @brief Merge two states.
     * @param state_one First state to merge.
     * @param state_two Second state to merge.
     * @return vector<int> Merged state.
     */
    State* merge_operator(const State* state_one, const State* state_two) const override;
    /**
     * @brief Get the state as a string.
     * @param state State of the problem.
     * @return string State as a string.
     */
    string get_state_as_string(const State* state) const override;

    /**
 * @brief Get a copy of the state.
 * @param state State of the problem.
 * @return vector<int> Copy of the state.
 */
    State* get_state_copy(const State* state) const override;

    /**
* @brief Gets the final state of the problem.
* @return Final state of the problem.
*/
    State* get_final_state() override;

private:
    vector<int> weights; /**< Matrix of item weights. */
    int capacity; /**< Right-hand side of the constraints. */
};

#endif //TEMPLATETYPE_KNAPSACKPROBLEMSTATE_H