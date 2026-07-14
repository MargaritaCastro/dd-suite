#include "FlowCuts.h"

template <typename T>
FlowCuts<T>::FlowCuts(DD<T>* DD) : graph_DD(nullptr), max_flow_instance(nullptr), time(0), verbose(false), tolerance(0.0001) {
    graph_DD = DD->get_decision_diagram();
    max_flow_instance = new MaxFlow<T>(DD);
}

template <typename T>
FlowCuts<T>::~FlowCuts() {
    graph_DD = nullptr;

    if (max_flow_instance != nullptr) {
        delete max_flow_instance;
        max_flow_instance = nullptr;
    }
}

template <typename T>
double FlowCuts<T>::get_time() {
    return time;
}

template <typename T>
string FlowCuts<T>::get_name() {
    return "Flow Cuts (Combinatorial Cuts)";
}

template <typename T>
pair<vector<double>, double> FlowCuts<T>::get_cut() {
    if (this->coefficients.empty()) {
        throw NoCutsFound();
    }
    return {this->coefficients, this->constant};
}

template <typename T>
double FlowCuts<T>::get_min_cut() {
    if (this->coefficients.empty()) {
        throw NoCutsFound();
    }
    return min_cut;
}

template <typename T>
bool FlowCuts<T>::generate_cut(vector<double>& x_values, bool verbose) {
    this->verbose = verbose;
    clock_t start_time = clock();
    bool find_cut = compute_cut(x_values);
    clock_t end_time = clock();
    time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC;

    if (find_cut) {
        if (verbose) {
            cout << endl << " '''A cut was found in the graph.'''" << endl;
            cout << "    The cut it is: " << this->get_inequality_string_representation() << endl << endl;
        }
    } else {
        if (verbose) {
            cout << endl << " '''No cut was found in the graph.''' " << endl << endl;
        }
        this->coefficients.clear();
        this->constant = 0;
    }
    return find_cut;
}

template <typename T>
bool FlowCuts<T>::compute_cut(const vector<double>& x_values) {
    this->coefficients.assign(graph_DD->structure.size() - 1, 0);
    this->constant = -1;
    this->x_values = x_values;

    if (verbose) {
        cout << "\nCalculating cut...\n" << endl;
    }
    return separate_min_cut();
}

template <typename T>
bool FlowCuts<T>::separate_min_cut() {
    double max_flow = max_flow_instance->solve_max_flow(x_values, verbose);

    if (max_flow >= 1 - tolerance) {
        return false;
    }

    get_min_cut_arcs();
    return is_constraint_violated();
}

template <typename T>
void FlowCuts<T>::get_min_cut_arcs() {
    vector<bool> visited = max_flow_instance->compute_visited_nodes();
    if (verbose) {
        cout << "visited nodes: ";
        for (const auto& layer : graph_DD->structure)
            for (const auto* node : layer)
                if (visited[node->get_id()])
                    cout << "node " << node->get_id() << ", ";
        cout << endl;
    }

    int num_nodes = 0;
    min_cut = 0;

    for (size_t layer_index = 0; layer_index < graph_DD->structure.size(); ++layer_index) {
        for (auto* node : graph_DD->structure[layer_index]) {
            if (!visited[node->get_id()]) {
                continue;
            }

            for (auto* arc : node->out_arcs) {
                if (visited[arc->child_node->get_id()]) {
                    continue;
                }
                if (arc->variable_value == 0) {
                    min_cut += 1 - x_values[layer_index];
                    this->coefficients[layer_index] += 1;
                    this->constant += 1;
                } else {
                    min_cut += x_values[layer_index];
                    this->coefficients[layer_index] -= 1;
                }
            }
        }
    }

    if (verbose) {
        cout << "coefficients = ";
        for (const auto& coef : this->coefficients) {
            cout << coef << " ";
        }
        cout << "... constant = " << this->constant << "... min_cut = " << min_cut << endl;
    }
}

template <typename T>
bool FlowCuts<T>::is_constraint_violated() const {
    double left_hand_side = 0;
    for (size_t i = 0; i < this->coefficients.size(); ++i) {
        left_hand_side += this->coefficients[i] * x_values[i];
    }

    if (verbose) {
        cout << "¿left_hand_side (" << left_hand_side << ") - constant (" << this->constant << ") >= tolerance (" << tolerance << ")?\n";
    }

    return left_hand_side - this->constant >= tolerance;
}
