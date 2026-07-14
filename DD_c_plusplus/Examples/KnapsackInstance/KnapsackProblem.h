#ifndef TEMPLATETYPE_KNAPSACKPROBLEM_H
#define TEMPLATETYPE_KNAPSACKPROBLEM_H

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
#include <memory>
#include <limits>

using namespace std;

/**
 * @brief Class representing a Knapsack Problem.
 * This class inherits from AbstractProblem and defines a knapsack problem,
 * where the goal is to maximize the total value of selected items,
 * subject to a maximum weight constraint.
 */
class KnapsackProblem : public AbstractProblem<int> {
public:
    /**
     * @brief Constructor for the KnapsackProblem class.
     * @param params KnapsackInstance containing initial_state, variables, weights and capacity.
     * @param sort If true, apply sort_variables() ordering heuristic.
     */
    KnapsackProblem(const KnapsackInstance& params, bool sort = true);

    /**
     * @brief Transition function of the problem.
     * @param previous_state Previous state of the problem.
     * @param variable_id Identifier of the variable.
     * @param variable_value Value of the variable.
     * @return pair<int, bool> New state of the problem and a boolean indicating if the state is valid.
     */
    bool transition_function(const int* previous_state, const int variable_index, int variable_value, int* scratch_state) const override;
    /**
     * @brief Get the priority for discarding a node.
     * @param state State of the problem.
     * @return int Priority for discarding a node.
     */
    int get_priority_for_discard_node(const int* state) const override;
    /**
     * @brief Get the priority for merging nodes.
     * @param node_id Identifier of the node.
     * @param state State of the problem.
     * @return int Priority for merging nodes.
     */
    int get_priority_for_merge_nodes(const int node_id, const int* state) const override;
    /**
     * @brief Merge two states.
     * @param state_one First state to merge.
     * @param state_two Second state to merge.
     * @return int Merged state.
     */
    int* merge_operator(const int* state_one, const int* state_two) const override;
    /**
     * @brief Get the state as a string.
     * @param state State of the problem.
     * @return string State as a string.
     */
    string get_state_as_string(const int* state) const override;

    /**
     * @brief Get a copy of the state.
     * @param state State of the problem.
     * @return int Copy of the state.
     */
    int* get_state_copy(const int* state) const override;

    /**
    * @brief Gets the final state of the problem.
    * @return Final state of the problem.
    */
    int* get_final_state() override;

private:
   vector<int> weights; /**< Matrix of item weights. */
   int capacity; /**< Right-hand side of the constraints. */

};

#endif //TEMPLATETYPE_KNAPSACKPROBLEM_H
