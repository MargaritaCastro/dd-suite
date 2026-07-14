#ifndef TEMPLATETYPE_GRAPH_H
#define TEMPLATETYPE_GRAPH_H

#include "Node.h"
#include "Arc.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

/**
 * @brief Class representing a graph.
 * @tparam T Type of the Node state values
 */
template <typename T>
class Graph {
public:
    /**
     * @brief Constructor for the Graph class.
     * @param initial_node Initial node of the graph.
     */
    Graph(Node<T>* initial_node);

    /**
     * @brief Destructor for the Graph class.
     */
    ~Graph();

    vector<list<Node<T>*>> structure; /**< Graph structure represented as a matrix of nodes separated by levels. */
    int actual_layer; /**< Index of the current layer of the graph being worked on. */

    /**
     * @brief Equality comparison operator overload for graphs.
     * @param other Graph to compare with.
     * @return bool true if the graphs are equal, false otherwise.
     */
    bool operator==(const Graph &other) const;

    /**
     * @brief Gets the number of node in the graph.
     * @return int Representing the number of node in the graph.
     */
    int get_node_count();

    /**
     * @brief Gets the number of arc in the graph.
     * @return int Representing the number of arc in the graph.
     */
    int get_arc_count();

    /**
 * @brief Gets the maximum width of the graph.
 * @return int Maximum width of the graph.
 */
    int get_actual_max_width();

    /**
     * @brief Adds a node to the graph.
     * @param node Node to add.
     */
    void add_node(Node<T>* node);

/**
     * @brief Adds an node to the graph.
     * @param node_id Node id to add
     * @param node_state Node state to add
     * @return node Node to add
     */
    Node<T>* create_and_add_node(int node_id, T* node_state);

    /**
     * @brief Adds a new layer to the graph.
     */
    void add_new_layer();

private:
    /**
     * @brief Compares two nodes.
     * @param nodeOne Node to compare.
     * @param nodeTwo Node to compare.
     * @return bool true if the nodes are equal, false otherwise.
     */
    static bool compare_two_nodes(Node<T>* nodeOne, Node<T>* nodeTwo) ;
    
};
#include "Graph.tpp"
#endif //TEMPLATETYPE_GRAPH_H
