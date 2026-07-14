import os
import sys
import heapq

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

from SourceCode.DDBuilder.AbstractDDBuilder import AbstractDDBuilder

class RelaxedPriorityDDBuilder(AbstractDDBuilder):
    '''
    SourceCode that constructs a relaxed decision diagram graph based on a given problem.
    '''

    def __init__(self, problem: 'AbstractProblem', max_width: int) -> None:
        '''
        Constructor for the RelaxedPriorityDDBuilder class.

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
        Merges nodes when the width of the graph exceeds the specified maximum.
        '''
        self._nodes_set: list = []

        for node in self.graph.structure[-1]:  
            priority: int = -self._problem.get_priority_for_merge_nodes(node.get_id(), node.state)
            heapq.heappush(self._nodes_set, (priority, node))

        while len(self._nodes_set) > self._max_width:
            self._reduce_layer_width()  

    def _reduce_layer_width(self) -> None:
        '''
        Reduces the layer width by merging nodes based on their priority.
        '''
        node_to_remove: 'Node' = heapq.heappop(self._nodes_set)[1]
        node_to_keep: 'Node' = heapq.heappop(self._nodes_set)[1]

        state_to_remove: str = self._problem.get_state_as_string(node_to_remove.state)
        state_to_keep: str = self._problem.get_state_as_string(node_to_keep.state)

        self._map_of_states.pop(state_to_remove, None)
        self._map_of_states.pop(state_to_keep, None)

        new_state: 'State' = self._problem.merge_operator(node_to_remove.state, node_to_keep.state)
        state_as_string: str = self._problem.get_state_as_string(new_state)

        if state_as_string in self._map_of_states:
            exist_node: 'Node' = self._map_of_states[state_as_string]
            self._merge_with_an_existing_node(node_to_remove, node_to_keep, exist_node)
        else:
            self._merge_when_doesnt_exist_node(node_to_remove, node_to_keep, new_state)
            priority = -self._problem.get_priority_for_merge_nodes(node_to_keep.get_id(), node_to_keep.state)
            heapq.heappush(self._nodes_set, (priority, node_to_keep))
            self._map_of_states[state_as_string] = node_to_keep
    
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
    
    def _merge_when_doesnt_exist_node(self, node_to_remove:'Node', node_to_keep: 'Node', new_state: 'State') -> None:
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
