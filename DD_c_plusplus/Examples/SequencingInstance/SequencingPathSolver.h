#ifndef DD_OPTIMIZATION_PROGRAM_SEQUENCINGPATHSOLVER_H
#define DD_OPTIMIZATION_PROGRAM_SEQUENCINGPATHSOLVER_H

#include "DD.h"
#include "GraphAlgorithms/ShortestLongestPath/PathStructure.h"
#include "SequencingProblem.h"
#include "SequencingState.h"

#include <string>
#include <vector>
#include <limits>
#include <chrono>
#include <cstdint>

using namespace std;

/**
 * @brief DP-based shortest-path solver for the single-machine sequencing problem.
 *
 * Computes the exact Σ w_j * C_j objective in a SEPARABLE way, so the diagram
 * no longer needs the accumulated completion time inside its state.
 *
 * Using Σ_j w_j C_j = Σ_i d_i * R_i (d_i = setup + processing added at step i,
 * R_i = total weight of the jobs scheduled at step i and later), the arc that
 * schedules job_id contributes (setup + p[job_id]) * (W_total - weight_already_scheduled).
 * The forward DP tracks last_job along each path (instead of the time), so it
 * stays correct on reduced / merged diagrams.
 *
 * The weight of the jobs already scheduled is tracked as the *set* of distinct
 * jobs seen along the path (a bitmask), not as a running sum. On a relaxed diagram
 * the merge operator intersects the scheduled sets, so a single path may schedule
 * the same job several times; a running sum would then add that job's weight once
 * per occurrence, exceed W_total, and make remaining_weight negative, which
 * collapses the bound. Because the bitmask is always a subset of the n jobs,
 * remaining_weight is non-negative by construction, and on a feasible path (a
 * permutation) the set after i arcs is exactly the set of scheduled jobs, so the
 * contribution is the exact R_i and the bound remains valid. The mask is a single
 * 64-bit word, so this solver supports instances with up to 64 jobs.
 *
 * Interface mirrors ShortestLongestPath<SequencingState> so it can be used
 * as a drop-in replacement inside run_dd_code for the Sequencing problem.
 */
class SequencingPathSolver {
public:
    explicit SequencingPathSolver(DD<SequencingState>& dd);

    void set_parameters(const vector<double>& weights, const string& objective);

    PathStructureSolution<SequencingState> solve();

    PathStructureSolution<SequencingState> get_solution() const;
    string get_time() const;

private:
    struct NodeDP {
        double   cost             = numeric_limits<double>::infinity();
        int      last_job         = -1;
        int      scheduled_weight = 0;   // total weight of the DISTINCT jobs scheduled along this path
        uint64_t scheduled_mask   = 0;   // bit j set if job j was already scheduled along this path
        bool     valid            = false;
    };

    Graph<SequencingState>*        graph_;
    const SequencingProblem*       problem_;
    string                        objective_;
    PathStructureSolution<SequencingState> solution_;
    chrono::duration<double>      time_;

    void run_dp();
    void initialize_dp_tables(vector<NodeDP>& dp_data, vector<Arc<SequencingState>*>& dp_arcs);
    void forward_pass(vector<NodeDP>& dp_data, vector<Arc<SequencingState>*>& dp_arcs, double cost_sign);
    void extract_solution(const vector<NodeDP>& dp_data, const vector<Arc<SequencingState>*>& dp_arcs);
    void backtrack_optimal_path(const vector<Arc<SequencingState>*>& dp_arcs, int terminal_id);
};

#endif //DD_OPTIMIZATION_PROGRAM_SEQUENCINGPATHSOLVER_H
