#include "IndependentSetGurobiClass.h"


IndependentSetGurobiClass::IndependentSetGurobiClass(const IndependentSetInstance& params, bool continuous)
        : AbstractProblemGurobi(params.variables.size(), params.objective_weights, continuous),
          edges(params.edges)
        {}


void IndependentSetGurobiClass::add_constraints() {
    // (1) Edge exclusion: x[i] + x[j] ≤ 1 for each edge (i,j) — adjacent vertices cannot both be selected
    for (const auto& edge : edges) {
        model->addConstr(x[edge.first - 1] + x[edge.second - 1] <= 1, "edge_" + to_string(edge.first) + "_" + to_string(edge.second));
    }
}


void IndependentSetGurobiClass::show_results() {
    if (model->get(GRB_IntAttr_Status) == GRB_OPTIMAL) {
        cout << "\n------------------------------" << endl;
        cout << "Maximum size of the independent set: " << model->get(GRB_DoubleAttr_ObjVal) << endl;
        cout << "Vertices in the independent set: " << endl;
        for (int i = 0; i < variable_length; ++i) {
            if (x[i].get(GRB_DoubleAttr_X) != 0) {
                cout << "Vertex " << i + 1 << ": fraction " << x[i].get(GRB_DoubleAttr_X) << endl;
            }
        }
    } else {
        cout << "No optimal solution found." << endl;
    }
}
