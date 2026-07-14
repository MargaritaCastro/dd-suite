#include "GraphAlgorithms/MaxFlow/MaxFlow.h"

template <typename T>
MaxFlow<T>::MaxFlow(DD<T>* DD) :
        graph_DD(DD->get_decision_diagram()), time(0), max_flow_computed(false) {
    init_arc_ids();
}

template <typename T>
void MaxFlow<T>::init_arc_ids() {
    int arc_id = 0;
    for (const auto& layer : graph_DD->structure)
        for (Node<T>* node : layer)
            for (Arc<T>* arc : node->out_arcs) {
                arc->arc_id = arc_id;
                arc_list.push_back(arc);
                arc_id += 2;
            }
    residual.assign(arc_id, 0.0);
}

template <typename T>
double MaxFlow<T>::get_time() const {
    return time;
}

template <typename T>
double MaxFlow<T>::get_max_flow() const {
    if (!max_flow_computed) {
        throw MaxFlowNotExecuted();
    }
    return max_flow;
}

template <typename T>
double MaxFlow<T>::solve_max_flow(const vector<double>& capacities, bool verbose, double tolerance) {
    this->tolerance = tolerance;
    set_capacities(capacities);
    this->verbose = verbose;

    auto start_time = chrono::high_resolution_clock::now();
    if (verbose) {
        cout << "\nCalculating maximum flow in the graph...\n" << endl;
    }
    max_flow = compute_max_flow();
    max_flow_computed = true;
    auto end_time = chrono::high_resolution_clock::now();
    time = chrono::duration<double>(end_time - start_time).count();
    if (verbose) {
        cout << "\n'''The maximum flow in the graph is " << max_flow << "'''\n" << endl;
    }
    return max_flow;
}

template <typename T>
void MaxFlow<T>::set_capacities(const vector<double>& capacities) {
    fill(residual.begin(), residual.end(), 0.0);

    for (int layer = 0; layer < (int)graph_DD->structure.size(); ++layer) {
        double cap = (layer < (int)capacities.size()) ? capacities[layer] : 0.0;
        for (Node<T>* node : graph_DD->structure[layer]) {
            for (Arc<T>* arc : node->out_arcs) {
                residual[arc->arc_id] = (arc->variable_value == 1) ? cap : 1.0 - cap;
            }
        }
    }
}

template <typename T>
double MaxFlow<T>::compute_max_flow() {
    double total_flow = 0.0;

    vector<int> path;
    visited_ids.clear();
    find_augmenting_path(graph_DD->structure[0].front(), graph_DD->structure.back().back(), path);

    while (!path.empty()) {
        double path_flow = numeric_limits<double>::infinity();
        for (int id : path)
            path_flow = min(path_flow, residual[id]);

        for (int id : path) {
            residual[id] -= path_flow;
            residual[id ^ 1] += path_flow;
        }

        total_flow += path_flow;

        if (verbose)
            print_path(path, path_flow);

        path.clear();
        visited_ids.clear();
        find_augmenting_path(graph_DD->structure[0].front(), graph_DD->structure.back().back(), path);
    }
    return total_flow;
}

template <typename T>
bool MaxFlow<T>::find_augmenting_path(Node<T>* start_node, Node<T>* terminal_node, vector<int>& path) {
    visited_ids.insert(start_node->get_id());

    if (start_node == terminal_node)
        return true;

    for (Arc<T>* arc : start_node->out_arcs) {
        int id = arc->arc_id;
        if (visited_ids.find(arc->child_node->get_id()) == visited_ids.end() && residual[id] > 0) {
            path.push_back(id);
            if (find_augmenting_path(arc->child_node, terminal_node, path))
                return true;
            path.pop_back();
        }
    }

    for (Arc<T>* arc : start_node->in_arcs) {
        int rev_id = arc->arc_id ^ 1;
        if (visited_ids.find(arc->parent_node->get_id()) == visited_ids.end() && residual[rev_id] > 0) {
            path.push_back(rev_id);
            if (find_augmenting_path(arc->parent_node, terminal_node, path))
                return true;
            path.pop_back();
        }
    }

    return false;
}

template <typename T>
void MaxFlow<T>::print_path(const vector<int>& path, double path_flow) const {
    vector<string> node_ids;
    for (int arc_id : path) {
        Arc<T>* arc = arc_list[arc_id >> 1];
        bool is_forward = (arc_id & 1) == 0;
        string p = to_string(arc->parent_node->get_id());
        string c = to_string(arc->child_node->get_id());
        if (is_forward) {
            if (node_ids.empty() || node_ids.back() != p) node_ids.push_back(p);
            node_ids.push_back(c);
        } else {
            if (node_ids.empty() || node_ids.back() != c) node_ids.push_back(c);
            node_ids.push_back(p);
        }
    }
    cout << "Path: " << node_ids[0];
    for (size_t i = 1; i < node_ids.size(); ++i)
        cout << " -> " << node_ids[i];
    cout << ", Flow: " << std::round(path_flow * 1000) / 1000 << endl;
}

template <typename T>
vector<bool> MaxFlow<T>::compute_visited_nodes() {
    auto visited = reset_visited_nodes();

    std::deque<Node<T>*> q_nodes(graph_DD->structure[0].begin(), graph_DD->structure[0].end());
    visited[q_nodes.front()->get_id()] = true;

    while (!q_nodes.empty()) {
        Node<T>* node = q_nodes.front();
        q_nodes.pop_front();

        for (auto* arc : node->out_arcs) {
            int child_id = arc->child_node->get_id();
            if (residual[arc->arc_id] > tolerance && !visited[child_id]) {
                visited[child_id] = true;
                q_nodes.push_back(arc->child_node);
            }
        }

        for (auto* arc : node->in_arcs) {
            int parent_id = arc->parent_node->get_id();
            if (residual[arc->arc_id ^ 1] > tolerance && !visited[parent_id]) {
                visited[parent_id] = true;
                q_nodes.push_back(arc->parent_node);
            }
        }
    }
    return visited;
}

template <typename T>
vector<bool> MaxFlow<T>::reset_visited_nodes() {
    return vector<bool>(graph_DD->get_node_count(), false);
}
