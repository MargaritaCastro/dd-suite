from SourceCode.DDStructure.Node import Node
from SourceCode.DDStructure.Arc import Arc
from SourceCode.DDStructure.Graph import Graph


class GraphFile:
    '''
    SourceCode responsible for generating a GML (Graph Modeling Language) file
    to represent a hierarchical directed graph with nodes and arcs.
    '''

    def __init__(self, file_name: str, graph: Graph, problem: 'AbstractProblem'):
        '''
        Constructor of the GraphFile class.

        Parameters:
        - file_name (str): Name of the GML file to be created.
        - graph (Graph): Graph object to be represented in the GML file.
        '''
        self.file_name: str = file_name
        self.graph: 'Graph' = graph
        self._problem: 'AbstractProblem' = problem

        self.is_graph_binary: bool = self._check_if_graph_is_binary()
        self._create_gml_file()
    
    def _check_if_graph_is_binary(self) -> bool:
        '''
        Determines if a graph is binary, i.e., if all its variables are binary.

        Returns:
        - bool: True if the graph is binary, False otherwise.
        '''

        for layer in self.graph.structure:
            for node in layer:
                if not self._is_node_binary(node):
                    return False
                
        return True
                
    def _is_node_binary(self, node: Node) -> bool:
        '''
        Determines if a node is binary, i.e., if its outgoing arcs have values of 0 or 1.

        Parameters:
        - node (Node): Node object that has arcs with variable values.

        Returns:
        - bool: True if the node is binary, False otherwise.
        '''
        for arc in node.out_arcs:
            if arc.variable_value != 0 and arc.variable_value != 1:
                return False
            
        return True
    

    def _create_gml_file(self) -> None:
        '''
        Creates the GML file and opens the file for writing.
        '''
        self.file = open(f"{self.file_name}.gml", 'w')
        self._start_file()
        self._add_nodes_and_arcs()
        self._end_file()

    def _start_file(self) -> None:
        '''
        Initializes the structure of the GML file with graph information.
        '''
        self.file.write("graph [\n")
        self.file.write("\t directed 1\n")
        self.file.write("\t hierarchic 1\n")

    def _add_nodes_and_arcs(self) -> None:
        '''
        Adds nodes to the GML file, along with their outgoing arcs information.
        '''
        arcs: list['Arc'] = []
        for layer in self.graph.structure:
            for node in layer:
                self._add_node(node)
                arcs += node.out_arcs      
        self._add_arcs(arcs)

    def _add_node(self, node: Node) -> None:
        '''
        Adds node information to the GML file.

        Parameters:
        - node (Node): Node object to be added to the GML file.
        '''
        self.file.write(f"\n node [\n")
        self.file.write(f"\t id {node.get_id()}\n")
        
        self.file.write(f"\t label \"{node.get_id()}             {self._problem.get_state_as_string(node.state)}\"\n")
        self.file.write(" \tgraphics [\n")
        self.file.write(f"\t type \"ellipse\"\n")
        self.file.write(f"\t hasFill 0\n")
        self.file.write("\t w 90.0   h 110.0\n")
        self.file.write("\t outline \"#000000\" ]\n")
        self.file.write(f"]\n")
        self.file.write(" \tLabelGraphics [\n")
        self.file.write(f"\t text \"{node.get_id()}             {self._problem.get_state_as_string(node.state)}\"\n")
        self.file.write(f"\t fontSize 12\n")
        self.file.write("\t fontName \"Dialog\"\n")
        self.file.write("\t model \"sides\"\n")
        self.file.write("\t anchor \"e\"\n")
        self.file.write("\t borderDistance -50.0\n")
        self.file.write(f"]\n")

    def _add_arcs(self, arcs: list[Arc]) -> None:
        '''
        Adds arcs to the GML file.

        Parameters:
        - arcs (list[Arc]): List of Arc objects to be added to the GML file.
        '''
        for arc in arcs:
            self._add_arc(arc)

    def _add_arc(self, arc: Arc) -> None:
        '''
        Adds arc information to the GML file.

        Parameters:
        - arc (Arc): Arc object to be added to the GML file.
        '''
        self.file.write(f"\nedge [\n")
        self._add_arc_source(arc)
        self._add_arc_target(arc)

        if not self.is_graph_binary:
            self._add_arc_label(arc)


        self._add_arc_graphics(arc)
        self.file.write(f"]\n")

    def _add_arc_source(self, arc: Arc) -> None:
        '''
        Writes the source node ID of the arc to the GML file.

        Parameters:
        - arc (Arc): Arc whose source node ID will be written.
        '''
        self.file.write(f"\t source {arc.parent_node.get_id()}\n")

    def _add_arc_label(self, arc: Arc) -> None:
        '''
        Writes the label of the arc to the GML file.

        Parameters:
        - arc (Arc): Arc whose variable value will be written as a label.
        '''
        self.file.write(f"\tlabel \"{arc.variable_value}\"\n")

    def _add_arc_target(self, arc: Arc) -> None:
        '''
        Writes the target node ID of the arc to the GML file.

        Parameters:
        - arc (Arc): Arc whose target node ID will be written.
        '''
        self.file.write(f"\ttarget {arc.child_node.get_id()}\n")

    def _add_arc_graphics(self, arc: Arc) -> None:
        '''
        Writes the graphics block for the arc to the GML file.
        Delegates to binary or normal arc graphics based on the graph type.

        Parameters:
        - arc (Arc): Arc whose graphics information will be written.
        '''
        self.file.write("\tgraphics [\n")

        if self.is_graph_binary:
            self._add_binary_arc_graphics(arc)

        else:
            self._add_normal_arc_graphics(arc)

        self.file.write(f"]")

    def _add_normal_arc_graphics(self, arc: Arc) -> None:
        '''
        Writes graphics attributes for a non-binary arc to the GML file.

        Parameters:
        - arc (Arc): Arc whose normal graphics attributes will be written.
        '''
        self.file.write(f"\tfill \"#000000\" 		targetArrow \"standard\"	 	 \n")

    def _add_binary_arc_graphics(self, arc: Arc) -> None:
        '''
        Writes graphics attributes for a binary arc to the GML file.
        Arcs with value 0 are drawn dashed and grey; arcs with value 1 are solid and black.

        Parameters:
        - arc (Arc): Arc whose binary graphics attributes will be written.
        '''
        if arc.variable_value == 0:
            self.file.write(f"\tfill \"#808080\" 		targetArrow \"standard\"\t  style\t  \"dashed\"\t\n")
        else:
            self.file.write(f"\tfill \"#000000\" 		targetArrow \"standard\"	 	 \n")
    
    def _end_file(self) -> None:
        '''
        Finalizes the structure of the GML file.
        '''
        self.file.write("]")