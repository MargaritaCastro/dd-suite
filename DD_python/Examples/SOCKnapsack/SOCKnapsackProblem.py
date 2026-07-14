import math
from SourceCode.Problems.AbstractProblemClass import AbstractProblem


class SOCKnapsackProblem(AbstractProblem):

    def __init__(self, params: 'SOCKnapsackInstance', constraint_index: int, sort: bool = True):
        '''
        Initialize the SOCKnapsack Problem instance.

        Parameters:
        - params (SOCKnapsackInstance): Instance of the SOC knapsack problem, containing:
            * initial_state: initial state list (will be copied).
            * variables (list[tuple[str, list[int]]]): variable names and their domains.
            * linear_weights (list[list[int]]): linear weights per constraint.
            * sqrt_weights (list[list[int]]): sqrt-term weights per constraint.
            * omega (int): scaling factor for the square-root term.
            * right_side_of_restrictions (list[int]): RHS per constraint.
        - constraint_index (int): index of the SOC constraint to model.
        - sort (bool): whether to sort variables by heuristic order.
        '''
        initial_state = params.initial_state[:]
        linear_weights = params.linear_weights[constraint_index]
        sqrt_weights = params.sqrt_weights[constraint_index]

        self.linear_weights: list[int] = []
        self.sqrt_weights: list[int] = []
        self.sqrt_weights_2: list[int] = []
        self.capacity: int = params.right_side_of_restrictions[constraint_index]
        self.omega: int = params.omega

        # Maps each original variable name to its 0-based index in the full (unpruned) variable list.
        # Use this to map from DD variables back to the Gurobi model's variable ordering.
        self.original_name_to_index: dict[str, int] = {name: i for i, (name, _) in enumerate(params.variables)}

        self._pruned_variables: list[tuple['Variable', 'Domain']] = []
        self._pruned_linear_weights: list[int] = []
        self._pruned_sqrt_weights: list[int] = []
        self._prune(params.variables, linear_weights, sqrt_weights)

        # Defaults for sort=False (overwritten by sort_variables when sort=True)
        self._sorted_linear_weights = self._pruned_linear_weights[:]
        self._sorted_sqrt_weights = self._pruned_sqrt_weights[:]

        super().__init__(initial_state, self._pruned_variables, sort)

        # AbstractProblem initializes dd_to_original_index as identity over pruned variables,
        # but SOCKnapsack needs indices into the full original variable list.
        if not sort:
            self.dd_to_original_index = [self.original_name_to_index[name] for name, _ in self._pruned_variables]

        self.linear_weights = self._sorted_linear_weights
        self.sqrt_weights = self._sorted_sqrt_weights
        self.sqrt_weights_2 = [x ** 2 for x in self.sqrt_weights]

        self.min_linear_weight = []
        self.min_sqrt_weight = []
        self.sum_linear_weight = []
        self.sum_sqrt_weight = []

        self._compute_min_and_sum_for_pruning()

        self.count_1 = 0
        self.count_2 = 0

        assert len(self.linear_weights) == len(self.ordered_variables), "weights and variables must have the same length"
        assert len(self.sqrt_weights) == len(self.ordered_variables), "weights and variables must have the same length"

    def sort_variables(self, variables: list[tuple['Variable', 'Domain']]) -> list[str]:
        '''
        Sort variables by decreasing ordering weight (linear + omega * sqrt) and build
        self.dd_to_original_index so callers can map DD position → original Gurobi index.
        '''
        paired = sorted(
            zip(variables, self._pruned_linear_weights, self._pruned_sqrt_weights),
            key=lambda x: x[1] + self.omega * x[2],
            reverse=True
        )

        self.dd_to_original_index: list[int] = [
            self.original_name_to_index[name] for name, _ in (item[0] for item in paired)
        ]
        self._sorted_linear_weights = [item[1] for item in paired]
        self._sorted_sqrt_weights = [item[2] for item in paired]

        return [name for (name, _), _, _ in paired]

    def _prune(self, variables: list[tuple['Variable', 'Domain']],
                                     linear_weights: list[int], sqrt_weights: list[int]) -> None:
        '''
        Remove variables whose linear and sqrt weights are both zero and collect
        the pruned weight lists aligned with self._pruned_variables.
        '''
        for i in range(len(linear_weights)):
            if linear_weights[i] != 0 or sqrt_weights[i] != 0:
                self._pruned_variables.append(variables[i])
                self._pruned_linear_weights.append(linear_weights[i])
                self._pruned_sqrt_weights.append(sqrt_weights[i])

    def _compute_min_and_sum_for_pruning(self):
        for i in range(len(self.linear_weights) - 1):
            self.min_linear_weight.append(min(self.linear_weights[i + 1:]))
            self.min_sqrt_weight.append(min(self.sqrt_weights_2[i + 1:]))
            self.sum_linear_weight.append(sum(self.linear_weights[i + 1:]))
            self.sum_sqrt_weight.append(sum(self.sqrt_weights_2[i + 1:]))

    def transition_function(self, previous_state: 'State', variable_index: int, variable_value: int, scratch_state: list) -> bool:
        if previous_state[0] < 0:
            scratch_state[0][0] = previous_state[0]
            scratch_state[0][1] = previous_state[1]
            return variable_value == 0 or previous_state[0] == -2

        if variable_value == 0:
            scratch_state[0][0] = previous_state[0]
            scratch_state[0][1] = previous_state[1]
        else:
            scratch_state[0][0] = previous_state[0] + self.linear_weights[variable_index] * variable_value
            scratch_state[0][1] = previous_state[1] + self.sqrt_weights_2[variable_index] * variable_value
            if scratch_state[0][0] + self.omega * math.sqrt(scratch_state[0][1]) > self.capacity:
                return False

        if variable_index < len(self.min_linear_weight):
            if self._is_only_zero_arcs_feasible_in_next_layers(scratch_state[0], variable_index):
                scratch_state[0][0] = -1
                scratch_state[0][1] = -1
                self.count_1 += 1
            elif scratch_state[0][0] >= 0 and self._is_one_arc_always_feasible_in_next_layers(scratch_state[0], variable_index):
                scratch_state[0][0] = -2
                scratch_state[0][1] = -2
                self.count_2 += 1

        return True

    def _is_only_zero_arcs_feasible_in_next_layers(self, new_state: 'State', variable_index : int) -> bool:
        if (new_state[0] + self.min_linear_weight[variable_index]
                + self.omega*math.sqrt(new_state[1] + self.min_sqrt_weight[variable_index]) > self.capacity):
            return True

        return False

    def _is_one_arc_always_feasible_in_next_layers(self, new_state: 'State', variable_index : int) -> bool:

        if (new_state[0] + self.sum_linear_weight[variable_index]
                + self.omega*math.sqrt(new_state[1] + self.sum_sqrt_weight[variable_index]) <= self.capacity):
            return True

        return False

    def get_priority_for_discard_node(self, state: 'State') -> int:
        '''
        Get the priority for discard a node.

        Parameters:
        - state ('State'): State of the node.
        '''
        if state[0] < 0:
            return state[0]
        return -(state[0] + self.omega * math.sqrt(state[1]))

    def get_priority_for_merge_nodes(self, id: int, state: 'State') -> int:
        '''
        Get the priority for merge two nodes.

        Parameters:
        - id (int): Id of the node.
        - state ('State'): State of the node.
        '''
        if state[0] < 0:
            return int(state[0])
        return -int(state[0] + self.omega * math.sqrt(state[1]))

    def merge_operator(self, state_one: 'State', state_two: 'State') -> 'State':
        '''
        Merge two states.

        Parameters:
        - state_one ('State'): First state.
        - state_two ('State'): Second state.

        Returns:
        - 'State': Merged state.
        '''
        # Sentinel -2 (always feasible) dominates: merging with anything stays always-feasible.
        # Sentinel -1 (only zero-arcs feasible) merged with a normal state [l>=0, s>=0] must
        # yield the normal state, because the normal state is more permissive. Taking min would
        # produce [-1,-1] and incorrectly block 1-arcs that the normal-state node allowed.
        s1_always = state_one[0] == -2
        s2_always = state_two[0] == -2
        if s1_always or s2_always:
            return [-2, -2]
        s1_zero_only = state_one[0] == -1
        s2_zero_only = state_two[0] == -1
        if s1_zero_only and not s2_zero_only:
            return state_two[:]
        if s2_zero_only and not s1_zero_only:
            return state_one[:]
        return [min(a, b) for a, b in zip(state_one, state_two)]

    def get_state_as_string(self, state: 'State') -> str:
        '''
        Get the state as a string.

        Parameters:
        - state ('State'): State of a node.
        '''
        return f"{state[0]},{state[1]}"

    def get_state_copy(self, state: 'State') -> 'State':
        '''
        Copy of a state.

        Parameters:
        - state(State): The state to copy. *READ IMPORTANT

        Returns:
        - State: A copy of the state.
        '''
        return state[:]
