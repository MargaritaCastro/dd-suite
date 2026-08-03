#include "RelaxedPriorityDDBuilder.h"

#include <utility>
#include <vector>
#include <algorithm>

using namespace std;

template <typename T>
bool NodeMergeComparator<T>::operator()(const Node<T>* node_one, const Node<T>* node_two) const {
    const int priority_one = this->problem.get_priority_for_merge_nodes(node_one->get_id(), node_one->state);
    const int priority_two = this->problem.get_priority_for_merge_nodes(node_two->get_id(), node_two->state);

    if (priority_one != priority_two) {
        return priority_one > priority_two;
    }
    return node_one->get_id() < node_two->get_id();
}

template <typename T>
RelaxedPriorityDDBuilder<T>::RelaxedPriorityDDBuilder(AbstractProblem<T>& problem, int max_width) :
        AbstractDDBuilder<T>(problem), max_width(max_width), nodes_set(problem) {
}

template <typename T>
void RelaxedPriorityDDBuilder<T>::specific_end_of_layer_function() {

    if (this->graph->structure.back().size() > max_width) {
        merge_nodes_when_width_is_greater_than_w();
        this->delete_nodes_current_layer(this->graph->actual_layer);
    }

}

template <typename T>
void RelaxedPriorityDDBuilder<T>::specific_end_of_construction_function() {
    this->bottom_up_pruner();
    this->adjust_node_number();
}

template <typename T>
void RelaxedPriorityDDBuilder<T>::merge_nodes_when_width_is_greater_than_w() {

    nodes_set.clear();
    for (Node<T>* node : this->graph->structure.back()) {
        nodes_set.insert(node);
    }

    while (nodes_set.size() > max_width) {
        reduce_layer_width();
    }
}

template <typename T>
void RelaxedPriorityDDBuilder<T>::reduce_layer_width() {
    Node<T>* node_to_remove = *nodes_set.begin();
    Node<T>* node_to_keep = *(++nodes_set.begin());

    nodes_set.erase(nodes_set.begin());
    nodes_set.erase(nodes_set.begin());

    this->map_of_states.erase(this->problem.get_state_as_string(node_to_remove->state));
    this->map_of_states.erase(this->problem.get_state_as_string(node_to_keep->state));

    auto new_state = this->problem.merge_operator(node_to_remove->state, node_to_keep->state);
    auto state_as_string = this->problem.get_state_as_string(new_state);

    if (this->map_of_states.count(state_as_string)) {
        Node<T>* exist_node = this->map_of_states[state_as_string];
        merge_with_an_existing_node(node_to_remove, node_to_keep, exist_node);
        delete new_state;
    } else {
        merge_when_doesnt_exist_node(node_to_remove, node_to_keep, new_state);
        nodes_set.insert(node_to_keep);
        this->map_of_states[state_as_string] = node_to_keep;
    }
}


template <typename T>
void RelaxedPriorityDDBuilder<T>::merge_with_an_existing_node(Node<T>* node_one, Node<T>* node_two, Node<T>* available_node) {
    redirect_in_arcs(node_one, available_node);
    redirect_in_arcs(node_two, available_node);
}

template <typename T>
void RelaxedPriorityDDBuilder<T>::merge_when_doesnt_exist_node(Node<T>* node_to_remove, Node<T>* node_to_keep, T* new_state) {
    redirect_in_arcs(node_to_remove, node_to_keep);
    change_new_state(node_to_keep, new_state);
}

template <typename T>
void RelaxedPriorityDDBuilder<T>::redirect_in_arcs(Node<T>* node_to_remove, Node<T>* node_to_keep) {

    for (Arc<T>* arc : node_to_remove->in_arcs) {
        arc->child_node = node_to_keep;
        node_to_keep->in_arcs.push_back(arc);
    }
    node_to_remove->in_arcs.clear();
}

template <typename T>
void RelaxedPriorityDDBuilder<T>::change_new_state(Node<T>* node, T* new_state) {
    delete node->state;
    node->state = new_state;
}
