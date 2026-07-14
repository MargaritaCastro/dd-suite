#include "SetCoverGurobiClass.h"

SetCoverGurobiClass::SetCoverGurobiClass(const SetCoverInstance& params, bool continuous)
        : AbstractProblemGurobi(params.variables.size(), params.objective_weights, continuous, GRB_MINIMIZE),
          restrictions_length(params.matrix_of_weight.size()),
          restrictions(params.matrix_of_weight)
          {}

void SetCoverGurobiClass::add_constraints() {
    // (1) Coverage: Σₛ A[r][s]*x[s] ≥ 1 for each element r — every element must be covered
    for (int r = 0; r < restrictions_length; ++r) {
        GRBLinExpr expr = 0;
        for (int s = 0; s < variable_length; ++s) {
            expr += restrictions[r][s] * x[s];
        }
        model->addConstr(expr >= 1, "Restriction_" + to_string(r));
    }
}

void SetCoverGurobiClass::show_results() {
    if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
        cout << "\n------------------------------" << endl;
        cout << "Minimum value of the selected subsets: " << model->get(GRB_DoubleAttr_ObjVal) << endl;
        cout << "Selected variables: " << endl;
        for (int s = 0; s < variable_length; ++s) {
            if (x[s].get(GRB_DoubleAttr_X) != 0) {
                cout << "Variable " << s + 1 << ": Fraction " << x[s].get(GRB_DoubleAttr_X) << " - Value " << objective_weights[s] << endl;
            }
        }
    } else {
        cout << "No optimal solution found." << endl;
    }
}

