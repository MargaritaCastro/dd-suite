#include "Graph.h"
#include <iostream>

using namespace std;

Graph<dynamic_bitset<>>* GetExactDDSetCover() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("111")) );
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_1 = new Node<dynamic_bitset<>>(1, new dynamic_bitset<>(string("111")));
    Arc<dynamic_bitset<>>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<dynamic_bitset<>>* node_2 = new Node<dynamic_bitset<>>(2, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_3 = new Node<dynamic_bitset<>>(3, new dynamic_bitset<>(string("111")));
    Arc<dynamic_bitset<>>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);

    Node<dynamic_bitset<>>* node_4 = new Node<dynamic_bitset<>>(4, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    graph->add_node(node_4);

    Node<dynamic_bitset<>>* node_5 = new Node<dynamic_bitset<>>(5, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_2_6 = new Arc(node_2, node_6, 1);
    node_2->out_arcs.push_back(arc_2_6);
    node_6->in_arcs.push_back(arc_2_6);
    graph->add_node(node_6);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("110")));
    Arc<dynamic_bitset<>>* arc_3_7 = new Arc(node_3, node_7, 1);
    node_3->out_arcs.push_back(arc_3_7);
    node_7->in_arcs.push_back(arc_3_7);
    graph->add_node(node_7);

    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_4_8_op1 = new Arc(node_4, node_8, 0);
    Arc<dynamic_bitset<>>* arc_4_8_op2 = new Arc(node_4, node_8, 1);
    node_4->out_arcs.push_back(arc_4_8_op1);
    node_4->out_arcs.push_back(arc_4_8_op2);
    node_8->in_arcs.push_back(arc_4_8_op1);
    node_8->in_arcs.push_back(arc_4_8_op2);
    graph->add_node(node_8);

    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_5_9_op1 = new Arc(node_5, node_9, 0);
    Arc<dynamic_bitset<>>* arc_5_9_op2 = new Arc(node_5, node_9, 1);
    node_5->out_arcs.push_back(arc_5_9_op1);
    node_5->out_arcs.push_back(arc_5_9_op2);
    node_9->in_arcs.push_back(arc_5_9_op1);
    node_9->in_arcs.push_back(arc_5_9_op2);
    graph->add_node(node_9);

    Node<dynamic_bitset<>>* node_10 = new Node<dynamic_bitset<>>(10, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_6_10_op1 = new Arc(node_6, node_10, 0);
    Arc<dynamic_bitset<>>* arc_6_10_op2 = new Arc(node_6, node_10, 1);
    node_6->out_arcs.push_back(arc_6_10_op1);
    node_6->out_arcs.push_back(arc_6_10_op2);
    node_10->in_arcs.push_back(arc_6_10_op1);
    node_10->in_arcs.push_back(arc_6_10_op2);
    graph->add_node(node_10);

    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_11 = new Node<dynamic_bitset<>>(11, new dynamic_bitset<>(string("110")));
    Arc<dynamic_bitset<>>* arc_7_11 = new Arc(node_7, node_11, 0);
    node_7->out_arcs.push_back(arc_7_11);
    node_11->in_arcs.push_back(arc_7_11);
    graph->add_node(node_11);

    Node<dynamic_bitset<>>* node_12 = new Node<dynamic_bitset<>>(12, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_7_12 = new Arc(node_7, node_12, 1);
    Arc<dynamic_bitset<>>* arc_8_12 = new Arc(node_8, node_12, 1);
    Arc<dynamic_bitset<>>* arc_9_12 = new Arc(node_9, node_12, 1);
    Arc<dynamic_bitset<>>* arc_10_12_op1 = new Arc(node_10, node_12, 0);
    Arc<dynamic_bitset<>>* arc_10_12_op2 = new Arc(node_10, node_12, 1);
    node_7->out_arcs.push_back(arc_7_12);
    node_8->out_arcs.push_back(arc_8_12);
    node_9->out_arcs.push_back(arc_9_12);
    node_10->out_arcs.push_back(arc_10_12_op1);
    node_10->out_arcs.push_back(arc_10_12_op2);
    node_12->in_arcs.push_back(arc_7_12);
    node_12->in_arcs.push_back(arc_8_12);
    node_12->in_arcs.push_back(arc_9_12);
    node_12->in_arcs.push_back(arc_10_12_op1);
    node_12->in_arcs.push_back(arc_10_12_op2);
    graph->add_node(node_12);

    Node<dynamic_bitset<>>* node_13 = new Node<dynamic_bitset<>>(13, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_8_13 = new Arc(node_8, node_13, 0);
    node_8->out_arcs.push_back(arc_8_13);
    node_13->in_arcs.push_back(arc_8_13);
    graph->add_node(node_13);

    Node<dynamic_bitset<>>* node_14 = new Node<dynamic_bitset<>>(14, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_9_14 = new Arc(node_9, node_14, 0);
    node_9->out_arcs.push_back(arc_9_14);
    node_14->in_arcs.push_back(arc_9_14);
    graph->add_node(node_14);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_15 = new Node<dynamic_bitset<>>(15, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_11_15 = new Arc(node_11, node_15, 1);
    Arc<dynamic_bitset<>>* arc_14_15_op1 = new Arc(node_14, node_15, 1);
    Arc<dynamic_bitset<>>* arc_14_15_op2 = new Arc(node_14, node_15, 0);
    node_11->out_arcs.push_back(arc_11_15);
    node_14->out_arcs.push_back(arc_14_15_op1);
    node_14->out_arcs.push_back(arc_14_15_op2);
    node_15->in_arcs.push_back(arc_11_15);
    node_15->in_arcs.push_back(arc_14_15_op1);
    node_15->in_arcs.push_back(arc_14_15_op2);
    graph->add_node(node_15);

    Node<dynamic_bitset<>>* node_16 = new Node<dynamic_bitset<>>(16, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_12_16_op1 = new Arc(node_12, node_16, 0);
    Arc<dynamic_bitset<>>* arc_12_16_op2 = new Arc(node_12, node_16, 1);
    Arc<dynamic_bitset<>>* arc_13_16 = new Arc(node_13, node_16, 1);
    node_12->out_arcs.push_back(arc_12_16_op1);
    node_12->out_arcs.push_back(arc_12_16_op2);
    node_13->out_arcs.push_back(arc_13_16);
    node_16->in_arcs.push_back(arc_12_16_op1);
    node_16->in_arcs.push_back(arc_12_16_op2);
    node_16->in_arcs.push_back(arc_13_16);
    graph->add_node(node_16);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_17 = new Node<dynamic_bitset<>>(17, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_15_17 = new Arc(node_15, node_17, 1);
    Arc<dynamic_bitset<>>* arc_16_17_op1 = new Arc(node_16, node_17, 0);
    Arc<dynamic_bitset<>>* arc_16_17_op2 = new Arc(node_16, node_17, 1);
    node_15->out_arcs.push_back(arc_15_17);
    node_16->out_arcs.push_back(arc_16_17_op1);
    node_16->out_arcs.push_back(arc_16_17_op2);
    node_17->in_arcs.push_back(arc_15_17);
    node_17->in_arcs.push_back(arc_16_17_op1);
    node_17->in_arcs.push_back(arc_16_17_op2);
    graph->add_node(node_17);

    return graph;
}

Graph<dynamic_bitset<>>* GetReduceDDSetCover() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("111")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_1 = new Node<dynamic_bitset<>>(1, new dynamic_bitset<>(string("111")));
    Arc<dynamic_bitset<>>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<dynamic_bitset<>>* node_2 = new Node<dynamic_bitset<>>(2, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_3 = new Node<dynamic_bitset<>>(3, new dynamic_bitset<>(string("111")));
    Arc<dynamic_bitset<>>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);

    Node<dynamic_bitset<>>* node_4 = new Node<dynamic_bitset<>>(4, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    graph->add_node(node_4);

    Node<dynamic_bitset<>>* node_5 = new Node<dynamic_bitset<>>(5, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_2_6 = new Arc(node_2, node_6, 1);
    node_2->out_arcs.push_back(arc_2_6);
    node_6->in_arcs.push_back(arc_2_6);
    graph->add_node(node_6);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("110")));
    Arc<dynamic_bitset<>>* arc_3_7 = new Arc(node_3, node_7, 1);
    node_3->out_arcs.push_back(arc_3_7);
    node_7->in_arcs.push_back(arc_3_7);
    graph->add_node(node_7);

    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_4_8_op1 = new Arc(node_4, node_8, 0);
    Arc<dynamic_bitset<>>* arc_4_8_op2 = new Arc(node_4, node_8, 1);
    node_4->out_arcs.push_back(arc_4_8_op1);
    node_4->out_arcs.push_back(arc_4_8_op2);
    node_8->in_arcs.push_back(arc_4_8_op1);
    node_8->in_arcs.push_back(arc_4_8_op2);
    graph->add_node(node_8);

    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_5_9_op1 = new Arc(node_5, node_9, 0);
    Arc<dynamic_bitset<>>* arc_5_9_op2 = new Arc(node_5, node_9, 1);
    node_5->out_arcs.push_back(arc_5_9_op1);
    node_5->out_arcs.push_back(arc_5_9_op2);
    node_9->in_arcs.push_back(arc_5_9_op1);
    node_9->in_arcs.push_back(arc_5_9_op2);
    graph->add_node(node_9);

    Node<dynamic_bitset<>>* node_10 = new Node<dynamic_bitset<>>(10, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_6_10_op1 = new Arc(node_6, node_10, 0);
    Arc<dynamic_bitset<>>* arc_6_10_op2 = new Arc(node_6, node_10, 1);
    node_6->out_arcs.push_back(arc_6_10_op1);
    node_6->out_arcs.push_back(arc_6_10_op2);
    node_10->in_arcs.push_back(arc_6_10_op1);
    node_10->in_arcs.push_back(arc_6_10_op2);
    graph->add_node(node_10);

    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_11 = new Node<dynamic_bitset<>>(11, new dynamic_bitset<>(string("110")));
    Arc<dynamic_bitset<>>* arc_7_11 = new Arc(node_7, node_11, 0);
    node_7->out_arcs.push_back(arc_7_11);
    node_11->in_arcs.push_back(arc_7_11);
    graph->add_node(node_11);

    Node<dynamic_bitset<>>* node_12 = new Node<dynamic_bitset<>>(12, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_7_12 = new Arc(node_7, node_12, 1);
    Arc<dynamic_bitset<>>* arc_8_12 = new Arc(node_8, node_12, 1);
    Arc<dynamic_bitset<>>* arc_9_12 = new Arc(node_9, node_12, 1);
    Arc<dynamic_bitset<>>* arc_10_12_op1 = new Arc(node_10, node_12, 0);
    Arc<dynamic_bitset<>>* arc_10_12_op2 = new Arc(node_10, node_12, 1);
    node_7->out_arcs.push_back(arc_7_12);
    node_8->out_arcs.push_back(arc_8_12);
    node_9->out_arcs.push_back(arc_9_12);
    node_10->out_arcs.push_back(arc_10_12_op1);
    node_10->out_arcs.push_back(arc_10_12_op2);
    node_12->in_arcs.push_back(arc_7_12);
    node_12->in_arcs.push_back(arc_8_12);
    node_12->in_arcs.push_back(arc_9_12);
    node_12->in_arcs.push_back(arc_10_12_op1);
    node_12->in_arcs.push_back(arc_10_12_op2);
    graph->add_node(node_12);

    Node<dynamic_bitset<>>* node_13 = new Node<dynamic_bitset<>>(13, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_8_13 = new Arc(node_8, node_13, 0);
    node_8->out_arcs.push_back(arc_8_13);
    node_13->in_arcs.push_back(arc_8_13);
    graph->add_node(node_13);

    Node<dynamic_bitset<>>* node_14 = new Node<dynamic_bitset<>>(14, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_9_14 = new Arc(node_9, node_14, 0);
    node_9->out_arcs.push_back(arc_9_14);
    node_14->in_arcs.push_back(arc_9_14);
    graph->add_node(node_14);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_15 = new Node<dynamic_bitset<>>(15, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_11_15 = new Arc(node_11, node_15, 1);
    Arc<dynamic_bitset<>>* arc_14_15_op1 = new Arc(node_14, node_15, 1);
    Arc<dynamic_bitset<>>* arc_14_15_op2 = new Arc(node_14, node_15, 0);
    node_11->out_arcs.push_back(arc_11_15);
    node_14->out_arcs.push_back(arc_14_15_op1);
    node_14->out_arcs.push_back(arc_14_15_op2);
    node_15->in_arcs.push_back(arc_11_15);
    node_15->in_arcs.push_back(arc_14_15_op1);
    node_15->in_arcs.push_back(arc_14_15_op2);
    graph->add_node(node_15);

    Node<dynamic_bitset<>>* node_16 = new Node<dynamic_bitset<>>(16, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_12_16_op1 = new Arc(node_12, node_16, 0);
    Arc<dynamic_bitset<>>* arc_12_16_op2 = new Arc(node_12, node_16, 1);
    Arc<dynamic_bitset<>>* arc_13_16 = new Arc(node_13, node_16, 1);
    node_12->out_arcs.push_back(arc_12_16_op1);
    node_12->out_arcs.push_back(arc_12_16_op2);
    node_13->out_arcs.push_back(arc_13_16);
    node_16->in_arcs.push_back(arc_12_16_op1);
    node_16->in_arcs.push_back(arc_12_16_op2);
    node_16->in_arcs.push_back(arc_13_16);
    graph->add_node(node_16);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_17 = new Node<dynamic_bitset<>>(17, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_15_17 = new Arc(node_15, node_17, 1);
    Arc<dynamic_bitset<>>* arc_16_17_op1 = new Arc(node_16, node_17, 0);
    Arc<dynamic_bitset<>>* arc_16_17_op2 = new Arc(node_16, node_17, 1);
    node_15->out_arcs.push_back(arc_15_17);
    node_16->out_arcs.push_back(arc_16_17_op1);
    node_16->out_arcs.push_back(arc_16_17_op2);
    node_17->in_arcs.push_back(arc_15_17);
    node_17->in_arcs.push_back(arc_16_17_op1);
    node_17->in_arcs.push_back(arc_16_17_op2);
    graph->add_node(node_17);

    return graph;
}

Graph<dynamic_bitset<>>* GetRestrictedDDSetCover() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("111")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_1 = new Node<dynamic_bitset<>>(1, new dynamic_bitset<>(string("111")));
    Arc<dynamic_bitset<>>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<dynamic_bitset<>>* node_2 = new Node<dynamic_bitset<>>(2, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_3 = new Node<dynamic_bitset<>>(3, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_1_3 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);

    Node<dynamic_bitset<>>* node_4 = new Node<dynamic_bitset<>>(4, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<dynamic_bitset<>>* node_5 = new Node<dynamic_bitset<>>(5, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_2_5 = new Arc(node_2, node_5, 1);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_3_6_op1 = new Arc(node_3, node_6, 0);
    Arc<dynamic_bitset<>>* arc_3_6_op2 = new Arc(node_3, node_6, 1);
    node_3->out_arcs.push_back(arc_3_6_op1);
    node_3->out_arcs.push_back(arc_3_6_op2);
    node_6->in_arcs.push_back(arc_3_6_op1);
    node_6->in_arcs.push_back(arc_3_6_op2);
    graph->add_node(node_6);

    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_4_7_op1 = new Arc(node_4, node_7, 0);
    Arc<dynamic_bitset<>>* arc_4_7_op2 = new Arc(node_4, node_7, 1);
    node_4->out_arcs.push_back(arc_4_7_op1);
    node_4->out_arcs.push_back(arc_4_7_op2);
    node_7->in_arcs.push_back(arc_4_7_op1);
    node_7->in_arcs.push_back(arc_4_7_op2);
    graph->add_node(node_7);

    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_5_8_op1 = new Arc(node_5, node_8, 0);
    Arc<dynamic_bitset<>>* arc_5_8_op2 = new Arc(node_5, node_8, 1);
    node_5->out_arcs.push_back(arc_5_8_op1);
    node_5->out_arcs.push_back(arc_5_8_op2);
    node_8->in_arcs.push_back(arc_5_8_op1);
    node_8->in_arcs.push_back(arc_5_8_op2);
    graph->add_node(node_8);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_6_9 = new Arc(node_6, node_9, 0);
    node_6->out_arcs.push_back(arc_6_9);
    node_9->in_arcs.push_back(arc_6_9);
    graph->add_node(node_9);

    Node<dynamic_bitset<>>* node_10 = new Node<dynamic_bitset<>>(10, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_6_10 = new Arc(node_6, node_10, 1);
    Arc<dynamic_bitset<>>* arc_7_10 = new Arc(node_7, node_10, 1);
    Arc<dynamic_bitset<>>* arc_8_10_op1 = new Arc(node_8, node_10, 0);
    Arc<dynamic_bitset<>>* arc_8_10_op2 = new Arc(node_8, node_10, 1);
    node_6->out_arcs.push_back(arc_6_10);
    node_7->out_arcs.push_back(arc_7_10);
    node_8->out_arcs.push_back(arc_8_10_op1);
    node_8->out_arcs.push_back(arc_8_10_op2);
    node_10->in_arcs.push_back(arc_6_10);
    node_10->in_arcs.push_back(arc_7_10);
    node_10->in_arcs.push_back(arc_8_10_op1);
    node_10->in_arcs.push_back(arc_8_10_op2);
    graph->add_node(node_10);

    Node<dynamic_bitset<>>* node_11 = new Node<dynamic_bitset<>>(11, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_7_11 = new Arc(node_7, node_11, 0);
    node_7->out_arcs.push_back(arc_7_11);
    node_11->in_arcs.push_back(arc_7_11);
    graph->add_node(node_11);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_12 = new Node<dynamic_bitset<>>(12, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_9_12 = new Arc(node_9, node_12, 1);
    Arc<dynamic_bitset<>>* arc_10_12_op1 = new Arc(node_10, node_12, 0);
    Arc<dynamic_bitset<>>* arc_10_12_op2 = new Arc(node_10, node_12, 1);
    node_9->out_arcs.push_back(arc_9_12);
    node_10->out_arcs.push_back(arc_10_12_op1);
    node_10->out_arcs.push_back(arc_10_12_op2);
    node_12->in_arcs.push_back(arc_9_12);
    node_12->in_arcs.push_back(arc_10_12_op1);
    node_12->in_arcs.push_back(arc_10_12_op2);
    graph->add_node(node_12);

    Node<dynamic_bitset<>>* node_13 = new Node<dynamic_bitset<>>(13, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_11_13_op1 = new Arc(node_11, node_13, 0);
    Arc<dynamic_bitset<>>* arc_11_13_op2 = new Arc(node_11, node_13, 1);
    node_11->out_arcs.push_back(arc_11_13_op1);
    node_11->out_arcs.push_back(arc_11_13_op2);
    node_13->in_arcs.push_back(arc_11_13_op1);
    node_13->in_arcs.push_back(arc_11_13_op2);
    graph->add_node(node_13);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_14 = new Node<dynamic_bitset<>>(14, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_12_14_op1 = new Arc(node_12, node_14, 0);
    Arc<dynamic_bitset<>>* arc_12_14_op2 = new Arc(node_12, node_14, 1);
    Arc<dynamic_bitset<>>* arc_13_14 = new Arc(node_13, node_14, 1);
    node_12->out_arcs.push_back(arc_12_14_op1);
    node_12->out_arcs.push_back(arc_12_14_op2);
    node_13->out_arcs.push_back(arc_13_14);
    node_14->in_arcs.push_back(arc_12_14_op1);
    node_14->in_arcs.push_back(arc_12_14_op2);
    node_14->in_arcs.push_back(arc_13_14);
    graph->add_node(node_14);

    return graph;
}

Graph<dynamic_bitset<>>* GetRelaxedDDSetCover() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("111")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_1 = new Node<dynamic_bitset<>>(1, new dynamic_bitset<>(string("111")));
    Arc<dynamic_bitset<>>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<dynamic_bitset<>>* node_2 = new Node<dynamic_bitset<>>(2, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_3 = new Node<dynamic_bitset<>>(3, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_1_3_op1 = new Arc(node_1, node_3, 0);
    Arc<dynamic_bitset<>>* arc_1_3_op2 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3_op1);
    node_1->out_arcs.push_back(arc_1_3_op2);
    node_3->in_arcs.push_back(arc_1_3_op1);
    node_3->in_arcs.push_back(arc_1_3_op2);
    graph->add_node(node_3);

    Node<dynamic_bitset<>>* node_4 = new Node<dynamic_bitset<>>(4, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<dynamic_bitset<>>* node_5 = new Node<dynamic_bitset<>>(5, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_2_5 = new Arc(node_2, node_5, 1);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_3_6_op1 = new Arc(node_3, node_6, 0);
    Arc<dynamic_bitset<>>* arc_3_6_op2 = new Arc(node_3, node_6, 1);
    node_3->out_arcs.push_back(arc_3_6_op1);
    node_3->out_arcs.push_back(arc_3_6_op2);
    node_6->in_arcs.push_back(arc_3_6_op1);
    node_6->in_arcs.push_back(arc_3_6_op2);
    graph->add_node(node_6);

    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_4_7_op1 = new Arc(node_4, node_7, 0);
    Arc<dynamic_bitset<>>* arc_4_7_op2 = new Arc(node_4, node_7, 1);
    node_4->out_arcs.push_back(arc_4_7_op1);
    node_4->out_arcs.push_back(arc_4_7_op2);
    node_7->in_arcs.push_back(arc_4_7_op1);
    node_7->in_arcs.push_back(arc_4_7_op2);
    graph->add_node(node_7);

    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_5_8_op1 = new Arc(node_5, node_8, 0);
    Arc<dynamic_bitset<>>* arc_5_8_op2 = new Arc(node_5, node_8, 1);
    node_5->out_arcs.push_back(arc_5_8_op1);
    node_5->out_arcs.push_back(arc_5_8_op2);
    node_8->in_arcs.push_back(arc_5_8_op1);
    node_8->in_arcs.push_back(arc_5_8_op2);
    graph->add_node(node_8);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_6_9 = new Arc(node_6, node_9, 0);
    node_6->out_arcs.push_back(arc_6_9);
    node_9->in_arcs.push_back(arc_6_9);
    graph->add_node(node_9);

    Node<dynamic_bitset<>>* node_10 = new Node<dynamic_bitset<>>(10, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_6_10 = new Arc(node_6, node_10, 1);
    Arc<dynamic_bitset<>>* arc_7_10 = new Arc(node_7, node_10, 1);
    Arc<dynamic_bitset<>>* arc_8_10_op1 = new Arc(node_8, node_10, 0);
    Arc<dynamic_bitset<>>* arc_8_10_op2 = new Arc(node_8, node_10, 1);
    node_6->out_arcs.push_back(arc_6_10);
    node_7->out_arcs.push_back(arc_7_10);
    node_8->out_arcs.push_back(arc_8_10_op1);
    node_8->out_arcs.push_back(arc_8_10_op2);
    node_10->in_arcs.push_back(arc_6_10);
    node_10->in_arcs.push_back(arc_7_10);
    node_10->in_arcs.push_back(arc_8_10_op1);
    node_10->in_arcs.push_back(arc_8_10_op2);
    graph->add_node(node_10);

    Node<dynamic_bitset<>>* node_11 = new Node<dynamic_bitset<>>(11, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_7_11 = new Arc(node_7, node_11, 0);
    node_7->out_arcs.push_back(arc_7_11);
    node_11->in_arcs.push_back(arc_7_11);
    graph->add_node(node_11);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_12 = new Node<dynamic_bitset<>>(12, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_9_12 = new Arc(node_9, node_12, 1);
    Arc<dynamic_bitset<>>* arc_10_12_op1 = new Arc(node_10, node_12, 0);
    Arc<dynamic_bitset<>>* arc_10_12_op2 = new Arc(node_10, node_12, 1);
    node_9->out_arcs.push_back(arc_9_12);
    node_10->out_arcs.push_back(arc_10_12_op1);
    node_10->out_arcs.push_back(arc_10_12_op2);
    node_12->in_arcs.push_back(arc_9_12);
    node_12->in_arcs.push_back(arc_10_12_op1);
    node_12->in_arcs.push_back(arc_10_12_op2);
    graph->add_node(node_12);

    Node<dynamic_bitset<>>* node_13 = new Node<dynamic_bitset<>>(13, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_11_13_op1 = new Arc(node_11, node_13, 0);
    Arc<dynamic_bitset<>>* arc_11_13_op2 = new Arc(node_11, node_13, 1);
    node_11->out_arcs.push_back(arc_11_13_op1);
    node_11->out_arcs.push_back(arc_11_13_op2);
    node_13->in_arcs.push_back(arc_11_13_op1);
    node_13->in_arcs.push_back(arc_11_13_op2);
    graph->add_node(node_13);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_14 = new Node<dynamic_bitset<>>(14, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_12_14_op1 = new Arc(node_12, node_14, 0);
    Arc<dynamic_bitset<>>* arc_12_14_op2 = new Arc(node_12, node_14, 1);
    Arc<dynamic_bitset<>>* arc_13_14 = new Arc(node_13, node_14, 1);
    node_12->out_arcs.push_back(arc_12_14_op1);
    node_12->out_arcs.push_back(arc_12_14_op2);
    node_13->out_arcs.push_back(arc_13_14);
    node_14->in_arcs.push_back(arc_12_14_op1);
    node_14->in_arcs.push_back(arc_12_14_op2);
    node_14->in_arcs.push_back(arc_13_14);
    graph->add_node(node_14);

    return graph;
}

Graph<dynamic_bitset<>>* GetFalseDDSetCover() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("001")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);


    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_1 = new Node<dynamic_bitset<>>(1, new dynamic_bitset<>(string("111")));
    Arc<dynamic_bitset<>>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<dynamic_bitset<>>* node_2 = new Node<dynamic_bitset<>>(2, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_3 = new Node<dynamic_bitset<>>(3, new dynamic_bitset<>(string("111")));
    Arc<dynamic_bitset<>>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);

    Node<dynamic_bitset<>>* node_4 = new Node<dynamic_bitset<>>(4, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    graph->add_node(node_4);

    Node<dynamic_bitset<>>* node_5 = new Node<dynamic_bitset<>>(5, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_2_6 = new Arc(node_2, node_6, 1);
    node_2->out_arcs.push_back(arc_2_6);
    node_6->in_arcs.push_back(arc_2_6);
    graph->add_node(node_6);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("110")));
    Arc<dynamic_bitset<>>* arc_3_7 = new Arc(node_3, node_7, 1);
    node_3->out_arcs.push_back(arc_3_7);
    node_7->in_arcs.push_back(arc_3_7);
    graph->add_node(node_7);

    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_4_8_op1 = new Arc(node_4, node_8, 0);
    Arc<dynamic_bitset<>>* arc_4_8_op2 = new Arc(node_4, node_8, 1);
    node_4->out_arcs.push_back(arc_4_8_op1);
    node_4->out_arcs.push_back(arc_4_8_op2);
    node_8->in_arcs.push_back(arc_4_8_op1);
    node_8->in_arcs.push_back(arc_4_8_op2);
    graph->add_node(node_8);

    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_5_9_op1 = new Arc(node_5, node_9, 0);
    Arc<dynamic_bitset<>>* arc_5_9_op2 = new Arc(node_5, node_9, 1);
    node_5->out_arcs.push_back(arc_5_9_op1);
    node_5->out_arcs.push_back(arc_5_9_op2);
    node_9->in_arcs.push_back(arc_5_9_op1);
    node_9->in_arcs.push_back(arc_5_9_op2);
    graph->add_node(node_9);

    Node<dynamic_bitset<>>* node_10 = new Node<dynamic_bitset<>>(10, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_6_10_op1 = new Arc(node_6, node_10, 0);
    Arc<dynamic_bitset<>>* arc_6_10_op2 = new Arc(node_6, node_10, 1);
    node_6->out_arcs.push_back(arc_6_10_op1);
    node_6->out_arcs.push_back(arc_6_10_op2);
    node_10->in_arcs.push_back(arc_6_10_op1);
    node_10->in_arcs.push_back(arc_6_10_op2);
    graph->add_node(node_10);

    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_11 = new Node<dynamic_bitset<>>(11, new dynamic_bitset<>(string("110")));
    Arc<dynamic_bitset<>>* arc_7_11 = new Arc(node_7, node_11, 0);
    node_7->out_arcs.push_back(arc_7_11);
    node_11->in_arcs.push_back(arc_7_11);
    graph->add_node(node_11);

    Node<dynamic_bitset<>>* node_12 = new Node<dynamic_bitset<>>(12, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_7_12 = new Arc(node_7, node_12, 1);
    Arc<dynamic_bitset<>>* arc_8_12 = new Arc(node_8, node_12, 1);
    Arc<dynamic_bitset<>>* arc_9_12 = new Arc(node_9, node_12, 1);
    Arc<dynamic_bitset<>>* arc_10_12_op1 = new Arc(node_10, node_12, 0);
    Arc<dynamic_bitset<>>* arc_10_12_op2 = new Arc(node_10, node_12, 1);
    node_7->out_arcs.push_back(arc_7_12);
    node_8->out_arcs.push_back(arc_8_12);
    node_9->out_arcs.push_back(arc_9_12);
    node_10->out_arcs.push_back(arc_10_12_op1);
    node_10->out_arcs.push_back(arc_10_12_op2);
    node_12->in_arcs.push_back(arc_7_12);
    node_12->in_arcs.push_back(arc_8_12);
    node_12->in_arcs.push_back(arc_9_12);
    node_12->in_arcs.push_back(arc_10_12_op1);
    node_12->in_arcs.push_back(arc_10_12_op2);
    graph->add_node(node_12);

    Node<dynamic_bitset<>>* node_13 = new Node<dynamic_bitset<>>(13, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_8_13 = new Arc(node_8, node_13, 0);
    node_8->out_arcs.push_back(arc_8_13);
    node_13->in_arcs.push_back(arc_8_13);
    graph->add_node(node_13);

    Node<dynamic_bitset<>>* node_14 = new Node<dynamic_bitset<>>(14, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_9_14 = new Arc(node_9, node_14, 0);
    node_9->out_arcs.push_back(arc_9_14);
    node_14->in_arcs.push_back(arc_9_14);
    graph->add_node(node_14);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_15 = new Node<dynamic_bitset<>>(15, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_11_15 = new Arc(node_11, node_15, 1);
    Arc<dynamic_bitset<>>* arc_14_15_op1 = new Arc(node_14, node_15, 1);
    Arc<dynamic_bitset<>>* arc_14_15_op2 = new Arc(node_14, node_15, 0);
    node_11->out_arcs.push_back(arc_11_15);
    node_14->out_arcs.push_back(arc_14_15_op1);
    node_14->out_arcs.push_back(arc_14_15_op2);
    node_15->in_arcs.push_back(arc_11_15);
    node_15->in_arcs.push_back(arc_14_15_op1);
    node_15->in_arcs.push_back(arc_14_15_op2);
    graph->add_node(node_15);

    Node<dynamic_bitset<>>* node_16 = new Node<dynamic_bitset<>>(16, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_12_16_op1 = new Arc(node_12, node_16, 0);
    Arc<dynamic_bitset<>>* arc_12_16_op2 = new Arc(node_12, node_16, 1);
    Arc<dynamic_bitset<>>* arc_13_16 = new Arc(node_13, node_16, 1);
    node_12->out_arcs.push_back(arc_12_16_op1);
    node_12->out_arcs.push_back(arc_12_16_op2);
    node_13->out_arcs.push_back(arc_13_16);
    node_16->in_arcs.push_back(arc_12_16_op1);
    node_16->in_arcs.push_back(arc_12_16_op2);
    node_16->in_arcs.push_back(arc_13_16);
    graph->add_node(node_16);

    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_17 = new Node<dynamic_bitset<>>(17, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_15_17 = new Arc(node_15, node_17, 1);
    Arc<dynamic_bitset<>>* arc_16_17_op1 = new Arc(node_16, node_17, 0);
    Arc<dynamic_bitset<>>* arc_16_17_op2 = new Arc(node_16, node_17, 1);
    node_15->out_arcs.push_back(arc_15_17);
    node_16->out_arcs.push_back(arc_16_17_op1);
    node_16->out_arcs.push_back(arc_16_17_op2);
    node_17->in_arcs.push_back(arc_15_17);
    node_17->in_arcs.push_back(arc_16_17_op1);
    node_17->in_arcs.push_back(arc_16_17_op2);
    graph->add_node(node_17);

    return graph;
}
