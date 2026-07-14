#include "RestrictedDDBuilder.h"

using namespace std;


template <typename T>
bool NodeDeleteComparator<T>::operator()(const Node<T>* node_one, const Node<T>* node_two) const {
    return this->problem.get_priority_for_discard_node(node_one->state) <
           this->problem.get_priority_for_discard_node(node_two->state);
}

template <typename T>
RestrictedDDBuilder<T>::RestrictedDDBuilder(AbstractProblem<T>& problem, int max_width) :
        AbstractDDBuilder<T>(problem), max_width(max_width), nodes_set(problem) {}

template <typename T>
void RestrictedDDBuilder<T>::specific_end_of_layer_function() {
    if (this->graph->structure.back().size() > max_width) {
        eliminate_nodes_when_width_is_greater_than_w();
    }

}

template <typename T>
void RestrictedDDBuilder<T>::specific_end_of_construction_function() {
    this->bottom_up_pruner();
    this->adjust_node_number();

}

template <typename T>
void RestrictedDDBuilder<T>::eliminate_nodes_when_width_is_greater_than_w() {

    list<Node<T>*> ordered_nodes = this->graph->structure.back();

    ordered_nodes.sort([this](Node<T>* node1, Node<T>* node2) {
        return this->problem.get_priority_for_discard_node(node1->state) <
               this->problem.get_priority_for_discard_node(node2->state);
    });

    auto it = ordered_nodes.begin();
    for (int i = 0; i < max_width; i++) it++;
    list<Node<T>*> nodesToEliminate(it, ordered_nodes.end());

    eliminate_nodes_from_layer(nodesToEliminate, this->graph->actual_layer);
}

template <typename T>
void RestrictedDDBuilder<T>::eliminate_nodes_from_layer(const list<Node<T>*>& nodes_to_eliminate, int layer_id) {

    for (auto& node : nodes_to_eliminate) {
        node->in_arcs.clear();
    }

    remove_outgoing_arcs_previous_layer(layer_id);
    this->delete_nodes_current_layer(layer_id);
}

template <typename T>
void RestrictedDDBuilder<T>::remove_outgoing_arcs_previous_layer(int layer_id){

    layer_id --;
    if (layer_id >= this->graph->structure.size() or layer_id <= 0) {
        return;
    }

    Arc<T>* outgoing_arc = nullptr;

    for (auto& node : this->graph->structure[layer_id]) {
        for (auto it_arc = node->out_arcs.begin(); it_arc != node->out_arcs.end(); ) {
            outgoing_arc = *it_arc;
            if ((*it_arc)->child_node->in_arcs.empty()) {
                it_arc = node->out_arcs.erase(it_arc);
                delete outgoing_arc;
            } else {
                ++it_arc;
            }
        }
    }
}



