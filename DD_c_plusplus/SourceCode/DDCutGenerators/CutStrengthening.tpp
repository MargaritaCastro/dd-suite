#include "CutStrengthening.h"

template <typename T>
CutStrengthening<T>::CutStrengthening(DD<T>* DD, bool verbose)
        : verbose(verbose), tolerance(0.0001), is_cut_strengthened(false) {
    graph_DD = DD->get_decision_diagram();
    set_initial_cost();
}

template <typename T>
double CutStrengthening<T>::get_time() const {
    return time;
}

template <typename T>
pair<vector<double>, double> CutStrengthening<T>::get_cut_strengthening() {
    if (!is_cut_strengthened) {
        throw NoCutStrengthening();
    }
    return {coefficients, constant};
}

template <typename T>
bool CutStrengthening<T>::cut_strengthening(const vector<double>& coefficients, double constant) {
    initialize_parameters(coefficients, constant);

    auto start_time = clock();
    if (verbose) {
        cout << "Calculating cut strengthening..." << endl;
    }
    compute_cut_strengthening();
    auto end_time = clock();
    time = static_cast<double>(end_time - start_time) / CLOCKS_PER_SEC;

    if (verbose) {
        if (is_cut_strengthened) {
            cout << endl << " '''A strengthened cut was found in the graph.''' " << endl;
            cout << "    The strengthened cut is: " << get_inequality_string_representation() << endl << endl;
        } else {
            cout << endl << "No strengthened cut was found in the graph." << endl << endl;
        }
    }

    return is_cut_strengthened;
}

template <typename T>
void CutStrengthening<T>::set_initial_cost() {
    cost_top = vector<double>(graph_DD->structure.back().back()->get_id() + 1, 0);
    cost_bottom = vector<double>(graph_DD->structure.back().back()->get_id() + 1, 0);
}

template <typename T>
void CutStrengthening<T>::initialize_parameters(const vector<double>& coefficients, double constant) {
    this->coefficients = coefficients;
    this->constant = constant;
    this->slacks = vector<double>(coefficients.size(), 0);
}

template <typename T>
bool CutStrengthening<T>::compute_cut_strengthening() {
    is_cut_strengthened = false;
    bool can_strengthen = get_bdd_slacks();

    while (can_strengthen) {
        int slack_strengthen_id = choose_slack_to_strengthen();
        if (slack_strengthen_id == -1) {
            if (verbose) {
                cout << "No slack found to strengthen." << endl;
            }
            break;
        }

        coefficients[slack_strengthen_id] += slacks[slack_strengthen_id];
        if (slacks[slack_strengthen_id] <= tolerance) {
            constant += slacks[slack_strengthen_id];
        }

        is_cut_strengthened = true;
        if (verbose) {
            cout << "The cut was strengthened to " << get_inequality_string_representation() << endl;
        }
        can_strengthen = get_bdd_slacks();
    }

    return is_cut_strengthened;
}

template <typename T>
bool CutStrengthening<T>::get_bdd_slacks() {
    compute_cost_top();

    Node<T>* sink = graph_DD->structure.back().front();
    if (constant > cost_top[sink->get_id()]) {
        if (verbose) {
            cout << "Elevating inequality constant from " << constant << " to " << cost_top[sink->get_id()] << endl;
        }
        constant = cost_top[sink->get_id()];
    } else if (constant < cost_top[sink->get_id()]) {
        if (verbose) {
            cout << "The cut cannot be strengthened as it does not satisfy " << constant << " >= " << cost_top[sink->get_id()] << endl;
        }
        return false;
    }

    compute_cost_bottom();
    compute_slacks();

    return true;
}

template <typename T>
void CutStrengthening<T>::compute_cost_top() {
    for (size_t layer_index = 1; layer_index < graph_DD->structure.size(); ++layer_index) {
        for (const auto& node : graph_DD->structure[layer_index]) {
            compute_cost_top_node(node, layer_index-1);
        }
    }
}

template <typename T>
void CutStrengthening<T>::compute_cost_top_node(Node<T>* node, size_t layer_index) {
    double value = 0;
    cost_top[node->get_id()] = -numeric_limits<double>::infinity();

    for (const auto& arc : node->in_arcs) {
        value = cost_top[arc->parent_node->get_id()] + arc->variable_value * coefficients[layer_index];
        if (cost_top[node->get_id()] < value) {
            cost_top[node->get_id()] = value;
        }
    }
}

template <typename T>
void CutStrengthening<T>::compute_cost_bottom() {
    for (int layer_index = graph_DD->structure.size() - 2; layer_index >= 0; --layer_index) {
        for (const auto& node : graph_DD->structure[layer_index]) {
            compute_cost_bottom_node(node, layer_index);
        }
    }
}

template <typename T>
void CutStrengthening<T>::compute_cost_bottom_node(Node<T>* node, size_t layer_index) {
    double value = 0;
    cost_bottom[node->get_id()] = -numeric_limits<double>::infinity();

    for (const auto& arc : node->out_arcs) {
        value = cost_bottom[arc->child_node->get_id()] + arc->variable_value * coefficients[layer_index];
        if (cost_bottom[node->get_id()] < value) {
            cost_bottom[node->get_id()] = value;
        }
    }

}

template <typename T>
void CutStrengthening<T>::compute_slacks() {
    for (size_t layer_index = 0; layer_index < graph_DD->structure.size() - 1; ++layer_index) {
        double zero_edges_cost = -numeric_limits<double>::infinity();
        double one_edges_cost = -numeric_limits<double>::infinity();

        for (const auto& node : graph_DD->structure[layer_index]) {
            for (const auto& arc : node->out_arcs) {
                double value = cost_top[node->get_id()] + cost_bottom[arc->child_node->get_id()] + arc->variable_value * coefficients[layer_index];
                if (arc->variable_value == 0 && value > zero_edges_cost) {
                    zero_edges_cost = value;
                } else if (arc->variable_value == 1 && value > one_edges_cost) {
                    one_edges_cost = value;
                }
            }
        }

        if (zero_edges_cost == -numeric_limits<double>::infinity() || one_edges_cost == -numeric_limits<double>::infinity()) {
            slacks[layer_index] = 0;
        } else {
            slacks[layer_index] = zero_edges_cost - one_edges_cost;
        }
    }
}

template <typename T>
int CutStrengthening<T>::choose_slack_to_strengthen() {
    int candidate = -1;
    double value = numeric_limits<double>::infinity();

    for (size_t variable = 0; variable < slacks.size(); ++variable) {
        if (abs(slacks[variable]) < value && abs(slacks[variable]) > tolerance) {
            value = abs(slacks[variable]);
            candidate = static_cast<int>(variable);
        }
    }

    return candidate;
}

template <typename T>
std::string CutStrengthening<T>::get_inequality_string_representation() const {
    std::ostringstream inequality;
    inequality << std::fixed << std::setprecision(1);

    for (size_t i = 0; i < coefficients.size(); ++i) {
        if (coefficients[i] != 0) {
            if (inequality.tellp() > 0) {
                inequality << " + ";
            }
            inequality << coefficients[i] << "x_" << i;
        }
    }
    inequality << " <= " << constant;
    return inequality.str();
}