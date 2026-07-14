#ifndef TEMPLATETYPE_ABSTRACTPROBLEMCLASS_H
#define TEMPLATETYPE_ABSTRACTPROBLEMCLASS_H

#include <vector>
#include <map>
#include <string>
#include <utility>
#include <stdexcept>

#include "../Exceptions/MyExceptions.h"


using namespace std;

/**
 * @brief Abstract class representing an optimization problem.
 * @tparam T Type of the Node state values
 */
template <typename T> class AbstractProblem {
public:
    /**
     * @brief Constructor for the AbstractProblem class.
     * @param initial_state Initial state of the problem.
     * @param variables Vector of pairs representing the problem variables and their domains.
     */
    AbstractProblem(T* initial_state, const vector<pair<string, vector<int>>>& variables);


    /**
     * @brief Transition function defining how to change from one state to another given a variable and its value.
     * @param previous_state Previous state.
     * @param variable_id Identifier of the variable to change.
     * @param variable_value Value of the variable.
     * @return Pair<T, bool> containing the resulting new state and a feasibility indicator.
     */
    virtual bool transition_function(const T* previous_state, const int variable_index, int variable_value, T* scratch_state) const = 0;

    /**
     * @brief Gets the priority for discarding a node from the decision diagram.
     * @param state State of the node to evaluate.
     * @return int Priority for discarding the node.
     */
    virtual int get_priority_for_discard_node(const T* state) const = 0;

    /**
     * @brief Gets the priority for merging nodes in the decision diagram.
     * @param node_id Identifier of the node to evaluate.
     * @param state State of the node to evaluate.
     * @return int Priority for merging the nodes.
     */
    virtual int get_priority_for_merge_nodes(const int node_id, const T* state) const = 0;

    /**
     * @brief Merge operator that combines two states into one.
     * @param state_one First state to merge.
     * @param state_two Second state to merge.
     * @return T New state resulting from the merge.
     */
    virtual T* merge_operator(const T* state_one, const T* state_two) const = 0;

    /**
    * @brief Gets the state as a string.
    * @return String representation of the state.
    */
    virtual string get_state_as_string(const T* state) const = 0;

    /**
     * @brief Returns the variable names in the desired processing order.
     * Override in derived classes to apply a custom ordering heuristic.
     * Called from derived constructors (not from AbstractProblem constructor).
     * @param variables Variables as provided to the constructor.
     * @return Variable names in the desired order.
     */
    virtual vector<string> sort_variables(const vector<pair<string, vector<int>>>& variables);

    /**
     * @brief Gets the copy of the state.
     * @return Copy of the state.
     */
    virtual T* get_state_copy(const T* state) const = 0;

    /**
     * @brief Gets the final state of the problem.
     * @return Final state of the problem.
     * @note If not implementend, it will return de initial state.
     */
    virtual T* get_final_state();

    T* initial_state; /**< Initial state of the problem. */
    vector<string> ordered_variables; /**< Vector containing the order of the variables. */
    map<string, vector<int>> variables_domain; /**< Map containing the variables and their domains. */
    map<string, int> name_to_index; /**< Map from variable name to its 0-based index. */
    map<int, string> index_to_name; /**< Map from 0-based index to variable name. */
    vector<int> dd_to_original_index; /**< Maps DD position → original variable index. Initialized as identity; overridden by sort_variables. */

    /**
     * @brief Destructor for the AbstractProblem class.
     */
    virtual ~AbstractProblem() = default;


protected:
    /**
     * @brief Reorders ordered_variables, name_to_index and index_to_name to match new_order.
     * Call from a derived constructor after AbstractProblem() to apply a custom variable ordering.
     * @param new_order Variable names in the desired processing order.
     */
    void apply_variable_order(const vector<string>& new_order);

private:
    /**
     * @brief Gets the variables and their domains from the vector of pairs provided in the constructor.
     * @param variables Vector of pairs representing the problem variables and their domains.
     */
    void get_variables(const vector<pair<string, vector<int>>>& variables);
};
#include "AbstractProblemClass.tpp"
#endif //TEMPLATETYPE_ABSTRACTPROBLEMCLASS_H
