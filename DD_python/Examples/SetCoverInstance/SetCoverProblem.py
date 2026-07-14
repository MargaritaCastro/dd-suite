from SourceCode.Problems.AbstractProblemClass import AbstractProblem
from bitarray import bitarray

class SetCoverProblem(AbstractProblem):

    def __init__(self, params: 'SetCoverStructure', sort: bool = True):
        '''
        Initialize the Set Cover Problem instance.

        Parameters:
        - params (SetCoverStructure): Instance of the set cover problem, containing:
            * initial_state: initial bitset state.
            * variables (list[tuple[str, list[int]]]): variable names and their domains.
            * matrix_of_weight (list[list[int]]): coverage matrix (rows=restrictions, cols=variables).
            * right_side_of_restrictions (list[int]): right-hand side of each restriction.
        - sort (bool): whether to sort variables by heuristic order.
        '''
        self._raw_matrix = params.matrix_of_weight
        super().__init__(params.initial_state, params.variables, sort)

        self.right_side_of_restrictions = self.convert_to_bitset(params.right_side_of_restrictions, len(params.matrix_of_weight))

        if not sort:
            self.matrix_of_weight = [
                self.convert_to_bitset(row, len(params.matrix_of_weight[0]))
                for row in params.matrix_of_weight
            ]
        self.matrix_of_weight_transpose = []

        self.check_atributes(params.variables, params.initial_state)
        self.create_transpose_matrix_of_weights(self.matrix_of_weight)
        self.calculate_row_max_index()

    def sort_variables(self, variables: list[tuple['Variable', 'Domain']]) -> list[str]:
        n_restrictions = len(self._raw_matrix)
        coverage = [
            sum(self._raw_matrix[row][i] for row in range(n_restrictions))
            for i in range(len(variables))
        ]
        self.dd_to_original_index = sorted(range(len(variables)), key=lambda i: -coverage[i])

        sorted_matrix = [[row[i] for i in self.dd_to_original_index] for row in self._raw_matrix]

        self.matrix_of_weight = [
            self.convert_to_bitset(row, len(sorted_matrix[0]))
            for row in sorted_matrix
        ]
    
        return [variables[i][0] for i in self.dd_to_original_index]

    def check_atributes(self, variables: list[tuple['Variable', 'Domain']], initial_state: 'State') -> None:
        '''
        Check the attributes of the problem.

        Parameters:
        - variables (list[tuple['Variable', 'Domain']]): List of tuples with the variables and their domains.
        - initial_state ('State'): Initial state of the problem.
        '''
        self.check_same_len_matrix_and_right_side(initial_state)
        self.check_same_len_rows_matrix_and_variables(variables)
    
    def check_same_len_matrix_and_right_side(self, initial_state: 'State') -> None:
        '''
        Check if the matrix of weight and the right side of restrictions have the same length.

        Parameters:
        - initial_state ('State'): Initial state of the problem.
        '''
        assert len(self.matrix_of_weight) == len(self.right_side_of_restrictions), "matrix_of_weight and right_side_of_restrictions must have the same length"
        assert len(initial_state) == len(self.right_side_of_restrictions) or len(initial_state) == 2*len(self.right_side_of_restrictions), "matrix_of_weight and right_side_of_restrictions must have the same length"
    
    def check_same_len_rows_matrix_and_variables(self, variables: list[tuple['Variable', 'Domain']]) -> None:
        '''
        Check if the rows of the matrix of weight and the right side of restrictions have the same length of the variables.

        Parameters:
        - variables (list[tuple['Variable', 'Domain']]): List of tuples with the variables and their domains.
        '''
        for row in range(len(self.matrix_of_weight)):
            assert len(self.matrix_of_weight[row]) == len(variables), "rows of matrix_of_weight and right_side_of_restrictions must have the same length of variables"

    def convert_to_bitset(self, vec, size):
        '''
        Convert a vector to a bitarray of a given size.

        Parameters:
        - vec (list[int]): Vector to convert. Values greater than 0 are set to True.
        - size (int): Size of the resulting bitarray.

        Returns:
        - bitarray: Bitarray representation of the vector.
        '''
        bs: bitarray = bitarray(size)
        bs.setall(False)
        for i, value in enumerate(vec):
            if value > 0:
                bs[i] = True
        return bs

    def create_transpose_matrix_of_weights(self, vector_matrix_of_weight: list[list[int]]) -> None:
        '''
        Build the transpose of the weight matrix as a list of bitarrays.

        Parameters:
        - vector_matrix_of_weight (list[list[int]]): Original weight matrix where each row
          represents a restriction and each column represents a variable.
        '''
        rows = len(vector_matrix_of_weight)
        var_num = len(vector_matrix_of_weight[0])

        self.matrix_of_weight_transpose = [bitarray(rows) for i in range(var_num) ]

        for i in range(rows):
            for j in range(var_num):
                if vector_matrix_of_weight[i][j] == 1:
                    self.matrix_of_weight_transpose[j][i] = True
    
    def calculate_row_max_index(self) -> None:
        '''
        Calculate the index of the last variable (column) set to 1 for each restriction (row).

        Populates self.max_indices where each entry is the 1-based index of the last
        True bit in the corresponding row of the weight matrix, or -1 if no bit is set.
        '''
        self.max_indices = []
        for row in self.matrix_of_weight:
            max_index = -1
            for col in range(len(row) - 1, -1, -1):
                if row[col]:
                    max_index = col + 1
                    break
            self.max_indices.append(max_index)

    def transition_function(self, previous_state: 'State', variable_index: int, variable_value: int, scratch_state: list) -> bool:
        if variable_value == 0:
            for row in range(len(previous_state)):
                if self.max_indices[row] != -1 and variable_index >= self.max_indices[row] - 1:
                    if previous_state[row]:
                        return False
            scratch_state[0][:] = previous_state
            return True
        scratch_state[0][:] = previous_state
        scratch_state[0] &= ~self.matrix_of_weight_transpose[variable_index]
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

        Returns:
        - int: Priority for merge the nodes.
        '''
        return state.count()

    def merge_operator(self, state_one: 'State', state_two: 'State') -> 'State':
        '''
        Merge operator of the problem.

        Parameters:
        - state_one ('State'): First state.
        - state_two ('State'): Second state.

        Returns:
        - 'State': Merged state.
        '''
        return state_one & state_two
    
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
        final_state = [0] * len(self.matrix_of_weight)
        return self.convert_to_bitset(final_state, len(self.matrix_of_weight))
