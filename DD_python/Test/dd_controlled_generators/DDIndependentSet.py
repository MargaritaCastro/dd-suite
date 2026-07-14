import os
import sys
from bitarray import bitarray

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

from SourceCode.DDStructure.Node import Node
from SourceCode.DDStructure.Arc import Arc
from SourceCode.DDStructure.Graph import Graph

def get_exact_dd_independent_set():
    node_0 = Node('0', bitarray('11111') )
    graph = Graph(node_0)

    # Crear la segunda capa de nodos
    graph.add_new_layer()
    node_1 = Node('1', bitarray('01111') )
    arc_0_1 = Arc(node_0, node_1, 0)
    node_0.out_arcs.append(arc_0_1)
    node_1.in_arcs.append(arc_0_1)
    graph.add_node(node_1)

    node_2 = Node('2', bitarray('00011') )
    arc_0_2 = Arc(node_0, node_2, 1)
    node_0.out_arcs.append(arc_0_2)
    node_2.in_arcs.append(arc_0_2)
    graph.add_node(node_2)

    graph.add_new_layer()
    node_3 = Node('3', bitarray('00111') )
    arc_1_3 = Arc(node_1, node_3, 0)
    node_1.out_arcs.append(arc_1_3)
    node_3.in_arcs.append(arc_1_3)
    graph.add_node(node_3)


    node_4 = Node('4', bitarray('00001') )
    arc_1_4 = Arc(node_1, node_4, 1)
    node_1.out_arcs.append(arc_1_4)
    node_4.in_arcs.append(arc_1_4)
    graph.add_node(node_4)

    node_5 = Node('5', bitarray('00011') )
    arc_2_5 = Arc(node_2, node_5, 0)
    node_2.out_arcs.append(arc_2_5)
    node_5.in_arcs.append(arc_2_5)
    graph.add_node(node_5)

    graph.add_new_layer()
    node_6 = Node('6', bitarray('00011') )
    arc_3_6 = Arc(node_3, node_6, 0)
    node_6.in_arcs.append(arc_3_6)
    node_3.out_arcs.append(arc_3_6)
    arc_5_6 = Arc(node_5, node_6, 0)
    node_5.out_arcs.append(arc_5_6)
    node_6.in_arcs.append(arc_5_6)
    graph.add_node(node_6)

    node_7 = Node('7', bitarray('00001') )
    arc_3_7 = Arc(node_3, node_7, 1)
    node_3.out_arcs.append(arc_3_7)
    node_7.in_arcs.append(arc_3_7)
    arc_4_7 = Arc(node_4, node_7, 0)
    node_4.out_arcs.append(arc_4_7)
    node_7.in_arcs.append(arc_4_7)
    graph.add_node(node_7)

    # Crear la quinta capa de nodos
    graph.add_new_layer()
    node_8 = Node('8', bitarray('00001') )
    arc_6_8 = Arc(node_6, node_8, 0)
    node_6.out_arcs.append(arc_6_8)
    node_8.in_arcs.append(arc_6_8)
    arc_7_8 = Arc(node_7, node_8, 0)
    node_7.out_arcs.append(arc_7_8)
    node_8.in_arcs.append(arc_7_8)
    graph.add_node(node_8)

    node_9 = Node('9', bitarray('00000') )
    arc_6_9 = Arc(node_6, node_9, 1)
    node_6.out_arcs.append(arc_6_9)
    node_9.in_arcs.append(arc_6_9)
    graph.add_node(node_9)

    # Crear la sexta capa de nodos
    graph.add_new_layer()
    node_10 = Node('10', bitarray('00000') )
    arc_8_10_1 = Arc(node_8, node_10, 0)
    node_8.out_arcs.append(arc_8_10_1)
    node_10.in_arcs.append(arc_8_10_1)
    arc_8_10_2 = Arc(node_8, node_10, 1)
    node_8.out_arcs.append(arc_8_10_2)
    node_10.in_arcs.append(arc_8_10_2)
    arc_9_10 = Arc(node_9, node_10, 0)
    node_9.out_arcs.append(arc_9_10)
    node_10.in_arcs.append(arc_9_10)
    graph.add_node(node_10)
    return graph

def get_restricted_dd_independent_set():
    node_0 = Node('0', bitarray('11111') )
    graph = Graph(node_0)

    # Crear la segunda capa de nodos
    graph.add_new_layer()
    node_1 = Node('1', bitarray('01111') )
    arc_0_1 = Arc(node_0, node_1, 0)
    node_0.out_arcs.append(arc_0_1)
    node_1.in_arcs.append(arc_0_1)
    graph.add_node(node_1)

    node_2 = Node('2', bitarray('00011') )
    arc_0_2 = Arc(node_0, node_2, 1)
    node_0.out_arcs.append(arc_0_2)
    node_2.in_arcs.append(arc_0_2)
    graph.add_node(node_2)

    graph.add_new_layer()
    node_3 = Node('3', bitarray('00001') )
    arc_1_3 = Arc(node_1, node_3, 1)
    node_1.out_arcs.append(arc_1_3)
    node_3.in_arcs.append(arc_1_3)
    graph.add_node(node_3)

    node_4 = Node('4', bitarray('00011') )
    arc_2_4 = Arc(node_2, node_4, 0)
    node_2.out_arcs.append(arc_2_4)
    node_4.in_arcs.append(arc_2_4)
    graph.add_node(node_4)


    graph.add_new_layer()
    node_5 = Node('5', bitarray('00001') )
    arc_3_5 = Arc(node_3, node_5, 0)
    node_5.in_arcs.append(arc_3_5)
    node_3.out_arcs.append(arc_3_5)
    graph.add_node(node_5)

    node_6 = Node('6', bitarray('00011') )
    arc_4_6 = Arc(node_4, node_6, 0)
    node_4.out_arcs.append(arc_4_6)
    node_6.in_arcs.append(arc_4_6)
    graph.add_node(node_6)

    # Crear la quinta capa de nodos
    graph.add_new_layer()
    node_7 = Node('7', bitarray('00001') )
    arc_5_7 = Arc(node_5, node_7, 0)
    node_5.out_arcs.append(arc_5_7)
    node_7.in_arcs.append(arc_5_7)
    arc_6_7 = Arc(node_6, node_7, 0)
    node_6.out_arcs.append(arc_6_7)
    node_7.in_arcs.append(arc_6_7)
    graph.add_node(node_7)

    node_8 = Node('8', bitarray('00000') )
    arc_6_8 = Arc(node_6, node_8, 1)
    node_6.out_arcs.append(arc_6_8)
    node_8.in_arcs.append(arc_6_8)
    graph.add_node(node_8)

    # Crear la sexta capa de nodos
    graph.add_new_layer()
    node_9 = Node('9', bitarray('00000') )
    arc_7_9_1 = Arc(node_7, node_9, 0)
    node_7.out_arcs.append(arc_7_9_1)
    node_9.in_arcs.append(arc_7_9_1)
    arc_7_9_2 = Arc(node_7, node_9, 1)
    node_7.out_arcs.append(arc_7_9_2)
    node_9.in_arcs.append(arc_7_9_2)
    arc_8_9 = Arc(node_8, node_9, 0)
    node_8.out_arcs.append(arc_8_9)
    node_9.in_arcs.append(arc_8_9)
    graph.add_node(node_9)
    return graph

def get_relaxed_dd_independent_set():
    node_0 = Node('0', bitarray('11111') )
    graph = Graph(node_0)

    # Crear la segunda capa de nodos
    graph.add_new_layer()
    node_1 = Node('1', bitarray('01111') )
    arc_0_1 = Arc(node_0, node_1, 0)
    node_0.out_arcs.append(arc_0_1)
    node_1.in_arcs.append(arc_0_1)
    graph.add_node(node_1)

    node_2 = Node('2', bitarray('00011') )
    arc_0_2 = Arc(node_0, node_2, 1)
    node_0.out_arcs.append(arc_0_2)
    node_2.in_arcs.append(arc_0_2)
    graph.add_node(node_2)

    graph.add_new_layer()
    node_3 = Node('3', bitarray('00001') )
    arc_1_3 = Arc(node_1, node_3, 1)
    node_1.out_arcs.append(arc_1_3)
    node_3.in_arcs.append(arc_1_3)
    graph.add_node(node_3)

    node_4 = Node('4', bitarray('00111') )
    arc_2_4 = Arc(node_2, node_4, 0)
    node_2.out_arcs.append(arc_2_4)
    node_4.in_arcs.append(arc_2_4)
    arc_1_4 = Arc(node_1, node_4, 0)
    node_1.out_arcs.append(arc_1_4)
    node_4.in_arcs.append(arc_1_4)
    graph.add_node(node_4)


    graph.add_new_layer()
    node_5 = Node('5', bitarray('00001') )
    arc_3_5 = Arc(node_3, node_5, 0)
    node_5.in_arcs.append(arc_3_5)
    node_3.out_arcs.append(arc_3_5)
    arc_4_5 = Arc(node_4, node_5, 1)
    node_5.in_arcs.append(arc_4_5)
    node_4.out_arcs.append(arc_4_5)
    graph.add_node(node_5)

    node_6 = Node('6', bitarray('00011') )
    arc_4_6 = Arc(node_4, node_6, 0)
    node_4.out_arcs.append(arc_4_6)
    node_6.in_arcs.append(arc_4_6)
    graph.add_node(node_6)

    # Crear la quinta capa de nodos
    graph.add_new_layer()
    node_7 = Node('7', bitarray('00001') )
    arc_5_7 = Arc(node_5, node_7, 0)
    node_5.out_arcs.append(arc_5_7)
    node_7.in_arcs.append(arc_5_7)
    arc_6_7 = Arc(node_6, node_7, 0)
    node_6.out_arcs.append(arc_6_7)
    node_7.in_arcs.append(arc_6_7)
    graph.add_node(node_7)

    node_8 = Node('8', bitarray('00000') )
    arc_6_8 = Arc(node_6, node_8, 1)
    node_6.out_arcs.append(arc_6_8)
    node_8.in_arcs.append(arc_6_8)
    graph.add_node(node_8)

    # Crear la sexta capa de nodos
    graph.add_new_layer()
    node_9 = Node('9', bitarray('00000') )
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