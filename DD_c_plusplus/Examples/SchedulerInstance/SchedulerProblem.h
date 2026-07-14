#ifndef DD_OPTIMIZATION_PROGRAM_SCHEDULERPROBLEM_H
#define DD_OPTIMIZATION_PROGRAM_SCHEDULERPROBLEM_H

#include "AbstractProblemClass.h"
#include "SchedulerInstance.h"
#include "SchedulerState.h"

#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <limits>

using namespace std;

/**
 * @brief Single-machine scheduling problem: minimise Σ w_j * C_j.
 *
 * State: SchedulerState { scheduled, last_job }
 *   - scheduled:    set of jobs already assigned to a slot
 *   - last_job:     index of the previous job (-1 = depot / start of sequence)

 * Variables: slot_0, slot_1, ..., slot_{n-1}
 * Domain:    {0, 1, ..., n-1}  (job indices)
 *
 * Based on: Cirè & van Hoeve (2013) — Multivalued Decision Diagrams for
 * Sequencing Problems, Operations Research 61(6):1411–1428.
 */
class SchedulerProblem : public AbstractProblem<SchedulerState> {
public:
    explicit SchedulerProblem(const SchedulerInstance& params);

    bool transition_function(
        const SchedulerState* previous_state,
        int variable_index,
        int job_id,
        SchedulerState* scratch_state) const override;

    int get_priority_for_discard_node(const SchedulerState* state) const override;
    int get_priority_for_merge_nodes(int node_id, const SchedulerState* state) const override;

    SchedulerState* merge_operator(
        const SchedulerState* state_one,
        const SchedulerState* state_two) const override;

    string get_state_as_string(const SchedulerState* state) const override;
    SchedulerState* get_state_copy(const SchedulerState* state) const override;
    SchedulerState* get_final_state() override;

    // Sum of processing times of the scheduled jobs (time-free progress proxy).
    int scheduled_processing(const set<int>& scheduled) const;

    // Problem data (public so SchedulerPathSolver can read them)
    vector<int>         processing_times;
    vector<int>         weights;
    vector<vector<int>> setup_times;   // (n+1) x n
    int                 n_jobs;
};

#endif //DD_OPTIMIZATION_PROGRAM_SCHEDULERPROBLEM_H
