

class Node():
    '''
    SourceCode representing a node in a directed acyclic graph.
    '''

    def __init__(self, node_id: int, state: 'State') -> None:
        '''
        Constructor for the Node class.

        Parameters:
        - id(int): Unique identifier of the node.
        - state(State): State associated with the node.
        '''
        self._id: int = node_id
        self.state: 'State' = state
        self.in_arcs: list['Arc'] = []
        self.out_arcs: list['Arc'] = []
        self._node_as_string: str = ""
    
    def to_string(self) -> str:
        '''
        Returns a string representation of the Node object.

        Returns:
        str: String representation of the Node object.
        '''
        if not self._node_as_string :
            self._node_as_string: str = str(self._id)

        return self._node_as_string

    def set_id(self, node_id: int) -> None:
        '''
        Sets the unique identifier of the node.

        Parameters:
        - id(int): Unique identifier of the node.
        '''
        self._id = node_id
        self._node_as_string = ""

    def get_id(self) -> int:
        '''
        Returns:
        int: Unique identifier of the node.
        '''
        return self._id

    def __eq__(self, other: 'Node')-> bool:
        '''
        Compares two Node objects.

        Parameters:
        - other(Node): Node object to compare.

        Returns:
        bool: True if the two Node objects are equal, False otherwise.
        '''
        if not isinstance(other, Node):
            return False

        same_in_arc: bool = len(self.in_arcs) == len(other.in_arcs) and all(a == b for a, b in zip(self.in_arcs, other.in_arcs))
        same_out_arc: bool = len(self.out_arcs) == len(other.out_arcs) and all(a == b for a, b in zip(self.out_arcs, other.out_arcs))
        
        return self.state == other.state and same_in_arc and same_out_arc
    
    def __lt__(self, other: 'Node') -> bool:
        '''
        Compares two Node objects.

        Parameters:
        - other(Node): Node object to compare.

        Returns:
        bool: True if the current Node object is less than the other Node object, False otherwise.
        '''
        return self.get_id() < other.get_id()