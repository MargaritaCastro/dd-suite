
class Arc():
    '''
    SourceCode representing an arc in a graph.
    '''

    def __init__(self, parent_node: 'Node', child_node: 'Node', variable_value: int):
        '''
        Constructor for the Arc class.

        Parameters:
        - parent_node(Node): Node from which the arc originates.
        - child_node(Node): Node to which the arc is directed.
        - variable_value(int): Value of the variable associated with the arc.
        '''
        self.parent_node: 'Node' = parent_node
        self.child_node: 'Node' = child_node
        self.variable_value: int = variable_value
        self.arc_id: int = -1
    
    def __eq__(self, other: 'Arc') -> bool:
        '''
        Compares two Arc objects.

        Parameters:
        - other(Arc): Arc object to compare.

        Returns:
        bool: True if the two Arc objects are equal, False otherwise.
        '''
        if isinstance(other, Arc):
            return (self.parent_node.get_id() == other.parent_node.get_id() and
                    self.child_node.get_id() == other.child_node.get_id() and
                    self.variable_value == other.variable_value)
        return False

    def to_string(self, variable_name: str = None) -> str:
        '''
        Returns a string representation of the Arc object.

        Parameters:
        - variable_name(str): Optional variable name to include in the output.

        Returns:
        str: String representation of the Arc object.
        '''
        value_part = f"({variable_name}={self.variable_value})" if variable_name else f"({self.variable_value})"
        return f"arc_{self.parent_node.to_string()}_{self.child_node.to_string()}{value_part}"