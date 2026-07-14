import os
import sys

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

import unittest

from SourceCode.DDStructure.Node import Node
from SourceCode.DDStructure.Arc import Arc
from SourceCode.DDStructure.Graph import Graph

class GraphEqualOperationTest(unittest.TestCase):
    def setUp(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        self.original_graph = Graph(node_0)

        # Layer 2
        self.original_graph.add_new_layer()
        node_1 = Node('1', [1, 1])
        arc_0_1 = Arc(node_0, node_1, 1)
        node_0.out_arcs.append(arc_0_1)
        node_1.in_arcs.append(arc_0_1)
        self.original_graph.add_node(node_1)

        node_2 = Node('2', [4, 4])
        arc_0_2 = Arc(node_0, node_2, 0)
        node_0.out_arcs.append(arc_0_2)
        node_2.in_arcs.append(arc_0_2)
        self.original_graph.add_node(node_2)

        # Layer 3
        self.original_graph.add_new_layer()
        node_3 = Node('3', [3, 4])
        arc_1_3 = Arc(node_1, node_3, 1)
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        arc_2_3 = Arc(node_2, node_3, 1)
        node_2.out_arcs.append(arc_2_3)
        node_3.in_arcs.append(arc_2_3)
        self.original_graph.add_node(node_3)

        node_4 = Node('4', [1, 4])
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        arc_2_4 = Arc(node_2, node_4, 0)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        self.original_graph.add_node(node_4)

        node_5 = Node('5', [2, 2])
        arc_2_5 = Arc(node_2, node_5, 0)
        node_2.out_arcs.append(arc_2_5)
        node_5.in_arcs.append(arc_2_5)
        self.original_graph.add_node(node_5)

        # Layer 4
        self.original_graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_3_6 = Arc(node_3, node_6, 0)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        arc_4_6_op0 = Arc(node_4, node_6, 0)
        node_4.out_arcs.append(arc_4_6_op0)
        node_6.in_arcs.append(arc_4_6_op0)
        arc_4_6_op1 = Arc(node_4, node_6, 1)
        node_4.out_arcs.append(arc_4_6_op1)
        node_6.in_arcs.append(arc_4_6_op1)
        arc_5_6 = Arc(node_5, node_6, 1)
        node_5.out_arcs.append(arc_5_6)
        node_6.in_arcs.append(arc_5_6)
        self.original_graph.add_node(node_6)

    def test_same_graph_equality(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        graph = Graph(node_0)

        # Layer 2
        graph.add_new_layer()
        node_1 = Node('1', [1, 1])
        arc_0_1 = Arc(node_0, node_1, 1)
        node_0.out_arcs.append(arc_0_1)
        node_1.in_arcs.append(arc_0_1)
        graph.add_node(node_1)

        node_2 = Node('2', [4, 4])
        arc_0_2 = Arc(node_0, node_2, 0)
        node_0.out_arcs.append(arc_0_2)
        node_2.in_arcs.append(arc_0_2)
        graph.add_node(node_2)

        # Layer 3
        graph.add_new_layer()
        node_3 = Node('3', [3, 4])
        arc_1_3 = Arc(node_1, node_3, 1)
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        arc_2_3 = Arc(node_2, node_3, 1)
        node_2.out_arcs.append(arc_2_3)
        node_3.in_arcs.append(arc_2_3)
        graph.add_node(node_3)

        node_4 = Node('4', [1, 4])
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        arc_2_4 = Arc(node_2, node_4, 0)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        graph.add_node(node_4)

        node_5 = Node('5', [2, 2])
        arc_2_5 = Arc(node_2, node_5, 0)
        node_2.out_arcs.append(arc_2_5)
        node_5.in_arcs.append(arc_2_5)
        graph.add_node(node_5)

        # Layer 4
        graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_3_6 = Arc(node_3, node_6, 0)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        arc_4_6_op0 = Arc(node_4, node_6, 0)
        node_4.out_arcs.append(arc_4_6_op0)
        node_6.in_arcs.append(arc_4_6_op0)
        arc_4_6_op1 = Arc(node_4, node_6, 1)
        node_4.out_arcs.append(arc_4_6_op1)
        node_6.in_arcs.append(arc_4_6_op1)
        arc_5_6 = Arc(node_5, node_6, 1)
        node_5.out_arcs.append(arc_5_6)
        node_6.in_arcs.append(arc_5_6)
        graph.add_node(node_6)

        self.assertTrue(self.original_graph == graph)
    
    def test_equivalent_graphs(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        graph = Graph(node_0)

        # Layer 2
        graph.add_new_layer()
        node_1 = Node('1', [4, 4])
        arc_0_1 = Arc(node_0, node_1, 0)
        node_0.out_arcs.append(arc_0_1)
        node_1.in_arcs.append(arc_0_1)
        graph.add_node(node_1)

        node_2 = Node('2', [1, 1])
        arc_0_2 = Arc(node_0, node_2, 1)
        node_0.out_arcs.append(arc_0_2)
        node_2.in_arcs.append(arc_0_2)
        graph.add_node(node_2)

        # Layer 3
        graph.add_new_layer()
        node_3 = Node('3', [2, 2])
        arc_1_3 = Arc(node_1, node_3, 0)
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        graph.add_node(node_3)

        node_4 = Node('4', [3, 4])
        arc_2_4 = Arc(node_2, node_4, 1)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        graph.add_node(node_4)

        node_5 = Node('5', [1, 4])
        arc_2_5 = Arc(node_2, node_5, 1)
        node_2.out_arcs.append(arc_2_5)
        node_5.in_arcs.append(arc_2_5)
        arc_1_5 = Arc(node_1, node_5, 0)
        node_1.out_arcs.append(arc_1_5)
        node_5.in_arcs.append(arc_1_5)
        graph.add_node(node_5)

        # Layer 4
        graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_4_6 = Arc(node_4, node_6, 0)
        node_4.out_arcs.append(arc_4_6)
        node_6.in_arcs.append(arc_4_6)
        arc_5_6_op0 = Arc(node_5, node_6, 0)
        node_5.out_arcs.append(arc_5_6_op0)
        node_6.in_arcs.append(arc_5_6_op0)
        arc_5_6_op1 = Arc(node_5, node_6, 1)
        node_5.out_arcs.append(arc_5_6_op1)
        node_6.in_arcs.append(arc_5_6_op1)
        arc_3_6 = Arc(node_3, node_6, 1)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        graph.add_node(node_6)

        self.assertTrue(self.original_graph == graph)
    
    def test_same_graphs_different_node_id(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        graph = Graph(node_0)

        # Layer 2
        graph.add_new_layer()
        node_1 = Node('1', [1, 1])
        arc_0_1 = Arc(node_0, node_1, 1)
        node_0.out_arcs.append(arc_0_1)
        node_1.in_arcs.append(arc_0_1)
        graph.add_node(node_1)

        node_2 = Node('2', [4, 4])
        arc_0_2 = Arc(node_0, node_2, 0)
        node_0.out_arcs.append(arc_0_2)
        node_2.in_arcs.append(arc_0_2)
        graph.add_node(node_2)

        # Layer 3
        graph.add_new_layer()
        node_3 = Node('3', [3, 4])
        arc_1_3 = Arc(node_1, node_3, 1)
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        arc_2_3 = Arc(node_2, node_3, 1)
        node_2.out_arcs.append(arc_2_3)
        node_3.in_arcs.append(arc_2_3)
        graph.add_node(node_3)

        node_4 = Node('15', [1, 4]) # Different node id
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        arc_2_4 = Arc(node_2, node_4, 0)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        graph.add_node(node_4)

        node_5 = Node('5', [2, 2])
        arc_2_5 = Arc(node_2, node_5, 0)
        node_2.out_arcs.append(arc_2_5)
        node_5.in_arcs.append(arc_2_5)
        graph.add_node(node_5)

        # Layer 4
        graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_3_6 = Arc(node_3, node_6, 0)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        arc_4_6_op0 = Arc(node_4, node_6, 0)
        node_4.out_arcs.append(arc_4_6_op0)
        node_6.in_arcs.append(arc_4_6_op0)
        arc_4_6_op1 = Arc(node_4, node_6, 1)
        node_4.out_arcs.append(arc_4_6_op1)
        node_6.in_arcs.append(arc_4_6_op1)
        arc_5_6 = Arc(node_5, node_6, 1)
        node_5.out_arcs.append(arc_5_6)
        node_6.in_arcs.append(arc_5_6)
        graph.add_node(node_6)

        self.assertTrue(self.original_graph == graph)

    def test_same_graph_different_variables(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        graph = Graph(node_0)

        # Layer 2
        graph.add_new_layer()
        node_1 = Node('1', [1, 1])
        arc_0_1 = Arc(node_0, node_1, 1)
        node_0.out_arcs.append(arc_0_1)
        node_1.in_arcs.append(arc_0_1)
        graph.add_node(node_1)

        node_2 = Node('2', [4, 4])
        arc_0_2 = Arc(node_0, node_2, 0) # Different variable
        node_0.out_arcs.append(arc_0_2) 
        node_2.in_arcs.append(arc_0_2)
        graph.add_node(node_2)

        # Layer 3
        graph.add_new_layer()
        node_3 = Node('3', [3, 4])
        arc_1_3 = Arc(node_1, node_3, 1)
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        arc_2_3 = Arc(node_2, node_3, 1)
        node_2.out_arcs.append(arc_2_3)
        node_3.in_arcs.append(arc_2_3)
        graph.add_node(node_3)

        node_4 = Node('4', [1, 4])
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        arc_2_4 = Arc(node_2, node_4, 0)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        graph.add_node(node_4)

        node_5 = Node('5', [2, 2])
        arc_2_5 = Arc(node_2, node_5, 0)
        node_2.out_arcs.append(arc_2_5)
        node_5.in_arcs.append(arc_2_5)
        graph.add_node(node_5)

        # Layer 4
        graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_3_6 = Arc(node_3, node_6, 0)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        arc_4_6_op0 = Arc(node_4, node_6, 0)
        node_4.out_arcs.append(arc_4_6_op0)
        node_6.in_arcs.append(arc_4_6_op0)
        arc_4_6_op1 = Arc(node_4, node_6, 1)
        node_4.out_arcs.append(arc_4_6_op1)
        node_6.in_arcs.append(arc_4_6_op1)
        arc_5_6 = Arc(node_5, node_6, 1)
        node_5.out_arcs.append(arc_5_6)
        node_6.in_arcs.append(arc_5_6)
        graph.add_node(node_6)

        self.assertTrue(self.original_graph == graph)

    def test_different_number_layers(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        graph = Graph(node_0)

        # Layer 2
        graph.add_new_layer()
        node_1 = Node('1', [4, 4])
        arc_0_1 = Arc(node_0, node_1, 0)
        node_0.out_arcs.append(arc_0_1)
        node_1.in_arcs.append(arc_0_1)
        graph.add_node(node_1)

        node_2 = Node('2', [1, 1])
        arc_0_2 = Arc(node_0, node_2, 1)
        node_0.out_arcs.append(arc_0_2)
        node_2.in_arcs.append(arc_0_2)
        graph.add_node(node_2)

        # Layer 3
        graph.add_new_layer()
        node_3 = Node('3', [2, 2])
        arc_1_3 = Arc(node_1, node_3, 0)
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        graph.add_node(node_3)

        node_4 = Node('4', [3, 4])
        arc_2_4 = Arc(node_2, node_4, 1)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        graph.add_node(node_4)

        node_5 = Node('5', [1, 4])
        arc_2_5 = Arc(node_2, node_5, 1)
        node_2.out_arcs.append(arc_2_5)
        node_5.in_arcs.append(arc_2_5)
        arc_1_5 = Arc(node_1, node_5, 0)
        node_1.out_arcs.append(arc_1_5)
        node_5.in_arcs.append(arc_1_5)
        graph.add_node(node_5)

        # Layer 4
        graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_4_6 = Arc(node_4, node_6, 0)
        node_4.out_arcs.append(arc_4_6)
        node_6.in_arcs.append(arc_4_6)
        arc_5_6_op0 = Arc(node_5, node_6, 0)
        node_5.out_arcs.append(arc_5_6_op0)
        node_6.in_arcs.append(arc_5_6_op0)
        arc_5_6_op1 = Arc(node_5, node_6, 1)
        node_5.out_arcs.append(arc_5_6_op1)
        node_6.in_arcs.append(arc_5_6_op1)
        arc_3_6 = Arc(node_3, node_6, 1)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        graph.add_node(node_6)

        # Layer 5
        graph.add_new_layer()
        node_7 = Node('7', [1, 7])
        graph.add_node(node_7)

        self.assertFalse(self.original_graph == graph)
    
    def test_different_graphs_more_nodes(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        graph = Graph(node_0)

        # Layer 2
        graph.add_new_layer()
        node_1 = Node('1', [1, 1])
        arc_0_1 = Arc(node_0, node_1, 1)
        node_0.out_arcs.append(arc_0_1)
        node_1.in_arcs.append(arc_0_1)
        graph.add_node(node_1)

        node_2 = Node('2', [4, 4])
        arc_0_2 = Arc(node_0, node_2, 0)
        node_0.out_arcs.append(arc_0_2)
        node_2.in_arcs.append(arc_0_2)
        graph.add_node(node_2)

        # Layer 3
        graph.add_new_layer()
        node_3 = Node('3', [3, 4])
        arc_1_3 = Arc(node_1, node_3, 1)
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        arc_2_3 = Arc(node_2, node_3, 1)
        node_2.out_arcs.append(arc_2_3)
        node_3.in_arcs.append(arc_2_3)
        graph.add_node(node_3)

        node_4 = Node('4', [1, 4])
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        arc_2_4 = Arc(node_2, node_4, 0)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        graph.add_node(node_4)

        node_5 = Node('5', [2, 2])
        arc_2_5 = Arc(node_2, node_5, 0)
        node_2.out_arcs.append(arc_2_5)
        node_5.in_arcs.append(arc_2_5)
        graph.add_node(node_5)

        # Layer 4
        graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_3_6 = Arc(node_3, node_6, 0)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        arc_4_6_op0 = Arc(node_4, node_6, 0)
        node_4.out_arcs.append(arc_4_6_op0)
        node_6.in_arcs.append(arc_4_6_op0)
        arc_4_6_op1 = Arc(node_4, node_6, 1)
        node_4.out_arcs.append(arc_4_6_op1)
        node_6.in_arcs.append(arc_4_6_op1)
        arc_5_6 = Arc(node_5, node_6, 1)
        node_5.out_arcs.append(arc_5_6)
        node_6.in_arcs.append(arc_5_6)
        graph.add_node(node_6)

        node_7 = Node('7', [1, 7]) # Extra node
        graph.add_node(node_7)

        self.assertFalse(self.original_graph == graph)
    
    def test_different_graph_less_nodes(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        graph = Graph(node_0)

        # Layer 2
        graph.add_new_layer()
        node_1 = Node('1', [1, 1])
        arc_0_1 = Arc(node_0, node_1, 1)
        node_0.out_arcs.append(arc_0_1)
        node_1.in_arcs.append(arc_0_1)
        graph.add_node(node_1)

        node_2 = Node('2', [4, 4])
        arc_0_2 = Arc(node_0, node_2, 0)
        node_0.out_arcs.append(arc_0_2)
        node_2.in_arcs.append(arc_0_2)
        graph.add_node(node_2)

        # Layer 3
        graph.add_new_layer()
        node_3 = Node('3', [3, 4])
        arc_1_3 = Arc(node_1, node_3, 1)
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        arc_2_3 = Arc(node_2, node_3, 1)
        node_2.out_arcs.append(arc_2_3)
        node_3.in_arcs.append(arc_2_3)
        graph.add_node(node_3)

        node_4 = Node('4', [1, 4])
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        arc_2_4 = Arc(node_2, node_4, 0)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        graph.add_node(node_4)

        # Node 5 is missing

        # Layer 4
        graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_3_6 = Arc(node_3, node_6, 0)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        arc_4_6_op0 = Arc(node_4, node_6, 0)
        node_4.out_arcs.append(arc_4_6_op0)
        node_6.in_arcs.append(arc_4_6_op0)
        arc_4_6_op1 = Arc(node_4, node_6, 1)
        node_4.out_arcs.append(arc_4_6_op1)
        node_6.in_arcs.append(arc_4_6_op1)
        graph.add_node(node_6)

        self.assertFalse(self.original_graph == graph)

    def test_different_graph_more_arcs(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        graph = Graph(node_0)

        # Layer 2
        graph.add_new_layer()
        node_1 = Node('1', [1, 1])
        arc_0_1_op0 = Arc(node_0, node_1, 1)
        node_0.out_arcs.append(arc_0_1_op0)
        node_1.in_arcs.append(arc_0_1_op0)
        arc_0_1_op1 = Arc(node_1, node_1, 0) # Extra arc
        node_0.out_arcs.append(arc_0_1_op1)
        node_1.in_arcs.append(arc_0_1_op1)
        graph.add_node(node_1)

        node_2 = Node('2', [4, 4])
        arc_0_2 = Arc(node_0, node_2, 0)
        node_0.out_arcs.append(arc_0_2)
        node_2.in_arcs.append(arc_0_2)
        graph.add_node(node_2)

        # Layer 3
        graph.add_new_layer()
        node_3 = Node('3', [3, 4])
        arc_1_3 = Arc(node_1, node_3, 1)
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        arc_2_3 = Arc(node_2, node_3, 1)
        node_2.out_arcs.append(arc_2_3)
        node_3.in_arcs.append(arc_2_3)
        graph.add_node(node_3)

        node_4 = Node('4', [1, 4])
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        arc_2_4 = Arc(node_2, node_4, 0)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        graph.add_node(node_4)

        node_5 = Node('5', [2, 2])
        arc_2_5 = Arc(node_2, node_5, 0)
        node_2.out_arcs.append(arc_2_5)
        node_5.in_arcs.append(arc_2_5)
        graph.add_node(node_5)

        # Layer 4
        graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_3_6 = Arc(node_3, node_6, 0)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        arc_4_6_op0 = Arc(node_4, node_6, 0)
        node_4.out_arcs.append(arc_4_6_op0)
        node_6.in_arcs.append(arc_4_6_op0)
        arc_4_6_op1 = Arc(node_4, node_6, 1)
        node_4.out_arcs.append(arc_4_6_op1)
        node_6.in_arcs.append(arc_4_6_op1)
        arc_5_6 = Arc(node_5, node_6, 1)
        node_5.out_arcs.append(arc_5_6)
        node_6.in_arcs.append(arc_5_6)
        graph.add_node(node_6)

        self.assertFalse(self.original_graph == graph)
    
    def test_different_points_of_arc(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        graph = Graph(node_0)

        # Layer 2
        graph.add_new_layer()
        node_1 = Node('1', [1, 1])
        arc_0_1 = Arc(node_0, node_1, 1)
        node_0.out_arcs.append(arc_0_1)
        node_1.in_arcs.append(arc_0_1)
        graph.add_node(node_1)

        node_2 = Node('2', [4, 4])
        arc_0_2 = Arc(node_0, node_2, 0)
        node_0.out_arcs.append(arc_0_2)
        node_2.in_arcs.append(arc_0_2)
        graph.add_node(node_2)

        # Layer 3
        graph.add_new_layer()
        node_3 = Node('3', [3, 4])
        arc_1_3 = Arc(node_1, node_3, 1)
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        arc_2_3 = Arc(node_2, node_3, 1)
        node_2.out_arcs.append(arc_2_3)
        node_3.in_arcs.append(arc_2_3)
        graph.add_node(node_3)

        node_4 = Node('4', [1, 4])
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        arc_2_4 = Arc(node_2, node_4, 0)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        graph.add_node(node_4)

        node_5 = Node('5', [2, 2])
        arc_1_5 = Arc(node_1, node_5, 0) # Different in node in arc
        node_1.out_arcs.append(arc_1_5)
        node_5.in_arcs.append(arc_1_5)
        graph.add_node(node_5)

        # Layer 4
        graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_3_6 = Arc(node_3, node_6, 0)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        arc_4_6_op0 = Arc(node_4, node_6, 0)
        node_4.out_arcs.append(arc_4_6_op0)
        node_6.in_arcs.append(arc_4_6_op0)
        arc_4_6_op1 = Arc(node_4, node_6, 1)
        node_4.out_arcs.append(arc_4_6_op1)
        node_6.in_arcs.append(arc_4_6_op1)
        arc_5_6 = Arc(node_5, node_6, 1)
        node_5.out_arcs.append(arc_5_6)
        node_6.in_arcs.append(arc_5_6)
        graph.add_node(node_6)

        self.assertFalse(self.original_graph == graph)
    
    def test_different_graph_less_arcs(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        graph = Graph(node_0)

        # Layer 2
        graph.add_new_layer()
        node_1 = Node('1', [1, 1])
        arc_0_1 = Arc(node_0, node_1, 1)
        node_0.out_arcs.append(arc_0_1)
        node_1.in_arcs.append(arc_0_1)
        graph.add_node(node_1)

        node_2 = Node('2', [4, 4])
        arc_0_2 = Arc(node_0, node_2, 0)
        node_0.out_arcs.append(arc_0_2)
        node_2.in_arcs.append(arc_0_2)
        graph.add_node(node_2)

        # Layer 3
        graph.add_new_layer()
        node_3 = Node('3', [3, 4])
        arc_1_3 = Arc(node_1, node_3, 1)
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        arc_2_3 = Arc(node_2, node_3, 1)
        node_2.out_arcs.append(arc_2_3)
        node_3.in_arcs.append(arc_2_3)
        graph.add_node(node_3)

        node_4 = Node('4', [1, 4])
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        arc_2_4 = Arc(node_2, node_4, 0)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        graph.add_node(node_4)

        node_5 = Node('5', [2, 2])
        arc_2_5 = Arc(node_2, node_5, 0)
        node_2.out_arcs.append(arc_2_5)
        node_5.in_arcs.append(arc_2_5)
        graph.add_node(node_5)

        # Layer 4
        graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_3_6 = Arc(node_3, node_6, 0)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        # Missing arc
        arc_4_6_op1 = Arc(node_4, node_6, 1) 
        node_4.out_arcs.append(arc_4_6_op1)
        node_6.in_arcs.append(arc_4_6_op1)
        arc_5_6 = Arc(node_5, node_6, 1)
        node_5.out_arcs.append(arc_5_6)
        node_6.in_arcs.append(arc_5_6)
        graph.add_node(node_6)

        self.assertFalse(self.original_graph == graph)
    
    def test_different_arc_value(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        graph = Graph(node_0)

        # Layer 2
        graph.add_new_layer()
        node_1 = Node('1', [1, 1])
        arc_0_1 = Arc(node_0, node_1, 1)
        node_0.out_arcs.append(arc_0_1)
        node_1.in_arcs.append(arc_0_1)
        graph.add_node(node_1)

        node_2 = Node('2', [4, 4])
        arc_0_2 = Arc(node_0, node_2, 0)
        node_0.out_arcs.append(arc_0_2)
        node_2.in_arcs.append(arc_0_2)
        graph.add_node(node_2)

        # Layer 3
        graph.add_new_layer()
        node_3 = Node('3', [3, 4])
        arc_1_3 = Arc(node_1, node_3, 0) # Different value
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        arc_2_3 = Arc(node_2, node_3, 1)
        node_2.out_arcs.append(arc_2_3)
        node_3.in_arcs.append(arc_2_3)
        graph.add_node(node_3)

        node_4 = Node('4', [1, 4])
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        arc_2_4 = Arc(node_2, node_4, 0)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        graph.add_node(node_4)

        node_5 = Node('5', [2, 2])
        arc_2_5 = Arc(node_2, node_5, 0)
        node_2.out_arcs.append(arc_2_5)
        node_5.in_arcs.append(arc_2_5)
        graph.add_node(node_5)

        # Layer 4
        graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_3_6 = Arc(node_3, node_6, 0)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        arc_4_6_op0 = Arc(node_4, node_6, 0)
        node_4.out_arcs.append(arc_4_6_op0)
        node_6.in_arcs.append(arc_4_6_op0)
        arc_4_6_op1 = Arc(node_4, node_6, 1)
        node_4.out_arcs.append(arc_4_6_op1)
        node_6.in_arcs.append(arc_4_6_op1)
        arc_5_6 = Arc(node_5, node_6, 1)
        node_5.out_arcs.append(arc_5_6)
        node_6.in_arcs.append(arc_5_6)
        graph.add_node(node_6)

        self.assertFalse(self.original_graph == graph)

    def test_different_state(self):
        # Layer 1
        node_0 = Node('0', [0, 0])
        graph = Graph(node_0)

        # Layer 2
        graph.add_new_layer()
        node_1 = Node('1', [1, 1])
        arc_0_1 = Arc(node_0, node_1, 1)
        node_0.out_arcs.append(arc_0_1)
        node_1.in_arcs.append(arc_0_1)
        graph.add_node(node_1)

        node_2 = Node('2', [1, 4]) # Different state
        arc_0_2 = Arc(node_0, node_2, 0)
        node_0.out_arcs.append(arc_0_2)
        node_2.in_arcs.append(arc_0_2)
        graph.add_node(node_2)

        # Layer 3
        graph.add_new_layer()
        node_3 = Node('3', [3, 4])
        arc_1_3 = Arc(node_1, node_3, 1)
        node_1.out_arcs.append(arc_1_3)
        node_3.in_arcs.append(arc_1_3)
        arc_2_3 = Arc(node_2, node_3, 1)
        node_2.out_arcs.append(arc_2_3)
        node_3.in_arcs.append(arc_2_3)
        graph.add_node(node_3)

        node_4 = Node('4', [1, 4])
        arc_1_4 = Arc(node_1, node_4, 1)
        node_1.out_arcs.append(arc_1_4)
        node_4.in_arcs.append(arc_1_4)
        arc_2_4 = Arc(node_2, node_4, 0)
        node_2.out_arcs.append(arc_2_4)
        node_4.in_arcs.append(arc_2_4)
        graph.add_node(node_4)

        node_5 = Node('5', [2, 2])
        arc_2_5 = Arc(node_2, node_5, 0)
        node_2.out_arcs.append(arc_2_5)
        node_5.in_arcs.append(arc_2_5)
        graph.add_node(node_5)

        # Layer 4
        graph.add_new_layer()
        node_6 = Node('6', [1, 6])
        arc_3_6 = Arc(node_3, node_6, 0)
        node_3.out_arcs.append(arc_3_6)
        node_6.in_arcs.append(arc_3_6)
        arc_4_6_op0 = Arc(node_4, node_6, 0)
        node_4.out_arcs.append(arc_4_6_op0)
        node_6.in_arcs.append(arc_4_6_op0)
        arc_4_6_op1 = Arc(node_4, node_6, 1)
        node_4.out_arcs.append(arc_4_6_op1)
        node_6.in_arcs.append(arc_4_6_op1)
        arc_5_6 = Arc(node_5, node_6, 1)
        node_5.out_arcs.append(arc_5_6)
        node_6.in_arcs.append(arc_5_6)
        graph.add_node(node_6)

        self.assertFalse(self.original_graph == graph)

if __name__ == '__main__':
    unittest.main()