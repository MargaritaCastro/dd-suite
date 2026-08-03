#ifndef DD_OPTIMIZATION_PROGRAM_SEQUENCINGPROBLEM_H
#define DD_OPTIMIZATION_PROGRAM_SEQUENCINGPROBLEM_H

#include "AbstractProblemClass.h"
#include "SequencingInstance.h"
#include "SequencingState.h"

#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <limits>

using namespace std;

/**
 * @brief Single-machine sequencing problem: minimise Σ w_j * C_j.
 *
 * State: SequencingState { scheduled, last_job }
 *   - scheduled:    set of jobs already assigned to a slot
 *   - last_job:     index of the previous job (-1 = depot / start of sequence)

 * Variables: slot_0, slot_1, ..., slot_{n-1}
 * Domain:    {0, 1, ..., n-1}  (job indices)
 *
 * Based on: Cirè & van Hoeve (2013) — Multivalued Decision Diagrams for
 * Sequencing Problems, Operations Research 61(6):1411–1428.
 */
class SequencingProblem : public AbstractProblem<SequencingState> {
public:
    explicit SequencingProblem(const SequencingInstance& params);

    bool transition_function(
        const SequencingState* previous_state,
        int variable_index,
        int job_id,
        SequencingState* scratch_state) const override;

    int get_priority_for_discard_node(const SequencingState* state) const override;
    int get_priority_for_merge_nodes(int node_id, const SequencingState* state) const override;

    SequencingState* merge_operator(
        const SequencingState* state_one,
        const SequencingState* state_two) const override;

    string get_state_as_string(const SequencingState* state) const override;
    SequencingState* get_state_copy(const SequencingState* state) const override;
    SequencingState* get_final_state() override;

    // Sum of processing times of the scheduled jobs (time-free progress proxy).
    int scheduled_processing(const set<int>& scheduled) const;

    // Problem data (public so SequencingPathSolver can read them)
    vector<int>         processing_times;
    vector<int>         weights;
    vector<vector<int>> setup_times;   // (n+1) x n
    int                 n_jobs;
};

#endif //DD_OPTIMIZATION_PROGRAM_SEQUENCINGPROBLEM_H
