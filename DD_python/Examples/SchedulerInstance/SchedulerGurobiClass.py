from gurobipy import GRB, quicksum
from Examples.AbstractGurobiProblems.AbstractProblemGurobiClass import AbstractProblemGurobi
from Examples.SchedulerInstance.SchedulerInstance import SchedulerStructure
from Examples.GuiStructure import CutType


class SchedulerGurobi(AbstractProblemGurobi):
    """
    MIP formulation for the single-machine scheduling problem minimising Σ w_j * C_j
    with sequence-dependent setup times.

    Variables:
      y[(DEPOT, j)] ∈ {0,1} : job j is the first in the sequence (DEPOT = n)
      y[(i, j)]    ∈ {0,1} : job i is immediately followed by job j  (i ≠ j)
      C[j]         ≥ 0      : completion time of job j

    Constraints:
      (1) Depot has exactly one outgoing arc.
      (2) Each job has exactly one incoming arc (from depot or another job).
      (3) Each job has at most one outgoing arc.
      (4) Big-M completion-time constraints that link C values to the chosen sequence.
          These also serve as subtour elimination.
    """

    DEPOT = None  

    def __init__(self, parameters: 'SchedulerStructure', continuous: bool = False) -> None:
        self.DEPOT = parameters.n_jobs

        super().__init__(
            variable_length=parameters.n_jobs,
            objective_weights=parameters.weights,
            continuous=continuous,
            min_max=GRB.MINIMIZE,
        )

        self.n: int = parameters.n_jobs
        self.p: list[int] = parameters.processing_times
        self.w: list[int] = parameters.weights
        self.s: list[list[int]] = parameters.setup_times  

    def _initialize_variables(self) -> None:
        '''
        Override variable initialisation (no binary x[i] per item)
        '''
        n, DEPOT = self.n, self.DEPOT
        M = self._big_M()
        self._M = M

        self.y: dict = {}
        for j in range(n):
            self.y[(DEPOT, j)] = self.model.addVar(vtype=GRB.BINARY, name=f'y_d_{j}')
        for i in range(n):
            for j in range(n):
                if i != j:
                    self.y[(i, j)] = self.model.addVar(vtype=GRB.BINARY, name=f'y_{i}_{j}')

        self.C = self.model.addVars(range(n), lb=0.0, ub=M, name='C')

    def _big_M(self) -> float:
        return sum(self.p) + sum(
            max(self.s[k][j] for k in range(self.n + 1)) for j in range(self.n)
        )

    def _set_objective(self) -> None:
        '''
        Objective: Minimise Σ w_j * C_j
        '''
        self.model.setObjective(
            quicksum(self.w[j] * self.C[j] for j in range(self.n)),
            GRB.MINIMIZE,
        )

    def _add_constraints(self) -> None:
        n, DEPOT, M = self.n, self.DEPOT, self._M

        # (1) Depot has exactly one outgoing arc
        self.model.addConstr(
            quicksum(self.y[(DEPOT, j)] for j in range(n)) == 1, "depot_out"
        )

        # (2) Each job has exactly one incoming arc
        for j in range(n):
            self.model.addConstr(
                self.y[(DEPOT, j)] + quicksum(self.y[(i, j)] for i in range(n) if i != j) == 1,
                f"in_{j}",
            )

        # (3) Each job has at most one outgoing arc
        for i in range(n):
            self.model.addConstr(
                quicksum(self.y[(i, j)] for j in range(n) if j != i) <= 1,
                f"out_{i}",
            )

        # (4) Completion-time big-M constraints
        for j in range(n):
            self.model.addConstr(
                self.C[j] >= self.s[0][j] + self.p[j] - M * (1 - self.y[(DEPOT, j)]),
                f"C_d_{j}",
            )
            for i in range(n):
                if i != j:
                    self.model.addConstr(
                        self.C[j] >= self.C[i] + self.s[i + 1][j] + self.p[j]
                        - M * (1 - self.y[(i, j)]),
                        f"C_{i}_{j}",
                    )

    # ------------------------------------------------------------------
    # FlowCuts / JointFlowCuts require a BDD; the Scheduler DD is an MDD.
    # TargetCuts (the correct cut for MDDs) is handled in GuiHandler before
    # reaching this method.  This override is a safety net for direct API use.
    # ------------------------------------------------------------------

    def optimize_with_cuts(self, dds, cut_type=None, cut_strengthening=False) -> None:
        if cut_type != CutType.TARGET:
            print(f"Note: Only TargetCuts is valid for the Scheduler (got {cut_type}). Running plain Gurobi.")
        self.optimize_model()

    def show_results(self) -> None:
        if self.model.status == GRB.OPTIMAL:
            n, DEPOT = self.n, self.DEPOT

            nxt: dict = {}
            for j in range(n):
                if self.y[(DEPOT, j)].x > 0.5:
                    nxt[DEPOT] = j
            for i in range(n):
                for j in range(n):
                    if i != j and self.y[(i, j)].x > 0.5:
                        nxt[i] = j

            sequence: list[int] = []
            cur = DEPOT
            for _ in range(n):
                if cur not in nxt:
                    break
                cur = nxt[cur]
                sequence.append(cur)

            print("\n------------------------------")
            print(f"Optimal Σ w_j*C_j: {self.model.objVal:.2f}")
            print(f"Sequence (job indices): {sequence}")
            for rank, j in enumerate(sequence):
                print(f"  Position {rank + 1}: job {j}  p={self.p[j]}, w={self.w[j]}, C={self.C[j].x:.2f}")
        else:
            print("No optimal solution found.")
