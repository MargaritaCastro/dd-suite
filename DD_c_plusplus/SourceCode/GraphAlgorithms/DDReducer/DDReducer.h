#ifndef TEMPLATETYPE_REDUCEDDBUILDER_H
#define TEMPLATETYPE_REDUCEDDBUILDER_H

#include "Graph.h"
#include <string>
#include <vector>

using namespace std;

/**
 * @brief Class representing reduction algorithm for a decision diagram.
 * @tparam T Type of the Node state values
 */
template <typename T>
class DDReducer{
public:
    /**
     * @brief Constructor for the DDReducer class.
     * @param graph Pointer to the original graph to be reduced.
     * @param problem Reference to the problem associated with the graph.
     */
    DDReducer(Graph<T>* graph, AbstractProblem<T>& problem);

    Graph<T>* graph;  /**< Pointer to the original graph to be reduced. */

    /**
     * @brief Builds the reduced decision diagram.
     * @param should_visualize Boolean indicating whether to visualize the building process.
     * @return Graph: Reduced decision diagram.
     */
    Graph<T>* reduce_decision_diagram(bool should_visualize);

    /**
     * @brief Destructor for the DDReducer class.
     */
    ~DDReducer() = default;

private:
    AbstractProblem<T>& problem; /**< Reference to the problem associated with the graph. */

    /**
     * @brief Nodes merged-away in the current layer, deleted after the child layer is swept.
     *        Their out-arc objects survive (referenced by the children's in_arcs) until
     *        clear_incoming_arcs() removes them, so the nodes must outlive that sweep.
     */
    vector<Node<T>*> merged_nodes;

    /**
     * @brief Reviews the current layer to merge nodes that reach the same node afterward
        with the same variable value.
     * @param layer Vector of nodes in the current layer.
     * @return True if at least one node was merged away in this layer.
     */
    bool review_layer_to_merge_nodes(list<Node<T>*>& layer);

    /**
     * @brief Gets the nodes of the path_print of a node.
     * @param node Node to get the path_print from.
     * @return Vector of strings with the nodes of the path_print.
     */
    void get_node_of_every_type_of_path(Node<T>* node, string& paths_of_node);

    /**
     * @brief Redirects the in arc of a node to another node.
     * @param node_to_remove Node to remove.
     * @param node_to_keep Node to keep.
     */
    void redirect_in_arcs(Node<T>* node_to_remove, Node<T>* node_to_keep);


    /**
     * @brief Removes stale incoming arcs from every node of a layer in a single linear pass.
     *        An in-arc is stale when its parent node was merged away (its out_arcs is empty);
     *        the arc object is deleted here. Replaces the per-arc linear search that made
     *        reduction quadratic in the node in-degree.
     * @param child_layer Layer whose nodes' incoming arcs are swept.
     */
    void clear_incoming_arcs(list<Node<T>*>& child_layer);

    /**
     * @brief Deletes the nodes merged away in the current layer and clears the holding list.
     *        Must be called only after clear_incoming_arcs() has deleted their stale out-arcs.
     */
    void flush_merged_nodes();

    /**
     * @brief Adjusts the node number after changes.
     */
    void adjust_nodes_id() const;

    /**
     * @brief Prints the graph.
     * @param should_visualize Boolean indicating whether to visualize the graph construction
     */
    void print_graph(bool should_visualize);

    /**
     * @brief Prints the graph.
     */
    void print() const;
};
#include "DDReducer.tpp"
#endif //TEMPLATETYPE_REDUCEDDBUILDER_H
