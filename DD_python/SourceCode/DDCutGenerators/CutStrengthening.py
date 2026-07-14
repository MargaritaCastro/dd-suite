from Exceptions.MyExceptions import NoCutStrengthening
from SourceCode.DDStructure import Node, Graph
import time
import math

class CutStrengthening():
    '''
    CutStrengthening class, responsible for solving the strengthening cut problem in a graph.
    '''

    def __init__(self, graph: Graph, verbose:bool=False) -> None:
        '''
        Constructor for the CutStrengthening class.

        Parameters:
        graph (Graph): The graph on which the cut strengthening will be performed.
        verbose (bool): Indicates if additional information will be shown on the console.
        is_graph (bool): Indicates if the graph is already a decision diagram graph.
        '''
        self.graph: 'Graph' = graph.get_decision_diagram()

        self._set_initial_cost()

        self.time: float = 0
        self.verbose: bool = verbose
        self.tolerance: float = 0.0001
        self._is_cut_strengthened: bool = False

    def _set_initial_cost(self) -> None:
        '''
        Initializes the cost dictionaries for top and bottom costs.
        '''
        self.cost_top = [0] * (self.graph.structure[-1][-1].get_id()+1)
        self.cost_bottom = [0] * (self.graph.structure[-1][-1].get_id()+1)

    def get_time(self) -> float:
        ''' 
        Returns the execution time of the strengthening algorithm. 
        
        Returns:
        - float: Execution time of the cut algorithm.
        '''
        return self.time
    
    def get_cut_strengthening(self) -> tuple[list[float], float]:
        ''' 
        Returns the precomputed values from the cut_strengthening() method.

        Returns:
        - tuple[list[float], float]: Tuple with the coefficients of the cut and the constant.
        '''
        if not self._is_cut_strengthened:
            raise NoCutStrengthening()
        
        return (self.coefficients, self.constant)
    
    def cut_strengthening(self, coefficients: list[float], constant: float) -> bool:
        '''
        Strengthens the cut by improving the coefficients and right-hand side value.

        Parameters:
        coefficients (list): List of coefficients to be strengthened.
        constant (float): Right-hand side value to be strengthened.

        Returns:
        bool: True if a strengthened cut is found, False otherwise.
        '''
        self._initialize_parameters(coefficients, constant)

        start_time: float = time.time()
        if self.verbose:
            print("Calculating cut strengthening...")
        self._compute_cut_strengthening()
        end_time: float = time.time() 
        self.time = end_time - start_time

        if self.verbose:
            if self._is_cut_strengthened:
                print(f"\n '''A strengthened cut was found in the graph.''' ")
                print(f"    The strengthened cut is:  {self._get_inequality_string_representation()}\n")
            else:
                print(f"\n '''No strengthened cut was found in the graph.''' \n")

        return self._is_cut_strengthened
    
    def _initialize_parameters(self, coefficients: list[float], constant: float) -> None:
        ''' 
        Initializes the parameters for the strengthening algorithm. 
        
        Parameters:
        coefficients (list): List of coefficients to be strengthened.
        constant (float): Right-hand side value to be strengthened.
        '''
        self.coefficients: list[float] = coefficients 
        self.constant: float = constant
        self.slacks: list[float] = [0] * len(coefficients) 
    
    def _compute_cut_strengthening(self) -> bool:
        '''
        Computes the strengthened cut.

        Returns:
        bool: True if a strengthened cut is found, False otherwise.
        '''
        self._is_cut_strengthened: bool = False
        can_strengthen: bool = self._get_bdd_slacks()

        while can_strengthen:
            slack_strengthen_id: int = self._choose_slack_to_strengthen()
            if slack_strengthen_id == -1:
                if self.verbose:
                    print("No slack found to strengthen.")
                break
                
            self.coefficients[slack_strengthen_id] += self.slacks[slack_strengthen_id]

            if self.slacks[slack_strengthen_id] <= self.tolerance:
                self.constant += self.slacks[slack_strengthen_id]

            self._is_cut_strengthened = True
            if self.verbose:
                print(f'The cut was strengthened to {self._get_inequality_string_representation()}')
            can_strengthen: bool = self._get_bdd_slacks()
        
        return self._is_cut_strengthened
    
    def _get_bdd_slacks(self) -> bool:
        '''
        Computes the slacks in the BDD structure.

        Returns:
        bool: True if the cut can be strengthened, False otherwise.
        '''
        self._compute_cost_top()

        sink: 'Node' = self.graph.structure[-1][0]
        if self.constant > self.cost_top[sink.get_id()]:
            if self.verbose:
                print(f"Elevating inequality constant from  {self.constant} to {self.cost_top[sink.get_id()]}")
            self.constant = self.cost_top[sink.get_id()]

        elif self.constant < self.cost_top[sink.get_id()]:
            if self.verbose:
                print(f"The cut cannot be strengthened as it does not satisfy  {self.constant} >= {self.cost_top[sink.get_id()]}")
            return False

        self._compute_cost_bottom()
        self._compute_slacks()

        return True
    
    def _compute_cost_top(self) -> None:
        ''' Computes the top costs for each node in the graph. '''
        for layer_index, layer in enumerate(self.graph.structure[1:]):
            for node in layer:
                self._compute_cost_top_node(node, layer_index)
    
    def _compute_cost_top_node(self, node: Node, layer_index: int) -> None:
        ''' 
        Computes the top cost for a specific node. 
        
        Parameters:
        node (Node): Node for which the top cost will be computed.
        layer_index (int): Index of the layer in which the node is located.
        '''
        value: float = 0
        node_id = node.get_id()
        self.cost_top[node_id] = -math.inf

        for arc in node.in_arcs:
            value: int = self.cost_top[arc.parent_node.get_id()] + arc.variable_value * self.coefficients[layer_index]

            if self.cost_top[node_id] < value:
                self.cost_top[node_id] = value
        
    def _compute_cost_bottom(self) -> None:
        ''' Computes the bottom costs for each node in the graph. '''
        for layer_index in reversed(range(len(self.graph.structure)-1)):
            for node in self.graph.structure[layer_index]:
                self._compute_cost_bottom_node(node, layer_index)
    
    def _compute_cost_bottom_node(self, node: Node, layer_index: int) -> None:
        ''' 
        Computes the bottom cost for a specific node. 
        
        Parameters:
        node (Node): Node for which the bottom cost will be computed.
        layer_index (int): Index of the layer in which the node is located.
        '''
        value: float = 0
        node_id = node.get_id()
        self.cost_bottom[node_id] = -math.inf

        for arc in node.out_arcs:
            value: int = self.cost_bottom[arc.child_node.get_id()] + arc.variable_value * self.coefficients[layer_index]

            if self.cost_bottom[node_id] < value:
                self.cost_bottom[node_id] = value
        
    def _compute_slacks(self) -> None:
        ''' Computes the slacks for each layer in the graph. '''

        for layer_index in range(len(self.graph.structure)-1):
            zero_edges_cost: float = -math.inf
            one_edges_cost: float = -math.inf

            for node in self.graph.structure[layer_index]:
                node_id = node.get_id()
                for arc in node.out_arcs:
                    value: float = self.cost_top[node_id] + self.cost_bottom[arc.child_node.get_id()] + arc.variable_value * self.coefficients[layer_index]
                    if arc.variable_value == 0 and value > zero_edges_cost:
                        zero_edges_cost = value
                    elif arc.variable_value == 1 and value > one_edges_cost:
                        one_edges_cost = value
            
            if zero_edges_cost is math.inf or one_edges_cost is math.inf:
                self.slacks[layer_index] = 0
            else:
                self.slacks[layer_index] = zero_edges_cost - one_edges_cost

    def _choose_slack_to_strengthen(self) -> int:
        ''' 
        Chooses which slack to strengthen based on the smallest absolute value greater than tolerance. 
        
        Returns:
        int: Index of the slack to be strengthened.
        '''
        candidate: int = -1
        value: float = math.inf

        for variable in range(len(self.slacks)):
            if abs(self.slacks[variable]) < value and abs(self.slacks[variable]) > self.tolerance:
                value = abs(self.slacks[variable])
                candidate = variable

        return candidate

    def _get_inequality_string_representation(self) -> str:
        ''' 
        Returns the inequality representing the strengthened cut. 
        
        Returns:
        str: String representing the strengthened cut.
        '''
        inequality: str = f"{' + '.join(f'{x}x_{i}' if(x!= 1 and x!=0) else f'x_{i}' for i, x in enumerate(self.coefficients) if x != 0)} <= {self.constant}"
        return inequality
