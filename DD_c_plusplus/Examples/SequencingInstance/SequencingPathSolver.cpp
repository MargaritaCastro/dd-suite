#include "SequencingPathSolver.h"
#include <iostream>
#include <algorithm>
#include <sstream>

SequencingPathSolver::SequencingPathSolver(DD<SequencingState>& dd)
    : graph_(dd.get_decision_diagram()),
      problem_(dynamic_cast<const SequencingProblem*>(&dd.problem)),
      relaxed_(dd.get_dd_kind() == DDKind::Relaxed),
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
    root_dp.cost                = 0.0;
    // Nothing is scheduled yet and the only possible previous job is the depot.
    root_dp.previous_jobs       = uint64_t{1} << 0;
    root_dp.seen_jobs           = 0;
    root_dp.scheduled_weight_hi = 0;
    root_dp.scheduled_weight_lo = 0;
    root_dp.valid               = true;
}

int SequencingPathSolver::best_setup(uint64_t previous_jobs, int job_id, bool charge_most) const {
    // Dearest setup among the possible previous jobs when charging the most, the
    // cheapest one otherwise.
    int best = -1;
    uint64_t remaining = previous_jobs;
    while (remaining) {
        int index = __builtin_ctzll(remaining);
        int candidate = problem_->setup_times[index][job_id];
        if (best < 0)          best = candidate;
        else if (charge_most)  best = max(best, candidate);
        else                   best = min(best, candidate);
        remaining &= remaining - 1;
    }
    return best;
}

SequencingPathSolver::NodeDP SequencingPathSolver::describe_node(
    const Node<SequencingState>* node, const vector<NodeDP>& dp_data, int total_weight) const
{
    // Merge into one description everything the incoming paths can look like.
    NodeDP current;
    current.scheduled_weight_lo = numeric_limits<int>::max();
    for (Arc<SequencingState>* arc : node->in_arcs) {
        const NodeDP& parent = dp_data[arc->parent_node->get_id()];
        if (!parent.valid) continue;

        const int job_id = arc->variable_value;
        const uint64_t job_bit = uint64_t{1} << job_id;
        const int job_weight = problem_->weights[job_id];

        current.valid          = true;
        current.previous_jobs |= uint64_t{1} << (job_id + 1);
        current.seen_jobs     |= parent.seen_jobs | job_bit;
        // A job already seen adds no weight, and the total is capped at W_total.
        current.scheduled_weight_hi = max(current.scheduled_weight_hi,
                                          min(total_weight, parent.scheduled_weight_hi + job_weight));
        current.scheduled_weight_lo = min(current.scheduled_weight_lo,
                                          parent.scheduled_weight_lo
                                              + ((parent.seen_jobs & job_bit) ? 0 : job_weight));
    }
    return current;
}

void SequencingPathSolver::forward_pass(
    vector<NodeDP>& dp_data, vector<Arc<SequencingState>*>& dp_arcs, double cost_sign)
{
    int total_weight = 0;
    for (int w : problem_->weights) total_weight += w;

    const bool maximising = cost_sign < 0;
    // Relaxed may not overcharge, the rest may not undercharge; max swaps it.
    const bool charge_most = (relaxed_ == maximising);

    for (size_t layer_idx = 1; layer_idx < graph_->structure.size(); ++layer_idx) {
        for (Node<SequencingState>* node : graph_->structure[layer_idx]) {
            int node_id = node->get_id();
            NodeDP current = describe_node(node, dp_data, total_weight);
            if (!current.valid) continue;

            for (Arc<SequencingState>* arc : node->in_arcs) {
                const NodeDP& parent = dp_data[arc->parent_node->get_id()];
                if (!parent.valid) continue;

                int job_id   = arc->variable_value;
                int setup    = best_setup(parent.previous_jobs, job_id, charge_most);
                int duration = setup + problem_->processing_times[job_id];
                // duration * (W_total - already_scheduled), taking the end of the
                // range that keeps the bound.
                int scheduled = charge_most ? parent.scheduled_weight_lo
                                            : parent.scheduled_weight_hi;
                double arc_cost = static_cast<double>(duration) * (total_weight - scheduled);
                double candidate_cost = parent.cost + cost_sign * arc_cost;

                if (candidate_cost < current.cost) {
                    current.cost     = candidate_cost;
                    dp_arcs[node_id] = arc;
                }
            }

            dp_data[node_id] = current;
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
