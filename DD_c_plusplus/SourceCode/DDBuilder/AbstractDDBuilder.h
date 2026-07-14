#ifndef TEMPLATETYPE_ABSTRACTDDBUILDER_H
#define TEMPLATETYPE_ABSTRACTDDBUILDER_H

#include "Graph.h"
#include "AbstractProblemClass.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <cassert>
#include <set>
#include <unordered_map>
#include <sstream>

/**
 * @brief Abstract class that provides the foundation for building decision diagrams.
 * @tparam T Type of the Node state values
 */
template <typename T>
class AbstractDDBuilder{
public:
    /**
     * @brief Constructor for the AbstractDDBuilder class.
     * @param problem Reference to the problem associated with the graph.
     */
    AbstractDDBuilder(AbstractProblem<T>& problem);

    Graph<T>* graph; /** Pointer to the graph to be built. */

    /**
     * @brief Builds the decision diagram.
     * @param should_visualize Boolean indicating whether to visualize the graph construction.
     * @return Pointer to the constructed graph.
     */
    Graph<T>* get_decision_diagram(bool should_visualize);

    /**< Explicit virtual destructor */
    virtual ~AbstractDDBuilder();

protected:
    int node_number; /**< Node counter for naming their IDs. */
    AbstractProblem<T>& problem; /**< Reference to the problem associated with the graph. */
    vector<string> variables; /**< List of problem variables. */
    vector<vector<int> > variables_domain; /**< Domain of the problem variables -- new. */
    unordered_map<string, Node<T>*> map_of_states; /**< Map storing the states of the nodes in each layer. */
    bool is_last_layer; /**< Bool that indicates if we are in the last layer of the construction process or not  */
    T* scratch_state; /**< Reusable scratch buffer for transition_function — avoids per-call heap allocation. */

    /**
    * @brief Prunes the graph from the bottom up from branches that doesn't reach the terminal node.
    */
    bool bottom_up_pruner();

    /**
    * @brief Delete nodes of a layer if its set of incoming arcs is empty
    * @param layer_id current layer to iterate over
    */
    bool delete_nodes_current_layer(int layer_id);

    /**
    * @brief Iterates over the outgoing arcs of the previous layer and delets them if the child node does not have outoging arcs
    * @param layer_id current layer id (the precedure iterates over --layer_id)
    */
    void delete_arcs_previous_layer(int layer_id);

    /**
     * @brief Adjusts the node number in the graph.
     */
    void adjust_node_number();

private:

    /**
     * @brief Creates a new layer in the graph for a given variable.
     * @param variable_id ID of the variable to create the new layer.
     */
    void set_variables_domains(map<string, vector<int>>& variables_domain_map);
    /**
     * @brief Creates a new layer in the graph for a given variable.
     * @param variable_id ID of the variable to create the new layer.
     */
    void create_new_layer(int variable_id);
    /**
     * @brief Creates new nodes in the new layer for a given variable.
     * @param variable_id ID of the variable to create the new nodes.
     */
    void create_new_nodes_in_the_new_layer(int variable_id);
    /**
     * @brief Creates arc for the terminal node in the last layer.
     * @param variable_value Value of the variable to create the arc.
     * @param parent_node Pointer to the node that already exists.
     */
    void create_arcs_for_the_terminal_node(int variable_value, Node<T>* parent_node);
    /**
     * @brief Creates arc for the new node.
     * @param parent_node Pointer to the node that already exists.
     * @param node_created Pointer to the node to be created.
     * @param variable_value Value of the variable to create the arc.
     */
    void create_arc_for_the_new_node(Node<T>* parent_node, Node<T>* node_created, int variable_value);

    /**
     * @brief Print the graph if visualization is requested.
     * @param should_visualize Boolean indicating whether to visualize the graph.
     */
    void print_graph(bool should_visualize);

    /**
     * @brief Print the content of each layer of the graph.
     */
    void print();

    /**
     * @brief Set the final state of the graph.
     */
    void set_final_state();

    /**
     * @brief Pure virtual method, which is executed at the end of each layer.
     */
    virtual void specific_end_of_layer_function() = 0;

    /**
     * @brief Pure virtual method, which is executed at the end of the entire construction.
     */
    virtual void specific_end_of_construction_function() = 0;
};

#include "AbstractDDBuilder.tpp"
#endif //TEMPLATETYPE_ABSTRACTDDBUILDER_H
