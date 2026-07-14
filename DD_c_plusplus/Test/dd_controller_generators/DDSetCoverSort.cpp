#include "Graph.h"
#include <iostream>

using namespace std;

Graph<dynamic_bitset<>>* GetExactDDSetCoverSort() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("111")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    // ---------------- LAYER 1 ----------------
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

    // ---------------- LAYER 2 ----------------
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

    // ---------------- LAYER 3 ----------------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("111")));
    Arc<dynamic_bitset<>>* arc_3_7 = new Arc(node_3, node_7, 0);
    node_3->out_arcs.push_back(arc_3_7);
    node_7->in_arcs.push_back(arc_3_7);
    graph->add_node(node_7);

    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("001")));
    Arc<dynamic_bitset<>>* arc_3_8 = new Arc(node_3, node_8, 1);
    node_3->out_arcs.push_back(arc_3_8);
    node_8->in_arcs.push_back(arc_3_8);
    graph->add_node(node_8);

    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_4_9 = new Arc(node_4, node_9, 0);
    node_4->out_arcs.push_back(arc_4_9);
    node_9->in_arcs.push_back(arc_4_9);
    graph->add_node(node_9);

    Node<dynamic_bitset<>>* node_10 = new Node<dynamic_bitset<>>(10, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_4_10 = new Arc(node_4, node_10, 1);
    Arc<dynamic_bitset<>>* arc_5_10 = new Arc(node_5, node_10, 1);
    Arc<dynamic_bitset<>>* arc_6_10_0 = new Arc(node_6, node_10, 0);
    Arc<dynamic_bitset<>>* arc_6_10_1 = new Arc(node_6, node_10, 1);
    node_4->out_arcs.push_back(arc_4_10);
    node_5->out_arcs.push_back(arc_5_10);
    node_6->out_arcs.push_back(arc_6_10_0);
    node_6->out_arcs.push_back(arc_6_10_1);
    node_10->in_arcs = {arc_4_10, arc_5_10, arc_6_10_0, arc_6_10_1};
    graph->add_node(node_10);

    Node<dynamic_bitset<>>* node_11 = new Node<dynamic_bitset<>>(11, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_5_11 = new Arc(node_5, node_11, 0);
    node_5->out_arcs.push_back(arc_5_11);
    node_11->in_arcs.push_back(arc_5_11);
    graph->add_node(node_11);

    // ---------------- LAYER 4 ----------------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_12 = new Node<dynamic_bitset<>>(12, new dynamic_bitset<>(string("110")));
    Arc<dynamic_bitset<>>* arc_7_12 = new Arc(node_7, node_12, 1);
    node_7->out_arcs.push_back(arc_7_12);
    node_12->in_arcs.push_back(arc_7_12);
    graph->add_node(node_12);

    Node<dynamic_bitset<>>* node_13 = new Node<dynamic_bitset<>>(13, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_8_13 = new Arc(node_8, node_13, 1);
    Arc<dynamic_bitset<>>* arc_10_13_0 = new Arc(node_10, node_13, 0);
    Arc<dynamic_bitset<>>* arc_10_13_1 = new Arc(node_10, node_13, 1);
    node_8->out_arcs.push_back(arc_8_13);
    node_10->out_arcs.push_back(arc_10_13_0);
    node_10->out_arcs.push_back(arc_10_13_1);
    node_13->in_arcs = {arc_8_13, arc_10_13_0, arc_10_13_1};
    graph->add_node(node_13);

    Node<dynamic_bitset<>>* node_14 = new Node<dynamic_bitset<>>(14, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_9_14_0 = new Arc(node_9, node_14, 0);
    Arc<dynamic_bitset<>>* arc_9_14_1 = new Arc(node_9, node_14, 1);
    node_9->out_arcs.push_back(arc_9_14_0);
    node_9->out_arcs.push_back(arc_9_14_1);
    node_14->in_arcs = {arc_9_14_0, arc_9_14_1};
    graph->add_node(node_14);

    Node<dynamic_bitset<>>* node_15 = new Node<dynamic_bitset<>>(15, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_11_15_0 = new Arc(node_11, node_15, 0);
    Arc<dynamic_bitset<>>* arc_11_15_1 = new Arc(node_11, node_15, 1);
    node_11->out_arcs.push_back(arc_11_15_0);
    node_11->out_arcs.push_back(arc_11_15_1);
    node_15->in_arcs = {arc_11_15_0, arc_11_15_1};
    graph->add_node(node_15);

    // ---------------- LAYER 5 ----------------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_16 = new Node<dynamic_bitset<>>(16, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_12_16 = new Arc(node_12, node_16, 1);
    Arc<dynamic_bitset<>>* arc_15_16_0 = new Arc(node_15, node_16, 0);
    Arc<dynamic_bitset<>>* arc_15_16_1 = new Arc(node_15, node_16, 1);
    node_12->out_arcs.push_back(arc_12_16);
    node_15->out_arcs.push_back(arc_15_16_0);
    node_15->out_arcs.push_back(arc_15_16_1);
    node_16->in_arcs = {arc_12_16, arc_15_16_0, arc_15_16_1};
    graph->add_node(node_16);

    Node<dynamic_bitset<>>* node_17 = new Node<dynamic_bitset<>>(17, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_13_17_0 = new Arc(node_13, node_17, 0);
    Arc<dynamic_bitset<>>* arc_13_17_1 = new Arc(node_13, node_17, 1);
    Arc<dynamic_bitset<>>* arc_14_17 = new Arc(node_14, node_17, 1);
    node_13->out_arcs.push_back(arc_13_17_0);
    node_13->out_arcs.push_back(arc_13_17_1);
    node_14->out_arcs.push_back(arc_14_17);
    node_17->in_arcs = {arc_13_17_0, arc_13_17_1, arc_14_17};
    graph->add_node(node_17);

    // ---------------- LAYER 6 ----------------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_18 = new Node<dynamic_bitset<>>(18, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_16_18 = new Arc(node_16, node_18, 1);
    Arc<dynamic_bitset<>>* arc_17_18_0 = new Arc(node_17, node_18, 0);
    Arc<dynamic_bitset<>>* arc_17_18_1 = new Arc(node_17, node_18, 1);
    node_16->out_arcs.push_back(arc_16_18);
    node_17->out_arcs.push_back(arc_17_18_0);
    node_17->out_arcs.push_back(arc_17_18_1);
    node_18->in_arcs = {arc_16_18, arc_17_18_0, arc_17_18_1};
    graph->add_node(node_18);

    return graph;
}

Graph<dynamic_bitset<>>* GetReduceDDSetCoverSort() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("111")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    // Layer 1
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_1 = new Node<dynamic_bitset<>>(1, new dynamic_bitset<>(string("111")));
    Node<dynamic_bitset<>>* node_2 = new Node<dynamic_bitset<>>(2, new dynamic_bitset<>(string("100")));

    Arc<dynamic_bitset<>>* arc_0_1 = new Arc(node_0, node_1, 0);
    Arc<dynamic_bitset<>>* arc_0_2 = new Arc(node_0, node_2, 1);

    node_0->out_arcs = {arc_0_1, arc_0_2};
    node_1->in_arcs = {arc_0_1};
    node_2->in_arcs = {arc_0_2};

    graph->add_node(node_1);
    graph->add_node(node_2);

    // Layer 2
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_3 = new Node<dynamic_bitset<>>(3, new dynamic_bitset<>(string("111")));
    Node<dynamic_bitset<>>* node_4 = new Node<dynamic_bitset<>>(4, new dynamic_bitset<>(string("010")));
    Node<dynamic_bitset<>>* node_5 = new Node<dynamic_bitset<>>(5, new dynamic_bitset<>(string("100")));
    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("000")));

    Arc<dynamic_bitset<>>* arc_1_3 = new Arc(node_1, node_3, 0);
    Arc<dynamic_bitset<>>* arc_1_4 = new Arc(node_1, node_4, 1);
    Arc<dynamic_bitset<>>* arc_2_5 = new Arc(node_2, node_5, 0);
    Arc<dynamic_bitset<>>* arc_2_6 = new Arc(node_2, node_6, 1);

    node_1->out_arcs = {arc_1_3, arc_1_4};
    node_2->out_arcs = {arc_2_5, arc_2_6};

    node_3->in_arcs = {arc_1_3};
    node_4->in_arcs = {arc_1_4};
    node_5->in_arcs = {arc_2_5};
    node_6->in_arcs = {arc_2_6};

    graph->add_node(node_3);
    graph->add_node(node_4);
    graph->add_node(node_5);
    graph->add_node(node_6);

    // Layer 3
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("111")));
    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("001")));
    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("010")));
    Node<dynamic_bitset<>>* node_10 = new Node<dynamic_bitset<>>(10, new dynamic_bitset<>(string("000")));
    Node<dynamic_bitset<>>* node_11 = new Node<dynamic_bitset<>>(11, new dynamic_bitset<>(string("100")));

    Arc<dynamic_bitset<>>* arc_3_7 = new Arc(node_3, node_7, 0);
    Arc<dynamic_bitset<>>* arc_3_8 = new Arc(node_3, node_8, 1);

    Arc<dynamic_bitset<>>* arc_4_9 = new Arc(node_4, node_9, 0);
    Arc<dynamic_bitset<>>* arc_4_10 = new Arc(node_4, node_10, 1);

    Arc<dynamic_bitset<>>* arc_5_11 = new Arc(node_5, node_11, 0);
    Arc<dynamic_bitset<>>* arc_5_10 = new Arc(node_5, node_10, 1);

    Arc<dynamic_bitset<>>* arc_6_10_0 = new Arc(node_6, node_10, 0);
    Arc<dynamic_bitset<>>* arc_6_10_1 = new Arc(node_6, node_10, 1);

    node_3->out_arcs = {arc_3_7, arc_3_8};
    node_4->out_arcs = {arc_4_9, arc_4_10};
    node_5->out_arcs = {arc_5_11, arc_5_10};
    node_6->out_arcs = {arc_6_10_0, arc_6_10_1};

    node_7->in_arcs = {arc_3_7};
    node_8->in_arcs = {arc_3_8};
    node_9->in_arcs = {arc_4_9};
    node_10->in_arcs = {arc_4_10, arc_5_10, arc_6_10_0, arc_6_10_1};
    node_11->in_arcs = {arc_5_11};

    graph->add_node(node_7);
    graph->add_node(node_8);
    graph->add_node(node_9);
    graph->add_node(node_10);
    graph->add_node(node_11);

    // Layer 4
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_12 = new Node<dynamic_bitset<>>(12, new dynamic_bitset<>(string("110")));
    Node<dynamic_bitset<>>* node_13 = new Node<dynamic_bitset<>>(13, new dynamic_bitset<>(string("000")));
    Node<dynamic_bitset<>>* node_14 = new Node<dynamic_bitset<>>(14, new dynamic_bitset<>(string("010")));
    Node<dynamic_bitset<>>* node_15 = new Node<dynamic_bitset<>>(15, new dynamic_bitset<>(string("100")));

    Arc<dynamic_bitset<>>* arc_7_12 = new Arc(node_7, node_12, 1);

    Arc<dynamic_bitset<>>* arc_8_13 = new Arc(node_8, node_13, 1);
    Arc<dynamic_bitset<>>* arc_10_13_0 = new Arc(node_10, node_13, 0);
    Arc<dynamic_bitset<>>* arc_10_13_1 = new Arc(node_10, node_13, 1);

    Arc<dynamic_bitset<>>* arc_9_14_0 = new Arc(node_9, node_14, 0);
    Arc<dynamic_bitset<>>* arc_9_14_1 = new Arc(node_9, node_14, 1);

    Arc<dynamic_bitset<>>* arc_11_15_0 = new Arc(node_11, node_15, 0);
    Arc<dynamic_bitset<>>* arc_11_15_1 = new Arc(node_11, node_15, 1);

    node_7->out_arcs = {arc_7_12};
    node_8->out_arcs = {arc_8_13};
    node_9->out_arcs = {arc_9_14_0, arc_9_14_1};
    node_10->out_arcs = {arc_10_13_0, arc_10_13_1};
    node_11->out_arcs = {arc_11_15_0, arc_11_15_1};

    node_12->in_arcs = {arc_7_12};
    node_13->in_arcs = {arc_8_13, arc_10_13_0, arc_10_13_1};
    node_14->in_arcs = {arc_9_14_0, arc_9_14_1};
    node_15->in_arcs = {arc_11_15_0, arc_11_15_1};

    graph->add_node(node_12);
    graph->add_node(node_13);
    graph->add_node(node_14);
    graph->add_node(node_15);

    // Layer 5
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_16 = new Node<dynamic_bitset<>>(16, new dynamic_bitset<>(string("100")));
    Node<dynamic_bitset<>>* node_17 = new Node<dynamic_bitset<>>(17, new dynamic_bitset<>(string("000")));

    Arc<dynamic_bitset<>>* arc_12_16 = new Arc(node_12, node_16, 1);
    Arc<dynamic_bitset<>>* arc_15_16_0 = new Arc(node_15, node_16, 0);
    Arc<dynamic_bitset<>>* arc_15_16_1 = new Arc(node_15, node_16, 1);

    Arc<dynamic_bitset<>>* arc_13_17_0 = new Arc(node_13, node_17, 0);
    Arc<dynamic_bitset<>>* arc_13_17_1 = new Arc(node_13, node_17, 1);
    Arc<dynamic_bitset<>>* arc_14_17 = new Arc(node_14, node_17, 1);

    node_12->out_arcs = {arc_12_16};
    node_15->out_arcs = {arc_15_16_0, arc_15_16_1};
    node_13->out_arcs = {arc_13_17_0, arc_13_17_1};
    node_14->out_arcs = {arc_14_17};

    node_16->in_arcs = {arc_12_16, arc_15_16_0, arc_15_16_1};
    node_17->in_arcs = {arc_13_17_0, arc_13_17_1, arc_14_17};

    graph->add_node(node_16);
    graph->add_node(node_17);

    // Layer 6 (terminal)
    graph->add_new_layer();
    Node<dynamic_bitset<>>* node_18 = new Node<dynamic_bitset<>>(18, new dynamic_bitset<>(string("000")));

    Arc<dynamic_bitset<>>* arc_16_18 = new Arc(node_16, node_18, 1);
    Arc<dynamic_bitset<>>* arc_17_18_0 = new Arc(node_17, node_18, 0);
    Arc<dynamic_bitset<>>* arc_17_18_1 = new Arc(node_17, node_18, 1);

    node_16->out_arcs = {arc_16_18};
    node_17->out_arcs = {arc_17_18_0, arc_17_18_1};

    node_18->in_arcs = {arc_16_18, arc_17_18_0, arc_17_18_1};

    graph->add_node(node_18);

    return graph;
}

Graph<dynamic_bitset<>>* GetRestrictedDDSetCoverSort() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("111")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    // -------- LAYER 1 --------
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

    // -------- LAYER 2 --------
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

    // -------- LAYER 3 (CLAVE: merge en u7) --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    graph->add_node(node_6);

    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_3_7 = new Arc(node_3, node_7, 1);
    Arc<dynamic_bitset<>>* arc_4_7 = new Arc(node_4, node_7, 1);
    Arc<dynamic_bitset<>>* arc_5_7_0 = new Arc(node_5, node_7, 0);
    Arc<dynamic_bitset<>>* arc_5_7_1 = new Arc(node_5, node_7, 1);

    node_3->out_arcs.push_back(arc_3_7);
    node_4->out_arcs.push_back(arc_4_7);
    node_5->out_arcs.push_back(arc_5_7_0);
    node_5->out_arcs.push_back(arc_5_7_1);

    node_7->in_arcs = {arc_3_7, arc_4_7, arc_5_7_0, arc_5_7_1};
    graph->add_node(node_7);

    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_4_8 = new Arc(node_4, node_8, 0);
    node_4->out_arcs.push_back(arc_4_8);
    node_8->in_arcs.push_back(arc_4_8);
    graph->add_node(node_8);

    // -------- LAYER 4 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_6_9_0 = new Arc(node_6, node_9, 0);
    Arc<dynamic_bitset<>>* arc_6_9_1 = new Arc(node_6, node_9, 1);
    node_6->out_arcs.push_back(arc_6_9_0);
    node_6->out_arcs.push_back(arc_6_9_1);
    node_9->in_arcs = {arc_6_9_0, arc_6_9_1};
    graph->add_node(node_9);

    Node<dynamic_bitset<>>* node_10 = new Node<dynamic_bitset<>>(10, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_7_10_0 = new Arc(node_7, node_10, 0);
    Arc<dynamic_bitset<>>* arc_7_10_1 = new Arc(node_7, node_10, 1);
    node_7->out_arcs.push_back(arc_7_10_0);
    node_7->out_arcs.push_back(arc_7_10_1);
    node_10->in_arcs = {arc_7_10_0, arc_7_10_1};
    graph->add_node(node_10);

    Node<dynamic_bitset<>>* node_11 = new Node<dynamic_bitset<>>(11, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_8_11_0 = new Arc(node_8, node_11, 0);
    Arc<dynamic_bitset<>>* arc_8_11_1 = new Arc(node_8, node_11, 1);
    node_8->out_arcs.push_back(arc_8_11_0);
    node_8->out_arcs.push_back(arc_8_11_1);
    node_11->in_arcs = {arc_8_11_0, arc_8_11_1};
    graph->add_node(node_11);

    // -------- LAYER 5 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_12 = new Node<dynamic_bitset<>>(12, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_9_12 = new Arc(node_9, node_12, 1);
    Arc<dynamic_bitset<>>* arc_10_12_0 = new Arc(node_10, node_12, 0);
    Arc<dynamic_bitset<>>* arc_10_12_1 = new Arc(node_10, node_12, 1);

    node_9->out_arcs.push_back(arc_9_12);
    node_10->out_arcs.push_back(arc_10_12_0);
    node_10->out_arcs.push_back(arc_10_12_1);

    node_12->in_arcs = {arc_9_12, arc_10_12_0, arc_10_12_1};
    graph->add_node(node_12);

    Node<dynamic_bitset<>>* node_13 = new Node<dynamic_bitset<>>(13, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_11_13_0 = new Arc(node_11, node_13, 0);
    Arc<dynamic_bitset<>>* arc_11_13_1 = new Arc(node_11, node_13, 1);
    node_11->out_arcs.push_back(arc_11_13_0);
    node_11->out_arcs.push_back(arc_11_13_1);
    node_13->in_arcs = {arc_11_13_0, arc_11_13_1};
    graph->add_node(node_13);

    // -------- LAYER 6 --------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_14 = new Node<dynamic_bitset<>>(14, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_12_14_0 = new Arc(node_12, node_14, 0);
    Arc<dynamic_bitset<>>* arc_12_14_1 = new Arc(node_12, node_14, 1);
    Arc<dynamic_bitset<>>* arc_13_14 = new Arc(node_13, node_14, 1);

    node_12->out_arcs.push_back(arc_12_14_0);
    node_12->out_arcs.push_back(arc_12_14_1);
    node_13->out_arcs.push_back(arc_13_14);

    node_14->in_arcs = {arc_12_14_0, arc_12_14_1, arc_13_14};
    graph->add_node(node_14);

    return graph;
}

Graph<dynamic_bitset<>>* GetRelaxedDDSetCoverSort() {
    Node<dynamic_bitset<>>* node_0 = new Node<dynamic_bitset<>>(0, new dynamic_bitset<>(string("111")));
    Graph<dynamic_bitset<>>* graph = new Graph(node_0);

    // ---------------- LAYER 1 ----------------
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

    // ---------------- LAYER 2 ----------------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_3 = new Node<dynamic_bitset<>>(3, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_1_3_0 = new Arc(node_1, node_3, 0);
    Arc<dynamic_bitset<>>* arc_1_3_1 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3_0);
    node_1->out_arcs.push_back(arc_1_3_1);
    node_3->in_arcs.push_back(arc_1_3_0);
    node_3->in_arcs.push_back(arc_1_3_1);
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

    // ---------------- LAYER 3 ----------------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_6 = new Node<dynamic_bitset<>>(6, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    graph->add_node(node_6);

    Node<dynamic_bitset<>>* node_7 = new Node<dynamic_bitset<>>(7, new dynamic_bitset<>(string("000")));

    Arc<dynamic_bitset<>>* arc_3_7 = new Arc(node_3, node_7, 1);
    Arc<dynamic_bitset<>>* arc_4_7 = new Arc(node_4, node_7, 1);
    Arc<dynamic_bitset<>>* arc_5_7_0 = new Arc(node_5, node_7, 0);
    Arc<dynamic_bitset<>>* arc_5_7_1 = new Arc(node_5, node_7, 1);

    node_3->out_arcs.push_back(arc_3_7);
    node_4->out_arcs.push_back(arc_4_7);
    node_5->out_arcs.push_back(arc_5_7_0);
    node_5->out_arcs.push_back(arc_5_7_1);

    node_7->in_arcs.push_back(arc_3_7);
    node_7->in_arcs.push_back(arc_4_7);
    node_7->in_arcs.push_back(arc_5_7_0);
    node_7->in_arcs.push_back(arc_5_7_1);

    graph->add_node(node_7);

    Node<dynamic_bitset<>>* node_8 = new Node<dynamic_bitset<>>(8, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_4_8 = new Arc(node_4, node_8, 0);
    node_4->out_arcs.push_back(arc_4_8);
    node_8->in_arcs.push_back(arc_4_8);
    graph->add_node(node_8);

    // ---------------- LAYER 4 ----------------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_9 = new Node<dynamic_bitset<>>(9, new dynamic_bitset<>(string("010")));
    Arc<dynamic_bitset<>>* arc_6_9_0 = new Arc(node_6, node_9, 0);
    Arc<dynamic_bitset<>>* arc_6_9_1 = new Arc(node_6, node_9, 1);
    node_6->out_arcs.push_back(arc_6_9_0);
    node_6->out_arcs.push_back(arc_6_9_1);
    node_9->in_arcs.push_back(arc_6_9_0);
    node_9->in_arcs.push_back(arc_6_9_1);
    graph->add_node(node_9);

    Node<dynamic_bitset<>>* node_10 = new Node<dynamic_bitset<>>(10, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_7_10_0 = new Arc(node_7, node_10, 0);
    Arc<dynamic_bitset<>>* arc_7_10_1 = new Arc(node_7, node_10, 1);
    node_7->out_arcs.push_back(arc_7_10_0);
    node_7->out_arcs.push_back(arc_7_10_1);
    node_10->in_arcs.push_back(arc_7_10_0);
    node_10->in_arcs.push_back(arc_7_10_1);
    graph->add_node(node_10);

    Node<dynamic_bitset<>>* node_11 = new Node<dynamic_bitset<>>(11, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_8_11_0 = new Arc(node_8, node_11, 0);
    Arc<dynamic_bitset<>>* arc_8_11_1 = new Arc(node_8, node_11, 1);
    node_8->out_arcs.push_back(arc_8_11_0);
    node_8->out_arcs.push_back(arc_8_11_1);
    node_11->in_arcs.push_back(arc_8_11_0);
    node_11->in_arcs.push_back(arc_8_11_1);
    graph->add_node(node_11);

    // ---------------- LAYER 5 ----------------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_12 = new Node<dynamic_bitset<>>(12, new dynamic_bitset<>(string("000")));
    Arc<dynamic_bitset<>>* arc_9_12 = new Arc(node_9, node_12, 1);
    Arc<dynamic_bitset<>>* arc_10_12_0 = new Arc(node_10, node_12, 0);
    Arc<dynamic_bitset<>>* arc_10_12_1 = new Arc(node_10, node_12, 1);

    node_9->out_arcs.push_back(arc_9_12);
    node_10->out_arcs.push_back(arc_10_12_0);
    node_10->out_arcs.push_back(arc_10_12_1);

    node_12->in_arcs.push_back(arc_9_12);
    node_12->in_arcs.push_back(arc_10_12_0);
    node_12->in_arcs.push_back(arc_10_12_1);

    graph->add_node(node_12);

    Node<dynamic_bitset<>>* node_13 = new Node<dynamic_bitset<>>(13, new dynamic_bitset<>(string("100")));
    Arc<dynamic_bitset<>>* arc_11_13_0 = new Arc(node_11, node_13, 0);
    Arc<dynamic_bitset<>>* arc_11_13_1 = new Arc(node_11, node_13, 1);
    node_11->out_arcs.push_back(arc_11_13_0);
    node_11->out_arcs.push_back(arc_11_13_1);
    node_13->in_arcs.push_back(arc_11_13_0);
    node_13->in_arcs.push_back(arc_11_13_1);
    graph->add_node(node_13);

    // ---------------- LAYER 6 ----------------
    graph->add_new_layer();

    Node<dynamic_bitset<>>* node_14 = new Node<dynamic_bitset<>>(14, new dynamic_bitset<>(string("000")));

    Arc<dynamic_bitset<>>* arc_12_14_0 = new Arc(node_12, node_14, 0);
    Arc<dynamic_bitset<>>* arc_12_14_1 = new Arc(node_12, node_14, 1);
    Arc<dynamic_bitset<>>* arc_13_14 = new Arc(node_13, node_14, 1);

    node_12->out_arcs.push_back(arc_12_14_0);
    node_12->out_arcs.push_back(arc_12_14_1);
    node_13->out_arcs.push_back(arc_13_14);

    node_14->in_arcs.push_back(arc_12_14_0);
    node_14->in_arcs.push_back(arc_12_14_1);
    node_14->in_arcs.push_back(arc_13_14);

    graph->add_node(node_14);

    return graph;
}

Graph<dynamic_bitset<>>* GetFalseDDSetCoverSort() {
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
