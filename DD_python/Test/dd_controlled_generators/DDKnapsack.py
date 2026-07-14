import os
import sys

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

from SourceCode.DDStructure.Node import Node
from SourceCode.DDStructure.Arc import Arc
from SourceCode.DDStructure.Graph import Graph

def get_exact_dd_knapsack():
    node_0 = Node('0', 0)
    graph = Graph(node_0)

    graph.add_new_layer()
    node_1 = Node('1', 0)
    arc_0_1 = Arc(node_0, node_1, 0)
    node_0.out_arcs.append(arc_0_1)
    node_1.in_arcs.append(arc_0_1)

    graph.add_node(node_1)
    node_2 = Node('2', 3)
    arc_0_2 = Arc(node_0, node_2, 1)
    node_0.out_arcs.append(arc_0_2)
    node_2.in_arcs.append(arc_0_2)
    graph.add_node(node_2)


    graph.add_new_layer()
    node_3 = Node('3', 0)
    arc_1_3 = Arc(node_1, node_3, 0)
    node_1.out_arcs.append(arc_1_3)
    node_3.in_arcs.append(arc_1_3)

    graph.add_node(node_3)
    node_4 = Node('4', 3)
    arc_1_4 = Arc(node_1, node_4, 1)
    node_1.out_arcs.append(arc_1_4)
    node_4.in_arcs.append(arc_1_4)
    arc_2_4 = Arc(node_2, node_4, 0)
    node_2.out_arcs.append(arc_2_4)
    node_4.in_arcs.append(arc_2_4)

    graph.add_node(node_4)
    node_5 = Node('5', 6)
    arc_2_5 = Arc(node_2, node_5, 1)
    node_2.out_arcs.append(arc_2_5)
    node_5.in_arcs.append(arc_2_5)
    graph.add_node(node_5)


    graph.add_new_layer()
    node_6 = Node('6', 0)
    arc_3_6 = Arc(node_3, node_6, 0)
    node_3.out_arcs.append(arc_3_6)
    node_6.in_arcs.append(arc_3_6)

    graph.add_node(node_6)
    node_7 = Node('7', 4)
    arc_3_7 = Arc(node_3, node_7, 1)
    node_3.out_arcs.append(arc_3_7)
    node_7.in_arcs.append(arc_3_7)

    graph.add_node(node_7)
    node_8 = Node('8', 3)
    arc_4_8 = Arc(node_4, node_8, 0)
    node_4.out_arcs.append(arc_4_8)
    node_8.in_arcs.append(arc_4_8)

    graph.add_node(node_8)
    node_9 = Node('9', 6)
    arc_5_9 = Arc(node_5, node_9, 0)
    node_5.out_arcs.append(arc_5_9)
    node_9.in_arcs.append(arc_5_9)
    graph.add_node(node_9)


    graph.add_new_layer()
    node_10 = Node('10', 0)
    arc_6_10_op1 = Arc(node_6, node_10, 0)
    node_6.out_arcs.append(arc_6_10_op1)
    node_10.in_arcs.append(arc_6_10_op1)
    arc_6_10_op2 = Arc(node_6, node_10, 1)
    node_6.out_arcs.append(arc_6_10_op2)
    node_10.in_arcs.append(arc_6_10_op2)
    arc_7_10 = Arc(node_7, node_10, 0)
    node_7.out_arcs.append(arc_7_10)
    node_10.in_arcs.append(arc_7_10)
    arc_8_10 = Arc(node_8, node_10, 0)
    node_8.out_arcs.append(arc_8_10)
    node_10.in_arcs.append(arc_8_10)
    arc_9_10 = Arc(node_9, node_10, 0)
    node_9.out_arcs.append(arc_9_10)
    node_10.in_arcs.append(arc_9_10)
    graph.add_node(node_10)

    return graph

def get_reduce_dd_knapsack():
    node_0 = Node('0', 0)
    graph = Graph(node_0)

    graph.add_new_layer()
    node_1 = Node('1', 0)
    arc_0_1 = Arc(node_0, node_1, 0)
    node_0.out_arcs.append(arc_0_1)
    node_1.in_arcs.append(arc_0_1)
    graph.add_node(node_1)

    node_2 = Node('2', 3)
    arc_0_2 = Arc(node_0, node_2, 1)
    node_0.out_arcs.append(arc_0_2)
    node_2.in_arcs.append(arc_0_2)
    graph.add_node(node_2)

    graph.add_new_layer()
    node_3 = Node('3', 0)
    arc_1_3 = Arc(node_1, node_3, 0)
    node_1.out_arcs.append(arc_1_3)
    node_3.in_arcs.append(arc_1_3)
    graph.add_node(node_3)

    node_4 = Node('4', 3)
    arc_1_4 = Arc(node_1, node_4, 1)
    node_1.out_arcs.append(arc_1_4)
    node_4.in_arcs.append(arc_1_4)
    arc_2_4_op1 = Arc(node_2, node_4, 0)
    node_2.out_arcs.append(arc_2_4_op1)
    node_4.in_arcs.append(arc_2_4_op1)
    arc_2_4_op2 = Arc(node_2, node_4, 1)
    node_2.out_arcs.append(arc_2_4_op2)
    node_4.in_arcs.append(arc_2_4_op2)
    graph.add_node(node_4)

    graph.add_new_layer()
    node_5 = Node('5', 0)
    arc_3_5 = Arc(node_3, node_5, 0)
    node_3.out_arcs.append(arc_3_5)
    node_5.in_arcs.append(arc_3_5)
    graph.add_node(node_5)

    node_6 = Node('6', 4)
    arc_3_6 = Arc(node_3, node_6, 1)
    node_3.out_arcs.append(arc_3_6)
    node_6.in_arcs.append(arc_3_6)

    arc_4_6 = Arc(node_4, node_6, 0)
    node_4.out_arcs.append(arc_4_6)
    node_6.in_arcs.append(arc_4_6)
    graph.add_node(node_6)

    graph.add_new_layer()
    node_7 = Node('7', 0)
    arc_5_7_op1 = Arc(node_5, node_7, 0)
    node_5.out_arcs.append(arc_5_7_op1)
    node_7.in_arcs.append(arc_5_7_op1)
    arc_5_7_op2 = Arc(node_5, node_7, 1)
    node_5.out_arcs.append(arc_5_7_op2)
    node_7.in_arcs.append(arc_5_7_op2)
    arc_6_7 = Arc(node_6, node_7, 0)
    node_6.out_arcs.append(arc_6_7)
    node_7.in_arcs.append(arc_6_7)
    graph.add_node(node_7)

    return graph

def get_relaxed_dd_knapsack():
    node_0 = Node('0', 0)
    graph = Graph(node_0)


    graph.add_new_layer()
    node_1 = Node('1', 0)
    arc_0_1 = Arc(node_0, node_1, 0)
    node_0.out_arcs.append(arc_0_1)
    node_1.in_arcs.append(arc_0_1)

    graph.add_node(node_1)
    node_2 = Node('2', 3)
    arc_0_2 = Arc(node_0, node_2, 1)
    node_0.out_arcs.append(arc_0_2)
    node_2.in_arcs.append(arc_0_2)
    graph.add_node(node_2)


    graph.add_new_layer()
    node_3 = Node('3', 0)
    arc_1_3 = Arc(node_1, node_3, 0)
    node_1.out_arcs.append(arc_1_3)
    node_3.in_arcs.append(arc_1_3)
    graph.add_node(node_3)

    node_4 = Node('4', 3)
    arc_1_4 = Arc(node_1, node_4, 1)
    node_1.out_arcs.append(arc_1_4)
    node_4.in_arcs.append(arc_1_4)
    arc_2_4 = Arc(node_2, node_4, 0)
    node_2.out_arcs.append(arc_2_4)
    node_4.in_arcs.append(arc_2_4)
    graph.add_node(node_4)

    node_5 = Node('5', 6)
    arc_2_5 = Arc(node_2, node_5, 1)
    node_2.out_arcs.append(arc_2_5)
    node_5.in_arcs.append(arc_2_5)
    graph.add_node(node_5)

    graph.add_new_layer()
    node_6 = Node('6', 4)
    arc_3_6= Arc(node_3, node_6, 1)
    node_3.out_arcs.append(arc_3_6)
    node_6.in_arcs.append(arc_3_6)
    graph.add_node(node_6)

    node_7 = Node('7', 0)
    arc_3_7 = Arc(node_3, node_7, 0)
    node_3.out_arcs.append(arc_3_7)
    node_7.in_arcs.append(arc_3_7)
    arc_4_7 = Arc(node_4, node_7, 0)
    node_4.out_arcs.append(arc_4_7)
    node_7.in_arcs.append(arc_4_7)
    graph.add_node(node_7)

    node_8 = Node('8', 6)
    arc_5_8 = Arc(node_5, node_8, 0)
    node_5.out_arcs.append(arc_5_8)
    node_8.in_arcs.append(arc_5_8)
    graph.add_node(node_8)

    graph.add_new_layer()
    node_9 = Node('9', 0)
    arc_6_9 = Arc(node_6, node_9, 0)
    node_6.out_arcs.append(arc_6_9)
    node_9.in_arcs.append(arc_6_9)

    arc_7_9_op1 = Arc(node_7, node_9, 0)
    node_7.out_arcs.append(arc_7_9_op1)
    node_9.in_arcs.append(arc_7_9_op1)
    arc_7_9_op2 = Arc(node_7, node_9, 1)
    node_7.out_arcs.append(arc_7_9_op2)
    node_9.in_arcs.append(arc_7_9_op2)

    arc_8_9 = Arc(node_8, node_9, 0)
    node_8.out_arcs.append(arc_8_9)
    node_9.in_arcs.append(arc_8_9)

    graph.add_node(node_9)
    return graph

def get_restricted_dd_knapsack():
    node_0 = Node('0', 0)
    graph = Graph(node_0)


    graph.add_new_layer()
    node_1 = Node('1', 0)
    arc_0_1 = Arc(node_0, node_1, 0)
    node_0.out_arcs.append(arc_0_1)
    node_1.in_arcs.append(arc_0_1)

    graph.add_node(node_1)
    node_2 = Node('2', 3)
    arc_0_2 = Arc(node_0, node_2, 1)
    node_0.out_arcs.append(arc_0_2)
    node_2.in_arcs.append(arc_0_2)
    graph.add_node(node_2)


    graph.add_new_layer()
    node_3 = Node('3', 0)
    arc_1_3 = Arc(node_1, node_3, 0)
    node_1.out_arcs.append(arc_1_3)
    node_3.in_arcs.append(arc_1_3)
    graph.add_node(node_3)

    node_4 = Node('4', 3)
    arc_1_4 = Arc(node_1, node_4, 1)
    node_1.out_arcs.append(arc_1_4)
    node_4.in_arcs.append(arc_1_4)
    arc_2_4 = Arc(node_2, node_4, 0)
    node_2.out_arcs.append(arc_2_4)
    node_4.in_arcs.append(arc_2_4)
    graph.add_node(node_4)

    node_5 = Node('5', 6)
    arc_2_5 = Arc(node_2, node_5, 1)
    node_2.out_arcs.append(arc_2_5)
    node_5.in_arcs.append(arc_2_5)
    graph.add_node(node_5)

    graph.add_new_layer()
    node_6 = Node('6', 4)
    arc_3_6= Arc(node_3, node_6, 1)
    node_3.out_arcs.append(arc_3_6)
    node_6.in_arcs.append(arc_3_6)
    graph.add_node(node_6)

    node_7 = Node('7', 3)
    arc_4_7 = Arc(node_4, node_7, 0)
    node_4.out_arcs.append(arc_4_7)
    node_7.in_arcs.append(arc_4_7)
    graph.add_node(node_7)

    node_8 = Node('8', 6)
    arc_5_8 = Arc(node_5, node_8, 0)
    node_5.out_arcs.append(arc_5_8)
    node_8.in_arcs.append(arc_5_8)
    graph.add_node(node_8)


    graph.add_new_layer()
    node_9 = Node('9', 0)
    arc_6_9 = Arc(node_6, node_9, 0)
    node_6.out_arcs.append(arc_6_9)
    node_9.in_arcs.append(arc_6_9)

    arc_7_9 = Arc(node_7, node_9, 0)
    node_7.out_arcs.append(arc_7_9)
    node_9.in_arcs.append(arc_7_9)

    arc_8_9 = Arc(node_8, node_9, 0)
    node_8.out_arcs.append(arc_8_9)
    node_9.in_arcs.append(arc_8_9)

    graph.add_node(node_9)
    return graph

def get_false_dd_knapsack():
    node_0 = Node('0', [0])
    graph = Graph(node_0)


    graph.add_new_layer()
    node_1 = Node('1', [0])
    arc_0_1 = Arc(node_0, node_1, 1)
    node_0.out_arcs.append(arc_0_1)
    node_1.in_arcs.append(arc_0_1)

    graph.add_node(node_1)
    node_2 = Node('2', [3])
    arc_0_2 = Arc(node_0, node_2, 0)
    node_0.out_arcs.append(arc_0_2)
    node_2.in_arcs.append(arc_0_2)
    graph.add_node(node_2)


    graph.add_new_layer()
    node_3 = Node('3', [0])
    arc_1_3 = Arc(node_1, node_3, 0)
    node_1.out_arcs.append(arc_1_3)
    node_3.in_arcs.append(arc_1_3)

    graph.add_node(node_3)
    node_4 = Node('4', [3])
    arc_1_4 = Arc(node_1, node_4, 1)
    node_1.out_arcs.append(arc_1_4)
    node_4.in_arcs.append(arc_1_4)
    arc_2_4 = Arc(node_2, node_4, 0)
    node_2.out_arcs.append(arc_2_4)
    node_4.in_arcs.append(arc_2_4)

    graph.add_node(node_4)
    node_5 = Node('5', [6])
    arc_2_5 = Arc(node_2, node_5, 1)
    node_2.out_arcs.append(arc_2_5)
    node_5.in_arcs.append(arc_2_5)
    graph.add_node(node_5)


    graph.add_new_layer()
    node_6 = Node('6', [0])
    arc_3_6 = Arc(node_3, node_6, 0)
    node_3.out_arcs.append(arc_3_6)
    node_6.in_arcs.append(arc_3_6)

    graph.add_node(node_6)
    node_7 = Node('7', [4])
    arc_3_7 = Arc(node_3, node_7, 1)
    node_3.out_arcs.append(arc_3_7)
    node_7.in_arcs.append(arc_3_7)

    graph.add_node(node_7)
    node_8 = Node('8', [3])
    arc_4_8 = Arc(node_4, node_8, 0)
    node_4.out_arcs.append(arc_4_8)
    node_8.in_arcs.append(arc_4_8)

    graph.add_node(node_8)
    node_9 = Node('9', [6])
    arc_5_9 = Arc(node_5, node_9, 0)
    node_5.out_arcs.append(arc_5_9)
    node_9.in_arcs.append(arc_5_9)
    graph.add_node(node_9)


    graph.add_new_layer()
    node_10 = Node('10', [0])
    arc_6_10_op1 = Arc(node_6, node_10, 0)
    node_6.out_arcs.append(arc_6_10_op1)
    node_10.in_arcs.append(arc_6_10_op1)
    arc_6_10_op2 = Arc(node_6, node_10, 1)
    node_6.out_arcs.append(arc_6_10_op2)
    node_10.in_arcs.append(arc_6_10_op2)
    arc_7_10 = Arc(node_7, node_10, 0)
    node_7.out_arcs.append(arc_7_10)
    node_10.in_arcs.append(arc_7_10)
    arc_8_10 = Arc(node_8, node_10, 0)
    node_8.out_arcs.append(arc_8_10)
    node_10.in_arcs.append(arc_8_10)
    arc_9_10 = Arc(node_9, node_10, 0)
    node_9.out_arcs.append(arc_9_10)
    node_10.in_arcs.append(arc_9_10)
    graph.add_node(node_10)
    return graph