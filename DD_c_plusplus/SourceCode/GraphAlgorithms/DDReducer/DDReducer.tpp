#include "DDReducer.h"

using namespace std;


template <typename T>
DDReducer<T>::DDReducer(Graph<T>* graph, AbstractProblem<T>& problem) :
    graph(graph),
    problem(problem){}

template <typename T>
Graph<T>* DDReducer<T>::reduce_decision_diagram(bool should_visualize) {

    // Bottom-up: after merging nodes in a layer, the merged-away nodes leave stale out-arcs
    // dangling in their children's in_arcs. Instead of searching and erasing each one (which is
    // quadratic in the node in-degree), we sweep the child layer once, in linear time.
    list<Node<T>*>* child_layer = nullptr;

    for (auto layer_it = graph->structure.rbegin(); layer_it != graph->structure.rend(); ++layer_it) {
        print_graph(should_visualize);
        bool merged = review_layer_to_merge_nodes(*layer_it);

        if (merged && child_layer != nullptr) {
            clear_incoming_arcs(*child_layer);   // delete stale out-arcs of the merged nodes
        }
        flush_merged_nodes();                    // safe now: their out-arcs were just removed

        child_layer = &(*layer_it);
    }

    adjust_nodes_id();
    print_graph(should_visualize);

    return graph;
}

template <typename T>
bool DDReducer<T>::review_layer_to_merge_nodes(list<Node<T>*>& layer) {

    unordered_map<string, Node<T>*> map_of_paths;
    string paths_of_node;
    map_of_paths.reserve(layer.size());
    auto node_it = layer.begin();
    Node<T>* node = nullptr;
    bool merged = false;

    while (node_it != layer.end()){
        node = *node_it;
        get_node_of_every_type_of_path(node,paths_of_node);

        auto [it, inserted] = map_of_paths.try_emplace(paths_of_node, node);
        if (!inserted) {
            redirect_in_arcs(node, it->second);
            // Keep the arc objects alive in the children's in_arcs; clear_incoming_arcs() deletes
            // them. The node is held in merged_nodes and deleted by flush_merged_nodes() afterwards.
            node->out_arcs.clear();
            merged_nodes.push_back(node);
            node_it = layer.erase(node_it);
            merged = true;
        } else {
            ++node_it;
        }
    }

    return merged;
}

template <typename T>
void DDReducer<T>::get_node_of_every_type_of_path(Node<T>* node,  string& paths_of_node) {
    paths_of_node.clear();
    for (Arc<T>* arc : node->out_arcs) {
        paths_of_node +=  arc->child_node->to_string() + to_string(arc->variable_value) + "_";
    }
}

template <typename T>
void DDReducer<T>::redirect_in_arcs(Node<T>* node_to_remove, Node<T>* node_to_keep) {

    node_to_keep->in_arcs.insert(node_to_keep->in_arcs.end(),
        node_to_remove->in_arcs.begin(),node_to_remove->in_arcs.end());

    for (Arc<T>* arc : node_to_remove->in_arcs) {
        arc->child_node = node_to_keep;
        //node_to_keep->in_arcs.push_back(arc);
    }
    node_to_remove->in_arcs.clear();
}

template <typename T>
void DDReducer<T>::clear_incoming_arcs(list<Node<T>*>& child_layer) {
    for (Node<T>* node : child_layer) {
        for (auto arc_it = node->in_arcs.begin(); arc_it != node->in_arcs.end(); ) {
            Arc<T>* arc = *arc_it;
            if (arc->parent_node->out_arcs.empty()) {   // parent was merged away → arc is stale
                arc_it = node->in_arcs.erase(arc_it);
                delete arc;
            } else {
                ++arc_it;
            }
        }
    }
}

template <typename T>
void DDReducer<T>::flush_merged_nodes() {
    for (Node<T>* node : merged_nodes) {
        delete node;
    }
    merged_nodes.clear();
}

template <typename T>
void DDReducer<T>::adjust_nodes_id() const {
    int initial_node_number = 0;
    for (list <Node<T>*>& layer : graph->structure) {
        for (Node<T>* node : layer) {
            node->set_id(initial_node_number);
            initial_node_number++;
        }
    }
}

template <typename T>
void DDReducer<T>::print_graph(bool should_visualize) {
    if (should_visualize) {
        print();
    }
}

template <typename T>
void DDReducer<T>::print() const {

    const vector<string>& variables = problem.ordered_variables;
    cout << "" << endl;
    for (size_t layer_idx = 0; layer_idx < graph->structure.size(); ++layer_idx) {
        cout << "------------------------------------------------------" << endl;
        if (layer_idx > 0 && layer_idx <= variables.size()) {
            cout << "[" << variables[layer_idx - 1] << "]" << "   ";
        }
        for (auto& node : graph->structure[layer_idx]) {
            string in_arcs_str;
            bool first = true;
            for (const auto& arc : node->in_arcs) {
                if (!first) in_arcs_str += ", ";
                in_arcs_str += arc->to_string();
                first = false;
            }
            cout << "u" << node->to_string() << " " << problem.get_state_as_string(node->state) << "(" << in_arcs_str << ")" << "   ";
        }
        cout << "" << endl;
    }
}
