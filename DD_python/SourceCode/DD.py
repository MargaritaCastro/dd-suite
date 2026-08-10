from SourceCode.DDBuilder.ExactDDBuilder.ExactDDBuilder import ExactDDBuilder
from SourceCode.GraphAlgorithms.DDReducer.DDReducer import DDReducer
from SourceCode.DDBuilder.RestrictedDDBuilder.RestrictedDDBuilder import RestrictedDDBuilder
from SourceCode.DDBuilder.RelaxedDDBuilder.RelaxedPriorityDDBuilder import RelaxedPriorityDDBuilder
from SourceCode.DDBuilder.RelaxedDDBuilder.RelaxedGroupingDDBuilder import RelaxedGroupingDDBuilder
from SourceCode.GraphVisualization.GraphFile import GraphFile
from Exceptions.MyExceptions import GraphAlreadyExists, NoGraphFound
import copy
import time


class DD():
    '''
    Decision Diagram (DD) class for creating and manipulating decision diagrams.
    '''

    def __init__(self, problem: 'AbstractProblem'):
        '''
        Constructor for the DD class.

        Parameters:
        problem(AbstractProblem): An instance of the problem class used to create the decision diagram.
        '''
        self._dd_builder_time: float = -0.00000000001
        self._dd_reduction_time: float = -0.0000000001

        self.problem: 'AbstractProblem' = problem
        self._graph_DD: 'Graph' = None
        self._dd_kind: str = 'exact'

    def create_decision_diagram(self, verbose: bool = False) -> None:
        '''
        Creates the decision diagram.

        Parameters:
        verbose (bool): If True, prints additional information during the creation process.
        '''

        if self._graph_DD is not None:
            raise GraphAlreadyExists()

        print("\nStarting the creation of the decision diagram...")
        start_time: float = time.time()  
        self.exact_dd_builder: 'ExactDDBuilder' = ExactDDBuilder(self.problem)
        self._graph_DD = self.exact_dd_builder.get_decision_diagram(verbose)
        self._dd_kind = 'exact'
        end_time: float = time.time()  
        self._dd_builder_time = round(end_time - start_time, 4)

        print(f"Decision diagram successfully created.")
    
    def create_restricted_decision_diagram(self, max_width: int, verbose: bool=False) -> None:
        '''
        Creates the restricted decision diagram.

        Parameters:
        max_width (int): Maximum width allowed for the decision diagram.
        verbose (bool): If True, prints additional information during the creation process.
        '''
        if self._graph_DD is not None:
            raise GraphAlreadyExists()

        print("\nStarting the creation of the restricted decision diagram...")
        start_time: float = time.time()  
        self.restricted_dd_builder: 'RestrictedDDBuilder' = RestrictedDDBuilder(self.problem, max_width)
        self._graph_DD: 'Graph' = self.restricted_dd_builder.get_decision_diagram(verbose)
        self._dd_kind = 'restricted'
        end_time: float = time.time()  
        self._dd_builder_time = round(end_time - start_time, 4)
        print(f"Restricted decision diagram creation completed.")
    
    def create_relax_priority_decision_diagram(self, max_width: int, verbose: bool=False) -> None:
        '''
        Creates the relaxed decision diagram merging nodes by priority.

        Parameters:
        max_width (int): Maximum width allowed for the decision diagram.
        verbose (bool): If True, prints additional information during the creation process.
        '''
        if self._graph_DD is not None:
            raise GraphAlreadyExists()

        print("\nStarting the creation of the relaxed decision diagram...")
        start_time: float = time.time()
        self.relaxed_dd_builder: 'RelaxedPriorityDDBuilder' = RelaxedPriorityDDBuilder(self.problem, max_width)
        self._graph_DD: 'Graph' = self.relaxed_dd_builder.get_decision_diagram(verbose)
        self._dd_kind = 'relaxed'
        end_time: float = time.time()
        self._dd_builder_time = round(end_time - start_time, 4)
        print(f"Relaxed decision diagram creation completed.")

    def create_relax_grouping_decision_diagram(self, max_width: int, verbose: bool=False) -> None:
        '''
        Creates the relaxed decision diagram merging nodes in groups by priority difference.

        Parameters:
        max_width (int): Maximum width allowed for the decision diagram.
        verbose (bool): If True, prints additional information during the creation process.
        '''
        if self._graph_DD is not None:
            raise GraphAlreadyExists()

        print("\nStarting the creation of the relax grouping decision diagram...")
        start_time: float = time.time()
        self.relaxed_dd_builder: 'RelaxedGroupingDDBuilder' = RelaxedGroupingDDBuilder(self.problem, max_width)
        self._graph_DD: 'Graph' = self.relaxed_dd_builder.get_decision_diagram(verbose)
        self._dd_kind = 'relaxed'
        end_time: float = time.time()
        self._dd_builder_time = round(end_time - start_time, 4)
        print(f"Relax grouping decision diagram creation completed.")

    def reduce_decision_diagram(self, verbose: bool=False) -> None:
        '''
        Creates the reduced decision diagram.

        Parameters:
        verbose (bool): If True, prints additional information during the reduction process.
        '''

        if self._graph_DD is None:
            raise NoGraphFound()

        print("\nStarting the reduction of the decision diagram...")
        start_time: float = time.time()

        self.reduce_dd_builder: 'DDReducer' = DDReducer(self._graph_DD, self.problem)
        self.reduce_dd_builder.reduce_decision_diagram(verbose)

        end_time: float = time.time()
        self._dd_reduction_time = round(end_time - start_time, 4)
        print(f"Reduced decision diagram creation completed.")

    def export_graph_file(self, file_name: str) -> None:
        '''
        Generates a .GML file with the current decision diagram.

        Parameters:
        file_name (str): The name of the file.
        '''
        if self._graph_DD is None or self._graph_DD.structure is []:
            raise NoGraphFound()

        GraphFile(file_name, self._graph_DD, self.problem)

    def get_decision_diagram(self) -> 'Graph':
        '''
        Returns the decision diagram graph object.

        Returns:
        Graph: The decision diagram graph.
        '''

        if self._graph_DD is None or self._graph_DD.structure is []:
            raise NoGraphFound()

        return self._graph_DD

    def get_decision_diagram_copy(self) -> 'Graph':
        '''
        Returns a deep copy of the decision diagram graph object.

        Returns:
        Graph: A deep copy of the decision diagram graph.
        '''
        if self._graph_DD is None or self._graph_DD.structure is []:
            raise NoGraphFound()

        return copy.deepcopy(self._graph_DD)
    
    def get_building_time(self) -> float:
        '''
        Returns the execution time of the DDBuilder.

        Returns:
        float: The execution time of the DDBuilder.
        '''
        return self._dd_builder_time

    def get_dd_kind(self) -> str:
        '''
        Returns which builder produced the diagram: 'exact', 'restricted' or
        'relaxed'. Reducing the diagram does not change it.

        Returns:
        str: The kind of decision diagram that was built.
        '''
        return self._dd_kind

    def get_reduction_time(self) -> float:
        '''
        Returns the execution time of the DDReducer.

        Returns:
        float: The execution time of the DDReducer.
        '''
        return self._dd_reduction_time

    @property
    def name_to_index(self) -> dict:
        '''
        Returns a dict mapping each variable's fancy name to its 0-based position index.
        Shortcut to self.problem.name_to_index.
        '''
        return self.problem.name_to_index

    @property
    def index_to_name(self) -> dict:
        '''
        Returns a dict mapping each 0-based position index to its variable's fancy name.
        Shortcut to self.problem.index_to_name.
        '''
        return self.problem.index_to_name
    
    