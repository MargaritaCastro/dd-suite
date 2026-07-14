import os
import sys
import math

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

from SourceCode.DDBuilder.AbstractDDBuilder import AbstractDDBuilder


class RelaxedGroupingDDBuilder(AbstractDDBuilder):
    '''
    Builds a relaxed decision diagram. Instead of merging the two highest-priority nodes one
    pair at a time, it merges nodes in groups according to how close their merge priorities are.

    The layer is reduced in passes with an increasing tolerance `difference`. In each pass the
    nodes are sorted by priority and consecutive nodes whose priority gap is <= difference are
    merged together into a single node. The tolerance starts at 0 (only equal priorities are
    merged) and grows by one until the layer fits within the maximum width.
    '''

    def __init__(self, problem: 'AbstractProblem', max_width: int) -> None:
        '''
        Constructor for the RelaxedGroupingDDBuilder class.

        Parameters:
        - problem(AbstractProblem): Problem object for which the graph will be constructed.
        - max_width(int): Maximum allowed width for the graph.
        '''
        super().__init__(problem)
        self._max_width: int = max_width

    def _specific_end_of_layer_function(self) -> None:
        '''
        Specific function to apply at each layer. This function merges nodes when the graph width
        exceeds the specified maximum.
        '''
        if len(self.graph.structure[-1]) > self._max_width:
            self._merge_nodes_when_width_is_greater_than_w()
            self._delete_nodes_current_layer(self.graph.actual_layer)

    def _specific_end_of_construction_function(self) -> None:
        '''
        Specific function to apply at the final layer. This function updates the number of nodes.
        '''
        self._bottom_up_pruner()
        self.adjust_node_number()

    def _merge_nodes_when_width_is_greater_than_w(self) -> None:
        '''
        Merges nodes when the width of the graph exceeds the specified maximum, grouping nodes by
        increasing priority difference.
        '''
        # Cache each node's priority once; carry (priority, node) pairs sorted by priority.
        alive: list = sorted(
            ((self._merge_priority(node), node) for node in self.graph.structure[-1]),
            key=lambda pair: pair[0],
        )
        difference: float = 0

        while len(alive) > self._max_width:
            alive = self._merge_nodes_with_difference(alive, difference)
            if len(alive) <= self._max_width:
                break
            # Skip tolerances that would merge nothing: jump to the next gap that does.
            difference = max(difference + 1, math.ceil(self._minimum_gap(alive)))

    def _merge_nodes_with_difference(self, alive: list, difference: float) -> list:
        '''
        Performs a single reduction pass over priority-sorted (priority, node) pairs: merges every
        run of consecutive nodes whose priority gap is <= difference. Stops merging once the
        maximum width is reached.

        Parameters:
        - alive(list): (priority, node) pairs sorted by priority.
        - difference(float): Maximum priority gap allowed between consecutive nodes of a group.

        Returns:
        - list: (priority, node) pairs alive after this pass, sorted by priority.
        '''
        new_alive: list = []
        width: int = len(alive)
        total: int = len(alive)
        index: int = 0

        while index < total:
            group: list = [alive[index]]
            next_index: int = index + 1

            while next_index < total and alive[next_index][0] - alive[next_index - 1][0] <= difference:
                group.append(alive[next_index])
                next_index += 1

            if len(group) > 1 and width > self._max_width:
                survivor: 'Node' = self._merge_group([pair[1] for pair in group])
                width -= (len(group) - 1)
                new_alive.append((self._merge_priority(survivor), survivor))
            else:
                new_alive.extend(group)

            index = next_index

        new_alive = self._deduplicate_pairs(new_alive)
        new_alive.sort(key=lambda pair: pair[0])
        return new_alive

    def _minimum_gap(self, alive: list) -> float:
        '''
        Returns the smallest priority gap between consecutive nodes of a priority-sorted list.

        Parameters:
        - alive(list): (priority, node) pairs sorted by priority (at least two pairs).

        Returns:
        - float: The smallest consecutive priority gap.
        '''
        return min(alive[i][0] - alive[i - 1][0] for i in range(1, len(alive)))

    def _merge_group(self, group: list) -> 'Node':
        '''
        Merges every node of a group into a single surviving node.

        Parameters:
        - group(list): Nodes to merge together (at least two).

        Returns:
        - Node: The surviving node after merging the whole group.
        '''
        survivor: 'Node' = group[0]

        for node in group[1:]:
            survivor = self._merge_two_nodes(node, survivor)

        return survivor

    def _merge_two_nodes(self, node_to_remove: 'Node', node_to_keep: 'Node') -> 'Node':
        '''
        Merges two nodes and returns the surviving node.

        Parameters:
        - node_to_remove(Node): Node that is absorbed by the merge.
        - node_to_keep(Node): Node that is kept as the merge anchor.

        Returns:
        - Node: The surviving node (either node_to_keep or a pre-existing node with the merged state).
        '''
        if node_to_remove is node_to_keep:
            return node_to_keep

        state_to_remove: str = self._problem.get_state_as_string(node_to_remove.state)
        state_to_keep: str = self._problem.get_state_as_string(node_to_keep.state)

        self._map_of_states.pop(state_to_remove, None)
        self._map_of_states.pop(state_to_keep, None)

        new_state: 'State' = self._problem.merge_operator(node_to_remove.state, node_to_keep.state)
        state_as_string: str = self._problem.get_state_as_string(new_state)

        if state_as_string in self._map_of_states:
            exist_node: 'Node' = self._map_of_states[state_as_string]
            self._merge_with_an_existing_node(node_to_remove, node_to_keep, exist_node)
            return exist_node

        self._merge_when_doesnt_exist_node(node_to_remove, node_to_keep, new_state)
        self._map_of_states[state_as_string] = node_to_keep
        return node_to_keep

    def _merge_priority(self, node: 'Node') -> float:
        '''
        Returns the merge priority of a node.

        Parameters:
        - node(Node): Node whose priority is computed.

        Returns:
        - float: The priority value used to sort and group nodes.
        '''
        return self._problem.get_priority_for_merge_nodes(node.get_id(), node.state)

    def _deduplicate_pairs(self, pairs: list) -> list:
        '''
        Removes repeated (priority, node) pairs (by node identity) keeping the original order.
        Needed because a merge can produce a state that already belongs to another surviving node.

        Parameters:
        - pairs(list): (priority, node) pairs that may contain duplicate nodes.

        Returns:
        - list: Pairs without duplicate nodes, in order.
        '''
        seen: set = set()
        unique: list = []

        for priority, node in pairs:
            if id(node) not in seen:
                seen.add(id(node))
                unique.append((priority, node))

        return unique

    def _merge_with_an_existing_node(self, node_one: 'Node', node_two: 'Node', available_node: 'Node') -> None:
        '''
        Merges two nodes with an existing node.

        Parameters:
        - node_one(Node): First node to merge.
        - node_two(Node): Second node to merge.
        - available_node(Node): Existing node with which the nodes are merged.
        '''
        self._redirect_in_arcs(node_one, available_node)
        self._redirect_in_arcs(node_two, available_node)

    def _merge_when_doesnt_exist_node(self, node_to_remove: 'Node', node_to_keep: 'Node', new_state: 'State') -> None:
        '''
        Merges two nodes when there is no existing node with the same state.

        Parameters:
        - node_to_remove(Node): Node to be removed.
        - node_to_keep(Node): Node to be kept.
        - new_state(State): New state resulting from the merge.
        '''
        self._redirect_in_arcs(node_to_remove, node_to_keep)
        self._change_new_state(node_to_keep, new_state)

    def _redirect_in_arcs(self, node_to_remove: 'Node', node_to_keep: 'Node') -> None:
        '''
        Redirects the incoming arcs of one node to another node.

        Parameters:
        - node_to_remove(Node): Node whose incoming arcs are redirected.
        - node_to_keep(Node): Node to which the arcs are redirected.
        '''
        for arc in node_to_remove.in_arcs:
            arc.child_node = node_to_keep
            node_to_keep.in_arcs.append(arc)

        node_to_remove.in_arcs = []

    def _change_new_state(self, node: 'Node', new_state: 'State') -> None:
        '''
        Changes the state of a node based on the states of the merged nodes.

        Parameters:
        - node(Node): Node whose state is changed.
        - new_state(State): New state to assign to the node.
        '''
        node.state = new_state
