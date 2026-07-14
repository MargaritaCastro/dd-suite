from SourceCode.DDStructure.Arc import Arc


class PathStructure:
    '''Structure to store information about the ShortestLongestPath algorithm.'''

    def __init__(self, value=0,arc=None):
        ''' 
        Constructor of the class.
        
        Parameters:
        - value: Value of the objective.
        - path: Path of the objective.
        - arcs: List of arcs.
        - previous: Previous objective.
        '''
        self.value: float = value
        self.arc: 'Arc'  = arc

    def reset(self):
        '''
        Resets the path structure to its initial state.

        Sets value to infinity and clears the associated arc reference.
        '''
        self.value: float = float('inf')
        self.arc: 'Arc' = None

class PathStructureSolution:

    def __init__(self, value=-111111, path="", arcs=[]):
        '''
        Constructor of the class.

        Parameters:
        - value: Value of the objective.
        - path: Path in string format explaining the route that results in the value.
        - arcs: List of arc objects in the same order as the path_print.
        '''
        self.value: float = value
        self.path_print: str = path
        self.path_arcs: list['Arc'] = arcs