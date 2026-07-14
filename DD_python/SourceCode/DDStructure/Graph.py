from SourceCode.DDStructure.Node import Node

class Graph():
    '''
    SourceCode representing a graph structure with layers of nodes.
    '''

    def __init__(self, initial_node: Node):
        '''
        Constructor for the Graph class.

        Parameters:
        - initial_node(Node): The initial node for the graph.

        Attributes:
        - nodes (list): A list of all nodes in the graph.
        - structure (list): A 2D list representing the structure of the graph in layers.
        - actual_layer (int): The index of the current layer in the graph.
        '''

        self.structure: list[list['Node']] = [[initial_node]]
        self.actual_layer: int = 0
    
    def __eq__(self, other: 'Graph') -> bool:
        '''
        Compares if two Graph objects are equal.

        Parameters:
        - other (Graph): The other Graph object to compare with.

        Returns:
        - bool: True if the objects are equal, False otherwise.
        '''
        if not isinstance(other, Graph):
            return False
        
        result: bool = True

        if len(self.structure) != len(other.structure):
            return False

        for i, layer in enumerate(self.structure):

            if len(layer) != len(other.structure[i]):
                return False

            for node in layer:
                there_is_equal_node: bool = False
                for other_node in other.structure[i]:
                    if node.state == other_node.state:
                        there_is_equal_node = there_is_equal_node or self._compare_two_nodes(node, other_node)

                result = result and there_is_equal_node
    
        return result

    def _compare_two_nodes(self, node1: Node, node2: Node) -> bool:
        '''
        Verifies between two nodes that their incoming and outgoing arcs are equal. That is, 
        the arcs have the same value and originate/terminate at a node with the same state.

        Parameters:
        - node1(Node): The first node to compare.
        - node2(Node): The second node to compare.

        Returns:
        - bool: True if the nodes are equal, False otherwise
        '''
        if len(node1.in_arcs) != len(node2.in_arcs) or len(node1.out_arcs) != len(node2.out_arcs):
            return False

        return_in_arcs: bool = True
        for arc in node1.in_arcs:
            there_is_equal_arc: bool = False

            for other_arc in node2.in_arcs:
                if arc.variable_value == other_arc.variable_value and arc.parent_node.state == other_arc.parent_node.state:
                    there_is_equal_arc = True

            return_in_arcs = return_in_arcs and there_is_equal_arc


        return_out_arcs: bool = True
        for arc in node1.out_arcs:
            there_is_equal_arc: bool = False

            for other_arc in node2.out_arcs:
                if arc.variable_value == other_arc.variable_value and arc.child_node.state == other_arc.child_node.state:
                    there_is_equal_arc = True
            return_out_arcs = return_out_arcs and there_is_equal_arc

        return return_in_arcs and return_out_arcs
    
    def get_node_count(self) -> int:
        '''
        Returns the number of nodes in the graph.

        Returns:
        - int: The number of nodes in the graph.
        '''
        count: int = 0
        for layer in self.structure:
            count += len(layer)
        return count

    def get_arc_count(self) -> int:
        '''
        Returns the number of arcs in the graph.

        Returns:
        - int: The number of arcs in the graph.
        '''
        count: int = 0
        for layer in self.structure:
            for node in layer:
                count += len(node.out_arcs)
        return count

    def get_actual_max_width(self) -> int:
        '''
        Returns the maximum width of the graph

        Returns:
        - int: The maximum width of the graph.
        '''
        max_width: int = 0
        for layer in self.structure:
            max_width = max(max_width, len(layer))
        return max_width

    def add_node(self, node: Node) -> None:
        '''
        Adds a node to the current layer of the graph.

        Parameters:
        - node(Node): Node object to add to the current layer.
        '''
        self.structure[self.actual_layer].append(node)

    def create_and_add_node(self, node_number: int, node_state: 'State') -> Node:
        '''
        Adds a node to the current layer of the graph.

        Parameters:
        - node_number(int): The number of the node to be added.
        - node_state(State): The state of the node to be added.

        Returns:
        - Node: The node that was added to the graph.
        '''
        new_node: 'Node' = Node(node_number, node_state)
        self.structure[self.actual_layer].append(new_node)
        return new_node

    def add_new_layer(self) -> None:
        '''Creates a new layer in the graph.'''
        self.structure.append([])
        self.actual_layer += 1