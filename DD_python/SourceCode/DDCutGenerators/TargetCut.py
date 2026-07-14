from Exceptions.MyExceptions import NoCutsFound, GurobiModelNotInitialized
from SourceCode.DD import DD
from SourceCode.DDCutGenerators.AbstractCutGenerator import AbstractCutGenerator

import time
from gurobipy import GRB, Model, LinExpr, GurobiError


class TargetCut(AbstractCutGenerator):
    """
    Target Cut generator (MDD version) from Algorithm 1 of:
    Tjandraatmadja & van Hoeve, "Target Cuts from Relaxed Decision Diagrams",
    INFORMS Journal on Computing, 2019.

    Supports general MDDs (Section 7). Given a relaxed MDD M representing a
    set S and a fractional point x̄, solves the LP over P*_{ω,MDD}(M):

        max  u^T (x̄ - ω)
        s.t. v_j ≤ v_i - ℓ·u_k   ∀ ℓ-arc (i,j) at layer k
             v_s = 1 + u^T ω,  v_t = 0

    where ω is the geometric center of S (Appendix B).

    If the LP is bounded with optimal value > 1:
        returns the cut  u*^T x ≤ 1 + u*^T ω  (inequality).
    If the LP is unbounded along ray u*:
        returns the equality  u*^T x = u*^T ω.
    If the optimal value ≤ 1:
        x̄ ∈ conv(S), no cut is generated.

    The section 5.4 perturbation heuristic to ensure facet-defining cuts
    is intentionally NOT implemented.
    """

    def __init__(self, DD: DD, tolerance: float = 0.0001) -> None:
        super().__init__(tolerance)

        self.graph_DD = DD.get_decision_diagram()
        self.time: float = 0
        self.verbose: bool = False
        self._is_equality: bool = False
        self._num_vars: int = len(self.graph_DD.structure) - 1
        self._nodes_to_vars: list[dict] = []

        self.omega: list[float] = self._compute_interior_point()

        try:
            self.model: Model = Model("target_cut_model")
        except GurobiError as e:
            raise GurobiModelNotInitialized(
                f"Gurobi model not initialized (possible license issue): {e}"
            ) from e

        self.model.setParam('OutputFlag', 0)
        # Required to retrieve unbounded ray when LP is unbounded
        self.model.setParam(GRB.Param.InfUnbdInfo, 1)

        self._build_model()

    def _compute_interior_point(self) -> list[float]:
        """
        Computes the geometric center of S via path-counting (Appendix B).

        For each arc (i,j) let n_ij = n_i^- * n_j^+, where n_i^- is the
        number of root-to-i paths and n_j^+ is the number of j-to-terminal
        paths.  The center coordinate for layer k is:

            ω_k = (1/N) · Σ_{arcs at layer k}  ℓ · n_ij

        with N = total paths = n_minus[terminal].
        """
        total_nodes = self.graph_DD.get_node_count()
        n_minus: list[int] = [0] * total_nodes
        n_plus: list[int] = [0] * total_nodes

        # Top-down pass: paths from root to each node
        root_id = self.graph_DD.structure[0][0].get_id()
        n_minus[root_id] = 1
        for layer in self.graph_DD.structure[:-1]:
            for node in layer:
                for arc in node.out_arcs:
                    n_minus[arc.child_node.get_id()] += n_minus[node.get_id()]

        # Bottom-up pass: paths from each node to terminal
        terminal_id = self.graph_DD.structure[-1][0].get_id()
        n_plus[terminal_id] = 1
        for layer in reversed(self.graph_DD.structure[1:]):
            for node in layer:
                for arc in node.in_arcs:
                    n_plus[arc.parent_node.get_id()] += n_plus[node.get_id()]

        N = n_minus[terminal_id]
        if N == 0:
            return [0.0] * self._num_vars

        omega: list[float] = []
        for layer in self.graph_DD.structure[:-1]:
            weighted_sum = 0.0
            for node in layer:
                nm = n_minus[node.get_id()]
                for arc in node.out_arcs:
                    n_ij = nm * n_plus[arc.child_node.get_id()]
                    weighted_sum += arc.variable_value * n_ij
            omega.append(weighted_sum / N)

        return omega

    def _build_model(self) -> None:
        """
        Builds the static part of the Gurobi LP (constraints only).
        The objective is set per solve call in _compute_cut.
        """
        self._nodes_to_vars = [{} for _ in range(len(self.graph_DD.structure))]

        # u[k]: dual variable for layer k (one per decision variable, unbounded)
        self.u = self.model.addVars(
            self._num_vars, lb=-GRB.INFINITY, ub=GRB.INFINITY, name="u"
        )

        # v[layer_index][node_index]: node potential variables (unbounded)
        self.v: dict[int, 'tupledict'] = {}
        for layer_index, layer in enumerate(self.graph_DD.structure):
            for node_index, node in enumerate(layer):
                self._nodes_to_vars[layer_index][node.get_id()] = node_index
            self.v[layer_index] = self.model.addVars(
                len(layer), lb=-GRB.INFINITY, ub=GRB.INFINITY,
                name=f"v_{layer_index}"
            )

        # Arc constraints: v_j ≤ v_i - ℓ·u_k  for every ℓ-arc (i,j) at layer k
        for layer_index, layer in enumerate(self.graph_DD.structure[:-1]):
            for node in layer:
                i_idx = self._nodes_to_vars[layer_index][node.get_id()]
                for arc in node.out_arcs:
                    j_idx = self._nodes_to_vars[layer_index + 1][arc.child_node.get_id()]
                    ell = arc.variable_value
                    self.model.addConstr(
                        self.v[layer_index + 1][j_idx]
                        - self.v[layer_index][i_idx]
                        + ell * self.u[layer_index]
                        <= 0
                    )

        # Source constraint: v_s = 1 + u^T ω
        source_rhs = LinExpr()
        source_rhs += 1.0
        for k in range(self._num_vars):
            source_rhs += self.omega[k] * self.u[k]
        self.model.addConstr(self.v[0][0] == source_rhs, "source_constr")

        # Sink constraint: v_t = 0
        t_idx = self._nodes_to_vars[self._num_vars][
            self.graph_DD.structure[-1][0].get_id()
        ]
        self.model.addConstr(self.v[self._num_vars][t_idx] == 0.0, "sink_constr")

        self.model.update()

    def generate_cut(self, x_values: list[float], verbose: bool = False) -> bool:
        """
        Runs Algorithm 1 to separate x̄ from conv(S).

        Parameters:
            x_values: fractional point x̄ to separate.
            verbose:  print cut details if True.

        Returns:
            True  if a cut was found (stored via get_cut()).
            False if x̄ ∈ conv(S) and no cut can be derived.
        """
        self.verbose = verbose
        start_time = time.time()
        found = self._compute_cut(x_values)
        self.time = time.time() - start_time

        if found:
            if self.verbose:
                cut_type = "equality" if self._is_equality else "inequality"
                print(f"\n'''A target cut ({cut_type}) was found.'''")
                print(f"    {self._get_inequality_string_representation()}\n")
        else:
            if self.verbose:
                print("\n'''No target cut found (x̄ ∈ conv(S)).'''\n")
            if hasattr(self, "coefficients"):
                del self.coefficients
            if hasattr(self, "constant"):
                del self.constant

        return found

    def _compute_cut(self, x_values: list[float]) -> bool:
        self._is_equality = False

        # Objective: max Σ_k (x̄_k - ω_k) · u_k
        obj = LinExpr()
        for k in range(self._num_vars):
            obj += (x_values[k] - self.omega[k]) * self.u[k]
        self.model.setObjective(obj, GRB.MAXIMIZE)
        self.model.update()

        self.model.setParam(GRB.Param.Threads, 1)
        self.model.setParam(GRB.Param.PreCrush, 1)
        self.model.optimize()

        status = self.model.Status

        if status == GRB.OPTIMAL:
            # Cut exists iff optimal value > 1 (x̄ violates u*^T x ≤ 1 + u*^T ω)
            if self.model.ObjVal <= 1.0 + self.tolerance:
                return False
            u_vals = [self.u[k].X for k in range(self._num_vars)]
            self.coefficients = u_vals
            self.constant = 1.0 + sum(u_vals[k] * self.omega[k]
                                      for k in range(self._num_vars))
            return True

        elif status == GRB.UNBOUNDED:
            # Unbounded ray u* → equality cut u*^T x = u*^T ω
            u_vars = [self.u[k] for k in range(self._num_vars)]
            ray = self.model.getAttr(GRB.Attr.UnbdRay, u_vars)
            self.coefficients = list(ray)
            self.constant = sum(self.coefficients[k] * self.omega[k]
                                for k in range(self._num_vars))
            self._is_equality = True
            return True

        return False

    def get_cut(self) -> tuple[list[float], float]:
        """
        Returns the last generated cut as (coefficients, constant) where
        the cut is  Σ_k coefficients[k] · x_k ≤ constant.
        For equality cuts (is_equality_cut() == True) read it as ==.
        """
        if not hasattr(self, "coefficients"):
            raise NoCutsFound()
        return (self.coefficients, self.constant)

    def get_time(self) -> float:
        return self.time

    def get_name(self) -> str:
        return "Target Cut (MDD)"

    def is_equality_cut(self) -> bool:
        """True when the last generated cut is an equality (unbounded LP ray)."""
        return self._is_equality
