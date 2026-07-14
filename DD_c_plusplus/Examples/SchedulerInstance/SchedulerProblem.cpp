#include "SchedulerProblem.h"

SchedulerProblem::SchedulerProblem(const SchedulerInstance& params)
    : AbstractProblem<SchedulerState>(params.initial_state, params.variables),
      processing_times(params.processing_times),
      weights(params.weights),
      setup_times(params.setup_times),
      n_jobs(params.n_jobs)
{}

bool SchedulerProblem::transition_function(
    const SchedulerState* prev, int /*variable_index*/, int job_id, SchedulerState* scratch_state) const
{
    if (prev->scheduled.count(job_id))
        return false;

    scratch_state->scheduled = prev->scheduled;
    scratch_state->scheduled.insert(job_id);
    scratch_state->last_job = job_id;
    return true;
}

int SchedulerProblem::scheduled_processing(const set<int>& scheduled) const {
    // Sum of processing times of the jobs already scheduled: a time-free proxy
    // for "how far along" a partial sequence is, replacing the old current_time.
    int total = 0;
    for (int j : scheduled)
        total += processing_times[j];
    return total;
}

int SchedulerProblem::get_priority_for_discard_node(const SchedulerState* state) const {
    // Restricted DD: discard nodes with the most committed processing first.
    return scheduled_processing(state->scheduled);
}

int SchedulerProblem::get_priority_for_merge_nodes(int /*node_id*/, const SchedulerState* state) const {
    // Relaxed DD: merge nodes with the most committed processing first.
    return scheduled_processing(state->scheduled);
}

SchedulerState* SchedulerProblem::merge_operator(
    const SchedulerState* s1, const SchedulerState* s2) const
{
    // Intersect scheduled sets → fewer committed jobs → more feasible paths remain
    // Keep last_job from the state with less committed processing (optimistic).
    set<int> merged_sched;
    for (int j : s1->scheduled)
        if (s2->scheduled.count(j))
            merged_sched.insert(j);

    if (scheduled_processing(s1->scheduled) <= scheduled_processing(s2->scheduled))
        return new SchedulerState{merged_sched, s1->last_job};
    else
        return new SchedulerState{merged_sched, s2->last_job};
}

string SchedulerProblem::get_state_as_string(const SchedulerState* state) const {
    return state->to_string();
}

SchedulerState* SchedulerProblem::get_state_copy(const SchedulerState* state) const {
    return new SchedulerState(*state);
}

SchedulerState* SchedulerProblem::get_final_state() {
    return new SchedulerState{{}, -1};
}
