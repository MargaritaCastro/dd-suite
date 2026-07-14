#include <iostream>
#include <string>
#include <vector>

#include "SourceCode/DD.h"
#include "SourceCode/GraphAlgorithms/ShortestLongestPath/ShortestLongestPath.h"
#include "Examples/SOCKnapsackInstance/SOCKnapsackInstance.h"
#include "Examples/SOCKnapsackInstance/SOCKnapsackProblem.h"
#include "Examples/SOCKnapsackInstance/SOCKnapsackGurobiClass.h"
#include "Examples/GuiStructure.h"

using namespace std;

int main() {

    // ============================
    // SETUP PROBLEM AND PARAMETERS
    // ============================

    // Code parameters
    bool verbose = false;
    int width = 2;

    // Input data
    string source_directory = fs::current_path().parent_path().string();
    string file_path = source_directory +  "/DataInstances/SOCKnapsack/knapsack_n5m2o1b5_0.txt";
    //string file_path = source_directory +  "/DataInstances/SOCKnapsack/medium/knapsack_n100m10o1b5_0.txt";

    // Read SOC Knapsack instance from file
    SOCKnapsackInstance sock_instance(file_path);
    int num_constraints = sock_instance.right_side_of_restrictions.size();

    // Create one SOCKnapsackProblem per constraint
    vector<SOCKnapsackProblem*> sock_constraints(num_constraints);
    vector<vector<double>*> sock_initial_states(num_constraints);
    for (int i = 0; i < num_constraints; i++) {
        sock_initial_states[i] = new vector<double>(*sock_instance.initial_state);
        sock_constraints[i] = new SOCKnapsackProblem(
            sock_initial_states[i],
            sock_instance,
            i
        );
    }

    // ============================
    // CREATE EXACT DD PER CONSTRAINT
    // ============================

    vector<DD<vector<double>>*> dd_exact(num_constraints);
    for (int i = 0; i < num_constraints; i++) {
        dd_exact[i] = new DD<vector<double>>(*sock_constraints[i]);
        dd_exact[i]->create_decision_diagram(verbose);
        //dd_exact[i]->export_graph_file("c++_sock_dd_exact_" + to_string(i));

        cout << "-- Exact DD information (constraint " << i << ") --\n";
        cout << "\tConstruction time (sec): " << dd_exact[i]->get_building_time() << "\n";
        cout << "\tNumber Layers: " << dd_exact[i]->get_decision_diagram()->actual_layer << "\n";
        cout << "\tMax width: " << dd_exact[i]->get_decision_diagram()->get_actual_max_width() << "\n";
        cout << "\tNumber of nodes: " << dd_exact[i]->get_decision_diagram()->get_node_count() << "\n";
        cout << "\tNumber of arcs: "  << dd_exact[i]->get_decision_diagram()->get_arc_count()  << "\n";
    }

    // ============================
    // CREATE AND SOLVE GUROBI MODEL
    // ============================

    SOCKnapsackGurobiClass gurobi_sock(sock_instance, false);
    gurobi_sock.create_model();
    gurobi_sock.optimize_with_cuts(dd_exact, CutType::JointFlow, false);



    // ============================
    // DELETE POINTERS
    // ============================
    for (int i = 0; i < num_constraints; i++) {
        delete dd_exact[i];
        delete sock_constraints[i];
        delete sock_initial_states[i];
    }
}
