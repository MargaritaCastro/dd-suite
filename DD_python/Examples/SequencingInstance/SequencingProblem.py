from SourceCode.Problems.AbstractProblemClass import AbstractProblem


class SequencingProblem(AbstractProblem):
    """
    Single-machine sequencing problem: minimise Σ w_j * C_j (weighted completion time).

    State: (frozenset_of_scheduled_jobs, last_job_id)
      - frozenset_of_scheduled_jobs: jobs already assigned to a slot
      - last_job_id: index of the previous job (-1 = depot / start of sequence)

    Variables: slot_0, slot_1, ..., slot_{n-1}
    Domain: {0, 1, ..., n-1} (job indices)

    Based on: Cirè & van Hoeve (2013) — Multivalued Decision Diagrams for
    Sequencing Problems, Operations Research 61(6):1411–1428.
    """

    def __init__(self, params: 'SequencingStructure', sort: bool = False):
        super().__init__(params.initial_state, params.variables, sort)
        self.processing_times: list[int] = params.processing_times
        self.weights: list[int] = params.weights
        self.setup_times: list[list[int]] = params.setup_times  # (n+1) x n
        self.n_jobs: int = params.n_jobs

    def transition_function(self, previous_state: 'State', variable_index: int, job_id: int, scratch_state: list) -> bool:
        scheduled, last_job = previous_state

        if job_id in scheduled:
            return False

        scratch_state[0] = (scheduled | {job_id}, job_id)
        return True

    def _scheduled_processing(self, scheduled: frozenset) -> int:
        '''
        Sum of processing times of the jobs already scheduled. Used as a
        time-free proxy for "how far along" a partial sequence is, replacing the
        accumulated current_time that used to live in the state.
        '''
        return sum(self.processing_times[j] for j in scheduled)

    def get_priority_for_discard_node(self, state: 'State') -> int:
        '''
        Discard nodes with the most committed processing first (least promising).
        '''
        scheduled, _ = state
        return self._scheduled_processing(scheduled)

    def get_priority_for_merge_nodes(self, node_id: int, state: 'State') -> int:
        '''
        Merge nodes with the most committed processing first.
        '''
        scheduled, _ = state
        return self._scheduled_processing(scheduled)

    def merge_operator(self, state_one: 'State', state_two: 'State') -> 'State':
        """
        Relaxation merge (Cirè & van Hoeve §4):
          - intersect scheduled sets → fewer committed jobs → more feasible paths remain
          - keep last_job from the state with less committed processing (optimistic)
        """
        sched1, last1 = state_one
        sched2, last2 = state_two
        merged_sched = sched1 & sched2
        merged_last = last1 if self._scheduled_processing(sched1) <= self._scheduled_processing(sched2) else last2
        return (merged_sched, merged_last)

    def get_state_as_string(self, state: 'State') -> str:
        scheduled, last_job = state
        return f"{sorted(scheduled)}_{last_job}"

    def get_state_copy(self, state: 'State') -> 'State':
        copy_state = state
        return copy_state
