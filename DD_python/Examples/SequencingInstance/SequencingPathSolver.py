import time
from SourceCode.GraphAlgorithms.ShortestLongestPath.PathStructure import PathStructureSolution


class SequencingPathSolver:
    """
    DP-based shortest/longest path solver for the single-machine sequencing problem.

    Computes the exact Σ w_j * C_j objective in a SEPARABLE way, so the diagram
    no longer needs the accumulated completion time inside its state.

    Using the identity
        Σ_j w_j C_j = Σ_i d_i * R_i,
    where d_i = setup(prev, job_i) + p[job_i] is the duration added at step i and
    R_i = Σ_{k>=i} w_{π_k} is the total weight of the jobs scheduled at step i and
    later, the contribution of the arc that schedules `job_id` becomes
        (setup + p[job_id]) * (W_total - weight_of_jobs_already_scheduled).
    This depends only on the parent's last_job and scheduled set (not on time),
    yet sums to exactly Σ_j w_j C_j along any full path.

    The forward DP still tracks last_job along each path (instead of the time), so
    it stays correct even on reduced / merged diagrams, where a node's stored state
    may not match a particular path.

    The weight of the jobs already scheduled is tracked as the *set* of distinct
    jobs seen along the path (a bitmask), not as a running sum. On a relaxed
    diagram the merge operator intersects the scheduled sets, so a single path may
    schedule the same job several times; a running sum would then add that job's
    weight once per occurrence, exceed W_total, and make `remaining_weight`
    negative, which collapses the bound (values below zero were observed). Because
    the bitmask is always a subset of the n jobs, `remaining_weight` is
    non-negative by construction, and on a feasible path (a permutation) the set
    after i arcs is exactly the set of scheduled jobs, so the contribution is the
    exact R_i and the bound remains valid.

    Interface mirrors ShortestLongestPath so it can be used as a drop-in
    replacement inside create_and_solve_dd.
    """

    def __init__(self, dd: 'DD'):
        self._graph = dd.get_decision_diagram()
        self._problem = dd.problem
        self._time: float = 0
        self._solution: PathStructureSolution = PathStructureSolution()
        self._objective: str = ""

    def set_parameters(self, weights: list, objective: str) -> None:
        self._objective = objective

    def get_time(self) -> str:
        return str(self._time)

    def get_solution(self) -> PathStructureSolution:
        return self._solution

    def solve(self) -> PathStructureSolution:
        print("\nStarting the process to solve the shortest or longest path in the graph...")
        start = time.time()
        self._run_dp()
        self._time = time.time() - start
        print("Shortest/longest path algorithm completed.")
        return self._solution

    def _run_dp(self) -> None:
        cost_sign = -1 if self._objective == "max" else 1
        dp_data, dp_arcs = self._initialize_dp_tables()
        self._forward_pass(dp_data, dp_arcs, cost_sign)
        self._extract_solution(dp_data, dp_arcs)

    def _initialize_dp_tables(self):
        total_nodes = self._graph.get_node_count()
        dp_data = [None] * total_nodes  # (cost, last_job, scheduled_weight, scheduled_mask)
        dp_arcs = [None] * total_nodes
        root_node = self._graph.structure[0][0]
        dp_data[root_node.get_id()] = (0, -1, 0, 0)
        return dp_data, dp_arcs

    def _forward_pass(self, dp_data, dp_arcs, cost_sign) -> None:
        total_weight = sum(self._problem.weights)
        for layer in self._graph.structure[1:]:
            for node in layer:
                node_id = node.get_id()
                for arc in node.in_arcs:
                    parent_id = arc.parent_node.get_id()
                    if dp_data[parent_id] is None:
                        continue
                    parent_cost, parent_last, parent_weight, parent_mask = dp_data[parent_id]
                    job_id = arc.variable_value
                    setup = self._problem.setup_times[parent_last + 1][job_id]
                    duration = setup + self._problem.processing_times[job_id]
                    # Separable contribution: duration * (weight of jobs scheduled
                    # now and later) = duration * (W_total - already_scheduled).
                    remaining_weight = total_weight - parent_weight
                    arc_cost = duration * remaining_weight
                    candidate_cost = parent_cost + cost_sign * arc_cost
                    if dp_data[node_id] is None or candidate_cost < dp_data[node_id][0]:
                        # A job already present in the mask adds no weight: on a
                        # relaxed diagram the same job may be scheduled twice, and
                        # counting it twice would push remaining_weight below zero.
                        job_bit = 1 << job_id
                        new_weight = parent_weight if parent_mask & job_bit \
                            else parent_weight + self._problem.weights[job_id]
                        dp_data[node_id] = (candidate_cost, job_id, new_weight, parent_mask | job_bit)
                        dp_arcs[node_id] = arc

    def _extract_solution(self, dp_data, dp_arcs) -> None:
        terminal_id = self._graph.structure[-1][0].get_id()
        optimal_cost = dp_data[terminal_id][0]
        if self._objective == "max":
            optimal_cost = -optimal_cost
        self._solution.value = optimal_cost
        self._solution.path_arcs, self._solution.path_print = self._backtrack_optimal_path(dp_arcs, terminal_id)

    def _backtrack_optimal_path(self, dp_arcs, terminal_id):
        path_arcs = []
        path_string = ""
        current_id = terminal_id
        while dp_arcs[current_id] is not None:
            arc = dp_arcs[current_id]
            path_string = f"-> {arc.to_string()}" + path_string
            path_arcs.insert(0, arc)
            current_id = arc.parent_node.get_id()
        return path_arcs, path_string[2:]
