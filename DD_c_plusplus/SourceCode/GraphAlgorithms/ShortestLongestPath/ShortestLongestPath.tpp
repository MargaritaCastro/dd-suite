#include "GraphAlgorithms/ShortestLongestPath/ShortestLongestPath.h"

template <typename T>
ShortestLongestPath<T>::ShortestLongestPath(DD<T>& dd) :
        graph(dd.get_decision_diagram()), time(std::chrono::duration<double>::zero()) {
}

template <typename T>
ShortestLongestPath<T>::~ShortestLongestPath() {
    for (auto& path : DP) {
        delete path;
    }
}

template <typename T>
void ShortestLongestPath<T>::set_parameters(vector<double> weights, string objective) {
    this->weights = weights;
    this->objective = objective;
    choose_transform_weights();
}

template <typename T>
string ShortestLongestPath<T>::get_time() {
    return to_string(time.count());
}

template <typename T>
PathStructureSolution<T> ShortestLongestPath<T>::get_solution() const {
    if (solution.value != -111111) {
        return solution;
    } else {
        throw ObjectiveFunctionNotExecuted();
    }
}


template <typename T>
PathStructureSolution<T> ShortestLongestPath<T>::solve() {
    check_if_parameters_are_set();

    auto start = chrono::high_resolution_clock::now();
    cout << endl;
    cout << "Starting the process to solve the shortest or longest path in the graph..." << endl;
    resolve_graph();
    auto end = chrono::high_resolution_clock::now();
    time = end - start;
    cout << "Shortest/longest path algorithm completed." << endl;

    return solution;
}

template <typename T>
void ShortestLongestPath<T>::check_if_parameters_are_set() {

    if (weights.empty()) {
        throw ParametersNotSetException();
    }
}

template <typename T>
void ShortestLongestPath<T>::resolve_graph() {

    int total_nodes = graph->get_node_count();

    DP = vector<PathStructure<T>*>(total_nodes, nullptr);

    PathStructure<T>* objective_answer = shortest_path_algorithm();

    if (objective == "max") {
        objective_answer->value = -objective_answer->value;
    }

    save_solution_path();
}

template <typename T>
void ShortestLongestPath<T>::choose_transform_weights() {
    if (objective == "max") {
        for (auto& weight : weights) {
            weight = -weight;
        }
    }
}

template <typename T>
void ShortestLongestPath<T>::save_solution_path(){

    string optimal_path = "";
    PathStructure<T>* current = DP.back();
    solution.path_arcs.clear();
    solution.value = current->value;

    while (current != nullptr) {
        if (current->arc == nullptr) break; // we reach the root node

        optimal_path =  "-> " + current->arc->to_string() + optimal_path;
        solution.path_arcs.push_front(current->arc);
        current = DP[current->arc->parent_node->get_id()];
    }

    solution.path_print = optimal_path.substr(2);

}

template <typename T>
PathStructure<T>* ShortestLongestPath<T>::shortest_path_algorithm() {

    reset_objective_nodes();

    int arc_aux_value = 0;
    int node_id = 0;
    int child_id = 0;

    for (int layer_id = 0; layer_id < graph->structure.size() - 1; layer_id++) {
        for (Node<T>* node : graph->structure[layer_id]) {
            node_id = node->get_id();

            // Find the smallest value and set structure
            for (Arc<T>* arc: node->out_arcs) {
                arc_aux_value = DP[node_id]->value + arc->variable_value * weights[layer_id];
                child_id = arc->child_node->get_id();
                if (DP[child_id]->value > arc_aux_value) {
                    DP[child_id]->value = arc_aux_value;
                    DP[child_id]->arc = arc;
                }
            }

        }
    }

    Node<T>* terminal_node = graph->structure.back().front();

    return DP[terminal_node->get_id()];
}

template <typename T>
void ShortestLongestPath<T>::reset_objective_nodes(){

    // Root-node
    Node<T>* root_node = graph->structure.front().front();
    DP[root_node->get_id()] = new PathStructure<T>(0, nullptr);

    // Other nodes
    for (int layer_id = 1; layer_id < graph->structure.size(); layer_id++) {
        for (Node<T>* node : graph->structure[layer_id]) {

            //Reset DP values
            if (DP[node->get_id()] == nullptr) {
                DP[node->get_id()] = new PathStructure<T>(numeric_limits<int>::max(), nullptr);
            } else {
                DP[node->get_id()]->value = numeric_limits<int>::max();
                DP[node->get_id()]->arc = nullptr;
            }
        }
    }

}