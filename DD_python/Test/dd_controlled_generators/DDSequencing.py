import os
import sys

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

from SourceCode.DDStructure.Node import Node
from SourceCode.DDStructure.Arc import Arc
from SourceCode.DDStructure.Graph import Graph

# Instance: 3 jobs, p=[2,3,1], w=[1,2,3]
# setup_times: depot=[1,1,1], from0=[0,2,1], from1=[2,0,1], from2=[1,2,0]

def get_exact_dd_sequencing():
    node_0 = Node('0', (frozenset(), -1))
    graph = Graph(node_0)

    graph.add_new_layer()
    node_1 = Node('1', (frozenset({0}), 0))
    arc_0_1 = Arc(node_0, node_1, 0)
    node_0.out_arcs.append(arc_0_1)
    node_1.in_arcs.append(arc_0_1)
    graph.add_node(node_1)

    node_2 = Node('2', (frozenset({1}), 1))
    arc_0_2 = Arc(node_0, node_2, 1)
    node_0.out_arcs.append(arc_0_2)
    node_2.in_arcs.append(arc_0_2)
    graph.add_node(node_2)

    node_3 = Node('3', (frozenset({2}), 2))
    arc_0_3 = Arc(node_0, node_3, 2)
    node_0.out_arcs.append(arc_0_3)
    node_3.in_arcs.append(arc_0_3)
    graph.add_node(node_3)

    graph.add_new_layer()
    node_4 = Node('4', (frozenset({0, 1}), 1))
    arc_1_4 = Arc(node_1, node_4, 1)
    node_1.out_arcs.append(arc_1_4)
    node_4.in_arcs.append(arc_1_4)
    graph.add_node(node_4)

    node_5 = Node('5', (frozenset({0, 2}), 2))
    arc_1_5 = Arc(node_1, node_5, 2)
    node_1.out_arcs.append(arc_1_5)
    node_5.in_arcs.append(arc_1_5)
    graph.add_node(node_5)

    node_6 = Node('6', (frozenset({0, 1}), 0))
    arc_2_6 = Arc(node_2, node_6, 0)
    node_2.out_arcs.append(arc_2_6)
    node_6.in_arcs.append(arc_2_6)
    graph.add_node(node_6)

    node_7 = Node('7', (frozenset({1, 2}), 2))
    arc_2_7 = Arc(node_2, node_7, 2)
    node_2.out_arcs.append(arc_2_7)
    node_7.in_arcs.append(arc_2_7)
    graph.add_node(node_7)

    node_8 = Node('8', (frozenset({0, 2}), 0))
    arc_3_8 = Arc(node_3, node_8, 0)
    node_3.out_arcs.append(arc_3_8)
    node_8.in_arcs.append(arc_3_8)
    graph.add_node(node_8)

    node_9 = Node('9', (frozenset({1, 2}), 1))
    arc_3_9 = Arc(node_3, node_9, 1)
    node_3.out_arcs.append(arc_3_9)
    node_9.in_arcs.append(arc_3_9)
    graph.add_node(node_9)

    graph.add_new_layer()
    node_10 = Node('10', (frozenset(), -1))
    arc_4_10 = Arc(node_4, node_10, 2)
    node_4.out_arcs.append(arc_4_10)
    node_10.in_arcs.append(arc_4_10)
    arc_5_10 = Arc(node_5, node_10, 1)
    node_5.out_arcs.append(arc_5_10)
    node_10.in_arcs.append(arc_5_10)
    arc_6_10 = Arc(node_6, node_10, 2)
    node_6.out_arcs.append(arc_6_10)
    node_10.in_arcs.append(arc_6_10)
    arc_7_10 = Arc(node_7, node_10, 0)
    node_7.out_arcs.append(arc_7_10)
    node_10.in_arcs.append(arc_7_10)
    arc_8_10 = Arc(node_8, node_10, 1)
    node_8.out_arcs.append(arc_8_10)
    node_10.in_arcs.append(arc_8_10)
    arc_9_10 = Arc(node_9, node_10, 0)
    node_9.out_arcs.append(arc_9_10)
    node_10.in_arcs.append(arc_9_10)
    graph.add_node(node_10)

    return graph


def get_reduce_dd_sequencing():
    node_0 = Node('0', (frozenset(), -1))
    graph = Graph(node_0)

    graph.add_new_layer()
    node_1 = Node('1', (frozenset({0}), 0))
    arc_0_1 = Arc(node_0, node_1, 0)
    node_0.out_arcs.append(arc_0_1)
    node_1.in_arcs.append(arc_0_1)
    graph.add_node(node_1)

    node_2 = Node('2', (frozenset({1}), 1))
    arc_0_2 = Arc(node_0, node_2, 1)
    node_0.out_arcs.append(arc_0_2)
    node_2.in_arcs.append(arc_0_2)
    graph.add_node(node_2)

    node_3 = Node('3', (frozenset({2}), 2))
    arc_0_3 = Arc(node_0, node_3, 2)
    node_0.out_arcs.append(arc_0_3)
    node_3.in_arcs.append(arc_0_3)
    graph.add_node(node_3)

    graph.add_new_layer()
    node_4 = Node('4', (frozenset({0, 1}), 1))
    arc_1_4 = Arc(node_1, node_4, 1)
    node_1.out_arcs.append(arc_1_4)
    node_4.in_arcs.append(arc_1_4)
    arc_2_4 = Arc(node_2, node_4, 0)
    node_2.out_arcs.append(arc_2_4)
    node_4.in_arcs.append(arc_2_4)
    graph.add_node(node_4)

    node_5 = Node('5', (frozenset({0, 2}), 2))
    arc_1_5 = Arc(node_1, node_5, 2)
    node_1.out_arcs.append(arc_1_5)
    node_5.in_arcs.append(arc_1_5)
    arc_3_5 = Arc(node_3, node_5, 0)
    node_3.out_arcs.append(arc_3_5)
    node_5.in_arcs.append(arc_3_5)
    graph.add_node(node_5)

    node_6 = Node('6', (frozenset({1, 2}), 2))
    arc_2_6 = Arc(node_2, node_6, 2)
    node_2.out_arcs.append(arc_2_6)
    node_6.in_arcs.append(arc_2_6)
    arc_3_6 = Arc(node_3, node_6, 1)
    node_3.out_arcs.append(arc_3_6)
    node_6.in_arcs.append(arc_3_6)
    graph.add_node(node_6)

    graph.add_new_layer()
    node_7 = Node('7', (frozenset(), -1))
    arc_4_7 = Arc(node_4, node_7, 2)
    node_4.out_arcs.append(arc_4_7)
    node_7.in_arcs.append(arc_4_7)
    arc_5_7 = Arc(node_5, node_7, 1)
    node_5.out_arcs.append(arc_5_7)
    node_7.in_arcs.append(arc_5_7)
    arc_6_7 = Arc(node_6, node_7, 0)
    node_6.out_arcs.append(arc_6_7)
    node_7.in_arcs.append(arc_6_7)
    graph.add_node(node_7)

    return graph


def get_restricted_dd_sequencing():
    node_0 = Node('0', (frozenset(), -1))
    graph = Graph(node_0)

    graph.add_new_layer()
    node_1 = Node('1', (frozenset({0}), 0))
    arc_0_1 = Arc(node_0, node_1, 0)
    node_0.out_arcs.append(arc_0_1)
    node_1.in_arcs.append(arc_0_1)
    graph.add_node(node_1)

    node_2 = Node('2', (frozenset({1}), 1))
    arc_0_2 = Arc(node_0, node_2, 1)
    node_0.out_arcs.append(arc_0_2)
    node_2.in_arcs.append(arc_0_2)
    graph.add_node(node_2)

    node_3 = Node('3', (frozenset({2}), 2))
    arc_0_3 = Arc(node_0, node_3, 2)
    node_0.out_arcs.append(arc_0_3)
    node_3.in_arcs.append(arc_0_3)
    graph.add_node(node_3)

    graph.add_new_layer()
    node_4 = Node('4', (frozenset({0, 2}), 2))
    arc_1_4 = Arc(node_1, node_4, 2)
    node_1.out_arcs.append(arc_1_4)
    node_4.in_arcs.append(arc_1_4)
    graph.add_node(node_4)

    node_5 = Node('5', (frozenset({1, 2}), 2))
    arc_2_5 = Arc(node_2, node_5, 2)
    node_2.out_arcs.append(arc_2_5)
    node_5.in_arcs.append(arc_2_5)
    graph.add_node(node_5)

    node_6 = Node('6', (frozenset({0, 2}), 0))
    arc_3_6 = Arc(node_3, node_6, 0)
    node_3.out_arcs.append(arc_3_6)
    node_6.in_arcs.append(arc_3_6)
    graph.add_node(node_6)

    graph.add_new_layer()
    node_7 = Node('7', (frozenset(), -1))
    arc_4_7 = Arc(node_4, node_7, 1)
    node_4.out_arcs.append(arc_4_7)
    node_7.in_arcs.append(arc_4_7)
    arc_5_7 = Arc(node_5, node_7, 0)
    node_5.out_arcs.append(arc_5_7)
    node_7.in_arcs.append(arc_5_7)
    arc_6_7 = Arc(node_6, node_7, 1)
    node_6.out_arcs.append(arc_6_7)
    node_7.in_arcs.append(arc_6_7)
    graph.add_node(node_7)

    return graph


def get_relaxed_dd_sequencing():
    # Graph equality matches nodes/arcs by state (not by id or arc order), so the
    # node ids and arc-insertion order below are only for readability.
    node_0 = Node('0', (frozenset(), -1))
    graph = Graph(node_0)

    node_1 = Node('1', (frozenset({0}), 0))
    node_2 = Node('2', (frozenset({1}), 1))
    node_3 = Node('3', (frozenset({2}), 2))
    node_4 = Node('4', (frozenset({2}), 2))
    node_5 = Node('5', (frozenset({1}), 2))
    node_6 = Node('6', (frozenset({0, 2}), 0))
    node_7 = Node('7', (frozenset(), -1))

    def connect(parent, child, value):
        arc = Arc(parent, child, value)
        parent.out_arcs.append(arc)
        child.in_arcs.append(arc)

    connect(node_0, node_1, 0)
    connect(node_0, node_2, 1)
    connect(node_0, node_3, 2)

    connect(node_1, node_5, 1)
    connect(node_1, node_4, 2)
    connect(node_2, node_5, 0)
    connect(node_2, node_5, 2)
    connect(node_3, node_6, 0)
    connect(node_3, node_4, 1)

    connect(node_4, node_7, 0)
    connect(node_4, node_7, 1)
    connect(node_5, node_7, 0)
    connect(node_5, node_7, 2)
    connect(node_6, node_7, 1)

    graph.add_new_layer()
    for node in (node_1, node_2, node_3):
        graph.add_node(node)

    graph.add_new_layer()
    for node in (node_4, node_5, node_6):
        graph.add_node(node)

    graph.add_new_layer()
    graph.add_node(node_7)

    return graph


def get_false_dd_sequencing():
    # Same structure as exact but with swapped arc values (0↔1) on root arcs
    node_0 = Node('0', (frozenset(), -1))
    graph = Graph(node_0)

    graph.add_new_layer()
    node_1 = Node('1', (frozenset({0}), 0))
    arc_0_1 = Arc(node_0, node_1, 1)  # wrong: should be 0
    node_0.out_arcs.append(arc_0_1)
    node_1.in_arcs.append(arc_0_1)
    graph.add_node(node_1)

    node_2 = Node('2', (frozenset({1}), 1))
    arc_0_2 = Arc(node_0, node_2, 0)  # wrong: should be 1
    node_0.out_arcs.append(arc_0_2)
    node_2.in_arcs.append(arc_0_2)
    graph.add_node(node_2)

    node_3 = Node('3', (frozenset({2}), 2))
    arc_0_3 = Arc(node_0, node_3, 2)
    node_0.out_arcs.append(arc_0_3)
    node_3.in_arcs.append(arc_0_3)
    graph.add_node(node_3)

    graph.add_new_layer()
    node_4 = Node('4', (frozenset({0, 1}), 1))
    arc_1_4 = Arc(node_1, node_4, 1)
    node_1.out_arcs.append(arc_1_4)
    node_4.in_arcs.append(arc_1_4)
    graph.add_node(node_4)

    node_5 = Node('5', (frozenset({0, 2}), 2))
    arc_1_5 = Arc(node_1, node_5, 2)
    node_1.out_arcs.append(arc_1_5)
    node_5.in_arcs.append(arc_1_5)
    graph.add_node(node_5)

    node_6 = Node('6', (frozenset({0, 1}), 0))
    arc_2_6 = Arc(node_2, node_6, 0)
    node_2.out_arcs.append(arc_2_6)
    node_6.in_arcs.append(arc_2_6)
    graph.add_node(node_6)

    node_7 = Node('7', (frozenset({1, 2}), 2))
    arc_2_7 = Arc(node_2, node_7, 2)
    node_2.out_arcs.append(arc_2_7)
    node_7.in_arcs.append(arc_2_7)
    graph.add_node(node_7)

    node_8 = Node('8', (frozenset({0, 2}), 0))
    arc_3_8 = Arc(node_3, node_8, 0)
    node_3.out_arcs.append(arc_3_8)
    node_8.in_arcs.append(arc_3_8)
    graph.add_node(node_8)

    node_9 = Node('9', (frozenset({1, 2}), 1))
    arc_3_9 = Arc(node_3, node_9, 1)
    node_3.out_arcs.append(arc_3_9)
    node_9.in_arcs.append(arc_3_9)
    graph.add_node(node_9)

    graph.add_new_layer()
    node_10 = Node('10', (frozenset(), -1))
    arc_4_10 = Arc(node_4, node_10, 2)
    node_4.out_arcs.append(arc_4_10)
    node_10.in_arcs.append(arc_4_10)
    arc_5_10 = Arc(node_5, node_10, 1)
    node_5.out_arcs.append(arc_5_10)
    node_10.in_arcs.append(arc_5_10)
    arc_6_10 = Arc(node_6, node_10, 2)
    node_6.out_arcs.append(arc_6_10)
    node_10.in_arcs.append(arc_6_10)
    arc_7_10 = Arc(node_7, node_10, 0)
    node_7.out_arcs.append(arc_7_10)
    node_10.in_arcs.append(arc_7_10)
    arc_8_10 = Arc(node_8, node_10, 1)
    node_8.out_arcs.append(arc_8_10)
    node_10.in_arcs.append(arc_8_10)
    arc_9_10 = Arc(node_9, node_10, 0)
    node_9.out_arcs.append(arc_9_10)
    node_10.in_arcs.append(arc_9_10)
    graph.add_node(node_10)

    return graph
