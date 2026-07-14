from SourceCode.GraphAlgorithms.ShortestLongestPath.PathStructure import PathStructure, PathStructureSolution
from Exceptions.MyExceptions import ObjectiveFunctionNotExecuted, ParametersNotSetException
from SourceCode.DD import DD

import time

class ShortestLongestPath():
    '''
    SourceCode representing an objective function controller for a decision diagram.
    '''
    
    def __init__(self, DD: DD):
        '''
        Constructor for the ObjectiveFunction class.

        Parameters:
        - DD (DD): Decision diagram that will be solved.
        '''

        self._graph: 'Graph' = DD.get_decision_diagram()
        self._time: float = 0
        self._solution: 'PathStructureSolution' = PathStructureSolution()
        self._weights: list[int] = []
        self._objective: str = ""
        self._DP = []
    
    def set_parameters(self, weights: list[int], objective: str) -> None:
        '''
        Sets the parameters of the path weights

        Parameters:
        - weights (list): List of weights associated with the nodes/arcs.
        - objective (str): Objective type ("max" for maximization or "min" for minimization).
        '''
        self._weights: list[int] = weights
        self._objective: str = objective
        self._choose_transform_weights()
    
    def get_time(self) -> str:
        ''' 
        Returns the execution time of the solving algorithm. 
        
        Returns:
        - float: The time in seconds.
        '''
        return str(self._time)
    
    def get_solution(self) -> PathStructureSolution:
        '''
        Returns the optimal path and corresponding arcs.

        Returns:
        - PathStructure: The optimal path and corresponding arcs.
        '''
        if self._solution.value != -111111:
            return self._solution
        else:
            raise ObjectiveFunctionNotExecuted()
    
    def solve(self) -> PathStructureSolution:
        '''
        Solves the given decision diagram using the previously set weights and objective.

        Returns:
        - PathStructure: Structure containing the solution of the path.
        '''

        self._check_if_parameters_are_set()
        start_time: float = time.time()
        print("\nStarting the process to solve the shortest or longest path in the graph...")
        self._resolve_graph()
        end_time: float = time.time() 
        self._time = end_time - start_time
        print("Shortest/longest path algorithm completed.")
        
        return self._solution
    
    def _check_if_parameters_are_set(self) -> None:
        '''
        Checks if the algorithm has been set.
        '''
        if not hasattr(self, "_objective") or not hasattr(self, "_weights"):
            raise ParametersNotSetException()

    def _choose_transform_weights(self) -> None:
        '''
        If the objective is "max", this function transforms each weight in the list of weights
        by multiplying it by -1, so that the maximization problem becomes a minimization problem.

        Note: This transformation is specific to the algorithm used.
        '''
        if self._objective == "max":
            self._weights = [-weight for weight in self._weights]
        
    def _resolve_graph(self) -> None:
        ''' Resolves the graph associated with the objective. '''

        objective_answer: PathStructure = self._shortest_path_algorithm()

        if self._objective == "max":
            objective_answer.value = -objective_answer.value

        self._save_solution_path()

    def _save_solution_path(self) -> None:
        ''' Saves the full path in the objective structure of the terminal node

        Parameters:
        - path_answer (PathStructure): The path structure to be saved.
        '''

        optimal_path: str = ""
        current: PathStructure = self._DP[-1]
        self._solution.path_arcs = []
        self._solution.value = current.value

        while current is not None:
            if current.arc is None:
                break

            optimal_path = f"-> {current.arc.to_string()}" + optimal_path

            self._solution.path_arcs.insert(0,current.arc)

            current = self._DP[current.arc.parent_node.get_id()]

        self._solution.path_print = optimal_path[2:]

    def _shortest_path_algorithm(self) -> PathStructure:
        ''' Shortest path algorithm in a directed acyclic graph (DAG) to solve the problem. 
        
        Returns:
        - PathStructure: PathStructure  Pointer to the objective structure corresponding to the terminal node.
        '''
        self._reset_objective_nodes()

        arc_aux_value = 0

        for layer_id in range(1, len(self._graph.structure)):
            for node in self._graph.structure[layer_id]:
                node_id = node.get_id()

                # Find the smallest value and set structure
                for arc in node.in_arcs:
                    arc_aux_value = self._DP[arc.parent_node.get_id()].value + arc.variable_value * self._weights[layer_id - 1]
                    if self._DP[node_id].value > arc_aux_value:
                        self._DP[node_id].value = arc_aux_value
                        self._DP[node_id].arc = arc

        terminal_node: 'Node' = self._graph.structure[-1][0]

        return self._DP[terminal_node.get_id()]

    def _reset_objective_nodes(self) -> None:
        ''' Reset the values of the objective nodes.. '''


        if len(self._DP) == 0:
            total_nodes: int = self._graph.get_node_count()
            self._DP = [PathStructure(float('inf'), None) for i in range(total_nodes)]

        else:
            for obj in self._DP:
                 obj.reset()


        root_node: 'Node' = self._graph.structure[0][0]
        root_node_id = root_node.get_id()
        self._DP[root_node_id].value = 0