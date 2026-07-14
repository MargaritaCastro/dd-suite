#ifndef DD_OPTIMIZATION_PROGRAM_SCHEDULERPATHSOLVER_H
#define DD_OPTIMIZATION_PROGRAM_SCHEDULERPATHSOLVER_H

#include "DD.h"
#include "GraphAlgorithms/ShortestLongestPath/PathStructure.h"
#include "SchedulerProblem.h"
#include "SchedulerState.h"

#include <string>
#include <vector>
#include <limits>
#include <chrono>

using namespace std;

/**
 * @brief DP-based shortest-path solver for the single-machine scheduling problem.
 *
 * Computes the exact Σ w_j * C_j objective in a SEPARABLE way, so the diagram
 * no longer needs the accumulated completion time inside its state.
 *
 * Using Σ_j w_j C_j = Σ_i d_i * R_i (d_i = setup + processing added at step i,
 * R_i = total weight of the jobs scheduled at step i and later), the arc that
 * schedules job_id contributes (setup + p[job_id]) * (W_total - weight_already_scheduled).
 * The forward DP tracks last_job and the accumulated scheduled weight along each
 * path (instead of the time), so it stays correct on reduced / merged diagrams.
 *
 * Interface mirrors ShortestLongestPath<SchedulerState> so it can be used
 * as a drop-in replacement inside run_dd_code for the Scheduler problem.
 */
class SchedulerPathSolver {
public:
    explicit SchedulerPathSolver(DD<SchedulerState>& dd);

    void set_parameters(const vector<double>& weights, const string& objective);

    PathStructureSolution<SchedulerState> solve();

    PathStructureSolution<SchedulerState> get_solution() const;
    string get_time() const;

private:
    struct NodeDP {
        double cost             = numeric_limits<double>::infinity();
        int    last_job         = -1;
        int    scheduled_weight = 0;   // total weight of jobs scheduled along this path
        bool   valid            = false;
    };

    Graph<SchedulerState>*        graph_;
    const SchedulerProblem*       problem_;
    string                        objective_;
    PathStructureSolution<SchedulerState> solution_;
    chrono::duration<double>      time_;

    void run_dp();
    void initialize_dp_tables(vector<NodeDP>& dp_data, vector<Arc<SchedulerState>*>& dp_arcs);
    void forward_pass(vector<NodeDP>& dp_data, vector<Arc<SchedulerState>*>& dp_arcs, double cost_sign);
    void extract_solution(const vector<NodeDP>& dp_data, const vector<Arc<SchedulerState>*>& dp_arcs);
    void backtrack_optimal_path(const vector<Arc<SchedulerState>*>& dp_arcs, int terminal_id);
};

#endif //DD_OPTIMIZATION_PROGRAM_SCHEDULERPATHSOLVER_H
