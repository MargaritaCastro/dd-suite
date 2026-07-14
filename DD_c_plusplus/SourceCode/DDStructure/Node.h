#ifndef TEMPLATETYPE_NODE_H
#define TEMPLATETYPE_NODE_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>
#include <list>

using namespace std;

// Forward declaration of the Arc class to avoid circular dependency issues.
template <typename T>
class Arc;

/**
 * @brief Class representing a node in a graph.
 * @tparam T Type of the Node state values
 */
template <typename T>
class Node {
public:
    /**
     * @brief Constructor for the Node class.
     * @param node_id Unique identifier for the node.
     * @param state State of the node represented as a vector of integers.
     */
    Node(int node_id, T* state);

    /**
     * @brief Default constructor for the Node class.
     */
    Node() = default;

    T* state; /**< State of the node, which can have different data types. */
    list<Arc<T>*> in_arcs; /**< List of arc entering the node. */
    list<Arc<T>*> out_arcs; /**< List of arc exiting the node. */

    /**
     * @brief Converts the node to a string.
     * @return String representation of the node.
     */
    const string& to_string();

    /**
     * @brief Overloads the equality operator for nodes.
     * @param other Node to compare with.
     * @return bool true if the nodes are equal, false otherwise.
     */
    bool operator==(const Node &other) const;

    /**
     * @brief Sets the node's ID.
     * @param id Unique identifier for the node.
     */
     void set_id(int id);

    /**
     * @brief Gets the node's ID.
     * @return Unique identifier for the node.
     */
    int get_id() const;

    /**
     * @brief Destructor for the Node class.
     */
    ~Node();


private:
    int id; /**< Unique identifier for the node. */
    string node_as_string; /**< String representation of the node. */
};

#include "Node.tpp"
#endif //TEMPLATETYPE_NODE_H
