#include "AbstractProblemGurobiClass.h"
#include <numeric>
#include <cmath>

// ============================================================
// DDCutCallback
// ============================================================

DDCutCallback::DDCutCallback(AbstractProblemGurobi& parent)
    : parent_(parent) {}

void DDCutCallback::callback() {
    if (where == GRB_CB_MIPNODE) {
        double node_count = getDoubleInfo(GRB_CB_MIPNODE_NODCNT);

        // Only inject cuts at the root node
        if (node_count == 0) {
            // Track the dual bound at the root node. The callback fires once per
            // root LP re-solve, so the last value captured here is the bound after
            // all root cuts have been added (used for the root gap).
            parent_.root_bound     = getDoubleInfo(GRB_CB_MIPNODE_OBJBND);
            parent_.root_bound_set = true;

            // Collect current LP relaxation solution
            vector<double> x_sol;
            x_sol.reserve(parent_.x.size());
            for (auto& var : parent_.x) {
                x_sol.push_back(getNodeRel(var));
            }

            for (size_t i = 0; i < parent_._cut_generate_fns.size(); ++i) {
                // Map x_sol to the DD's variable ordering
                auto [x_sol_dd, var_ids] = parent_._variable_mapping_fns[i](x_sol);

                if (parent_._cut_generate_fns[i](x_sol_dd)) {
                    auto [coefficients, constant] = parent_._cut_get_fns[i]();

                    if (parent_._use_cut_strengthening && i < parent_._cut_strengthening_fns.size()) {
                        if (parent_._cut_strengthening_fns[i](coefficients, constant)) {
                            auto [scoeff, lconst] = parent_._cut_strengthening_get_fns[i]();
                            coefficients = scoeff;
                            constant     = lconst;
                        }
                    }

                    GRBLinExpr expr = 0;
                    for (size_t j = 0; j < coefficients.size(); ++j) {
                        expr += coefficients[j] * parent_.x[var_ids[j]];
                    }
                    addCut(expr <= constant);
                    parent_.num_cuts_added++;
                }
            }
        }
    }
}

// ============================================================
// RootBoundCallback
// ============================================================

RootBoundCallback::RootBoundCallback(AbstractProblemGurobi& parent)
    : parent_(parent) {}

void RootBoundCallback::callback() {
    if (where == GRB_CB_MIPNODE) {
        if (getDoubleInfo(GRB_CB_MIPNODE_NODCNT) == 0) {
            parent_.root_bound     = getDoubleInfo(GRB_CB_MIPNODE_OBJBND);
            parent_.root_bound_set = true;
        }
    }
}

// ============================================================
// AbstractProblemGurobi
// ============================================================

AbstractProblemGurobi::AbstractProblemGurobi(int variable_length,
                                             vector<double> objective_weights,
                                             bool continuous,
                                             int min_max)
    : continuous(continuous),
      variable_length(variable_length),
      objective_weights(objective_weights),
      min_max(min_max)
{
    cuts = {};
}

AbstractProblemGurobi::~AbstractProblemGurobi() {
    x.clear();
    if (model != nullptr) {
        delete model;
        model = nullptr;
    }
    if (env != nullptr) {
        delete env;
        env = nullptr;
    }
}

void AbstractProblemGurobi::initialize_model() {
    env   = new GRBEnv();
    model = new GRBModel(*env);
    model->set(GRB_DoubleParam_TimeLimit, TIME_LIMIT_SECONDS);
    model->set(GRB_IntParam_Threads, THREADS);
}

void AbstractProblemGurobi::create_model() {
    initialize_model();
    initialize_variables();
    add_cuts();
    set_objective();
    add_constraints();
}

void AbstractProblemGurobi::optimize_model() {
    auto start = chrono::steady_clock::now();
    root_bound_set = false;
    model->update();

    RootBoundCallback cb(*this);
    model->setCallback(&cb);

    model->optimize();
    model->setCallback(nullptr);
    show_results();
    auto end = chrono::steady_clock::now();
    optimization_time = chrono::duration<double>(end - start);
}

string AbstractProblemGurobi::get_optimization_time() {
    return to_string(optimization_time.count());
}

double AbstractProblemGurobi::get_root_gap() {
    if (!root_bound_set || model == nullptr) {
        return 0.0;
    }
    try {
        double opt = model->get(GRB_DoubleAttr_ObjVal);
        if (std::abs(opt) < 1e-12) {
            return 0.0;
        }
        return std::abs(root_bound - opt) / std::abs(opt);
    } catch (const GRBException&) {
        return 0.0;
    }
}

double AbstractProblemGurobi::get_final_gap() {
    if (model == nullptr) {
        return 0.0;
    }
    try {
        return model->get(GRB_DoubleAttr_MIPGap);
    } catch (const GRBException&) {
        return 0.0;
    }
}

void AbstractProblemGurobi::add_additional_cuts(vector<pair<vector<double>, double>> new_cuts) {
    cuts.insert(cuts.end(), new_cuts.begin(), new_cuts.end());
    for (const auto& cut : new_cuts) {
        GRBLinExpr expr = 0;
        for (size_t i = 0; i < cut.first.size(); ++i) {
            expr += cut.first[i] * x[i];
        }
        model->addConstr(expr <= cut.second);
    }
}

void AbstractProblemGurobi::initialize_variables() {
    x.clear();
    for (int i = 0; i < variable_length; ++i) {
        if (continuous) {
            x.push_back(model->addVar(0.0, 1.0, 0.0, GRB_CONTINUOUS, "x_" + to_string(i + 1)));
        } else {
            x.push_back(model->addVar(0.0, 1.0, 0.0, GRB_BINARY, "x_" + to_string(i + 1)));
        }
    }
}

void AbstractProblemGurobi::add_cuts() {
    for (const auto& cut : cuts) {
        GRBLinExpr expr = 0;
        for (size_t i = 0; i < cut.first.size(); ++i) {
            expr += cut.first[i] * x[i];
        }
        model->addConstr(expr <= cut.second);
    }
}

void AbstractProblemGurobi::set_objective() {
    GRBLinExpr obj = 0;
    for (int i = 0; i < variable_length; ++i) {
        obj += objective_weights[i] * x[i];
    }
    model->setObjective(obj, min_max);
}

void AbstractProblemGurobi::show_results() {
    cout << "Objective value: " << model->get(GRB_DoubleAttr_ObjVal) << endl;
    for (size_t i = 0; i < x.size(); ++i) {
        cout << "x[" << i << "] = " << x[i].get(GRB_DoubleAttr_X) << endl;
    }
}

void AbstractProblemGurobi::add_constraints() {
    // Pure virtual — implemented by derived classes
}

pair<vector<double>, vector<int>> AbstractProblemGurobi::get_variable_values(
    const vector<double>& x_sol,
    const vector<int>& /*dd_to_original_index*/) const
{
    // Default: identity mapping — DD variables align 1-to-1 with model x variables
    vector<int> var_ids(x_sol.size());
    iota(var_ids.begin(), var_ids.end(), 0);
    return {x_sol, var_ids};
}
