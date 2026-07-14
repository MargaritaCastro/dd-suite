#include "KnapsackGurobiClass.h"

KnapsackGurobiClass::KnapsackGurobiClass(const KnapsackInstance& params, bool continuous)
        : AbstractProblemGurobi(params.variables.size(), params.objective_weights, continuous),
          weights(params.weights),
          capacity(params.right_side_of_restrictions)
        {}


void KnapsackGurobiClass::add_constraints() {
    // (1) Capacity constraint: total weight of selected items must not exceed capacity
    GRBLinExpr expr = 0;
    for (int a = 0; a < variable_length; ++a) {
        expr += weights[a] * x[a];
    }
    model->addConstr(expr <= capacity, "Capacity");
}

void KnapsackGurobiClass::show_results() {
    if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
        cout << "\n------------------------------" << endl;
        cout << "Optimal total value: " << model->get(GRB_DoubleAttr_ObjVal) << endl;
        cout << "Items included in the knapsack: " << endl;
        for (int a = 0; a < variable_length; ++a) {
            if (x[a].get(GRB_DoubleAttr_X) != 0) {
                cout << "Article " << a << " - Weight: " << weights[a] << ", Benefit: " << objective_weights[a] << ", Fraction: " << x[a].get(GRB_DoubleAttr_X) << endl;
            }
        }
    } else {
        cout << "No optimal solution found." << endl;
    }
}

