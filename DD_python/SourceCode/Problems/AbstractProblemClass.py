from abc import ABC, abstractmethod


class AbstractProblem(ABC):
    '''
    This class provides an interface for users to implement their own Problem classes.
    These can be used with the provided DD class to solve decision diagrams.

    *IMPORTANT: Whenever 'state' is mentioned, it must follow the same type as given in initial_state.
    '''

    def __init__(self, initial_state: 'State', variables: list[tuple['Variable', 'Domain']], sort: bool = True) -> None:
        '''
        Constructor for the AbstractProblem class.

        Parameters:
        - initial_state (State): The initial state of the problem, which can be a list, int,
        string, etc., depending on what is required for the problem.
        - variables (list[tuple['Variable', 'Domain']]): A list of tuples containing the
        variables of the problem and their domain.
        '''
        self.initial_state: 'State' = initial_state
        self.variables_domain: dict[str, list[int]] = dict(variables)
        self.dd_to_original_index: list[int] = list(range(len(variables)))
        self.ordered_variables: list[str] = (
            self.sort_variables(variables) if sort else [name for name, _ in variables]
        )

        # Bidirectional mapping between variable names and their position index (0-based).
        # Use these instead of parsing names (e.g. int("x_3"[2:]) - 1) anywhere in the code.
        self.name_to_index: dict[str, int] = {name: i for i, name in enumerate(self.ordered_variables)}
        self.index_to_name: dict[int, str] = {i: name for i, name in enumerate(self.ordered_variables)}

    def sort_variables(self, variables: list[tuple['Variable', 'Domain']], **kwargs) -> list[str]:
        '''
        Override to define a custom variable ordering. Called during __init__ and must return
        the ordered variable names ready to assign to self.ordered_variables.
        Subclasses may use self attributes or **kwargs to compute the ordering.

        Parameters:
        - variables (list[tuple['Variable', 'Domain']]): The original variable list.
        - **kwargs: Any additional data the subclass needs to compute the ordering.

        Returns:
        - list[str]: Variable names in the desired processing order.
        '''
        return [name for name, _ in variables]

    @abstractmethod
    def transition_function(self, previous_state: 'State', variable_id: int, variable_value: int, scratch_state: list) -> bool:
        '''
        Abstract method to define the transition function.

        Writes the new state into scratch_state[0] and returns True if the transition is feasible.
        scratch_state[0] must not be read on a False return.

        Parameters:
        - previous_state('State'): The previous state. *READ IMPORTANT
        - variable_id(int): The identifier of the variable being modified.
        - variable_value(int): The value of the variable being modified.
        - scratch_state(list): Single-element list used as a scratch buffer; set scratch_state[0] to the new state.

        Returns:
        - bool: True if the new state is feasible, False otherwise.
        '''
        raise NotImplementedError("The method transition_function has not been implemented yet")
    
   
    def get_priority_for_discard_node(self, state: 'State') -> int:
        '''
        Abstract method to define the priority function for discarding nodes, i.e., 
        the node with the highest priority will be discarded first.

        Parameters:
        - state('State'): The state of the node to order. *READ IMPORTANT

        Returns:
        - int: The priority value of the state.
        '''
        raise NotImplementedError("The method get_priority_for_discard_node has not been implemented yet")

   
    def get_priority_for_merge_nodes(self, node_id: int, state: 'State') -> int:
        '''
        Abstract method to define the priority function for merging nodes. 
        After ordering them, the two nodes with the highest priorities will be merged.

        Parameters:
        - node_id(int): The identifier of the node to order.
        - state('State'): The state of the node to order. *READ IMPORTANT

        Returns:
        - int: The priority value of the node.
        '''
        raise NotImplementedError("The method get_priority_for_merge_nodes has not been implemented yet")

    
    def merge_operator(self, state_one: 'State', state_two:'State') -> 'State':
        '''
        Abstract method to define a new state when two nodes are merged.

        Parameters:
        - state_one(State): The first state to merge. *READ IMPORTANT
        - state_two(State): The second state to merge. *READ IMPORTANT

        Returns:
        - State: The resulting state from the merge.
        '''
        raise NotImplementedError("The method merge_operator has not been implemented yet")
    
    @abstractmethod
    def get_state_as_string(self, state: 'State') -> str:
        '''
        Abstract method to define a string representation of the state.

        Parameters:
        - state(State): The state to represent as a string. *READ IMPORTANT

        Returns:
        - str: The string representation of the state.
        '''
        raise NotImplementedError("The method get_state_as_string has not been implemented yet")
    
    @abstractmethod
    def get_state_copy(self, state: 'State') -> 'State':
        '''
        Abstract method to define a copy of a state.

        Parameters:
        - state(State): The state to copy. *READ IMPORTANT

        Returns:
        - State: A copy of the state.
        '''
        raise NotImplementedError("The method get_state_copy has not been implemented yet")
    
    def get_final_state(self) -> 'State':
        '''
        Abstract method to define the final state of the problem.

        Returns:
        - State: The final state of the problem.
        '''
        return self.initial_state