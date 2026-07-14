//
// Created by antonia blanco on 18-01-24.
//

#include "Graph.h"
#include <typeinfo>

template <typename T>
Graph<T>::Graph(Node<T>* initial_node) : structure(), actual_layer(0) {
    structure.push_back( list<Node<T>*>() );
    structure[0].push_back(initial_node);
}

template <typename T>
Graph<T>::~Graph() {
    for (size_t i = 0; i < structure.size(); ++i) {
        for (auto& node: structure[i]) {
            for (auto& arc : node->out_arcs) {
                if (arc != nullptr) {
                    delete arc;
                }
            }
            delete node;
        }
    }
    structure.clear();
}

template <typename T>
bool Graph<T>::operator==(const Graph& other) const {
    bool result = true;
    bool there_is_equal_node;

    if (structure.size() != other.structure.size()) {return false;}

    for (size_t i = 0; i < structure.size(); ++i) {
        if (structure[i].size() != other.structure[i].size()) {return false;}
    
        for (auto node : structure[i]) {

            there_is_equal_node = false;
            for (auto other_node: other.structure[i]) {
                if (*node->state == *other_node->state) {
                    there_is_equal_node = there_is_equal_node or compare_two_nodes(node, other_node);
                }
            }
            result = result and there_is_equal_node;
        }
    }

    return result;
}

template <typename T>
bool Graph<T>::compare_two_nodes(Node<T>* nodeOne,  Node<T>* nodeTwo) {
    if (nodeOne->in_arcs.size() != nodeTwo->in_arcs.size() or nodeOne->out_arcs.size() != nodeTwo->out_arcs.size()) {
        return false;
    }

    bool return_in_arcs = true;

    for (auto& arc : nodeOne->in_arcs) {
        bool there_is_equal_arc = false;

        for (auto& other_arc : nodeTwo->in_arcs) {
            if (arc->variable_value == other_arc->variable_value and *arc->parent_node->state == *other_arc->parent_node->state) {
                there_is_equal_arc = true;
            }
        }
        return_in_arcs = return_in_arcs and there_is_equal_arc;
    }

    bool return_out_arcs = true;
    for (auto& arc : nodeOne->out_arcs) {
        bool there_is_equal_arc = false;

        for (auto& other_arc : nodeTwo->out_arcs) {
            if (arc->variable_value == other_arc->variable_value and *arc->child_node->state == *other_arc->child_node->state) {
                there_is_equal_arc = true;
            }
        }
        return_out_arcs = return_out_arcs and there_is_equal_arc;
    }

    return return_in_arcs and return_out_arcs;
}

template <typename T>
int Graph<T>::get_node_count() {
    int count = 0;
    for (const auto& layer : structure) {
        count += layer.size();
    }
    return count;
}

template <typename T>
int Graph<T>::get_arc_count() {
    int count = 0;
    for (const auto& layer : structure) {
        for (const auto& node : layer) {
            count += node->out_arcs.size();
        }
    }
    return count;
}

template <typename T>
int Graph<T>::get_actual_max_width() {
    if (structure.empty()) return 0;

    int max_width = 0;
    for (const auto& layer : structure) {
        max_width = std::max(max_width, static_cast<int>(layer.size()));
    }
    return max_width;
}


template <typename T>
void Graph<T>::add_node(Node<T>* node) {
    structure[actual_layer].push_back(node);
}

template <typename T>
Node<T>* Graph<T>::create_and_add_node(int node_id, T* node_state) {
    Node<T> *new_node = new Node(node_id, node_state);
    structure[actual_layer].push_back(new_node);
    return new_node;
}

template <typename T>
void Graph<T>::add_new_layer() {
    actual_layer++;
    structure.push_back(list<Node<T>*>());
}



