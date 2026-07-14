#ifndef TEMPLATETYPE_RELAXEDPRIORITYDDBUILDER_H
#define TEMPLATETYPE_RELAXEDPRIORITYDDBUILDER_H

#include "AbstractDDBuilder.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <set>

using namespace std;

/**
 * @brief Structure representing a comparator for merging nodes.
 * @tparam T Type of the Node state values
 */
template <typename T>
struct NodeMergeComparator {
    AbstractProblem<T>& problem;
    NodeMergeComparator(AbstractProblem<T>& problem) : problem(problem) {}
    bool operator()(const Node<T>* node_one, const Node<T>* node_two) const;
};

/**
 * @brief Class representing a builder for a relaxed decision diagram.
 * @tparam T Type of the Node state values
 */
template <typename T>
class RelaxedPriorityDDBuilder: public AbstractDDBuilder<T> {
public:
    /**
     * @brief Constructor for the RelaxedPriorityDDBuilder class.
     * @param problem Reference to an AbstractProblem object representing the problem to be solved.
     * @param max_width Maximum allowed width for the decision diagram.
     */
    RelaxedPriorityDDBuilder(AbstractProblem<T>& problem, int max_width);

private:
    int max_width; /**< Maximum allowed width for the decision diagram. */

    multiset<Node<T>*, NodeMergeComparator<T>> nodes_set; /**< Set of nodes ordered by priority. */

    /**
     * @brief Specific function for the layer of the graph in the construction of a relaxed decision diagram.
     * Implements the mandatory function from the parent class.
     */
    void specific_end_of_layer_function() override;

    /**
     * @brief Specific function for the final layer of the graph in the construction of a relaxed decision diagram.
     * Implements the mandatory function from the parent class.
     */
    void specific_end_of_construction_function() override;

    /**
     * @brief Merges nodes when the width of the graph exceeds the specified maximum.
     */
    void merge_nodes_when_width_is_greater_than_w();
    /**
     * @brief Reduces the layer width by merging nodes based on their priority.
     */
    void reduce_layer_width();
    /**
     * @brief Merges two nodes with an existing node.
     * @param node_one First node to merge.
     * @param node_two Second node to merge.
     * @param available_node Node to merge with because has the same state.
     */
    void merge_with_an_existing_node(Node<T>* node_one, Node<T>* node_two, Node<T>* available_node);
    /**
     * @brief Merges two nodes when there is no existing node with the same state.
     * @param node_to_remove Node to remove.
     * @param node_to_keep Node to keep.
     * @param new_state New state for the node.
     */
    void merge_when_doesnt_exist_node(Node<T>* node_to_remove, Node<T>* node_to_keep, T* new_state);
    /**
     * @brief Redirects the incoming arc of one node to another node.
     * @param node_to_remove Node to remove.
     * @param node_to_keep Node to keep.
     */
    void redirect_in_arcs(Node<T>* node_to_remove, Node<T>* node_to_keep);
    /**
     * @brief Changes the state of a node based on the states of the merged nodes.
     * @param node Node to change the state.
     * @param new_state New state for the node.
     */
    void change_new_state(Node<T>* node, T* new_state);


};
#include "RelaxedPriorityDDBuilder.tpp"
#endif //TEMPLATETYPE_RELAXEDPRIORITYDDBUILDER_H
