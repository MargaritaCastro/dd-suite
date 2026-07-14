#include "Graph.h"

using namespace std;

Graph<int>* GetExactDDKnapsack() {
    Node<int>* node_0 = new Node<int>(0, new int(0));
    Graph<int>* graph = new Graph(node_0);

    graph->add_new_layer();
    Node<int>* node_1 = new Node<int>(1, new int(0));
    Arc<int>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);

    graph->add_node(node_1);
    Node<int>* node_2 = new Node<int>(2, new int(3));
    Arc<int>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();
    Node<int>* node_3 = new Node<int>(3, new int(0));
    Arc<int>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);

    graph->add_node(node_3);
    Node<int>* node_4 = new Node<int>(4, new int(3));
    Arc<int>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<int>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);

    graph->add_node(node_4);
    Node<int>* node_5 = new Node<int>(5, new int(6));
    Arc<int>* arc_2_5 = new Arc(node_2, node_5, 1);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();
    Node<int>* node_6 = new Node<int>(6, new int(0));
    Arc<int>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);

    graph->add_node(node_6);
    Node<int>* node_7 = new Node<int>(7, new int(4));
    Arc<int>* arc_3_7 = new Arc(node_3, node_7, 1);
    node_3->out_arcs.push_back(arc_3_7);
    node_7->in_arcs.push_back(arc_3_7);

    graph->add_node(node_7);
    Node<int>* node_8 = new Node<int>(8, new int(3));
    Arc<int>* arc_4_8 = new Arc(node_4, node_8, 0);
    node_4->out_arcs.push_back(arc_4_8);
    node_8->in_arcs.push_back(arc_4_8);

    graph->add_node(node_8);
    Node<int>* node_9 = new Node<int>(9, new int(6));
    Arc<int>* arc_5_9 = new Arc(node_5, node_9, 0);
    node_5->out_arcs.push_back(arc_5_9);
    node_9->in_arcs.push_back(arc_5_9);
    graph->add_node(node_9);


    graph->add_new_layer();
    Node<int>* node_10 = new Node<int>(10, new int(0));
    Arc<int>* arc_6_10_op1 = new Arc(node_6, node_10, 0);
    node_6->out_arcs.push_back(arc_6_10_op1);
    node_10->in_arcs.push_back(arc_6_10_op1);
    Arc<int>* arc_6_10_op2 = new Arc(node_6, node_10, 1);
    node_6->out_arcs.push_back(arc_6_10_op2);
    node_10->in_arcs.push_back(arc_6_10_op2);
    Arc<int>* arc_7_10 = new Arc(node_7, node_10, 0);
    node_7->out_arcs.push_back(arc_7_10);
    node_10->in_arcs.push_back(arc_7_10);
    Arc<int>* arc_8_10 = new Arc(node_8, node_10, 0);
    node_8->out_arcs.push_back(arc_8_10);
    node_10->in_arcs.push_back(arc_8_10);
    Arc<int>* arc_9_10 = new Arc(node_9, node_10, 0);
    node_9->out_arcs.push_back(arc_9_10);
    node_10->in_arcs.push_back(arc_9_10);
    graph->add_node(node_10);

    return graph;
}

Graph<int>* GetReduceDDKnapsack() {
    Node<int>* node_0 = new Node<int>(0, new int(0));
    Graph<int>* graph = new Graph(node_0);

    graph->add_new_layer();
    Node<int>* node_1 = new Node<int>(1, new int(0));
    Arc<int>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<int>* node_2 = new Node<int>(2, new int(3));
    Arc<int>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();
    Node<int>* node_3 = new Node<int>(3, new int(0));
    Arc<int>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);

    Node<int>* node_4 = new Node<int>(4, new int(3));
    Arc<int>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<int>* arc_2_4_op1 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4_op1);
    node_4->in_arcs.push_back(arc_2_4_op1);
    Arc<int>* arc_2_4_op2 = new Arc(node_2, node_4, 1);
    node_2->out_arcs.push_back(arc_2_4_op2);
    node_4->in_arcs.push_back(arc_2_4_op2);
    graph->add_node(node_4);

    graph->add_new_layer();
    Node<int>* node_5 = new Node<int>(5, new int(0));
    Arc<int>* arc_3_5 = new Arc(node_3, node_5, 0);
    node_3->out_arcs.push_back(arc_3_5);
    node_5->in_arcs.push_back(arc_3_5);
    graph->add_node(node_5);

    Node<int>* node_6 = new Node<int>(6, new int(4));
    Arc<int>* arc_3_6 = new Arc(node_3, node_6, 1);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);

    Arc<int>* arc_4_6 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6);
    node_6->in_arcs.push_back(arc_4_6);
    graph->add_node(node_6);

    graph->add_new_layer();
    Node<int>* node_7 = new Node<int>(7, new int(0));
    Arc<int>* arc_5_7_op1 = new Arc(node_5, node_7, 0);
    node_5->out_arcs.push_back(arc_5_7_op1);
    node_7->in_arcs.push_back(arc_5_7_op1);
    Arc<int>* arc_5_7_op2 = new Arc(node_5, node_7, 1);
    node_5->out_arcs.push_back(arc_5_7_op2);
    node_7->in_arcs.push_back(arc_5_7_op2);
    Arc<int>* arc_6_7 = new Arc(node_6, node_7, 0);
    node_6->out_arcs.push_back(arc_6_7);
    node_7->in_arcs.push_back(arc_6_7);
    graph->add_node(node_7);

    return graph;
}

Graph<int>* GetRestrictedDDKnapsack() {
    Node<int>* node_0 = new Node<int>(0, new int(0));;
    Graph<int>* graph = new Graph(node_0);;

    graph->add_new_layer();
    Node<int>* node_1 = new Node<int>(1, new int(0));
    Arc<int>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);

    graph->add_node(node_1);
    Node<int>* node_2 = new Node<int>(2, new int(3));
    Arc<int>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();
    Node<int>* node_3 = new Node<int>(3, new int(0));
    Arc<int>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);

    Node<int>* node_4 = new Node<int>(4, new int(3));
    Arc<int>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<int>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<int>* node_5 = new Node<int>(5, new int(6));
    Arc<int>* arc_2_5 = new Arc(node_2, node_5, 1);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();
    Node<int>* node_6 = new Node<int>(6, new int(4));
    Arc<int>* arc_3_6 = new Arc(node_3, node_6, 1);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    graph->add_node(node_6);

    Node<int>* node_7 = new Node<int>(7, new int(3));
    Arc<int>* arc_4_7 = new Arc(node_4, node_7, 0);
    node_4->out_arcs.push_back(arc_4_7);
    node_7->in_arcs.push_back(arc_4_7);
    graph->add_node(node_7);

    Node<int>* node_8 = new Node<int>(8, new int(6));
    Arc<int>* arc_5_8 = new Arc(node_5, node_8, 0);
    node_5->out_arcs.push_back(arc_5_8);
    node_8->in_arcs.push_back(arc_5_8);
    graph->add_node(node_8);


    graph->add_new_layer();
    Node<int>* node_9 = new Node<int>(9, new int(0));

    Arc<int>* arc_6_9 = new Arc(node_6, node_9, 0);
    node_6->out_arcs.push_back(arc_6_9);
    node_9->in_arcs.push_back(arc_6_9);

    Arc<int>* arc_7_9 = new Arc(node_7, node_9, 0);
    node_7->out_arcs.push_back(arc_7_9);
    node_9->in_arcs.push_back(arc_7_9);

    Arc<int>* arc_8_9 = new Arc(node_8, node_9, 0);
    node_8->out_arcs.push_back(arc_8_9);
    node_9->in_arcs.push_back(arc_8_9);

    graph->add_node(node_9);

    return graph;
}

Graph<int>* GetRelaxedDDKnapsack() {
    Node<int>* node_0 = new Node<int>(0, new int(0));;
    Graph<int>* graph = new Graph(node_0);;

    graph->add_new_layer();
    Node<int>* node_1 = new Node<int>(1, new int(0));
    Arc<int>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);

    graph->add_node(node_1);
    Node<int>* node_2 = new Node<int>(2, new int(3));
    Arc<int>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);


    graph->add_new_layer();
    Node<int>* node_3 = new Node<int>(3, new int(0));
    Arc<int>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);

    Node<int>* node_4 = new Node<int>(4, new int(3));
    Arc<int>* arc_1_4 =new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<int>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    auto* node_5 = new Node<int>(5, new int(6));
    auto* arc_2_5 = new Arc(node_2, node_5, 1);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();
    Node<int>* node_6 = new Node<int>(6, new int(4));
    Arc<int>* arc_3_6= new Arc(node_3, node_6, 1);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    graph->add_node(node_6);

    Node<int>* node_7 = new Node<int>(7, new int(0));
    Arc<int>* arc_3_7 = new Arc(node_3, node_7, 0);
    Arc<int>* arc_4_7 = new Arc(node_4, node_7, 0);
    node_3->out_arcs.push_back(arc_3_7);
    node_4->out_arcs.push_back(arc_4_7);
    node_7->in_arcs.push_back(arc_3_7);
    node_7->in_arcs.push_back(arc_4_7);
    graph->add_node(node_7);

    Node<int>* node_8 = new Node<int>(8, new int(6));
    Arc<int>* arc_5_8 = new Arc(node_5, node_8, 0);
    node_5->out_arcs.push_back(arc_5_8);
    node_8->in_arcs.push_back(arc_5_8);
    graph->add_node(node_8);


    graph->add_new_layer();
    Node<int>* node_9 = new Node<int>(9, new int(0));
    Arc<int>* arc_6_9 = new Arc(node_6, node_9, 0);
    node_6->out_arcs.push_back(arc_6_9);
    node_9->in_arcs.push_back(arc_6_9);

    Arc<int>* arc_7_9_op1 = new Arc(node_7, node_9, 0);
    Arc<int>* arc_7_9_op2 = new Arc(node_7, node_9, 1);
    node_7->out_arcs.push_back(arc_7_9_op1);
    node_7->out_arcs.push_back(arc_7_9_op2);
    node_9->in_arcs.push_back(arc_7_9_op1);
    node_9->in_arcs.push_back(arc_7_9_op2);

    Arc<int>* arc_8_9 = new Arc(node_8, node_9, 0);
    node_8->out_arcs.push_back(arc_8_9);
    node_9->in_arcs.push_back(arc_8_9);

    graph->add_node(node_9);


    return graph;
}

Graph<int>* GetFalseDDKnapsack() {
    Node<int>* node_0 = new Node<int>(0, new int(0));
    Graph<int>* graph = new Graph(node_0);

    graph->add_new_layer();
    Node<int>* node_1 = new Node<int>(1, new int(0));
    Arc<int>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);

    graph->add_node(node_1);
    Node<int>* node_2 = new Node<int>(2, new int(4));;
    Arc<int>* arc_0_2 = new Arc(node_0, node_2, 1);;
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();
    Node<int>* node_3 = new Node<int>(3, new int(0));
    Arc<int>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);

    graph->add_node(node_3);
    Node<int>* node_4 = new Node<int>(4, new int(4));
    Arc<int>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<int>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);

    graph->add_node(node_4);
    Node<int>* node_5 = new Node<int>(5, new int(6));
    Arc<int>* arc_2_5 = new Arc(node_2, node_5, 1);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();
    Node<int>* node_6 = new Node<int>(6, new int(0));
    Arc<int>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);

    graph->add_node(node_6);
    Node<int>* node_7 = new Node<int>(7, new int(4));
    Arc<int>* arc_3_7 = new Arc(node_3, node_7, 1);
    node_3->out_arcs.push_back(arc_3_7);
    node_7->in_arcs.push_back(arc_3_7);

    graph->add_node(node_7);
    Node<int>* node_8 = new Node<int>(8, new int(4));
    Arc<int>* arc_4_8 = new Arc(node_4, node_8, 0);
    node_4->out_arcs.push_back(arc_4_8);
    node_8->in_arcs.push_back(arc_4_8);

    graph->add_node(node_8);
    Node<int>* node_9 = new Node<int>(9, new int(6));
    Arc<int>* arc_5_9 = new Arc(node_5, node_9, 0);
    node_5->out_arcs.push_back(arc_5_9);
    node_9->in_arcs.push_back(arc_5_9);
    graph->add_node(node_9);


    graph->add_new_layer();
    Node<int>* node_10 = new Node<int>(10, new int(0));
    Arc<int>* arc_6_10_op1 = new Arc(node_6, node_10, 0);
    node_6->out_arcs.push_back(arc_6_10_op1);
    node_10->in_arcs.push_back(arc_6_10_op1);
    Arc<int>* arc_6_10_op2 = new Arc(node_6, node_10, 1);
    node_6->out_arcs.push_back(arc_6_10_op2);
    node_10->in_arcs.push_back(arc_6_10_op2);
    Arc<int>* arc_7_10 = new Arc(node_7, node_10, 0);
    node_7->out_arcs.push_back(arc_7_10);
    node_10->in_arcs.push_back(arc_7_10);
    Arc<int>* arc_8_10 = new Arc(node_8, node_10, 0);
    node_8->out_arcs.push_back(arc_8_10);
    node_10->in_arcs.push_back(arc_8_10);
    Arc<int>* arc_9_10 = new Arc(node_9, node_10, 0);
    node_9->out_arcs.push_back(arc_9_10);
    node_10->in_arcs.push_back(arc_9_10);
    graph->add_node(node_10);

    graph->add_new_layer();
    Node<int>* node_11 = new Node<int>(11, new int(6));
    Arc<int>* arc_10_11 = new Arc(node_10, node_11, 0);
    node_10->out_arcs.push_back(arc_10_11);
    node_11->in_arcs.push_back(arc_10_11);
    graph->add_node(node_11);

    return graph;
}