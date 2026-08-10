import time
from SourceCode.GraphAlgorithms.ShortestLongestPath.PathStructure import PathStructureSolution


class _NodeDP:
    """Best cost reaching a node plus the range its paths span, which prices the
    outgoing arcs: possible last jobs (bitmask with the +1 offset of `setup_times`),
    weight already scheduled (hi/lo) and jobs seen. One state per node -> a point."""

    __slots__ = ("cost", "previous_jobs", "scheduled_weight_hi",
                 "scheduled_weight_lo", "seen_jobs")

    def __init__(self, cost, previous_jobs, scheduled_weight_hi,
                 scheduled_weight_lo, seen_jobs):
        self.cost = cost
        self.previous_jobs = previous_jobs
        self.scheduled_weight_hi = scheduled_weight_hi
        self.scheduled_weight_lo = scheduled_weight_lo
        self.seen_jobs = seen_jobs


class SequencingPathSolver:
    """Shortest/longest path solver for Sequencing, drop-in for ShortestLongestPath.
    Σ w_j C_j is charged per arc as (setup + p[job]) * (W_total - already scheduled);
    both terms depend on the path, so each node covers all of its paths (`_NodeDP`)
    and its arcs take the extreme that keeps the bound (relaxed under, the rest over).
    """

    def __init__(self, dd: 'DD'):
        self._graph = dd.get_decision_diagram()
        self._problem = dd.problem
        self._relaxed: bool = dd.get_dd_kind() == 'relaxed'
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
        dp_data: list = [None] * total_nodes
        dp_arcs = [None] * total_nodes
        root_node = self._graph.structure[0][0]
        # Nothing is scheduled yet and the only possible previous job is the depot.
        dp_data[root_node.get_id()] = _NodeDP(0, 1 << 0, 0, 0, 0)
        return dp_data, dp_arcs

    def _best_setup(self, previous_jobs: int, job_id: int, charge_most: bool) -> int:
        """Setup for `job_id` over the possible previous jobs: the dearest one when
        charging the most, the cheapest one otherwise."""
        setup_times = self._problem.setup_times
        best = None
        remaining = previous_jobs
        while remaining:
            lowest_bit = remaining & -remaining
            candidate = setup_times[lowest_bit.bit_length() - 1][job_id]
            if best is None:
                best = candidate
            elif charge_most:
                best = max(best, candidate)
            else:
                best = min(best, candidate)
            remaining ^= lowest_bit
        return best

    def _describe_node(self, incoming, total_weight) -> _NodeDP:
        """Merge into one description everything the incoming paths can look like."""
        node_dp = _NodeDP(float("inf"), 0, 0, float("inf"), 0)
        for arc, parent in incoming:
            job_id = arc.variable_value
            job_bit = 1 << job_id
            job_weight = self._problem.weights[job_id]
            node_dp.previous_jobs |= 1 << (job_id + 1)
            node_dp.seen_jobs |= parent.seen_jobs | job_bit
            # A job already seen adds no weight, and the total is capped at W_total.
            node_dp.scheduled_weight_hi = max(
                node_dp.scheduled_weight_hi,
                min(total_weight, parent.scheduled_weight_hi + job_weight))
            node_dp.scheduled_weight_lo = min(
                node_dp.scheduled_weight_lo,
                parent.scheduled_weight_lo
                + (0 if parent.seen_jobs & job_bit else job_weight))
        return node_dp

    def _forward_pass(self, dp_data, dp_arcs, cost_sign) -> None:
        total_weight = sum(self._problem.weights)
        maximising = cost_sign < 0
        # Relaxed may not overcharge, the rest may not undercharge; max swaps it.
        charge_most = (self._relaxed == maximising)
        for layer in self._graph.structure[1:]:
            for node in layer:
                node_id = node.get_id()
                incoming = [(arc, dp_data[arc.parent_node.get_id()])
                            for arc in node.in_arcs
                            if dp_data[arc.parent_node.get_id()] is not None]
                if not incoming:
                    continue

                node_dp = self._describe_node(incoming, total_weight)

                for arc, parent in incoming:
                    job_id = arc.variable_value
                    setup = self._best_setup(parent.previous_jobs, job_id, charge_most)
                    duration = setup + self._problem.processing_times[job_id]
                    # duration * (W_total - already_scheduled), taking the end of
                    # the range that keeps the bound.
                    scheduled = parent.scheduled_weight_lo if charge_most \
                        else parent.scheduled_weight_hi
                    arc_cost = duration * (total_weight - scheduled)
                    candidate_cost = parent.cost + cost_sign * arc_cost
                    if candidate_cost < node_dp.cost:
                        node_dp.cost = candidate_cost
                        dp_arcs[node_id] = arc

                dp_data[node_id] = node_dp

    def _extract_solution(self, dp_data, dp_arcs) -> None:
        terminal_id = self._graph.structure[-1][0].get_id()
        optimal_cost = dp_data[terminal_id].cost
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
