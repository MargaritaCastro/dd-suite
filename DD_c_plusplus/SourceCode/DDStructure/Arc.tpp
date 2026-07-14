#include "Arc.h"
#include "Node.h"

#include <string>

template <typename T>
Arc<T>::Arc(Node<T>* parent_node, Node<T>* child_node, int variable_value)
        : parent_node(parent_node), child_node(child_node), variable_value(variable_value) {}

template <typename T>
string Arc<T>::to_string(const string& variable_name) const {
    string base = "arc_" + std::to_string(parent_node->get_id()) + "_" + std::to_string(child_node->get_id());
    if (!variable_name.empty()) {
        base += "(" + variable_name + "=" + std::to_string(variable_value) + ")";
    } else {
        base += "(" + std::to_string(variable_value) + ")";
    }
    return base;
}

template <typename T>
bool Arc<T>::operator==(const Arc<T> &other) const {
    return parent_node->get_id() == other.parent_node->get_id() and child_node->get_id() == other.child_node->get_id() and
           variable_value == other.variable_value;
}


