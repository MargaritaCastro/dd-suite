from abc import ABC, abstractmethod

import os
import sys

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

from SourceCode.DDStructure.Node import Node
from SourceCode.DDStructure.Arc import Arc
from SourceCode.DDStructure.Graph import Graph
from SourceCode.Problems.AbstractProblemClass import AbstractProblem


class AbstractDDBuilder(ABC):
    '''
    Abstract class that provides the foundation for building decision diagrams.
    '''

    def __init__(self, problem: AbstractProblem) -> None:
        '''
        Constructor for the AbstractDDBuilder class.

        Parameters:
        - problem: Reference to the problem associated with the graph.
        '''
        self._node_number: int = 1
        self.graph: 'Graph' = None
        self._problem: 'AbstractProblem' = problem
        self._variables: list[str] = problem.ordered_variables
        self._variables_domain: list[list[int]] = []
        self._map_of_states: dict[str, Node] = {}
        self._is_last_layer: bool = False
        self._scratch_state: list = [problem.get_state_copy(problem.initial_state)]

        self._set_variables_domain(problem.variables_domain)
        self._initialize_graph(problem.initial_state)

    def _set_variables_domain(self, variables_domain: dict[str, list[int]]) -> None:
        '''
        Populates the ordered list of variable domains from the problem's domain mapping.

        Parameters:
        - variables_domain: Dictionary mapping variable names to their list of possible values.
        '''
        for domains in variables_domain.values():
            self._variables_domain.append(domains)
        
    def _initialize_graph(self, initial_state: 'State') -> None:
        '''
        Initializes the graph with a root node

        Parameters:
        - initial_state: Initial state of the root node.
        '''
        node_root: 'Node' = Node(0, initial_state)
        self.graph = Graph(node_root)
    
    def get_decision_diagram(self, should_visualize: bool) -> Graph:
        '''
        Builds the decision diagram.

        Parameters:
        - should_visualize: Boolean indicating whether to visualize the graph construction.

        Returns:
        - Graph: Pointer to the constructed graph
        '''
        for variable_id in range(len(self._variables)):
            self._create_new_layer(variable_id)
            self._specific_end_of_layer_function()
            self._print_graph(should_visualize)
        
        self._specific_end_of_construction_function()
        self._set_final_state()
        self._print_graph(should_visualize)

        return self.graph
    
    def _create_new_layer(self, variable_id: int) -> None:
        '''
        Creates a new layer in the graph for a given variable.

        Parameters:
        - variable_id: Index of the variable for which the new layer is created.
        '''
        self.graph.add_new_layer()
        self._create_new_nodes_in_the_new_layer(variable_id)

    def _create_new_nodes_in_the_new_layer(self, variable_id: int) -> None:
        '''
        Creates nodes in the new layer of the graph.

        Parameters:
        - variable_id: Index of the variable for which nodes are created in the new layer.
        '''

        last_layer_index: int = len(self.graph.structure) - 2
        self._map_of_states = {}
        variable_name: str = self._variables[variable_id]
        variable_index: int = self._problem.name_to_index[variable_name]
        self._is_last_layer = variable_id == len(self._variables) - 1

        for parent_node in self.graph.structure[last_layer_index]:

            for variable_value in self._variables_domain[variable_id]:

                is_feasible = self._problem.transition_function(
                    parent_node.state, variable_index, variable_value, self._scratch_state
                )

                if is_feasible:
                    self._create_new_node(variable_value, parent_node, self._scratch_state[0])

    def _create_new_node(self, variable_value: int, parent_node: Node, node_state: 'State') -> None:
        '''
        Creates a new node in the graph.

        Parameters:
        - variable_value: Value of the variable for which the node is created.
        - parent_node: Pointer to the node in the previous layer.
        - node_state: State of the new node.
        '''

        if self._is_last_layer and self.graph.structure[-1] != []:
            self._create_arcs_for_the_terminal_node(variable_value, parent_node)
        else:
            state_as_string: str = self._problem.get_state_as_string(node_state)

            if state_as_string in self._map_of_states:
                self._create_arc_for_the_new_node(parent_node, self._map_of_states[state_as_string], variable_value)

            else:
                node_state = self._problem.get_state_copy(node_state)
                new_node: 'Node' = self.graph.create_and_add_node(self._node_number, node_state)
                self._map_of_states[state_as_string] = new_node

                self._create_arc_for_the_new_node(parent_node, new_node, variable_value)
                self._node_number += 1

    def _create_arcs_for_the_terminal_node(self, variable_value: int, parent_node: Node) -> None:
        '''
        Creates arcs for the terminal node in the last layer.

        Parameters:
        - parent_node: Node in the previous layer.
        - variable_value: Value of the variable for which the arcs are created.
        '''
        terminal_node: 'Node' = self.graph.structure[-1][-1]
        self._create_arc_for_the_new_node(parent_node, terminal_node, variable_value)


    def _create_arc_for_the_new_node(self, parent_node: Node, node_created: Node, variable_value: int) -> None:
        '''
        Creates an arc for an existing node.

        Parameters:
        - parent_node: Node in the previous layer.
        - node_created: Newly created node.
        - variable_value: Value of the variable for which the arc is created.
        '''

        arc: Arc = Arc(parent_node, node_created, variable_value)

        parent_node.out_arcs.append(arc)
        node_created.in_arcs.append(arc)
    
    def _bottom_up_pruner(self) -> bool:
        '''
        Prunes the graph from the bottom up.
        '''
        delete_some_node = False

        for layer_id in range(len(self.graph.structure) - 2, 0, -1):
            self._delete_arcs_previous_layer(layer_id)
            if self._delete_nodes_current_layer(layer_id):
                delete_some_node = True

        return delete_some_node

    def _delete_arcs_previous_layer(self, layer_id: int) -> None:
        '''
        Removes outgoing arcs from nodes in the layer before layer_id whose child nodes
        have no outgoing arcs (i.e., dead-end children).

        Parameters:
        - layer_id: Index of the current layer; arcs are pruned from layer_id - 1.
        '''
        layer_id -= 1
        if not (0 <= layer_id < len(self.graph.structure)):
            return

        for node in self.graph.structure[layer_id]:
            live_arcs = []
            remove_some_arc : bool = False

            for arc in node.out_arcs:
                if not arc.child_node.out_arcs:
                    arc.child_node.in_arcs.clear()
                    remove_some_arc = True
                else:
                    live_arcs.append(arc)

            if remove_some_arc:
                node.out_arcs[:] = live_arcs

    def _delete_nodes_current_layer(self, layer_id) -> bool:
        '''
        Removes nodes with no incoming arcs from the specified layer.

        Parameters:
        - layer_id: Index of the layer to prune.

        Returns:
        - bool: True if at least one node was removed, False otherwise.
        '''
        layer = self.graph.structure[layer_id]
        new_layer = []
        delete_some_node = False

        for node in layer:
            if  node.in_arcs:
                new_layer.append(node)
            else:
                delete_some_node = True

        if delete_some_node:
            layer[:] = new_layer

        return delete_some_node

    
    def _set_final_state(self) -> None:
        '''
        Sets the state of the terminal node to the problem's final state.
        '''
        self.graph.structure[-1][-1].state = self._problem.get_final_state()

    def _print_graph(self, should_visualize: bool) -> None:
        '''
        Print the graph if visualization is requested.

        Parameters:
        - should_visualize: Boolean indicating whether to visualize the graph.
        '''
        if should_visualize:
            self._print()
 
    def _print(self) -> None:
        '''
        Print the content of each layer of the graph.
        '''
        print("")
        for layer_idx, layer in enumerate(self.graph.structure):
            print("------------------------------------------------------")
            if 0 < layer_idx <= len(self._variables):
                print(f"[{self._variables[layer_idx - 1]}]", end="   ")
            for node in layer:
                in_arcs_str: str = ", ".join(arc.to_string() for arc in node.in_arcs)
                print("u" + node.to_string() + " " + self._problem.get_state_as_string(node.state) + "(" + in_arcs_str + ")", end="   ")
            print("")
    
    def adjust_node_number(self) -> None:
        '''
        Adjusts the number of nodes in the graph.
        '''
        initial_node_number: int = 0
        for layer in self.graph.structure:
            for node in layer:
                node.set_id(initial_node_number)
                initial_node_number += 1
    
    @abstractmethod
    def _specific_end_of_layer_function(self) -> None:
        '''
        Pure virtual method for specific layer function.
        '''
        pass

    @abstractmethod
    def _specific_end_of_construction_function(self) -> None:
        '''
        Pure virtual method specific final function.
        '''
        pass
