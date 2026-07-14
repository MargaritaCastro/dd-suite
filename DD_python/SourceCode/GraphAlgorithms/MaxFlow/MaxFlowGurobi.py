from Exceptions.MyExceptions import MaxFlowNotExecuted, GurobiModelNotInitialized
from SourceCode.DD import DD
from gurobipy import GRB, Model, quicksum, GurobiError
import time


class MaxFlowGurobi():
    '''
    MaxFlowGurobi class, which is responsible for solving the maximum flow problem in a graph. With the implementation of
    Gurobi, it seeks to find the maximum flow that can pass through a graph, from a starting node to an end node.
    This algorithm can only be used to solve binary decision diagrams (BDDs) and was specifically designed to address certain
    max-flow gurobi problems from the paper "A combinatorial cut-and-lift procedure with an application to 0–1 second-order
    conic programming. Mathematical Programming."

    Castro, M. P., Cire, A. A., & Beck, J. C. (2022). A combinatorial cut-and-lift procedure with an application to 0–1
    second-order conic programming. Mathematical Programming, 196(1), 115-171.
    '''
    def __init__(self, DD: DD) -> None:
        '''
        Constructor of the MaxFlowGurobi class.

        Parameters:
        DD (DD): Decision diagram that will be solved.
        '''
        self.graph_DD = DD.get_decision_diagram()
        self.time: float = 0
        self._init_arc_ids()

    def _init_arc_ids(self) -> None:
        '''
        Assigns a unique integer arc_id to every arc once at construction time.
        Even IDs are forward arcs; arc_id ^ 1 is the paired reverse arc.
        '''
        arc_id = 0
        arc_list = []
        for layer in self.graph_DD.structure:
            for node in layer:
                for arc in node.out_arcs:
                    arc.arc_id = arc_id
                    arc_list.append(arc)
                    arc_id += 2
        self._arc_list = arc_list
        self._arc_count: int = len(arc_list)
        self.capacities: list[float] = [0.0] * self._arc_count

    def get_time(self) -> float:
        '''
        Returns the execution time of the gurobi algorithm.

        Returns:
        - float: Execution time of the cut algorithm.
        '''
        return self.time

    def get_max_flow(self) -> float:
        '''
        Returns the precomputed values in the solve_max_flow() section.

        Returns:
        - float: Maximum flow in the graph.
        '''
        if not hasattr(self, "maxflow"):
            raise MaxFlowNotExecuted()
        return self.maxflow

    def solve_max_flow(self, capacities: list[float]) -> float:
        '''
        Solves the decision diagram, obtaining the best solution for the objective function. Returns this value.

        Parameters:
        capacities (list): List with the capacities of the nodes in the graph.

        Returns:
        - float: Maximum flow in the graph.
        '''
        self._set_capacities(capacities)
        start_time: float = time.time()
        self.maxflow = self._compute_max_flow()
        end_time: float = time.time()
        self.time = end_time - start_time
        print(f"\n ''''The maximum flow in the graph is {self.maxflow}''' \n")
        return self.maxflow

    def _set_capacities(self, capacities: list[float]) -> None:
        '''
        Fills the capacity list indexed by arc_id >> 1.

        Parameters:
        capacities (list): List with the capacities of the nodes in the graph.
        '''
        cap_list = self.capacities
        for layer_idx, layer in enumerate(self.graph_DD.structure):
            cap = capacities[layer_idx] if layer_idx < len(capacities) else 0.0
            for node in layer:
                for arc in node.out_arcs:
                    cap_list[arc.arc_id >> 1] = cap if arc.variable_value == 1 else 1.0 - cap

    def _compute_max_flow(self) -> float:
        '''
        Computes the maximum flow that can pass through the graph. Initializes the Gurobi model.

        Returns:
        - float: Maximum flow in the graph.
        '''
        try:
            self.model = Model('MaxFlow')
        except GurobiError as e:
            self.model = None
            raise GurobiModelNotInitialized(
                f"Gurobi model not initialized (possible license issue): {e}"
            ) from e

        if self.model is None:
            raise GurobiModelNotInitialized(
                "Gurobi model not initialized (possible license issue)"
            )

        self.flows = [None] * self._arc_count

        self._initialize_variables()
        self._add_constraints()
        self._set_objective()
        self._optimize()

        return self.model.objVal

    def _initialize_variables(self) -> None:
        ''' Initializes one flow variable per arc, indexed by arc_id >> 1. '''
        for arc in self._arc_list:
            idx = arc.arc_id >> 1
            self.flows[idx] = self.model.addVar(
                lb=0, ub=self.capacities[idx],
                name=f'flow_{arc.parent_node.get_id()}_{arc.child_node.get_id()}'
            )

    def _add_constraints(self) -> None:
        '''
        Adds flow conservation constraints. O(E) — iterates in_arcs/out_arcs directly
        instead of the previous O(V^2) search over all node pairs.
        '''
        source_id = self.graph_DD.structure[0][0].get_id()
        sink_id = self.graph_DD.structure[-1][0].get_id()

        for layer in self.graph_DD.structure:
            for node in layer:
                if node.get_id() == source_id or node.get_id() == sink_id:
                    continue
                inflow = quicksum(self.flows[arc.arc_id >> 1] for arc in node.in_arcs)
                outflow = quicksum(self.flows[arc.arc_id >> 1] for arc in node.out_arcs)
                self.model.addConstr(inflow == outflow, name=f'flow_conservation_{node.get_id()}')

    def _set_objective(self) -> None:
        ''' Sets the objective to maximize outflow from source. '''
        source = self.graph_DD.structure[0][0]
        outflow = quicksum(self.flows[arc.arc_id >> 1] for arc in source.out_arcs)
        self.model.setObjective(outflow, GRB.MAXIMIZE)

    def _optimize(self) -> None:
        ''' Optimizes the model to find the maximum flow. '''
        self.model.update()
        self.model.optimize()
        self._show_results()

    def _show_results(self) -> None:
        ''' Displays the results of the optimization. '''
        if self.model.status == GRB.OPTIMAL:
            print("Maximum flow:", self.model.objVal)
            for arc in self._arc_list:
                flow_var = self.flows[arc.arc_id >> 1]
                if flow_var.X > 0:
                    print(f'Flow in {arc.parent_node.get_id()} -> {arc.child_node.get_id()} ({arc.variable_value}): {round(flow_var.X, 3)}')
        else:
            print("No optimal solution found.")
