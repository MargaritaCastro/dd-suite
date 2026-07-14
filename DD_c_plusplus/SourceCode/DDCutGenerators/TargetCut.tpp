#include "TargetCut.h"

template <typename T>
TargetCut<T>::TargetCut(DD<T>* dd, double tolerance): time_(0), verbose_(false), is_equality_(false)
{
    this->tolerance = tolerance;
    graph_DD = dd->get_decision_diagram();
    num_vars_ = static_cast<int>(graph_DD->structure.size()) - 1;
    nodes_to_vars_.resize(graph_DD->structure.size());

    omega = _compute_interior_point();

    env_   = new GRBEnv();
    model_ = new GRBModel(*env_);
    model_->set(GRB_IntParam_OutputFlag, 0);
    model_->set(GRB_IntParam_InfUnbdInfo, 1);

    _build_model();
}

template <typename T>
TargetCut<T>::~TargetCut() {
    graph_DD = nullptr;
    if (model_ != nullptr) { delete model_; model_ = nullptr; }
    if (env_   != nullptr) { delete env_;   env_   = nullptr; }
}

template <typename T>
vector<double> TargetCut<T>::_compute_interior_point() {
    const int total_nodes = graph_DD->get_node_count();
    vector<long long> n_minus(total_nodes, 0LL), n_plus(total_nodes, 0LL);

    // Top-down: paths from root to each node
    n_minus[graph_DD->structure[0].front()->get_id()] = 1;
    for (int l = 0; l < num_vars_; ++l)
        for (auto* node : graph_DD->structure[l])
            for (auto* arc : node->out_arcs)
                n_minus[arc->child_node->get_id()] += n_minus[node->get_id()];

    // Bottom-up: paths from each node to terminal
    n_plus[graph_DD->structure.back().front()->get_id()] = 1;
    for (int l = num_vars_; l >= 1; --l)
        for (auto* node : graph_DD->structure[l])
            for (auto* arc : node->in_arcs)
                n_plus[arc->parent_node->get_id()] += n_plus[node->get_id()];

    long long N = n_minus[graph_DD->structure.back().front()->get_id()];
    if (N == 0) return vector<double>(num_vars_, 0.0);

    vector<double> result;
    result.reserve(num_vars_);
    for (int l = 0; l < num_vars_; ++l) {
        double weighted_sum = 0.0;
        for (auto* node : graph_DD->structure[l]) {
            long long nm = n_minus[node->get_id()];
            for (auto* arc : node->out_arcs) {
                long long n_ij = nm * n_plus[arc->child_node->get_id()];
                weighted_sum += arc->variable_value * n_ij;
            }
        }
        result.push_back(weighted_sum / static_cast<double>(N));
    }
    return result;
}

template <typename T>
void TargetCut<T>::_build_model() {
    // Assign layer-local index to each node
    for (int l = 0; l < (int)graph_DD->structure.size(); ++l) {
        int idx = 0;
        for (auto* node : graph_DD->structure[l])
            nodes_to_vars_[l][node->get_id()] = idx++;
    }

    // u[k]: unbounded dual variable for layer k
    u_.resize(num_vars_);
    for (int k = 0; k < num_vars_; ++k)
        u_[k] = model_->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,
                               "u_" + to_string(k));

    // v[l][idx]: unbounded potential variable for each node
    v_.resize(graph_DD->structure.size());
    for (int l = 0; l < (int)graph_DD->structure.size(); ++l) {
        int n = static_cast<int>(graph_DD->structure[l].size());
        v_[l].resize(n);
        for (int i = 0; i < n; ++i)
            v_[l][i] = model_->addVar(-GRB_INFINITY, GRB_INFINITY, 0.0, GRB_CONTINUOUS,
                                      "v_" + to_string(l) + "_" + to_string(i));
    }

    // Arc constraints: v[l+1][j] - v[l][i] + ell * u[l] <= 0
    for (int l = 0; l < num_vars_; ++l)
        for (auto* node : graph_DD->structure[l]) {
            int i_idx = nodes_to_vars_[l][node->get_id()];
            for (auto* arc : node->out_arcs) {
                int j_idx = nodes_to_vars_[l + 1][arc->child_node->get_id()];
                double ell = static_cast<double>(arc->variable_value);
                model_->addConstr(v_[l + 1][j_idx] - v_[l][i_idx] + ell * u_[l] <= 0.0);
            }
        }

    // Source constraint: v[0][0] == 1 + sum(omega[k] * u[k])
    GRBLinExpr source_rhs = 1.0;
    for (int k = 0; k < num_vars_; ++k)
        source_rhs += omega[k] * u_[k];
    model_->addConstr(v_[0][0] == source_rhs, "source_constr");

    // Sink constraint: v[n][terminal_idx] == 0
    int t_idx = nodes_to_vars_[num_vars_][graph_DD->structure.back().front()->get_id()];
    model_->addConstr(v_[num_vars_][t_idx] == 0.0, "sink_constr");

    model_->update();
}

template <typename T>
bool TargetCut<T>::generate_cut(vector<double>& x_values, bool verbose) {
    verbose_ = verbose;
    auto start = chrono::high_resolution_clock::now();
    bool found = _compute_cut(x_values);
    auto end   = chrono::high_resolution_clock::now();
    time_ = chrono::duration<double>(end - start).count();

    if (!found) {
        this->coefficients.clear();
        if (verbose_)
            cout << "\n'''No target cut found (x̄ ∈ conv(S)).'''\n\n";
    } else if (verbose_) {
        string cut_kind = is_equality_ ? "equality" : "inequality";
        cout << "\n'''A target cut (" << cut_kind << ") was found.'''\n";
        cout << "    " << this->get_inequality_string_representation() << "\n\n";
    }
    return found;
}

template <typename T>
bool TargetCut<T>::_compute_cut(const vector<double>& x_values) {
    is_equality_ = false;

    // Objective: max sum((x_bar[k] - omega[k]) * u[k])
    GRBLinExpr obj = 0.0;
    for (int k = 0; k < num_vars_; ++k)
        obj += (x_values[k] - omega[k]) * u_[k];

    model_->setObjective(obj, GRB_MAXIMIZE);
    model_->set(GRB_IntParam_Threads, 1);
    model_->set(GRB_IntParam_PreCrush, 1);
    model_->update();
    model_->optimize();

    int status = model_->get(GRB_IntAttr_Status);

    if (status == GRB_OPTIMAL) {
        double obj_val = model_->get(GRB_DoubleAttr_ObjVal);
        if (obj_val <= 1.0 + this->tolerance) return false;

        this->coefficients.resize(num_vars_);
        for (int k = 0; k < num_vars_; ++k)
            this->coefficients[k] = u_[k].get(GRB_DoubleAttr_X);

        this->constant = 1.0;
        for (int k = 0; k < num_vars_; ++k)
            this->constant += this->coefficients[k] * omega[k];
        return true;
    }

    if (status == GRB_UNBOUNDED) {
        this->coefficients.resize(num_vars_);
        for (int k = 0; k < num_vars_; ++k)
            this->coefficients[k] = u_[k].get(GRB_DoubleAttr_UnbdRay);

        this->constant = 0.0;
        for (int k = 0; k < num_vars_; ++k)
            this->constant += this->coefficients[k] * omega[k];
        is_equality_ = true;
        return true;
    }

    return false;
}

template <typename T>
pair<vector<double>, double> TargetCut<T>::get_cut() {
    if (this->coefficients.empty()) throw NoCutsFound();
    return {this->coefficients, this->constant};
}

template <typename T>
double TargetCut<T>::get_time() { return time_; }

template <typename T>
string TargetCut<T>::get_name() { return "Target Cut (MDD)"; }

template <typename T>
bool TargetCut<T>::is_equality_cut() const { return is_equality_; }
