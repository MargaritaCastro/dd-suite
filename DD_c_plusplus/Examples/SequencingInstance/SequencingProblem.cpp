#include "SequencingProblem.h"

SequencingProblem::SequencingProblem(const SequencingInstance& params)
    : AbstractProblem<SequencingState>(params.initial_state, params.variables),
      processing_times(params.processing_times),
      weights(params.weights),
      setup_times(params.setup_times),
      n_jobs(params.n_jobs)
{}

bool SequencingProblem::transition_function(
    const SequencingState* prev, int /*variable_index*/, int job_id, SequencingState* scratch_state) const
{
    if (prev->scheduled.count(job_id))
        return false;

    scratch_state->scheduled = prev->scheduled;
    scratch_state->scheduled.insert(job_id);
    scratch_state->last_job = job_id;
    return true;
}

int SequencingProblem::scheduled_processing(const set<int>& scheduled) const {
    // Sum of processing times of the jobs already scheduled: a time-free proxy
    // for "how far along" a partial sequence is, replacing the old current_time.
    int total = 0;
    for (int j : scheduled)
        total += processing_times[j];
    return total;
}

int SequencingProblem::get_priority_for_discard_node(const SequencingState* state) const {
    // Restricted DD: discard nodes with the most committed processing first.
    return scheduled_processing(state->scheduled);
}

int SequencingProblem::get_priority_for_merge_nodes(int /*node_id*/, const SequencingState* state) const {
    // Relaxed DD: merge nodes with the most committed processing first.
    return scheduled_processing(state->scheduled);
}

SequencingState* SequencingProblem::merge_operator(
    const SequencingState* s1, const SequencingState* s2) const
{
    // Intersect scheduled sets → fewer committed jobs → more feasible paths remain
    // Keep last_job from the state with less committed processing (optimistic).
    set<int> merged_sched;
    for (int j : s1->scheduled)
        if (s2->scheduled.count(j))
            merged_sched.insert(j);

    if (scheduled_processing(s1->scheduled) <= scheduled_processing(s2->scheduled))
        return new SequencingState{merged_sched, s1->last_job};
    else
        return new SequencingState{merged_sched, s2->last_job};
}

string SequencingProblem::get_state_as_string(const SequencingState* state) const {
    return state->to_string();
}

SequencingState* SequencingProblem::get_state_copy(const SequencingState* state) const {
    return new SequencingState(*state);
}

SequencingState* SequencingProblem::get_final_state() {
    return new SequencingState{{}, -1};
}
