#include <iostream>
#include <string>
#include <vector>
#include <filesystem>

#include "SourceCode/DD.h"
#include "SourceCode/GraphAlgorithms/ShortestLongestPath/ShortestLongestPath.h"
#include "Examples/KnapsackInstance/KnapsackInstance.h"
#include "Examples/KnapsackInstance/KnapsackProblem.h"
#include "Examples/KnapsackInstance/KnapsackGurobiClass.h"

using namespace std;
namespace fs = filesystem;

int main() {

    // ============================
    // SETUP PROBLEM AND PARAMETERS
    // ============================

    bool verbose = false;
    int  width   = 3;

    // Instance file (same as Python main_knapsack.py)
    string source_directory = fs::current_path().parent_path().string();
    string file_path = source_directory + "/DataInstances/Knapsack/Custom/knapsack_instance_v5_d10_seed1.txt";

    // Load instance
    KnapsackInstance knapsack_instance(file_path);

    // Build KnapsackProblem for DD construction
    KnapsackProblem knapsack_problem(knapsack_instance);

    // ============================
    // CREATE EXACT DD
    // ============================

    DD<int> dd_exact(knapsack_problem);
    dd_exact.create_decision_diagram(verbose);
    dd_exact.reduce_decision_diagram(verbose);

    cout << "\n-- Exact DD --\n";
    cout << "\tConstruction time (sec): " << dd_exact.get_building_time()                     << "\n";
    cout << "\tReduction time   (sec): "  << dd_exact.get_reduction_time()                    << "\n";
    cout << "\tNodes: "                   << dd_exact.get_decision_diagram()->get_node_count() << "\n";
    cout << "\tArcs:  "                   << dd_exact.get_decision_diagram()->get_arc_count()  << "\n";

    // ============================
    // CREATE RELAXED DD
    // ============================

    DD<int> dd_relaxed(knapsack_problem);
    dd_relaxed.create_relax_priority_decision_diagram(width, verbose);
    dd_relaxed.reduce_decision_diagram(verbose);

    cout << "\n-- Relaxed DD (width=" << width << ") --\n";
    cout << "\tConstruction time (sec): " << dd_relaxed.get_building_time()                     << "\n";
    cout << "\tReduction time   (sec): "  << dd_relaxed.get_reduction_time()                    << "\n";
    cout << "\tNodes: "                   << dd_relaxed.get_decision_diagram()->get_node_count() << "\n";
    cout << "\tArcs:  "                   << dd_relaxed.get_decision_diagram()->get_arc_count()  << "\n";

    // ============================
    // CREATE RESTRICTED DD
    // ============================

    DD<int> dd_restricted(knapsack_problem);
    dd_restricted.create_restricted_decision_diagram(width, verbose);
    dd_restricted.reduce_decision_diagram(verbose);

    cout << "\n-- Restricted DD (width=" << width << ") --\n";
    cout << "\tConstruction time (sec): " << dd_restricted.get_building_time()                     << "\n";
    cout << "\tReduction time   (sec): "  << dd_restricted.get_reduction_time()                    << "\n";
    cout << "\tNodes: "                   << dd_restricted.get_decision_diagram()->get_node_count() << "\n";
    cout << "\tArcs:  "                   << dd_restricted.get_decision_diagram()->get_arc_count()  << "\n";

    // Export graph in .gml format
    dd_exact.export_graph_file("knapsack_exact_dd_file");
    dd_relaxed.export_graph_file("knapsack_relaxed_dd_file");
    dd_restricted.export_graph_file("knapsack_restricted_dd_file");

    // ============================
    // SHORTEST / LONGEST PATH
    // ============================

    // Optimal solution via exact DD
    ShortestLongestPath longest_path_exact(dd_exact);
    longest_path_exact.set_parameters(knapsack_instance.objective_weights, "max");
    auto answer = longest_path_exact.solve();
    cout << "\n-- Optimal solution (exact DD longest path) --\n";
    cout << "\tOptimal value:  " << answer.value      << "\n";
    cout << "\tPath:           " << answer.path_print << "\n";
    cout << "\tSolution time:  " << longest_path_exact.get_time() << " sec\n";

    // Dual (upper) bound via relaxed DD
    ShortestLongestPath longest_path_relaxed(dd_relaxed);
    longest_path_relaxed.set_parameters(knapsack_instance.objective_weights, "max");
    answer = longest_path_relaxed.solve();
    cout << "\n-- Dual bound (relaxed DD longest path) --\n";
    cout << "\tUpper bound:   " << answer.value << "\n";
    cout << "\tSolution time: " << longest_path_relaxed.get_time() << " sec\n";

    // Primal (lower) bound via restricted DD
    ShortestLongestPath longest_path_restricted(dd_restricted);
    longest_path_restricted.set_parameters(knapsack_instance.objective_weights, "max");
    answer = longest_path_restricted.solve();
    cout << "\n-- Primal bound (restricted DD longest path) --\n";
    cout << "\tLower bound:   " << answer.value << "\n";
    cout << "\tSolution time: " << longest_path_restricted.get_time() << " sec\n";

    // ============================
    // GUROBI – BASELINE
    // ============================

    cout << "\n============================\n";
    cout << "Gurobi baseline (no DD cuts)\n";
    cout << "============================\n";
    KnapsackGurobiClass gurobi_base(knapsack_instance, false);
    gurobi_base.create_model();
    gurobi_base.optimize_model();
    cout << "Optimization time: " << gurobi_base.get_optimization_time() << " sec\n";

    // ============================
    // GUROBI – DD CUTS
    // ============================

    cout << "\n============================\n";
    cout << "Gurobi with DD cuts\n";
    cout << "============================\n";
    KnapsackGurobiClass gurobi_cuts(knapsack_instance, false);
    gurobi_cuts.create_model();
    gurobi_cuts.optimize_with_cuts(vector<DD<int>*>{&dd_relaxed}, CutType::JointFlow, /*cut_strengthening=*/false);
    cout << "Optimization time: " << gurobi_cuts.get_optimization_time() << " sec\n";

    return 0;

}
