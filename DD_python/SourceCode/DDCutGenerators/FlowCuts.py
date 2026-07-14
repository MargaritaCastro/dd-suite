from Exceptions.MyExceptions import NoCutsFound
from SourceCode.DD import DD
from SourceCode.GraphAlgorithms.MaxFlow.MaxFlow import MaxFlow
from SourceCode.DDCutGenerators.AbstractCutGenerator import AbstractCutGenerator
import time

class FlowCuts(AbstractCutGenerator):
    '''
    FlowCuts class, responsible for solving the cut problem in a graph. Utilizes the MaxFlow class
    to find the maximum flow and determine the minimum cuts in a given graph.
    This algorithm can only be used to solve binary decision diagrams (BDDs) and was specifically designed
    to address certain max-flow problems from the paper "A combinatorial cut-and-lift procedure with an
    application to 0-1 second-order conic programming. Mathematical Programming, section 5.3."

    Castro, M. P., Cire, A. A., & Beck, J. C. (2022). A combinatorial cut-and-lift procedure with an
    application to 0-1 second-order conic programming. Mathematical Programming, 196(1), 115-171.
    
    '''

    def __init__(self, DD: DD, is_graph: bool=False, tolerance: float=0.0001) -> None:
        '''
        Constructor for the Cuts class.

        Parameters:
        DD (DD): Decision diagram to be solved.
        tolerance (float): tolerance value for the linear programming model, to avoid numeric errors.
        verbose (bool): Indicates if additional information will be shown on the console.
        '''
        super().__init__(tolerance)

        self.graph_DD: 'Graph' = DD.get_decision_diagram()
        self.max_flow_instance: 'MaxFlow' = MaxFlow(DD)

        self.time: float = 0
        self.verbose: bool = False
    
    def get_time(self) -> str:
        ''' 
        Returns the execution time of the cut algorithm. 

        Returns:
        - Execution time of the cut algorithm.
        '''
        return str(self.time)

    def get_name(self) -> str:
        ''' 
        Returns the name of the cut. 

        Returns:
        - String representation of the cut.
        '''
        return "Flow Cuts (Combinatorial Cuts)"
    
    def get_cut(self) -> tuple[list[float], float]:
        ''' 
        Returns the precomputed values from the generate_cut() method. 

        Returns:
        - Tuple with the coefficients of the cut and the constant.
        '''
        if not hasattr(self, "coefficients"):
            raise NoCutsFound()
        
        return (self.coefficients, self.constant)
    
    def get_min_cut(self) -> float:
        ''' 
        Returns the precomputed value of the min cut 
        
        Returns:
        - Minimum cut value.
        '''
        if not hasattr(self, "min_cut"):
            raise NoCutsFound()

        return self.min_cut
    
    def generate_cut(self, x_values: list[float], verbose:bool =False) -> bool:
        '''
        Solves the decision diagram, finding a cut. Returns this value.

        Parameters:
        x_values (list): List of x values in the graph. (Equivalent to capacities)
        verbose (bool): Indicates if additional information will be shown on the console.

        Returns:
        - Boolean indicating whether the cut is feasible.
        '''
        self.verbose = verbose
        start_time: float = time.time()
        find_cut: bool = self._compute_cut(x_values)
        end_time: float = time.time() 
        self.time = end_time - start_time

        if find_cut:
            if self.verbose:
                print(f"\n'''A cut was found in the graph.'''")
                print(f"    The cut it is: {self._get_inequality_string_representation()}\n")
        else:
            if verbose:
                print(f"\n '''No cut was found in the graph.''' \n")
            del self.coefficients
            del self.constant

        return find_cut
    
    def _compute_cut(self, x_values: list[float]) -> bool:
        '''
        Computes the cut in the graph.

        Parameters:
        x_values (list): List of x values in the graph. (Equivalent to capacities)

        Returns:
        - Boolean indicating whether the cut is feasible.
        '''
        self.coefficients: list[int] = [0] * (len(self.graph_DD.structure) -  1 )
        self.constant: int = -1
        self.x_values: list[float] = x_values

        if self.verbose:
            print("\nCalculating cut...\n")
        
        return self._separate_min_cut()
    
    def _separate_min_cut(self) -> bool:
        '''
        Separates the minimum cut in the graph.

        Returns:
        - Boolean indicating whether the cut is feasible.
        '''
        max_flow: float = self.max_flow_instance.solve_max_flow(self.x_values, self.verbose)

        if max_flow >= 1 - self.tolerance:
            return False
        
        self._get_min_cut_arcs()
        return self._is_constraint_violated()
    
    def _get_min_cut_arcs(self) -> None:
        ''' Gets the minimum cut arcs in the graph. '''
        visited: list[bool] = self.max_flow_instance.compute_visited_nodes()

        if self.verbose:
            print("visited nodes: ", end=" ")
            for layer in self.graph_DD.structure:
                for node in layer:
                    if visited[node.get_id()]:
                        print(f'node {node.get_id()},', end=" ")
            print("\n")

        num_nodes: int = 0
        self.min_cut = 0

        for layer_index, layer in enumerate(self.graph_DD.structure):
            for node in layer:
                if not visited[node.get_id()]:
                    continue

                for arc in node.out_arcs:
                    if visited[arc.child_node.get_id()]:
                        continue
                    if arc.variable_value == 0:
                        self.min_cut += 1 - self.x_values[layer_index]
                        self.coefficients[layer_index] += 1
                        self.constant += 1
                    else:
                        self.min_cut += self.x_values[layer_index]
                        self.coefficients[layer_index] -= 1
        
        if self.verbose:
            print("coefficients = ", self.coefficients, "... constant = ", self.constant, "... min_cut = ", self.min_cut)

    def _is_constraint_violated(self) -> bool:
        '''
        Checks if the constraint is violated.

        Returns:
        - Boolean indicating whether the constraint is violated
        '''
        left_hand_side: int = 0
        for i in range(len(self.coefficients)):
            left_hand_side += self.coefficients[i] * self.x_values[i]
        
        if self.verbose:
            print("¿left_hand_side (", left_hand_side, ") - constant (", self.constant, ") >= tolerance (", self.tolerance, ")?")

        return left_hand_side - self.constant >= self.tolerance
    