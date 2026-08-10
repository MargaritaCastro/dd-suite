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
 * @brief Shortest-path solver for Sequencing, drop-in for ShortestLongestPath.
 * Σ w_j C_j is charged per arc as (setup + p[job]) * (W_total - already scheduled);
 * both terms depend on the path, so each node covers all of its paths (see NodeDP)
 * and its arcs take the extreme that keeps the bound (relaxed under, the rest over).
 */
class SequencingPathSolver {
public:
    explicit SequencingPathSolver(DD<SequencingState>& dd);

    void set_parameters(const vector<double>& weights, const string& objective);

    PathStructureSolution<SequencingState> solve();

    PathStructureSolution<SequencingState> get_solution() const;
    string get_time() const;

private:
    /// Best cost reaching a node plus the range its paths span, which prices the
    /// outgoing arcs. With a single state per node the range collapses to a point.
    struct NodeDP {
        double   cost                = numeric_limits<double>::infinity();
        uint64_t previous_jobs       = 0;   // bit 0 = depot (-1), bit j+1 = job j, as in setup_times
        uint64_t seen_jobs           = 0;   // bit j set if some path here scheduled job j
        int      scheduled_weight_hi = 0;   // largest weight already scheduled among those paths
        int      scheduled_weight_lo = 0;   // smallest weight already scheduled among those paths
        bool     valid               = false;
    };

    Graph<SequencingState>*        graph_;
    const SequencingProblem*       problem_;
    bool                          relaxed_;   // diagram built by a relaxed builder
    string                        objective_;
    PathStructureSolution<SequencingState> solution_;
    chrono::duration<double>      time_;

    void run_dp();
    void initialize_dp_tables(vector<NodeDP>& dp_data, vector<Arc<SequencingState>*>& dp_arcs);
    void forward_pass(vector<NodeDP>& dp_data, vector<Arc<SequencingState>*>& dp_arcs, double cost_sign);
    int  best_setup(uint64_t previous_jobs, int job_id, bool charge_most) const;
    NodeDP describe_node(const Node<SequencingState>* node, const vector<NodeDP>& dp_data,
                         int total_weight) const;
    void extract_solution(const vector<NodeDP>& dp_data, const vector<Arc<SequencingState>*>& dp_arcs);
    void backtrack_optimal_path(const vector<Arc<SequencingState>*>& dp_arcs, int terminal_id);
};

#endif //DD_OPTIMIZATION_PROGRAM_SEQUENCINGPATHSOLVER_H
