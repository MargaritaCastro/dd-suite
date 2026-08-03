#include "SequencingPathSolver.h"
#include <iostream>
#include <algorithm>
#include <sstream>

SequencingPathSolver::SequencingPathSolver(DD<SequencingState>& dd)
    : graph_(dd.get_decision_diagram()),
      problem_(dynamic_cast<const SequencingProblem*>(&dd.problem)),
      time_(chrono::duration<double>(0))
{}

void SequencingPathSolver::set_parameters(const vector<double>& /*weights*/, const string& objective) {
    objective_ = objective;
}

PathStructureSolution<SequencingState> SequencingPathSolver::solve() {
    cout << "\nStarting the process to solve the shortest or longest path in the graph...\n";
    auto start = chrono::steady_clock::now();
    run_dp();
    time_ = chrono::steady_clock::now() - start;
    cout << "Shortest/longest path algorithm completed.\n";
    return solution_;
}

PathStructureSolution<SequencingState> SequencingPathSolver::get_solution() const {
    return solution_;
}

string SequencingPathSolver::get_time() const {
    return to_string(time_.count());
}

void SequencingPathSolver::run_dp() {
    double cost_sign = (objective_ == "max") ? -1.0 : 1.0;
    vector<NodeDP> dp_data;
    vector<Arc<SequencingState>*> dp_arcs;
    initialize_dp_tables(dp_data, dp_arcs);
    forward_pass(dp_data, dp_arcs, cost_sign);
    extract_solution(dp_data, dp_arcs);
}

void SequencingPathSolver::initialize_dp_tables(
    vector<NodeDP>& dp_data, vector<Arc<SequencingState>*>& dp_arcs)
{
    int total_nodes = graph_->get_node_count();
    dp_data.resize(total_nodes);
    dp_arcs.assign(total_nodes, nullptr);
    Node<SequencingState>* root_node = graph_->structure.front().front();
    NodeDP& root_dp = dp_data[root_node->get_id()];
    root_dp.cost             = 0.0;
    root_dp.last_job         = -1;
    root_dp.scheduled_weight = 0;
    root_dp.scheduled_mask   = 0;
    root_dp.valid            = true;
}

void SequencingPathSolver::forward_pass(
    vector<NodeDP>& dp_data, vector<Arc<SequencingState>*>& dp_arcs, double cost_sign)
{
    int total_weight = 0;
    for (int w : problem_->weights) total_weight += w;

    for (size_t layer_idx = 1; layer_idx < graph_->structure.size(); ++layer_idx) {
        for (Node<SequencingState>* node : graph_->structure[layer_idx]) {
            int node_id = node->get_id();
            for (Arc<SequencingState>* arc : node->in_arcs) {
                int parent_id = arc->parent_node->get_id();
                const NodeDP& parent = dp_data[parent_id];
                if (!parent.valid) continue;

                int job_id   = arc->variable_value;
                int setup    = problem_->setup_times[parent.last_job + 1][job_id];
                int duration = setup + problem_->processing_times[job_id];
                // Separable contribution: duration * (weight of jobs scheduled now
                // and later) = duration * (W_total - already_scheduled).
                int remaining_weight = total_weight - parent.scheduled_weight;
                double arc_cost = static_cast<double>(duration) * remaining_weight;
                double candidate_cost = parent.cost + cost_sign * arc_cost;

                NodeDP& current = dp_data[node_id];
                if (!current.valid || candidate_cost < current.cost) {
                    const uint64_t job_bit = uint64_t{1} << job_id;
                    const int parent_weight = parent.scheduled_weight;
                    const uint64_t parent_mask = parent.scheduled_mask;
                    current.cost             = candidate_cost;
                    current.last_job         = job_id;
                    current.scheduled_weight = (parent_mask & job_bit)
                                                   ? parent_weight
                                                   : parent_weight + problem_->weights[job_id];
                    current.scheduled_mask   = parent_mask | job_bit;
                    current.valid            = true;
                    dp_arcs[node_id] = arc;
                }
            }
        }
    }
}

void SequencingPathSolver::extract_solution(
    const vector<NodeDP>& dp_data, const vector<Arc<SequencingState>*>& dp_arcs)
{
    Node<SequencingState>* terminal_node = graph_->structure.back().front();
    int terminal_id = terminal_node->get_id();
    double optimal_cost = dp_data[terminal_id].cost;
    if (objective_ == "max") optimal_cost = -optimal_cost;
    solution_.value = optimal_cost;
    backtrack_optimal_path(dp_arcs, terminal_id);
}

void SequencingPathSolver::backtrack_optimal_path(
    const vector<Arc<SequencingState>*>& dp_arcs, int terminal_id)
{
    solution_.path_arcs.clear();
    string path_string;
    int current_id = terminal_id;
    while (dp_arcs[current_id] != nullptr) {
        Arc<SequencingState>* arc = dp_arcs[current_id];
        path_string = "-> " + arc->to_string() + path_string;
        solution_.path_arcs.push_front(arc);
        current_id = arc->parent_node->get_id();
    }
    if (path_string.size() >= 2)
        path_string = path_string.substr(2);
    solution_.path_print = path_string;
}
