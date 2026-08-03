#include "SequencingGurobiClass.h"
#include <numeric>
#include <algorithm>
#include <iomanip>
#include <sstream>

SequencingGurobiClass::SequencingGurobiClass(const SequencingInstance& params, bool continuous)
    : AbstractProblemGurobi(params.n_jobs, params.objective_weights, continuous, GRB_MINIMIZE),
      n_(params.n_jobs),
      DEPOT_(params.n_jobs),
      p_(params.processing_times),
      w_(params.weights),
      s_(params.setup_times)
{}

double SequencingGurobiClass::big_M() const {
    double M = 0;
    for (int j : p_) M += j;
    for (int j = 0; j < n_; ++j) {
        int max_setup = 0;
        for (int k = 0; k <= n_; ++k)
            if (k != j + 1)
                max_setup = max(max_setup, s_[k][j]);
        M += max_setup;
    }
    return M;
}

void SequencingGurobiClass::initialize_variables() {
    double M = big_M();

    // y[{DEPOT, j}] for j in 0..n-1
    for (int j = 0; j < n_; ++j)
        y_[{DEPOT_, j}] = model->addVar(0.0, 1.0, 0.0, GRB_BINARY, "y_d_" + to_string(j));

    // y[{i, j}] for i != j
    for (int i = 0; i < n_; ++i)
        for (int j = 0; j < n_; ++j)
            if (i != j)
                y_[{i, j}] = model->addVar(0.0, 1.0, 0.0, GRB_BINARY,
                                            "y_" + to_string(i) + "_" + to_string(j));

    // C[j] continuous, bounds [0, M]
    C_.resize(n_);
    for (int j = 0; j < n_; ++j)
        C_[j] = model->addVar(0.0, M, 0.0, GRB_CONTINUOUS, "C_" + to_string(j));
}

void SequencingGurobiClass::set_objective() {
    GRBLinExpr obj = 0;
    for (int j = 0; j < n_; ++j)
        obj += w_[j] * C_[j];
    model->setObjective(obj, GRB_MINIMIZE);
}

void SequencingGurobiClass::add_constraints() {
    double M = big_M();

    // (1) Depot has exactly one outgoing arc
    GRBLinExpr depot_out = 0;
    for (int j = 0; j < n_; ++j)
        depot_out += y_[{DEPOT_, j}];
    model->addConstr(depot_out == 1, "depot_out");

    for (int j = 0; j < n_; ++j) {
        // (2) Each job has exactly one incoming arc (from depot or another job)
        GRBLinExpr in_expr = y_[{DEPOT_, j}];
        for (int i = 0; i < n_; ++i)
            if (i != j) in_expr += y_[{i, j}];
        model->addConstr(in_expr == 1, "in_" + to_string(j));

        // (3) Each job has at most one outgoing arc
        GRBLinExpr out_expr = 0;
        for (int k = 0; k < n_; ++k)
            if (k != j) out_expr += y_[{j, k}];
        model->addConstr(out_expr <= 1, "out_" + to_string(j));

        // (4) Completion-time big-M constraints from depot
        model->addConstr(
            C_[j] >= s_[0][j] + p_[j] - M * (1 - y_[{DEPOT_, j}]),
            "C_d_" + to_string(j));

        // (4) Completion-time big-M constraints from other jobs
        for (int i = 0; i < n_; ++i) {
            if (i == j) continue;
            model->addConstr(
                C_[j] >= C_[i] + s_[i + 1][j] + p_[j] - M * (1 - y_[{i, j}]),
                "C_" + to_string(i) + "_" + to_string(j));
        }
    }
}

void SequencingGurobiClass::optimize_with_cuts(
    vector<DD<SequencingState>*> /*dds*/, CutType /*cut_type*/, bool /*cut_strengthening*/)
{
    // Full callback integration requires mapping MDD slot variables to the
    // Gurobi model variables (y[(i,j)], C[j]) — left for future work.
    optimize_model();
}

void SequencingGurobiClass::show_results() {
    if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
        // Reconstruct sequence from y values
        map<int,int> nxt;
        for (int j = 0; j < n_; ++j)
            if (y_[{DEPOT_, j}].get(GRB_DoubleAttr_X) > 0.5)
                nxt[DEPOT_] = j;
        for (int i = 0; i < n_; ++i)
            for (int j = 0; j < n_; ++j)
                if (i != j && y_[{i, j}].get(GRB_DoubleAttr_X) > 0.5)
                    nxt[i] = j;

        vector<int> sequence;
        int cur = DEPOT_;
        for (int k = 0; k < n_; ++k) {
            if (!nxt.count(cur)) break;
            cur = nxt[cur];
            sequence.push_back(cur);
        }

        auto fmt = [](double v) {
            ostringstream s;
            s << fixed << setprecision(2) << v;
            return s.str();
        };

        cout << "\n------------------------------\n";
        cout << "Optimal Σ w_j*C_j: " << fmt(model->get(GRB_DoubleAttr_ObjVal)) << "\n";
        cout << "Sequence (job indices): [";
        for (int k = 0; k < (int)sequence.size(); ++k) {
            if (k) cout << ", ";
            cout << sequence[k];
        }
        cout << "]\n";
        for (int rank = 0; rank < (int)sequence.size(); ++rank) {
            int j = sequence[rank];
            cout << "  Position " << rank + 1
                 << ": job " << j
                 << "  p=" << p_[j]
                 << ", w=" << w_[j]
                 << ", C=" << fmt(C_[j].get(GRB_DoubleAttr_X)) << "\n";
        }
    } else {
        cout << "No optimal solution found.\n";
    }
}
