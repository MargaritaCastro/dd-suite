#include "Graph.h"
#include <iostream>

using namespace std;

Graph<dynamic_bitset<>>* GetExactDDIndependentSetSort() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("11111")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    // -------- Layer 1 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_1 = new Node<dynamic_bitset<>>(1, new dynamic_bitset<>(string("11110")));
    Arc<dynamic_bitset<>>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<dynamic_bitset<>>* node_2 = new Node<dynamic_bitset<>>(2, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    // -------- Layer 2 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_3 = new Node<dynamic_bitset<>>(3, new dynamic_bitset<>(string("11100")));
    Arc<dynamic_bitset<>>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);

    Node<dynamic_bitset<>>* node_4 = new Node<dynamic_bitset<>>(4, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);

    Arc<dynamic_bitset<>>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);

    graph->add_node(node_4);

    // -------- Layer 3 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_5 = new Node<dynamic_bitset<>>(5, new dynamic_bitset<>(string("11000")));
    Arc<dynamic_bitset<>>* arc_3_5 = new Arc(node_3, node_5, 0);
    node_3->out_arcs.push_back(arc_3_5);
    node_5->in_arcs.push_back(arc_3_5);
    graph->add_node(node_5);

    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("01000")));
    Arc<dynamic_bitset<>>* arc_3_6 = new Arc(node_3, node_6, 1);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    graph->add_node(node_6);

    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_4_7 = new Arc(node_4, node_7, 0);
    node_4->out_arcs.push_back(arc_4_7);
    node_7->in_arcs.push_back(arc_4_7);
    graph->add_node(node_7);

    // -------- Layer 4 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("10000")));

    Arc<dynamic_bitset<>>* arc_5_8_1 = new Arc(node_5, node_8, 0);
    Arc<dynamic_bitset<>>* arc_5_8_2 = new Arc(node_5, node_8, 1);
    node_5->out_arcs.push_back(arc_5_8_1);
    node_5->out_arcs.push_back(arc_5_8_2);
    node_8->in_arcs.push_back(arc_5_8_1);
    node_8->in_arcs.push_back(arc_5_8_2);

    Arc<dynamic_bitset<>>* arc_7_8 = new Arc(node_7, node_8, 0);
    node_7->out_arcs.push_back(arc_7_8);
    node_8->in_arcs.push_back(arc_7_8);

    graph->add_node(node_8);

    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("00000")));

    Arc<dynamic_bitset<>>* arc_6_9_1 = new Arc(node_6, node_9, 0);
    Arc<dynamic_bitset<>>* arc_6_9_2 = new Arc(node_6, node_9, 1);
    node_6->out_arcs.push_back(arc_6_9_1);
    node_6->out_arcs.push_back(arc_6_9_2);
    node_9->in_arcs.push_back(arc_6_9_1);
    node_9->in_arcs.push_back(arc_6_9_2);

    graph->add_node(node_9);

    // -------- Layer 5 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_10 = new Node<dynamic_bitset<>>(10, new dynamic_bitset<>(string("00000")));

    Arc<dynamic_bitset<>>* arc_8_10_1 = new Arc(node_8, node_10, 0);
    Arc<dynamic_bitset<>>* arc_8_10_2 = new Arc(node_8, node_10, 1);
    node_8->out_arcs.push_back(arc_8_10_1);
    node_8->out_arcs.push_back(arc_8_10_2);
    node_10->in_arcs.push_back(arc_8_10_1);
    node_10->in_arcs.push_back(arc_8_10_2);

    Arc<dynamic_bitset<>>* arc_9_10 = new Arc(node_9, node_10, 0);
    node_9->out_arcs.push_back(arc_9_10);
    node_10->in_arcs.push_back(arc_9_10);

    graph->add_node(node_10);

    return graph;
}

Graph<dynamic_bitset<>>* GetReduceDDIndependentSetSort() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("11111")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    // -------- Layer 1 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_1 = new Node<dynamic_bitset<>>(1, new dynamic_bitset<>(string("11110")));
    Arc<dynamic_bitset<>>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<dynamic_bitset<>>* node_2 = new Node<dynamic_bitset<>>(2, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    // -------- Layer 2 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_3 = new Node<dynamic_bitset<>>(3, new dynamic_bitset<>(string("11100")));
    Arc<dynamic_bitset<>>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);

    Node<dynamic_bitset<>>* node_4 = new Node<dynamic_bitset<>>(4, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);

    Arc<dynamic_bitset<>>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);

    graph->add_node(node_4);

    // -------- Layer 3 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_5 = new Node<dynamic_bitset<>>(5, new dynamic_bitset<>(string("11000")));
    Arc<dynamic_bitset<>>* arc_3_5 = new Arc(node_3, node_5, 0);
    node_3->out_arcs.push_back(arc_3_5);
    node_5->in_arcs.push_back(arc_3_5);
    graph->add_node(node_5);

    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("01000")));
    Arc<dynamic_bitset<>>* arc_3_6 = new Arc(node_3, node_6, 1);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    graph->add_node(node_6);

    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_4_7 = new Arc(node_4, node_7, 0);
    node_4->out_arcs.push_back(arc_4_7);
    node_7->in_arcs.push_back(arc_4_7);
    graph->add_node(node_7);

    // -------- Layer 4 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("10000")));

    Arc<dynamic_bitset<>>* arc_5_8_1 = new Arc(node_5, node_8, 0);
    Arc<dynamic_bitset<>>* arc_5_8_2 = new Arc(node_5, node_8, 1);
    node_5->out_arcs.push_back(arc_5_8_1);
    node_5->out_arcs.push_back(arc_5_8_2);
    node_8->in_arcs.push_back(arc_5_8_1);
    node_8->in_arcs.push_back(arc_5_8_2);

    Arc<dynamic_bitset<>>* arc_7_8 = new Arc(node_7, node_8, 0);
    node_7->out_arcs.push_back(arc_7_8);
    node_8->in_arcs.push_back(arc_7_8);

    graph->add_node(node_8);

    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("00000")));

    Arc<dynamic_bitset<>>* arc_6_9_1 = new Arc(node_6, node_9, 0);
    Arc<dynamic_bitset<>>* arc_6_9_2 = new Arc(node_6, node_9, 1);
    node_6->out_arcs.push_back(arc_6_9_1);
    node_6->out_arcs.push_back(arc_6_9_2);
    node_9->in_arcs.push_back(arc_6_9_1);
    node_9->in_arcs.push_back(arc_6_9_2);

    graph->add_node(node_9);

    // -------- Layer 5 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_10 = new Node<dynamic_bitset<>>(10, new dynamic_bitset<>(string("00000")));

    Arc<dynamic_bitset<>>* arc_8_10_1 = new Arc(node_8, node_10, 0);
    Arc<dynamic_bitset<>>* arc_8_10_2 = new Arc(node_8, node_10, 1);
    node_8->out_arcs.push_back(arc_8_10_1);
    node_8->out_arcs.push_back(arc_8_10_2);
    node_10->in_arcs.push_back(arc_8_10_1);
    node_10->in_arcs.push_back(arc_8_10_2);

    Arc<dynamic_bitset<>>* arc_9_10 = new Arc(node_9, node_10, 0);
    node_9->out_arcs.push_back(arc_9_10);
    node_10->in_arcs.push_back(arc_9_10);

    graph->add_node(node_10);

    return graph;
    
}

Graph<dynamic_bitset<>>* GetRestrictedDDIndependentSetSort() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("11111")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    // Layer 1
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_1 = new Node<dynamic_bitset<>>(1, new dynamic_bitset<>(string("11110")));
    Arc<dynamic_bitset<>>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<dynamic_bitset<>>* node_2 = new Node<dynamic_bitset<>>(2, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    // Layer 2
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_3 = new Node<dynamic_bitset<>>(3, new dynamic_bitset<>(string("11100")));
    Arc<dynamic_bitset<>>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);

    Node<dynamic_bitset<>>* node_4 = new Node<dynamic_bitset<>>(4, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);

    Arc<dynamic_bitset<>>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    // Layer 3
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_5 = new Node<dynamic_bitset<>>(5, new dynamic_bitset<>(string("01000")));
    Arc<dynamic_bitset<>>* arc_3_5 = new Arc(node_3, node_5, 1);
    node_3->out_arcs.push_back(arc_3_5);
    node_5->in_arcs.push_back(arc_3_5);
    graph->add_node(node_5);

    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_4_6 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6);
    node_6->in_arcs.push_back(arc_4_6);
    graph->add_node(node_6);

    // Layer 4
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("00000")));
    
    Arc<dynamic_bitset<>>* arc_5_7_op1 = new Arc(node_5, node_7, 0);
    node_5->out_arcs.push_back(arc_5_7_op1);
    node_7->in_arcs.push_back(arc_5_7_op1);

    Arc<dynamic_bitset<>>* arc_5_7_op2 = new Arc(node_5, node_7, 1);
    node_5->out_arcs.push_back(arc_5_7_op2);
    node_7->in_arcs.push_back(arc_5_7_op2);

    graph->add_node(node_7);

    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_6_8 = new Arc(node_6, node_8, 0);
    node_6->out_arcs.push_back(arc_6_8);
    node_8->in_arcs.push_back(arc_6_8);
    graph->add_node(node_8);

    // Layer 5
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("00000")));

    Arc<dynamic_bitset<>>* arc_7_9 = new Arc(node_7, node_9, 0);
    node_7->out_arcs.push_back(arc_7_9);
    node_9->in_arcs.push_back(arc_7_9);

    Arc<dynamic_bitset<>>* arc_8_9_op1 = new Arc(node_8, node_9, 0);
    node_8->out_arcs.push_back(arc_8_9_op1);
    node_9->in_arcs.push_back(arc_8_9_op1);

    Arc<dynamic_bitset<>>* arc_8_9_op2 = new Arc(node_8, node_9, 1);
    node_8->out_arcs.push_back(arc_8_9_op2);
    node_9->in_arcs.push_back(arc_8_9_op2);

    graph->add_node(node_9);

    return graph;
}

Graph<dynamic_bitset<>>* GetRelaxedDDIndependentSetSort() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("11111")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    // Layer 1
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_1 = new Node<dynamic_bitset<>>(1, new dynamic_bitset<>(string("11110")));
    Arc<dynamic_bitset<>>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<dynamic_bitset<>>* node_2 = new Node<dynamic_bitset<>>(2, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    // Layer 2
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_3 = new Node<dynamic_bitset<>>(3, new dynamic_bitset<>(string("11100")));
    Arc<dynamic_bitset<>>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);

    Node<dynamic_bitset<>>* node_4 = new Node<dynamic_bitset<>>(4, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);

    Arc<dynamic_bitset<>>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    // Layer 3
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_5 = new Node<dynamic_bitset<>>(5, new dynamic_bitset<>(string("11000")));

    Arc<dynamic_bitset<>>* arc_3_5_op0 = new Arc(node_3, node_5, 0);
    node_3->out_arcs.push_back(arc_3_5_op0);
    node_5->in_arcs.push_back(arc_3_5_op0);

    Arc<dynamic_bitset<>>* arc_3_5_op1 = new Arc(node_3, node_5, 1);
    node_3->out_arcs.push_back(arc_3_5_op1);
    node_5->in_arcs.push_back(arc_3_5_op1);

    graph->add_node(node_5);

    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_4_6 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6);
    node_6->in_arcs.push_back(arc_4_6);
    graph->add_node(node_6);

    // Layer 4
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("10000")));

    Arc<dynamic_bitset<>>* arc_5_7_op0 = new Arc(node_5, node_7, 0);
    node_5->out_arcs.push_back(arc_5_7_op0);
    node_7->in_arcs.push_back(arc_5_7_op0);

    Arc<dynamic_bitset<>>* arc_5_7_op1 = new Arc(node_5, node_7, 1);
    node_5->out_arcs.push_back(arc_5_7_op1);
    node_7->in_arcs.push_back(arc_5_7_op1);

    Arc<dynamic_bitset<>>* arc_6_7 = new Arc(node_6, node_7, 0);
    node_6->out_arcs.push_back(arc_6_7);
    node_7->in_arcs.push_back(arc_6_7);

    graph->add_node(node_7);

    // Layer 5 (terminal)
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("00000")));

    Arc<dynamic_bitset<>>* arc_7_8_op0 = new Arc(node_7, node_8, 0);
    node_7->out_arcs.push_back(arc_7_8_op0);
    node_8->in_arcs.push_back(arc_7_8_op0);

    Arc<dynamic_bitset<>>* arc_7_8_op1 = new Arc(node_7, node_8, 1);
    node_7->out_arcs.push_back(arc_7_8_op1);
    node_8->in_arcs.push_back(arc_7_8_op1);

    graph->add_node(node_8);

    return graph;
}

Graph<dynamic_bitset<>>* GetFalseDDIndependentSetSort() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("11110")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_1 = new Node<dynamic_bitset<>>(1, new dynamic_bitset<>(string("01111")));
    Arc<dynamic_bitset<>>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<dynamic_bitset<>>* node_2 = new Node<dynamic_bitset<>>(2, new dynamic_bitset<>(string("11000")));
    Arc<dynamic_bitset<>>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_3 = new Node<dynamic_bitset<>>(3, new dynamic_bitset<>(string("11100")));
    Arc<dynamic_bitset<>>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);


    Node<dynamic_bitset<>>* node_4 = new Node<dynamic_bitset<>>(4, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    graph->add_node(node_4);

    Node<dynamic_bitset<>>* node_5 = new Node<dynamic_bitset<>>(5, new dynamic_bitset<>(string("11000")));
    Arc<dynamic_bitset<>>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("11000")));
    Arc<dynamic_bitset<>>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_6->in_arcs.push_back(arc_3_6);
    node_3->out_arcs.push_back(arc_3_6);
    Arc<dynamic_bitset<>>* arc_5_6 = new Arc(node_5, node_6, 0);
    node_5->out_arcs.push_back(arc_5_6);
    node_6->in_arcs.push_back(arc_5_6);
    graph->add_node(node_6);

    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_3_7 = new Arc(node_3, node_7, 1);
    node_3->out_arcs.push_back(arc_3_7);
    node_7->in_arcs.push_back(arc_3_7);
    Arc<dynamic_bitset<>>* arc_4_7 = new Arc(node_4, node_7, 0);
    node_4->out_arcs.push_back(arc_4_7);
    node_7->in_arcs.push_back(arc_4_7);
    graph->add_node(node_7);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("10000")));
    Arc<dynamic_bitset<>>* arc_6_8 = new Arc(node_6, node_8, 0);
    node_6->out_arcs.push_back(arc_6_8);
    node_8->in_arcs.push_back(arc_6_8);
    Arc<dynamic_bitset<>>* arc_7_8 = new Arc(node_7, node_8, 0);
    node_7->out_arcs.push_back(arc_7_8);
    node_8->in_arcs.push_back(arc_7_8);
    graph->add_node(node_8);

    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("00000")));
    Arc<dynamic_bitset<>>* arc_6_9 = new Arc(node_6, node_9, 1);
    node_6->out_arcs.push_back(arc_6_9);
    node_9->in_arcs.push_back(arc_6_9);
    graph->add_node(node_9);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_10 = new Node<dynamic_bitset<>>(10, new dynamic_bitset<>(string("00000")));
    Arc<dynamic_bitset<>>* arc_8_10_1 = new Arc(node_8, node_10, 0);
    node_8->out_arcs.push_back(arc_8_10_1);
    node_10->in_arcs.push_back(arc_8_10_1);
    Arc<dynamic_bitset<>>* arc_8_10_2 = new Arc(node_8, node_10, 1);
    node_8->out_arcs.push_back(arc_8_10_2);
    node_10->in_arcs.push_back(arc_8_10_2);
    Arc<dynamic_bitset<>>* arc_9_10 = new Arc(node_9, node_10, 0);
    node_9->out_arcs.push_back(arc_9_10);
    node_10->in_arcs.push_back(arc_9_10);
    graph->add_node(node_10);


    return graph;
}