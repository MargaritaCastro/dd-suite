import os
import sys
from bitarray import bitarray

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

from SourceCode.Problems.AbstractProblemClass import AbstractProblem

class IndependentSetProblem(AbstractProblem):

    def __init__(self, params: 'IndependentSetStructure', sort: bool = True) -> None:
        '''
        Initialize the Independent Set Problem instance.

        Parameters:
        - params (IndependentSetStructure): Instance of the independent set problem, containing:
            * initial_state: initial bitset state.
            * variables (list[tuple[str, list[int]]]): variable names and their domains.
            * dict_node_neighbors (dict[str, list[int]]): adjacency list per node.
        - sort (bool): whether to sort variables by heuristic order.
        '''
        self._dict_node_neighbors = params.dict_node_neighbors
        super().__init__(params.initial_state, params.variables, sort)

        n = len(params.dict_node_neighbors)
        self.node_neighbors: list[bitarray] = [bitarray(n) for _ in range(n)]
        for bs in self.node_neighbors:
            bs.setall(False)

        for key, neighbors in params.dict_node_neighbors.items():
            var_index = self.name_to_index[key]
            for neighbor_num in neighbors:
                self.node_neighbors[var_index][self.name_to_index[f"x_{neighbor_num}"]] = True

        self.check_atributes(params.variables, params.dict_node_neighbors)


    def sort_variables(self, variables: list[tuple['Variable', 'Domain']]) -> list[str]:
        sorted_names = sorted(
            [name for name, _ in variables],
            key=lambda name: len(self._dict_node_neighbors[name]),
            reverse=True
        )
        name_to_original_index = {name: i for i, (name, _) in enumerate(variables)}
        self.dd_to_original_index = [name_to_original_index[name] for name in sorted_names]
        return sorted_names

    def check_atributes(self, variables: list[tuple['Variable', 'Domain']], dict_node_neighbors: dict[str, list[int]]) -> None:
        ''' 
        Check the attributes of the problem.

        Parameters:
        - variables (list[tuple['Variable', 'Domain']]): List of tuples with the variables and their domains.
        '''
        self.check_same_variables(variables,dict_node_neighbors)
        self.check_neighbors_must_be_integers(dict_node_neighbors)
        self.check_consistent_dictionary_of_neighbors(dict_node_neighbors)

    def check_same_variables(self, variables: list[tuple['Variable', 'Domain']], dict_node_neighbors: dict[str, list[int]]) -> None:
        '''
        Check if the variables are the same between the dictionary of neighbors and the list of variables.

        Parameters:
        - variables (list[tuple['Variable', 'Domain']]): List of tuples with the variables and their domains.
        '''
        assert dict(variables).keys() == dict_node_neighbors.keys(), "Variables must be the same between dictionaries"
    
    def check_neighbors_must_be_integers(self, dict_node_neighbors: dict[str, list[int]]) -> None:
        ''' 
        Check if the neighbors are integers.
        '''
        for key in dict_node_neighbors.keys():
            for value in dict_node_neighbors.get(key, []):
                assert isinstance(value, int), "Values must be integers"
    
    def check_consistent_dictionary_of_neighbors(self, dict_node_neighbors: dict[str, list[int]]) -> None:
        '''
        Check if the dictionary of neighbors is consistent.
        '''
        for key in dict_node_neighbors.keys():
            for value in dict_node_neighbors.get(key, []):
                assert int(key[2:]) in dict_node_neighbors.get("x_"+str(value), []), "Dictionary of neighbors must be consistent"

    def transition_function(self, previous_state: 'State', variable_index: int, variable_value: int, scratch_state: list) -> bool:
        if variable_value == 0:
            scratch_state[0][:] = previous_state
            if previous_state[variable_index]:
                scratch_state[0][variable_index] = False
            return True
        if not previous_state[variable_index]:
            return False
        scratch_state[0][:] = previous_state
        scratch_state[0] &= ~self.node_neighbors[variable_index]
        scratch_state[0][variable_index] = False
        return True

    def get_priority_for_discard_node(self, state: 'State') -> int:
        '''
        Get the priority for discard a node.

        Parameters:
        - state ('State'): State of the problem.

        Returns:
        - int: Priority for discard the node.
        '''
        return state.count(True)
    
    def get_priority_for_merge_nodes(self, node_id: int, state: 'State') -> int:
        '''
        Get the priority for merge two nodes.

        Parameters:
        - id (int): Id of the node.
        - state ('State'): State of the problem.

        Returns:
        - int: Priority for merge the nodes.
        '''
        return state.count(True)

    def merge_operator(self, state_one: 'State', state_two: 'State') -> 'State':
        '''
        Merge two states.

        Parameters:
        - state_one ('State'): First state.
        - state_two ('State'): Second state.

        Returns:
        - 'State': Merged state.
        '''
        return state_one | state_two
    
    def get_state_as_string(self, state: 'State') -> str:
        '''
        Get the state as a string.

        Parameters:
        - state ('State'): State of a node.

        Returns:
        - str: State as a string.
        '''
        return state.to01()
    
    def get_state_copy(self, state: 'State') -> 'State':
        '''
        Copy of a state.

        Parameters:
        - state(State): The state to copy. *READ IMPORTANT

        Returns:
        - State: A copy of the state.
        '''
        return state.copy()
    
    def get_final_state(self) -> 'State':
        '''
        Final state of the problem.

        Returns:
        - State: The final state of the problem.
        '''
        final_state: list[int] = [0] * len(self.node_neighbors)
        return self.convert_to_bitset(final_state, len(self.node_neighbors))

    def convert_to_bitset(self, vec, size):
        '''
        Convert a list to a bitset.
        '''
        bs: bitarray = bitarray(size)
        bs.setall(False)
        for value in vec:
            if value > 0:
                bs[value - 1] = True
        return bs
