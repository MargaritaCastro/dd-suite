#include <iostream>
#include <string>
#include <vector>

#include "SourceCode/DD.h"
#include "SourceCode/GraphAlgorithms/ShortestLongestPath/ShortestLongestPath.h"
#include "Examples/KnapsackInstance/KnapsackProblem.h"
#include "Examples/KnapsackInstance/KnapsackGurobiClass.h"
#include "SourceCode/GraphAlgorithms/MaxFlow/MaxFlowGurobi.h"
#include "SourceCode/DDCutGenerators/FlowCuts.h"
#include "SourceCode/DDCutGenerators/CutStrengthening.h"
#include "SourceCode/DDCutGenerators/JointFlowCuts.h"

using namespace std;

int main() {

    // ============================
    // Setup Knapsack Problem
    // ============================

    // Code Parameters
    bool verbose = false;
    int width = 2;

    // Input data
    int* initial_state =  new int(0);
    vector<pair<string, vector<int>>> variables = {
            {"x_1", {0, 1}},
            {"x_2", {0, 1}},
            {"x_3", {0, 1}},
            {"x_4", {0, 1}}
    };

    vector<int> weights = {7, 3, 2, 1};
    vector<int> values = {1, 1, 1, 1};
    int capacity = 8;
    string objective = "max";
    int variable_length = 4;

    // Knapsack problem
    KnapsackProblem* knapsack_instance = new KnapsackProblem(initial_state, variables, weights, capacity);

    //Create exact DD
    DD<int>* dd_instance = new DD(*knapsack_instance);
    dd_instance->create_decision_diagram(true);
    dd_instance->export_graph_file("debug_cuts");

    //Create Gurobi model and solve
    KnapsackGurobiClass* gurobiInstance = new KnapsackGurobiClass(variable_length, weights, values, capacity, true);
    gurobiInstance->create_model();
    gurobiInstance->optimize_model();

    //Print solution
    cout << "Gurobi Solution: ";
    for (int i = 0; i < gurobiInstance->x.size(); ++i) {
        double value = gurobiInstance->x[i].get(GRB_DoubleAttr_X);
        cout << value << " ";
    }
    cout << endl; // Expected {0.2857142857142857, 1.0, 1.0, 1.0}

    //Max Flow test
    MaxFlowGurobi<int>* maxFlowInstance =  new MaxFlowGurobi<int>(dd_instance);

    maxFlowInstance->solve_max_flow({0.2857142857142857, 1.0, 1.0, 1.0});
    double sol_max_flow = maxFlowInstance->get_max_flow();
    cout << "Solution Max flow = " <<  sol_max_flow << " Expected 0.7142857142857143 " << endl;
    delete maxFlowInstance;

    //Max flow 2.0
    MaxFlow<int>* maxFlowInstanceNew =  new MaxFlow<int>(dd_instance);

    maxFlowInstanceNew->solve_max_flow({0.2857142857142857, 1.0, 1.0, 1.0});
    sol_max_flow = maxFlowInstanceNew->get_max_flow();
    cout << "Solution Max flow = " <<  sol_max_flow << " Expected 0.7142857142857143 " << endl;
    delete maxFlowInstanceNew;

    //CutStrengthening
    CutStrengthening<int> cut_strengthening_instance(dd_instance, true);
    cut_strengthening_instance.cut_strengthening({0.5, 1, 1, 0}, 2);
    auto strengthened_cut = cut_strengthening_instance.get_cut_strengthening();
    const vector<double>& expected_coefficients = {1, 0, 1, 0};

    cout << "Strengthened Cut : ";
    for (size_t i = 0; i < expected_coefficients.size(); ++i) {
        cout << strengthened_cut.first[i] << " ";
    }
    cout << "<= " << strengthened_cut.second  << endl;

  }