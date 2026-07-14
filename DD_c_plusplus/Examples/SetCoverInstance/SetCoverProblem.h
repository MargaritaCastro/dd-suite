#ifndef TEMPLATETYPE_SetCoverPROBLEM_H
#define TEMPLATETYPE_SetCoverPROBLEM_H

#include "MyExceptions.h"
#include "AbstractProblemClass.h"
#include "SetCoverInstance.h"

#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cassert>
#include <set>
#include <unordered_set>
#include <bitset>
#include <utility>
#include <boost/dynamic_bitset.hpp>

using namespace std;
using namespace boost;

//constexpr int BITSET_SIZE_SET_COVERING = BITSET_SIZE_SET_COVERING_MACRO;
//constexpr int BITSET_MATRIX_STATE = BITSET_MATRIX_STATE_MACRO;


/**
 * @brief Class representing a Set Cover Problem.
 * This class inherits from AbstractProblem and defines a set cover problem,
 * where the goal is to find the smallest set of subsets that cover all elements.
 */
class SetCoverProblem : public AbstractProblem<dynamic_bitset<>> {
public:
    /**
     * @brief Constructor for the SetCoverProblem class.
     * @param params SetCoverInstance containing initial_state, variables, matrix_of_weight and right_side_of_restrictions.
     * @param sort If true, apply sort_variables() ordering heuristic.
     */
    SetCoverProblem(const SetCoverInstance& params, bool sort = true);

    /**
     * @brief Transition function of the problem.
     * @param previous_state Previous state of the problem.
     * @param variable_id Identifier of the variable.
     * @param variable_value Value of the variable.
     * @return pair<dynamic_bitset<>*, bool> New state of the problem and a boolean indicating if the state is valid.
     */
    bool transition_function(const dynamic_bitset<>* previous_state, const int variable_index, int variable_value, dynamic_bitset<>* scratch_state) const override;

    /**
     * @brief Get the priority for discarding a node.
     * @param state State of the problem.
     * @return int Priority for discarding a node.
     */
    int get_priority_for_discard_node(const dynamic_bitset<>* state) const override;

    /**
     * @brief Get the priority for merging nodes.
     * @param node_id Identifier of the node.
     * @param state State of the problem.
     * @return int Priority for merging nodes.
     */
    int get_priority_for_merge_nodes(const int node_id, const dynamic_bitset<>* state) const override;

    /**
     * @brief Merge two states.
     * @param state_one First state to merge.
     * @param state_two Second state to merge.
     * @return dynamic_bitset<> Merged state.
     */
    dynamic_bitset<>* merge_operator(const dynamic_bitset<>* state_one, const dynamic_bitset<>* state_two) const override;

    /**
     * @brief Get the state as a string.
     * @param state State of the problem.
     * @return string State as a string.
     */
    string get_state_as_string(const dynamic_bitset<>* state) const override;

    /**
     * @brief Get a copy of the state.
     * @param state State of the problem.
     * @return dynamic_bitset<> Copy of the state.
     */
    dynamic_bitset<>* get_state_copy(const dynamic_bitset<>* state) const override;

    /**
 * @brief Gets the final state of the problem.
 * @return Final state of the problem.
 */
    dynamic_bitset<>* get_final_state() override;

    /**
     * @brief Returns variable names sorted descending by coverage count (column sums).
     * Also reorders _raw_matrix to match the returned order.
     */
    vector<string> sort_variables(const vector<pair<string, vector<int>>>& variables) override;

private:
    vector<vector<int>> _raw_matrix;
    vector<dynamic_bitset<>> matrix_of_weight; /**< Matrix of weights of the elements. */
    vector<dynamic_bitset<>> matrix_of_weight_transpose; /**< Transpose matrix of weights of the elements. */
    dynamic_bitset<> right_side_of_restrictions;
    vector<int> max_indices;

    /**
     * @brief Check the attributes of the problem.
     * @param variables Problem variables, represented as pairs of names and domains.
     * @param initial_state Initial state of the problem.
     */
    void check_atributes(const vector<pair<string, vector<int>>>& variables, dynamic_bitset<>* initial_state);
    /**
     * @brief Check if the matrix and the right side have the same length.
     * @param initial_state Initial state of the problem.
     */
    void check_same_len_matrix_and_right_side(dynamic_bitset<>* initial_state);
    /**
     * @brief Check if the matrix and the variables have the same length.
     * @param variables Problem variables, represented as pairs of names and domains.
     */
    void check_same_len_rows_matrix_and_variables(const vector<pair<string, vector<int>>>& variables);

    /**
     * @brief Convert a vector of integers to a bitset.
     * @param vec Vector of integers to convert.
     * @return dynamic_bitset<> Bitset representing the vector.
     */
    dynamic_bitset<>* convert_to_bitset(const vector<int>& vec, int size);

    /**
     * @brief create transpose matrix of weights and save it as a vector of dynamic_bitsets
     * @param vec Vector of integers to convert.
     */
    void create_transpose_matrix_of_weights(vector<vector<int>>& vector_matrix_of_weight);


    void calculate_row_max_index();
};
#endif //TEMPLATETYPE_SetCoverPROBLEM_H
