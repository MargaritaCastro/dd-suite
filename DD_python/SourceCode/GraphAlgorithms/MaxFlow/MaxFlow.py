from Exceptions.MyExceptions import MaxFlowNotExecuted
from SourceCode.DD import DD
from SourceCode.DDStructure import Node, Graph
from collections import deque
import time


class MaxFlow():
    '''
    MaxFlow class, which is responsible for solving the maximum flow problem in a graph. With the implementation of
    Gurobi, it seeks to find the maximum flow that can pass through a graph, from a starting node to an end node.
    This algorithm can only be used to solve binary decision diagrams (BDDs) and was specifically designed to address certain
    max-flow gurobi problems from the paper "A combinatorial cut-and-lift procedure with an application to 0–1 second-order
    conic programming. Mathematical Programming."

    Castro, M. P., Cire, A. A., & Beck, J. C. (2022). A combinatorial cut-and-lift procedure with an application to 0–1
    second-order conic programming. Mathematical Programming, 196(1), 115-171.
    '''

    def __init__(self, DD: DD, verbose: bool=False) -> None:
        '''
        Constructor of the MaxFlow class.

        Parameters:
        DD (DD): Decision diagram that will be solved.
        '''
        self.graph_DD: 'Graph' = DD.get_decision_diagram()
        self.time: float = 0
        self.verbose: bool = verbose
        self._init_arc_ids()

    def _init_arc_ids(self) -> None:
        '''
        Assigns a unique integer arc_id to every arc once at construction time.
        Even IDs are forward arcs; arc_id ^ 1 is always the paired reverse arc.
        Builds arc_list and pre-allocates the residual vector.
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
        self._arc_count: int = arc_id >> 1
        self.residual: list[float] = [0.0] * arc_id

    def get_time(self) -> float:
        '''
        Returns the execution time of the max-flow algorithm.

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
        if not hasattr(self, "max_flow"):
            raise MaxFlowNotExecuted()
        return self.max_flow

    def solve_max_flow(self, capacities: list[float], verbose: bool=False, tolerance=0.0001) -> float:
        '''
        Solves the decision diagram, obtaining the best solution for the objective function
        provided in develop_solver. Returns this value.

        Parameters:
        capacities (list): List with the capacities of the nodes in the graph.
        verbose (bool): Indicates if additional information will be displayed in the console.

        Returns:
        - float: Maximum flow in the graph.
        '''
        self.tolerance: float = tolerance
        self._set_capacities(capacities)
        self.verbose: bool = verbose

        start_time: float = time.time()
        if self.verbose:
            print("\nCalculating maximum flow in the graph...\n")
        self.max_flow: float = self._compute_max_flow()
        end_time: float = time.time()
        self.time = end_time - start_time

        if self.verbose:
            print(f"\n '''The maximum flow in the graph is  {self.max_flow}''' \n")

        return self.max_flow

    def _set_capacities(self, capacities: list[float]) -> None:
        '''
        Resets the residual vector and fills forward capacities from the given layer capacities.
        Arc IDs are already assigned at construction; this is O(E) with no allocation.

        Parameters:
        capacities (list): List with the capacities per layer.
        '''
        self.residual = [0.0] * (self._arc_count * 2)

        for layer_idx, layer in enumerate(self.graph_DD.structure):
            cap = capacities[layer_idx] if layer_idx < len(capacities) else 0.0
            for node in layer:
                for arc in node.out_arcs:
                    self.residual[arc.arc_id] = cap if arc.variable_value == 1 else 1.0 - cap

    def _compute_max_flow(self) -> float:
        '''
        Computes the maximum flow that can pass through the graph.

        Returns:
        - float: Maximum flow in the graph.
        '''
        max_flow: float = 0.0
        source = self.graph_DD.structure[0][0]
        sink = self.graph_DD.structure[-1][0]

        path: list[int] = self._find_augmenting_path(source, sink)

        while path is not None:
            path_flow = min(self.residual[i] for i in path)

            for i in path:
                self.residual[i] -= path_flow
                self.residual[i ^ 1] += path_flow

            max_flow += path_flow

            if self.verbose:
                self._print_path(path, path_flow)

            path = self._find_augmenting_path(source, sink)

        return max_flow

    def _find_augmenting_path(self, start_node: Node, terminal_node: Node,
                               visited: set=None, path: list=None) -> list[int]:
        '''
        Performs backtracking to find an augmenting path in the residual graph.
        The path is a list of residual arc IDs (even = forward, odd = backward).

        Parameters:
        start_node (Node): Starting node of the path.
        terminal_node (Node): End node of the path.
        visited (set): Set of visited node IDs.
        path (list): List of residual arc IDs in the path.

        Returns:
        - list[int] | None: Arc ID path if found, None otherwise.
        '''
        if visited is None:
            visited = set()
        if path is None:
            path = []

        visited.add(start_node.get_id())

        if start_node is terminal_node:
            return path

        for arc in start_node.out_arcs:
            if arc.child_node.get_id() not in visited and self.residual[arc.arc_id] > 0:
                path.append(arc.arc_id)
                result = self._find_augmenting_path(arc.child_node, terminal_node, visited, path)
                if result is not None:
                    return result
                path.pop()

        for arc in start_node.in_arcs:
            rev_id = arc.arc_id ^ 1
            if arc.parent_node.get_id() not in visited and self.residual[rev_id] > 0:
                path.append(rev_id)
                result = self._find_augmenting_path(arc.parent_node, terminal_node, visited, path)
                if result is not None:
                    return result
                path.pop()

        return None

    def _print_path(self, path: list[int], path_flow: float) -> None:
        '''
        Prints a path and its flow.

        Parameters:
        path (list[int]): List of residual arc IDs in the path.
        path_flow (float): Flow of the path.
        '''
        node_ids: list[str] = []
        for arc_id in path:
            arc = self._arc_list[arc_id >> 1]
            is_forward = (arc_id & 1) == 0
            p = str(arc.parent_node.get_id())
            c = str(arc.child_node.get_id())
            if is_forward:
                if not node_ids or node_ids[-1] != p:
                    node_ids.append(p)
                node_ids.append(c)
            else:
                if not node_ids or node_ids[-1] != c:
                    node_ids.append(c)
                node_ids.append(p)
        print("Path:", " -> ".join(node_ids), ", Flow:", round(path_flow, 3))

    def compute_visited_nodes(self) -> list[bool]:
        '''
        Returns a list of bools indexed by node ID indicating reachability from source.

        Returns:
        - list[bool]: visited[node_id] is True if the node is reachable from source.
        '''
        visited: list[bool] = [False] * self.graph_DD.get_node_count()

        source = self.graph_DD.structure[0][0]
        visited[source.get_id()] = True
        queue: deque = deque([source])

        while queue:
            node: 'Node' = queue.popleft()

            for arc in node.out_arcs:
                child_id = arc.child_node.get_id()
                if self.residual[arc.arc_id] > self.tolerance and not visited[child_id]:
                    visited[child_id] = True
                    queue.append(arc.child_node)

            for arc in node.in_arcs:
                parent_id = arc.parent_node.get_id()
                if self.residual[arc.arc_id ^ 1] > self.tolerance and not visited[parent_id]:
                    visited[parent_id] = True
                    queue.append(arc.parent_node)

        return visited
