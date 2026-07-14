#include "SOCKnapsackGurobiClass.h"

SOCKnapsackGurobiClass::SOCKnapsackGurobiClass(const SOCKnapsackInstance& params, bool continuous)
    : AbstractProblemGurobi(
          static_cast<int>(params.variables.size()),
          params.objective_weights,
          continuous,
          GRB_MAXIMIZE),
      params(params),
      num_constraints(static_cast<int>(params.right_side_of_restrictions.size()))
{}

void SOCKnapsackGurobiClass::add_constraints() {
    y_vars.resize(num_constraints);
    z_vars.resize(num_constraints);

    for (int j = 0; j < num_constraints; ++j) {
        y_vars[j].resize(variable_length);
        for (int i = 0; i < variable_length; ++i) {
            y_vars[j][i] = model->addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS,
                                         "y_" + to_string(j) + "_" + to_string(i));
        }
        z_vars[j] = model->addVar(0.0, GRB_INFINITY, 0.0, GRB_CONTINUOUS,
                                  "z_" + to_string(j));

        model->update();

        // (1) y[j][i] = sqrt_weights[j][i] * x[i] — defines each y auxiliary variable
        for (int i = 0; i < variable_length; ++i) {
            model->addConstr(
                params.sqrt_weights[j][i] * x[i] - y_vars[j][i] == 0.0,
                "y_def_" + to_string(j) + "_" + to_string(i));
        }

        // (2) omega * z[j] = rhs[j] − Σᵢ linear_weights[j][i]*x[i] — defines each z auxiliary variable
        GRBLinExpr linear_sum = 0;
        for (int i = 0; i < variable_length; ++i) {
            linear_sum += params.linear_weights[j][i] * x[i];
        }
        model->addConstr(
            params.right_side_of_restrictions[j] - linear_sum - params.omega * z_vars[j] == 0.0,
            "z_def_" + to_string(j));

        // (3) Σᵢ y[j][i]² ≤ z[j]² — second-order cone constraint per restriction j
        GRBQuadExpr soc_lhs = 0;
        for (int i = 0; i < variable_length; ++i) {
            soc_lhs += y_vars[j][i] * y_vars[j][i];
        }
        GRBQuadExpr soc_rhs = z_vars[j] * z_vars[j];
        model->addQConstr(soc_lhs - soc_rhs <= 0.0, "soc_" + to_string(j));
    }
}

pair<vector<double>, vector<int>> SOCKnapsackGurobiClass::get_variable_values(
    const vector<double>& x_sol,
    const vector<int>& dd_to_original_index) const
{
    // Override: the SOCK model has auxiliary variables (y, z) not in the DD.
    // x_sol contains only the x variables; map them to the DD's ordering.
    vector<double> x_sol_dd;
    x_sol_dd.reserve(dd_to_original_index.size());
    for (int var_id : dd_to_original_index)
        x_sol_dd.push_back(x_sol[var_id]);
    return {x_sol_dd, dd_to_original_index};
}

void SOCKnapsackGurobiClass::show_results() {
    if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
        cout << "\n------------------------------" << endl;
        cout << "Optimal total value: " << model->get(GRB_DoubleAttr_ObjVal) << endl;
        cout << "Items in optimal solution: " << endl;
        for (int i = 0; i < variable_length; ++i) {
            if (x[i].get(GRB_DoubleAttr_X) != 0.0) {
                cout << "Article " << i
                     << ", Benefit: " << params.objective_weights[i]
                     << ", Fraction: " << x[i].get(GRB_DoubleAttr_X) << endl;
            }
        }
    } else {
        cout << "No optimal solution found." << endl;
    }
}
