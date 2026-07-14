#ifndef TEMPLATETYPE_INDEPENDENTSETPROBLEM_H
#define TEMPLATETYPE_INDEPENDENTSETPROBLEM_H

#include "MyExceptions.h"
#include "AbstractProblemClass.h"
#include "IndependentSetInstance.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cassert>
#include <set>
#include <unordered_set>
#include <boost/dynamic_bitset.hpp>


using namespace std;
using namespace boost;


class IndependentSetProblem : public AbstractProblem<dynamic_bitset<>> {
public:
    /**
     * @brief Constructor for the IndependentSetProblem class.
     * @param params IndependentSetInstance containing initial_state, variables and dict_node_neighbors.
     * @param sort If true, apply sort_variables() ordering heuristic.
     */
    IndependentSetProblem(const IndependentSetInstance& params, bool sort = true);

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
     * @brief Returns variable names sorted descending by number of neighbors.
     */
    vector<string> sort_variables(const vector<pair<string, vector<int>>>& variables) override;

private:
    vector<dynamic_bitset<> > node_neighbors;
    map<string, vector<int>> _dict_node_neighbors;

    /**
     * @brief Check the atributes of the problem.
     * @param variables
     * @param dict_node_neighbors
     */
    void check_atributes(const vector<pair<string, vector<int>>>& variables, const map<string, vector<int>>&);

    /**
     * @brief Check if the variables are the same in the variables and the dictionary of neighbors.
     * @param variables Problem variables, represented as pairs of names and domains.
     * @param dict_node_neighbors Dictionary of neighbors.
     */
    void check_same_variables(const vector<pair<string, vector<int>>>& variables, const map<string, vector<int>>& dict_node_neighbors);
    /**
     * @brief Check if the matrix and the right side of the restrictions have the same length.
     * @param initial_state Initial state of the problem.
     */
    void check_consistent_dictionary_of_neighbors(const map<string, vector<int>>& dict_node_neighbors);

    unordered_set<string> keysToVector(const map<string, vector<int>>& variables); /**< Gets keys from a map of pairs. */
    unordered_set<string> keysOfVector(const vector<pair<string, vector<int>>>& variables); /**< Gets keys from a vector of pairs. */

    /**
     * @brief Checks if a neighbor is consistent.
     * @param key Node key.
     * @param value Neighbor value.
     * @return bool true if the neighbor is consistent, false otherwise.
     */
    bool isNeighborConsistent(const string& key, int value, const map<string, vector<int>>& dict_node_neighbors);

    /**
     * @brief Convert a vector of integers to a bitset.
     * @param vec Vector of integers.
     * @return dynamic_bitset<>* Bitset.
     */
    dynamic_bitset<>* convert_to_bitset(const vector<int>& vec, int size);

};

#endif //TEMPLATETYPE_INDEPENDENTSETPROBLEM_H
