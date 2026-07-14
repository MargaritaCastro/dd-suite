from SourceCode.Problems.AbstractProblemClass import AbstractProblem


class KnapsackProblem(AbstractProblem):

    def __init__(self, params: 'KnapsackStructure', sort: bool = True):
        '''
        Initialize the KnapsackInstance Problem instance.

        Parameters:
        - params (KnapsackStructure): Instance of the knapsack problem, containing:
            * initial_state: initial capacity state.
            * variables (list[tuple[str, list[int]]]): variable names and their domains.
            * weights (list[int]): weights of each item.
            * right_side_of_restrictions (int): maximum capacity.
        - sort (bool): whether to sort variables by heuristic order.
        '''
        super().__init__(params.initial_state, params.variables, sort)

        self.weights: list[int] = params.weights
        self.capacity: int = params.right_side_of_restrictions

        assert len(self.weights) == len(params.variables), "weights and variables must have the same length"

    def transition_function(self, previous_state: 'State', variable_index: int, variable_value: int, scratch_state: list) -> bool:
        if variable_value == 0:
            scratch_state[0] = previous_state
            return True
        new_val = previous_state + self.weights[variable_index] * variable_value
        scratch_state[0] = new_val
        return new_val <= self.capacity
    
    def get_priority_for_discard_node(self, state: 'State') -> int:
        '''
        Get the priority for discard a node.

        Parameters:
        - state ('State'): State of the node.
        '''
        return -state
    
    def get_priority_for_merge_nodes(self, id: int, state: 'State') -> int:
        '''
        Get the priority for merge two nodes.

        Parameters:
        - id (int): Id of the node.
        - state ('State'): State of the node.
        '''
        return -state

    def merge_operator(self, state_one: 'State', state_two: 'State') -> 'State':
        '''
        Merge two states.

        Parameters:
        - state_one ('State'): First state.
        - state_two ('State'): Second state.

        Returns:
        - 'State': Merged state.
        '''
        return min(state_one, state_two)
    
    def get_state_as_string(self, state: 'State') -> str:
        '''
        Get the state as a string.

        Parameters:
        - state ('State'): State of a node.
        '''
        return str(state)
    
    def get_state_copy(self, state: 'State') -> 'State':
        '''
        Copy of a state.

        Parameters:
        - state(State): The state to copy. *READ IMPORTANT

        Returns:
        - State: A copy of the state.
        '''
        copy_state = state
        return copy_state
        

