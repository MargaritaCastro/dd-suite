import os
import sys
import time
from abc import ABC, abstractmethod

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
root_dir = os.path.abspath(os.path.join(parent_dir, os.pardir))
sys.path.append(root_dir)

from gurobipy import Model, GRB, quicksum, GurobiError

from Exceptions.MyExceptions import GurobiModelNotInitialized
from Examples.GuiStructure import CutType
from SourceCode.DDCutGenerators.FlowCuts import FlowCuts
from SourceCode.DDCutGenerators.JointFlowCuts import JointFlowCuts
from SourceCode.DDCutGenerators.CutStrengthening import CutStrengthening
from SourceCode.DDCutGenerators.TargetCut import TargetCut

_CUT_GENERATOR_MAP = {
    CutType.FLOW:       FlowCuts,
    CutType.JOINT_FLOW: JointFlowCuts,
    CutType.TARGET:     TargetCut,
}

class AbstractProblemGurobi(ABC):
    ## One-hour solver time limit, matching Castro, Cire & Beck (arXiv:2003.06363).
    ## Needed for the final gap to be meaningful: without it every instance solves
    ## to optimality and MIPGap is always 0.
    TIME_LIMIT_SECONDS: int = 3600

    ## Single-threaded so the branch-and-cut search is reproducible: Gurobi's
    ## parallel determinism depends on a fixed thread count, and a wall-clock
    ## TimeLimit under shared-cluster load makes multi-threaded runs flip
    ## borderline instances between solved/unsolved.
    THREADS: int = 1

    def __init__(self, variable_length: int, objective_weights: list[int], continuous: bool=False, min_max:int=GRB.MAXIMIZE):
        """
        Constructor for the AbstractProblem class.
        """
        self.cuts: list[tuple[list[float], float]] = []
        self.continuous: bool = continuous
        self.variable_length: int = variable_length
        self.objective_weights: list[int] = objective_weights
        self.min_max: int = min_max
        self._optimization_time: float = 0

        ## Dual bound at the root node (after cuts), used to compute the root gap
        self.root_bound: float = None

        self.model: 'gurobipy.Model' = None

        ## DD cut generation attributes
        self.dds: list = []
        self.cut_generators: list = []
        self.cut_strengthener: list = []
        self.cut_strengthening: bool = False
        self.num_cuts_added: int = 0
    
    def create_model(self) -> None:
        try:
            self._initialize_model()
            self._initialize_variables()
            self.add_cuts()
            self._set_objective()
            self._add_constraints()
        except GurobiError as e:
            print("Error code " + str(e.errno) + ": " + str(e))

        except AttributeError as e:
            print("Encountered an attribute error in abstract:", e)


    def _initialize_model(self) -> None:
        '''
        Initialize the model.
        '''
        try:
            self.model = Model("GurobiProblem")
        except GurobiError as e:
            self.model = None
            raise GurobiModelNotInitialized(
                f"Gurobi model not initialized (possible license issue): {e}"
            ) from e

        if self.model is None:
            raise GurobiModelNotInitialized(
                "Gurobi model not initialized (possible license issue)"
            )

        self.model.setParam('TimeLimit', self.TIME_LIMIT_SECONDS)
        self.model.setParam('Threads', self.THREADS)

        self.set_variable_length: 'range' = range(self.variable_length)

    def _initialize_variables(self) -> None:
        ''' Initialize the decision variables for the model. '''
        if not self.continuous:
            self.x = self.model.addVars(self.set_variable_length, vtype=GRB.BINARY, name='x')
        else:
            self.x = self.model.addVars(self.set_variable_length, vtype=GRB.CONTINUOUS, lb=0, ub=1, name="x")

    def _set_objective(self):
        ''' 
        Set the objective function of the model.
        '''
        self.model.setObjective(quicksum(self.objective_weights[i]*self.x[i] for i in self.set_variable_length), self.min_max)

    def add_cuts(self) -> None:
        '''
        Add cuts to the model
        '''
        for cut in self.cuts:
            self.model.addConstr(quicksum(cut[0][i]*self.x[i] for i in self.set_variable_length) <= cut[1])

    def optimize_model(self) -> None:
        '''
        Optimize the model.
        '''
        start_time: float = time.time()

        self.root_bound = None
        self.model.update()
        self.model.optimize(lambda model, where: self._record_root_bound(model, where))
        self.show_results()

        end_time: float = time.time()
        self._optimization_time = round(end_time - start_time, 4)

    def get_optimization_time(self) -> float:
        '''
        Get the optimization time.
        '''
        return self._optimization_time

    def add_additional_cuts(self, new_cuts: list[tuple[list[float], float]]) -> None:
        '''
        Add additional cuts to the model.

        Parameters:
        - new_cuts: Cuts to add
        '''
        self.cuts += new_cuts
        for cut in self.cuts:
            self.model.addConstr(quicksum(cut[0][i]*self.x[i] for i in self.set_variable_length) <= cut[1])

    def optimize_with_cuts(self, dds: list, cut_type: CutType, cut_strengthening: bool = False) -> None:
        '''
        Optimize the model using DD-based cuts injected via Gurobi callback.

        Parameters:
        - dds: List of DD objects used to generate cuts.
        - cut_type: Which cut generator to use (CutType enum).
        - cut_strengthening: If True, apply cut strengthening after generation.
        '''
        start_time: float = time.time()
        self.cut_strengthening = cut_strengthening
        self.num_cuts_added = 0
        self.root_bound = None

        self._set_up_dd_cut_generators(dds, cut_type)

        self.model.setParam('PreCrush', 1)
        self.model.update()
        self.model.optimize(lambda model, where: self._dd_cuts_callback(model, where))
        self.show_results()

        end_time: float = time.time()
        self._optimization_time = round(end_time - start_time, 4)

    def _set_up_dd_cut_generators(self, dds: list, cut_type: CutType) -> None:
        '''
        Initialize cut generators and strengtheners for each DD.
        The generator class is resolved from _CUT_GENERATOR_MAP via cut_type.
        '''
        generator_class = _CUT_GENERATOR_MAP[cut_type]
        self.dds = dds
        self.cut_generators = []
        self.cut_strengthener = []

        for dd in self.dds:
            self.cut_generators.append(generator_class(dd))
            if self.cut_strengthening:
                self.cut_strengthener.append(CutStrengthening(dd))

    def _dd_cuts_callback(self, model, where) -> None:
        '''
        Gurobi callback: generates and injects DD cuts at the root node.
        '''
        if where == GRB.Callback.MIPNODE:
            node_count = model.cbGet(GRB.Callback.MIPNODE_NODCNT)

            if node_count == 0:
                # Track the dual bound at the root node. The callback fires once per
                # root LP re-solve, so the last value captured here is the bound after
                # all root cuts have been added (used for the root gap).
                self.root_bound = model.cbGet(GRB.Callback.MIPNODE_OBJBND)

                x_sol = list(model.cbGetNodeRel(self.x).values())

                for i in range(len(self.dds)):
                    x_sol_dd, var_ids = self._get_variable_values(x_sol, self.dds[i])
                    if self.cut_generators[i].generate_cut(x_sol_dd):
                        coefficients, constant = self.cut_generators[i].get_cut()

                        if self.cut_strengthening:
                            if self.cut_strengthener[i].cut_strengthening(coefficients, constant):
                                coefficients, constant = self.cut_strengthener[i].get_cut_strengthening()

                        model.cbCut(quicksum(self.x[var_ids[j]] * coefficients[j] for j in range(len(var_ids))) <= constant)
                        self.num_cuts_added += 1

    def _record_root_bound(self, model, where) -> None:
        '''
        Gurobi callback used by plain solves (no DD cuts): records the dual bound
        at the root node so the root gap can be computed afterwards.
        '''
        if where == GRB.Callback.MIPNODE and model.cbGet(GRB.Callback.MIPNODE_NODCNT) == 0:
            self.root_bound = model.cbGet(GRB.Callback.MIPNODE_OBJBND)

    def get_root_gap(self) -> float:
        '''
        Root node gap as defined by Castro, Cire & Beck (arXiv:2003.06363):
        relative gap between the dual bound at the root node (after cuts) and the
        optimal integer objective. Returned as a fraction (multiply by 100 for %).

        Returns 0.0 when the root bound was not captured (e.g. continuous/LP models
        where no branch-and-bound root node exists) or the optimum is ~0.
        '''
        if self.root_bound is None or self.model is None:
            return 0.0
        try:
            opt: float = self.model.objVal
        except (AttributeError, GurobiError):
            return 0.0
        if abs(opt) < 1e-12:
            return 0.0
        return abs(self.root_bound - opt) / abs(opt)

    def get_final_gap(self) -> float:
        '''
        Final optimality gap at termination (Gurobi MIPGap), as a fraction.
        This is the paper's "Final Gap": 0 when solved to optimality, > 0 when the
        solver stops at the time limit. Returns 0.0 for continuous/LP models.
        '''
        if self.model is None:
            return 0.0
        try:
            gap: float = self.model.getAttr("MIPGap")
        except (AttributeError, GurobiError):
            return 0.0
        return gap if gap is not None else 0.0

    def _get_variable_values(self, x_sol: list, dd) -> tuple[list, list]:
        '''
        Map x_sol values to the variables of a given DD.

        Default implementation assumes the DD covers all model variables in order
        (valid for problems with a single complete DD: Knapsack, IndependentSet, SetCover).

        Override this method in subclasses where the DD covers only a subset of variables
        or uses a different ordering (e.g. SOCKnapsack with auxiliary variables).

        Parameters:
        - x_sol: Current LP relaxation values for self.x.
        - dd: The DD object whose variables we need to extract.

        Returns:
        - (x_sol_dd, var_ids): values and indices aligned with the DD's variables.
        '''
        return x_sol, list(range(self.variable_length))

    @abstractmethod
    def show_results(self) -> str:
        '''
        Show the results of the model.
        '''
        print(f"Objective value: {self.model.ObjVal:.6g}")
        for i, var in enumerate(self.x):
            print(f"x[{i}] = {var.X}")

    @abstractmethod
    def _add_constraints(self) -> None:
        """
        Abstract method to add constraints to the model.
        """
        raise NotImplementedError("add_constraints() debe ser implementado en las clases derivadas.")
