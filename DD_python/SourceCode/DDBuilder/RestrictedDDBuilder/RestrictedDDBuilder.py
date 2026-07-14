from SourceCode.DDBuilder.AbstractDDBuilder import AbstractDDBuilder

class RestrictedDDBuilder(AbstractDDBuilder):
    '''
    SourceCode that constructs a restricted decision diagram based on a given problem.
    '''
    
    def __init__(self, problem: 'AbstractProblem', max_width: int) -> None:
        '''
        Constructor for the restricted diagram class.

        Parameters:
        - problem: Problem object for which the graph will be constructed.
        - max_width: Maximum allowed width for the graph.
        '''
        super().__init__(problem)
        self._max_width: int = max_width
            
    def _specific_end_of_layer_function(self) -> None:
        '''
        Function to apply at each layer, specific to this class. Calls the function that
        decides to eliminate a node when the graph width is greater than the specified width.
        '''
        if len(self.graph.structure[-1]) > self._max_width:
            self._eliminate_nodes_when_width_is_greater_than_w()
            self._delete_nodes_current_layer(self.graph.actual_layer)

    def _specific_end_of_construction_function(self) -> None:
        '''
        Function to apply at the last layer, specific to this class.
        First calls the function that eliminates nodes without outgoing arcs,
        then calls the function that updates the number of nodes.
        '''
        self._bottom_up_pruner()
        self.adjust_node_number()
    
    def _eliminate_nodes_when_width_is_greater_than_w(self) -> None:
        '''
        Decides to eliminate nodes when the graph width is greater than the maximum allowed width.
        '''

        ordered_nodes: list['Node'] = sorted(self.graph.structure[-1], key=lambda node: self._problem.get_priority_for_discard_node(node.state))
        nodes_to_eliminate: list['Node'] = ordered_nodes[self._max_width:] or []

        for node in nodes_to_eliminate:
            node.in_arcs.clear()

        layer_id = len(self.graph.structure)
        self._remove_outgoing_arcs_previous_layer(layer_id)


    def _remove_outgoing_arcs_previous_layer(self, layer_id: int) -> None:
        '''
        Removes outgoing arcs from the previous layer that point to nodes with no incoming arcs.

        Parameters:
        - layer_id: Index of the current layer; the function operates on layer_id - 1.
        '''
        layer_id -= 1
        if layer_id >= len(self.graph.structure) or layer_id <= 0:
            return

        for node in self.graph.structure[layer_id]:
            node.out_arcs[:] = [ arc for arc in node.out_arcs
                                if arc.child_node.in_arcs ]