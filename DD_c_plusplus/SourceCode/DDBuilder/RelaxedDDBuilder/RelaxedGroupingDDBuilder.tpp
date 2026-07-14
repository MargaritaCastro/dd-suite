#include "RelaxedGroupingDDBuilder.h"

#include <utility>
#include <vector>
#include <algorithm>
#include <unordered_set>

using namespace std;

template <typename T>
RelaxedGroupingDDBuilder<T>::RelaxedGroupingDDBuilder(AbstractProblem<T>& problem, int max_width) :
        AbstractDDBuilder<T>(problem), max_width(max_width) {
}

template <typename T>
void RelaxedGroupingDDBuilder<T>::specific_end_of_layer_function() {
    if (this->graph->structure.back().size() > max_width) {
        merge_nodes_when_width_is_greater_than_w();
        this->delete_nodes_current_layer(this->graph->actual_layer);
    }
}

template <typename T>
void RelaxedGroupingDDBuilder<T>::specific_end_of_construction_function() {
    this->bottom_up_pruner();
    this->adjust_node_number();
}

template <typename T>
void RelaxedGroupingDDBuilder<T>::merge_nodes_when_width_is_greater_than_w() {
    // Cache each node's priority once; carry (priority, node) pairs sorted by priority.
    vector<pair<int, Node<T>*>> alive;
    alive.reserve(this->graph->structure.back().size());
    for (Node<T>* node : this->graph->structure.back()) {
        alive.emplace_back(merge_priority(node), node);
    }
    sort(alive.begin(), alive.end(),
         [](const pair<int, Node<T>*>& a, const pair<int, Node<T>*>& b) { return a.first < b.first; });

    int difference = 0;
    while (static_cast<int>(alive.size()) > max_width) {
        alive = merge_nodes_with_difference(alive, difference);
        if (static_cast<int>(alive.size()) <= max_width) {
            break;
        }
        // Skip tolerances that would merge nothing: jump to the next gap that does.
        difference = max(difference + 1, minimum_gap(alive));
    }
}

template <typename T>
vector<pair<int, Node<T>*>> RelaxedGroupingDDBuilder<T>::merge_nodes_with_difference(
        vector<pair<int, Node<T>*>>& alive, int difference) {

    vector<pair<int, Node<T>*>> new_alive;
    int width = static_cast<int>(alive.size());
    size_t total = alive.size();
    size_t index = 0;

    while (index < total) {
        size_t next_index = index + 1;
        while (next_index < total && alive[next_index].first - alive[next_index - 1].first <= difference) {
            next_index++;
        }

        size_t group_size = next_index - index;
        if (group_size > 1 && width > max_width) {
            vector<Node<T>*> group;
            group.reserve(group_size);
            for (size_t k = index; k < next_index; k++) {
                group.push_back(alive[k].second);
            }
            Node<T>* survivor = merge_group(group);
            width -= static_cast<int>(group_size - 1);
            new_alive.emplace_back(merge_priority(survivor), survivor);
        } else {
            for (size_t k = index; k < next_index; k++) {
                new_alive.push_back(alive[k]);
            }
        }

        index = next_index;
    }

    // A merge can produce a state that already belongs to another surviving node: drop duplicates.
    unordered_set<Node<T>*> seen;
    vector<pair<int, Node<T>*>> unique_alive;
    unique_alive.reserve(new_alive.size());
    for (const auto& entry : new_alive) {
        if (seen.insert(entry.second).second) {
            unique_alive.push_back(entry);
        }
    }

    sort(unique_alive.begin(), unique_alive.end(),
         [](const pair<int, Node<T>*>& a, const pair<int, Node<T>*>& b) { return a.first < b.first; });
    return unique_alive;
}

template <typename T>
Node<T>* RelaxedGroupingDDBuilder<T>::merge_group(vector<Node<T>*>& group) {
    Node<T>* survivor = group[0];
    for (size_t k = 1; k < group.size(); k++) {
        survivor = merge_two_nodes(group[k], survivor);
    }
    return survivor;
}

template <typename T>
Node<T>* RelaxedGroupingDDBuilder<T>::merge_two_nodes(Node<T>* node_to_remove, Node<T>* node_to_keep) {
    if (node_to_remove == node_to_keep) {
        return node_to_keep;
    }

    this->map_of_states.erase(this->problem.get_state_as_string(node_to_remove->state));
    this->map_of_states.erase(this->problem.get_state_as_string(node_to_keep->state));

    auto new_state = this->problem.merge_operator(node_to_remove->state, node_to_keep->state);
    auto state_as_string = this->problem.get_state_as_string(new_state);

    if (this->map_of_states.count(state_as_string)) {
        Node<T>* exist_node = this->map_of_states[state_as_string];
        merge_with_an_existing_node(node_to_remove, node_to_keep, exist_node);
        delete new_state;
        return exist_node;
    }

    merge_when_doesnt_exist_node(node_to_remove, node_to_keep, new_state);
    this->map_of_states[state_as_string] = node_to_keep;
    return node_to_keep;
}

template <typename T>
int RelaxedGroupingDDBuilder<T>::merge_priority(Node<T>* node) {
    return this->problem.get_priority_for_merge_nodes(node->get_id(), node->state);
}

template <typename T>
int RelaxedGroupingDDBuilder<T>::minimum_gap(const vector<pair<int, Node<T>*>>& alive) {
    int smallest = alive[1].first - alive[0].first;
    for (size_t i = 2; i < alive.size(); i++) {
        int gap = alive[i].first - alive[i - 1].first;
        if (gap < smallest) {
            smallest = gap;
        }
    }
    return smallest;
}

template <typename T>
void RelaxedGroupingDDBuilder<T>::merge_with_an_existing_node(Node<T>* node_one, Node<T>* node_two, Node<T>* available_node) {
    redirect_in_arcs(node_one, available_node);
    redirect_in_arcs(node_two, available_node);
}

template <typename T>
void RelaxedGroupingDDBuilder<T>::merge_when_doesnt_exist_node(Node<T>* node_to_remove, Node<T>* node_to_keep, T* new_state) {
    redirect_in_arcs(node_to_remove, node_to_keep);
    change_new_state(node_to_keep, new_state);
}

template <typename T>
void RelaxedGroupingDDBuilder<T>::redirect_in_arcs(Node<T>* node_to_remove, Node<T>* node_to_keep) {
    for (Arc<T>* arc : node_to_remove->in_arcs) {
        arc->child_node = node_to_keep;
        node_to_keep->in_arcs.push_back(arc);
    }
    node_to_remove->in_arcs.clear();
}

template <typename T>
void RelaxedGroupingDDBuilder<T>::change_new_state(Node<T>* node, T* new_state) {
    delete node->state;
    node->state = new_state;
}
