#ifndef TEMPLATETYPE_ARC_H
#define TEMPLATETYPE_ARC_H

#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;

// Forward declaration of the Node class to avoid circular dependency problems
template <typename T>
class Node;

/**
 * @brief Class representing an arc in a graph.
 * @tparam T Type of the Node state values
 */
template <typename T>
class Arc{
public:
    /**
     * @brief Constructor for the Arc class.
     * @param parent_node Pointer to the node from which the arc originates.
     * @param child_node Pointer to the node into which the arc enters.
     * @param variable_value Value taken by the associated variable.
     */
    Arc(Node<T>* parent_node, Node<T>* child_node, int variable_value);

    Node<T>* parent_node; /**< Pointer to the node from which the arc originates. */
    Node<T>* child_node; /**< Pointer to the node into which the arc enters. */
    int variable_value; /**< Value taken by the associated variable */
    int arc_id = -1; /**< Residual graph index assigned by MaxFlow (even=forward, arc_id^1=reverse). */

    /**
     * @brief Converts the arc to a string. Shows (value) by default, or (name=value) when name is provided.
     * @param variable_name Optional variable name to include in the output.
     * @return string representation of the arc.
     */
    string to_string(const string& variable_name = "") const;

    /**
     * @brief Equality comparison operator overload for arc.
     * @param other Arc to compare with.
     * @return bool if the arc are equal, false otherwise.
     */
    bool operator==(const Arc &other) const;

    /**
     * @brief Default destructor for the Arc class.
     */
    ~Arc() = default;
};

#include "Arc.tpp"
#endif //TEMPLATETYPE_ARC_H
