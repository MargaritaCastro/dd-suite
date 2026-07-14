#include "JointFlowCuts.h"

template <typename T>
JointFlowCuts<T>::JointFlowCuts(DD<T>* DD) : time(0), verbose(false), tolerance(0.0001){
    env = new GRBEnv();
    model = new GRBModel(*env);
    model->set(GRB_IntParam_OutputFlag, 0);   // silenciar log de Gurobi (igual que Python)
    graph_DD = DD->get_decision_diagram();

    set_variables();
    set_constraints();
}

template <typename T>
void JointFlowCuts<T>::set_variables() {
    int num_vars = graph_DD->structure.size() - 1;
    int node_index = 0;

    mu.resize(num_vars);
    lambda_vars.resize(num_vars);
    nodes_to_vars.resize(num_vars + 1);
    w.resize(graph_DD->structure.size());

    for (int i = 0; i < num_vars; ++i) {
        string muName = "mu_" + to_string(i);
        mu[i] = model->addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, muName);
    }
    for (int i = 0; i < num_vars; ++i) {
        string lambdaName = "lambda_" + to_string(i);
        lambda_vars[i] = model->addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS, lambdaName);
    }

    for (int layer_index = 0; layer_index < graph_DD->structure.size(); ++layer_index) {
        int nodes_in_layer = graph_DD->structure[layer_index].size();

        w[layer_index].resize(nodes_in_layer);
        node_index = 0;

        for (auto& node :  graph_DD->structure[layer_index]) {
            nodes_to_vars[layer_index][node->get_id()] = node_index;
            string varName = "w_" + to_string(layer_index) + "_" + to_string(node_index);
            w[layer_index][node_index] = model->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS, varName);

            node_index++;
        }
    }
}

template <typename T>
void JointFlowCuts<T>::set_constraints() {
    int num_vars = graph_DD->structure.size() - 1;

    model->addConstr(w[0][0] == 1, "initial_constraint");
    model->addConstr(w[num_vars][0] == 0, "final_constraint");

    for (int l = 0; l < graph_DD->structure.size() - 1; ++l) {
        for (const auto& node : graph_DD->structure[l]) {
            for (const auto& arc : node->out_arcs) {
                int source_var_index = nodes_to_vars[l][arc->parent_node->get_id()];
                int target_var_index = nodes_to_vars[l + 1][arc->child_node->get_id()];

                if (arc->variable_value == 0) {
                    model->addConstr(w[l + 1][target_var_index] - w[l][source_var_index] + mu[l] >= 0);
                } else {
                    model->addConstr(w[l + 1][target_var_index] - w[l][source_var_index] + lambda_vars[l] >= 0);
                }
            }
        }
    }
}

template <typename T>
JointFlowCuts<T>::~JointFlowCuts() {
    graph_DD = nullptr;

    if (env != nullptr) {
        delete env;
        env = nullptr;
    }

    if (model != nullptr) {
        delete model;
        model = nullptr;
    }
}

template <typename T>
double JointFlowCuts<T>::get_time() {
    return time;
}

template <typename T>
string JointFlowCuts<T>::get_name() {
    return "Joint Flow Cuts (Dual Cuts)";
}

template <typename T>
pair<vector<double>, double> JointFlowCuts<T>::get_cut() {
    if (this->coefficients.empty()) {
        throw NoCutsFound();
    }
    return {this->coefficients, this->constant};
}

template <typename T>
bool JointFlowCuts<T>::generate_cut(vector<double>& x_values, bool verbose) {
    this->verbose = verbose;
    auto start_time = chrono::high_resolution_clock::now();
    bool find_cut = compute_cut(x_values);
    auto end_time = chrono::high_resolution_clock::now();
    time = chrono::duration<double>(end_time - start_time).count();

    if (verbose) {
        if (find_cut) {
            cout << "A cut was found in the graph." << endl;
            cout << "The cut it is: " << this->get_inequality_string_representation() << endl;
        } else {
            cout << "No cut was found in the graph." << endl;

        }
    }

    return find_cut;
}

template <typename T>
bool JointFlowCuts<T>::compute_cut(const vector<double>& x_values) {
    this->coefficients.resize(graph_DD->structure.size() - 1, 0);
    this->constant = -1;
    this->x_values = x_values;

    if (verbose) {
        cout << "\nCalculating cut...\n" << endl;
    }

    update_lp_model();
    return solve_lp_model();
}

template <typename T>
void JointFlowCuts<T>::update_lp_model() {
    set_objective();
    model->update();
}

template <typename T>
bool JointFlowCuts<T>::solve_lp_model() {
    model->set(GRB_IntParam_PreCrush, 1);
    model->set(GRB_IntParam_Threads, 1);
    model->optimize();

    for (int i = 0; i < graph_DD->structure.size() - 1; ++i) {
        this->coefficients[i] = mu[i].get(GRB_DoubleAttr_X) - lambda_vars[i].get(GRB_DoubleAttr_X);
        this->constant += mu[i].get(GRB_DoubleAttr_X);
    }

    return abs(model->get(GRB_DoubleAttr_ObjVal) - 1) >= tolerance;
}

template <typename T>
void JointFlowCuts<T>::set_objective() {
    GRBLinExpr objExpr;

    for (int i = 0; i < graph_DD->structure.size() - 1; ++i) {
        objExpr += x_values[i] * lambda_vars[i] + (1 - x_values[i]) * mu[i];
    }

    model->setObjective(objExpr, GRB_MINIMIZE);
    model->update();
}