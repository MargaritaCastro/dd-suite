#ifndef TEMPLATETYPE_RELAXEDGROUPINGDDBUILDER_H
#define TEMPLATETYPE_RELAXEDGROUPINGDDBUILDER_H

#include "AbstractDDBuilder.h"

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <algorithm>

using namespace std;

/**
 * @brief Builder for a relaxed decision diagram that merges nodes in groups by priority difference.
 *
 * Instead of merging the two highest-priority nodes one pair at a time (RelaxedPriorityDDBuilder),
 * the layer is reduced in passes with an increasing tolerance: in each pass the nodes are sorted by
 * priority and consecutive nodes whose priority gap is <= tolerance are merged into a single node.
 * The tolerance starts at 0 (only equal priorities are merged) and grows until the layer fits in the
 * maximum width.
 *
 * Two optimizations keep this fast:
 *   - Priorities are computed once per node and carried as (priority, node) pairs.
 *   - The tolerance jumps to the next gap that actually merges something instead of stepping by one.
 *
 * @tparam T Type of the Node state values
 */
template <typename T>
class RelaxedGroupingDDBuilder: public AbstractDDBuilder<T> {
public:
    /**
     * @brief Constructor for the RelaxedGroupingDDBuilder class.
     * @param problem Reference to an AbstractProblem object representing the problem to be solved.
     * @param max_width Maximum allowed width for the decision diagram.
     */
    RelaxedGroupingDDBuilder(AbstractProblem<T>& problem, int max_width);

private:
    int max_width; /**< Maximum allowed width for the decision diagram. */

    /**
     * @brief Specific function applied at the end of each layer; merges nodes if the width is exceeded.
     */
    void specific_end_of_layer_function() override;

    /**
     * @brief Specific function applied at the end of construction; prunes and renumbers nodes.
     */
    void specific_end_of_construction_function() override;

    /**
     * @brief Merges nodes when the width of the graph exceeds the specified maximum, grouping by
     * increasing priority difference.
     */
    void merge_nodes_when_width_is_greater_than_w();

    /**
     * @brief Performs a single reduction pass over priority-sorted (priority, node) pairs, merging
     * every run of consecutive nodes whose gap is <= difference. Stops once max width is reached.
     * @param alive (priority, node) pairs sorted by priority.
     * @param difference Maximum priority gap allowed between consecutive nodes of a group.
     * @return (priority, node) pairs alive after this pass, sorted by priority.
     */
    vector<pair<int, Node<T>*>> merge_nodes_with_difference(vector<pair<int, Node<T>*>>& alive, int difference);

    /**
     * @brief Merges every node of a group into a single surviving node.
     * @param group Nodes to merge together (at least two).
     * @return The surviving node after merging the whole group.
     */
    Node<T>* merge_group(vector<Node<T>*>& group);

    /**
     * @brief Merges two nodes and returns the surviving node.
     * @param node_to_remove Node that is absorbed by the merge.
     * @param node_to_keep Node that is kept as the merge anchor.
     * @return The surviving node (node_to_keep or a pre-existing node with the merged state).
     */
    Node<T>* merge_two_nodes(Node<T>* node_to_remove, Node<T>* node_to_keep);

    /**
     * @brief Returns the merge priority of a node.
     * @param node Node whose priority is computed.
     * @return The priority value used to sort and group nodes.
     */
    int merge_priority(Node<T>* node);

    /**
     * @brief Returns the smallest priority gap between consecutive nodes of a priority-sorted list.
     * @param alive (priority, node) pairs sorted by priority (at least two pairs).
     * @return The smallest consecutive priority gap.
     */
    int minimum_gap(const vector<pair<int, Node<T>*>>& alive);

    /**
     * @brief Merges two nodes with an existing node.
     */
    void merge_with_an_existing_node(Node<T>* node_one, Node<T>* node_two, Node<T>* available_node);
    /**
     * @brief Merges two nodes when there is no existing node with the same state.
     */
    void merge_when_doesnt_exist_node(Node<T>* node_to_remove, Node<T>* node_to_keep, T* new_state);
    /**
     * @brief Redirects the incoming arcs of one node to another node.
     */
    void redirect_in_arcs(Node<T>* node_to_remove, Node<T>* node_to_keep);
    /**
     * @brief Changes the state of a node based on the states of the merged nodes.
     */
    void change_new_state(Node<T>* node, T* new_state);
};

#include "RelaxedGroupingDDBuilder.tpp"
#endif //TEMPLATETYPE_RELAXEDGROUPINGDDBUILDER_H
