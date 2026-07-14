
class DDReducer():
    '''
    SourceCode representing reduction algorithm for a decision diagram
    '''

    def __init__(self, graph: 'Graph', problem: 'AbstractProblem') -> None:
        '''
        Constructor for the DDReducer class.

        Parameters:
        - graph (Graph): The decision diagram graph to reduce.
        - problem(AbstractProblem): The problem instance associated with the graph.
        '''

        self._graph:'Graph'  = graph
        self._problem: 'Problem' = problem

        self._remove_node_from_layer: bool = False
    
    def reduce_decision_diagram(self, should_visualize: bool) -> 'Graph':
        '''
        Reduces the decision diagram graph and returns it.

        Parameters:
        - should_visualize (bool): Indicates whether to visualize the graph during the process.

        Returns:
        - Graph: The reduced decision diagram graph.
        '''

        previous_layer: list['Node'] = self._graph.structure[-1]

        for layer in reversed(self._graph.structure[:-1]):
            if should_visualize:
                self._print_graph()

            self._review_layer_to_merge_nodes(layer)

            if self._remove_node_from_layer:
                self._clear_incoming_arcs(previous_layer)

            previous_layer = layer

        self._adjust_nodes_id()

        if should_visualize:
            self._print_graph()
                
        return self._graph


    def _review_layer_to_merge_nodes(self, layer: list['Node']) -> None:
        '''
        Reviews the current layer to merge nodes that reach the same node afterward
        with the same variable value.

        Parameters:
        - layer: List of nodes in the current layer.
        '''

        map_of_paths: dict = {}
        self._remove_node_from_layer = False

        new_layer = []
        for node in layer:
            path = self._get_node_of_every_type_of_path(node)
            if path in map_of_paths:
                self._redirect_in_arcs(node, map_of_paths[path])
                node.out_arcs.clear()
                self._remove_node_from_layer = True
            else:
                map_of_paths[path] = node
                new_layer.append(node)

        if self._remove_node_from_layer :
            layer[:] = new_layer

    def _get_node_of_every_type_of_path(self, node: 'Node') -> str:
        '''
        Gets the nodes of the path of a node.

        Parameters:
        - node(Node): Node for which to get the path nodes.

        Returns:
        - str: A string containing nodes of the path as keys and variable values as values.
        '''

        path_of_nodes: str = ""
        for arc in node.out_arcs:
            path_of_nodes += f"{arc.child_node.to_string()}{arc.variable_value}_"

        return path_of_nodes

    
    def _redirect_in_arcs(self, node_to_remove: 'Node', node_to_keep: 'Node') -> None:
        '''
        Redirects the incoming arcs of one node to another node.

        Parameters:
        - node_to_remove: The node whose incoming arcs are redirected.
        - node_to_keep: The node to which the arcs are redirected.
        '''
        for arc in node_to_remove.in_arcs:
            arc.child_node = node_to_keep
            node_to_keep.in_arcs.append(arc)
        node_to_remove.in_arcs.clear()

    def _clear_incoming_arcs(self, layer:  list['Node']) -> None:
        '''
            Clear the incoming arcs of the layer. The function checks that all incoming arcs come from parents with outgoing arcs.

            Parameters:
            - layer: current layer of the graph to examine.
        '''
        for node in layer:
            for i in range(len(node.in_arcs) -1 , -1, -1):
                if not node.in_arcs[i].parent_node.out_arcs:
                    del node.in_arcs[i]

    def _adjust_nodes_id(self) -> None:
        '''
        Updates the names of the nodes in the graph.
        '''       
        initial_node_number: int = 0
        for layer in self._graph.structure:
            for node in layer:
                node.set_id(initial_node_number)
                initial_node_number += 1

    def _print_graph(self) -> None:
        '''
        Prints the content of each layer of the graph.
        '''
        print("")
        variables = self._problem.ordered_variables
        for layer_idx, layer in enumerate(self._graph.structure):
            print("------------------------------------------------------")
            if 0 < layer_idx <= len(variables):
                print(f"[{variables[layer_idx - 1]}]", end="   ")
            for node in layer:
                in_arcs_str = ", ".join(arc.to_string() for arc in node.in_arcs)
                print("u" + node.to_string() + " " + self._problem.get_state_as_string(node.state) + "(" + in_arcs_str + ")", end="   ")
            print("")