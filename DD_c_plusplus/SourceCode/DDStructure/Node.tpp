
#include "Node.h"
#include <iostream>
#include <sstream>
#include <vector>
#include <string>
#include <algorithm>

using namespace std;


template <typename T>
Node<T>::Node(int node_id, T* state) : id(node_id), state(state) { }

template <typename T>
Node<T>::~Node() {
    if (state != nullptr) {
        delete state;
    }
}

template <typename T>
const string& Node<T>::to_string() {
    if (node_as_string.empty()) {
        node_as_string = std::to_string(id);
    }

    return node_as_string;
}

template <typename T>
void Node<T>::set_id(int id) {
    this->id = id;
    node_as_string.clear();
}

template <typename T>
int Node<T>::get_id() const {
    return id;
}

template <typename T>
bool Node<T>::operator==(const Node<T> &other) const {
    bool same_in_arc;
    bool same_out_arc;
    if (in_arcs.size() == other.in_arcs.size()) {
        same_in_arc = std::equal(in_arcs.begin(), in_arcs.end(), other.in_arcs.begin());
    } else {
        same_in_arc = false;
    }
    if (out_arcs.size() == other.out_arcs.size()) {
        same_out_arc = std::equal(out_arcs.begin(), out_arcs.end(), other.out_arcs.begin());
    } else {
        same_out_arc = false;
    }
    return state == other.state and same_in_arc and same_out_arc;
}
