#include "Graph.h"
#include "../../Examples/SequencingInstance/SequencingState.h"

using namespace std;

// Instance: n=3, p=[2,3,1], w=[1,2,3]
// setup_times: depot=[1,1,1], from0=[0,2,1], from1=[2,0,1], from2=[1,2,0]
//
// State = SequencingState{ scheduled, last_job }. The accumulated completion time
// is NOT part of the state (the objective is computed separably in
// SequencingPathSolver), so two nodes sharing (scheduled, last_job) are equivalent.

Graph<SequencingState>* GetExactDDSequencing() {
    auto* node_0 = new Node<SequencingState>(0, new SequencingState{{}, -1});
    auto* graph = new Graph<SequencingState>(node_0);

    // Layer 1
    graph->add_new_layer();
    auto* node_1 = new Node<SequencingState>(1, new SequencingState{{0}, 0});
    auto* arc_0_1 = new Arc<SequencingState>(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    auto* node_2 = new Node<SequencingState>(2, new SequencingState{{1}, 1});
    auto* arc_0_2 = new Arc<SequencingState>(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    auto* node_3 = new Node<SequencingState>(3, new SequencingState{{2}, 2});
    auto* arc_0_3 = new Arc<SequencingState>(node_0, node_3, 2);
    node_0->out_arcs.push_back(arc_0_3);
    node_3->in_arcs.push_back(arc_0_3);
    graph->add_node(node_3);

    // Layer 2
    graph->add_new_layer();
    auto* node_4 = new Node<SequencingState>(4, new SequencingState{{0, 1}, 1});
    auto* arc_1_4 = new Arc<SequencingState>(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    graph->add_node(node_4);

    auto* node_5 = new Node<SequencingState>(5, new SequencingState{{0, 2}, 2});
    auto* arc_1_5 = new Arc<SequencingState>(node_1, node_5, 2);
    node_1->out_arcs.push_back(arc_1_5);
    node_5->in_arcs.push_back(arc_1_5);
    graph->add_node(node_5);

    auto* node_6 = new Node<SequencingState>(6, new SequencingState{{0, 1}, 0});
    auto* arc_2_6 = new Arc<SequencingState>(node_2, node_6, 0);
    node_2->out_arcs.push_back(arc_2_6);
    node_6->in_arcs.push_back(arc_2_6);
    graph->add_node(node_6);

    auto* node_7 = new Node<SequencingState>(7, new SequencingState{{1, 2}, 2});
    auto* arc_2_7 = new Arc<SequencingState>(node_2, node_7, 2);
    node_2->out_arcs.push_back(arc_2_7);
    node_7->in_arcs.push_back(arc_2_7);
    graph->add_node(node_7);

    auto* node_8 = new Node<SequencingState>(8, new SequencingState{{0, 2}, 0});
    auto* arc_3_8 = new Arc<SequencingState>(node_3, node_8, 0);
    node_3->out_arcs.push_back(arc_3_8);
    node_8->in_arcs.push_back(arc_3_8);
    graph->add_node(node_8);

    auto* node_9 = new Node<SequencingState>(9, new SequencingState{{1, 2}, 1});
    auto* arc_3_9 = new Arc<SequencingState>(node_3, node_9, 1);
    node_3->out_arcs.push_back(arc_3_9);
    node_9->in_arcs.push_back(arc_3_9);
    graph->add_node(node_9);

    // Layer 3 (terminal)
    graph->add_new_layer();
    auto* node_10 = new Node<SequencingState>(10, new SequencingState{{}, -1});
    auto* arc_4_10 = new Arc<SequencingState>(node_4, node_10, 2);
    node_4->out_arcs.push_back(arc_4_10);
    node_10->in_arcs.push_back(arc_4_10);
    auto* arc_5_10 = new Arc<SequencingState>(node_5, node_10, 1);
    node_5->out_arcs.push_back(arc_5_10);
    node_10->in_arcs.push_back(arc_5_10);
    auto* arc_6_10 = new Arc<SequencingState>(node_6, node_10, 2);
    node_6->out_arcs.push_back(arc_6_10);
    node_10->in_arcs.push_back(arc_6_10);
    auto* arc_7_10 = new Arc<SequencingState>(node_7, node_10, 0);
    node_7->out_arcs.push_back(arc_7_10);
    node_10->in_arcs.push_back(arc_7_10);
    auto* arc_8_10 = new Arc<SequencingState>(node_8, node_10, 1);
    node_8->out_arcs.push_back(arc_8_10);
    node_10->in_arcs.push_back(arc_8_10);
    auto* arc_9_10 = new Arc<SequencingState>(node_9, node_10, 0);
    node_9->out_arcs.push_back(arc_9_10);
    node_10->in_arcs.push_back(arc_9_10);
    graph->add_node(node_10);

    return graph;
}

Graph<SequencingState>* GetReduceDDSequencing() {
    auto* node_0 = new Node<SequencingState>(0, new SequencingState{{}, -1});
    auto* graph = new Graph<SequencingState>(node_0);

    graph->add_new_layer();
    auto* node_1 = new Node<SequencingState>(1, new SequencingState{{0}, 0});
    auto* arc_0_1 = new Arc<SequencingState>(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1); node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    auto* node_2 = new Node<SequencingState>(2, new SequencingState{{1}, 1});
    auto* arc_0_2 = new Arc<SequencingState>(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2); node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    auto* node_3 = new Node<SequencingState>(3, new SequencingState{{2}, 2});
    auto* arc_0_3 = new Arc<SequencingState>(node_0, node_3, 2);
    node_0->out_arcs.push_back(arc_0_3); node_3->in_arcs.push_back(arc_0_3);
    graph->add_node(node_3);

    graph->add_new_layer();
    // {0,1}_1 kept (id=4), {0,1}_0 merged into it
    auto* node_4 = new Node<SequencingState>(4, new SequencingState{{0, 1}, 1});
    auto* arc_1_4 = new Arc<SequencingState>(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4); node_4->in_arcs.push_back(arc_1_4);
    auto* arc_2_4 = new Arc<SequencingState>(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4); node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    // {0,2}_2 kept (id=5), {0,2}_0 merged into it
    auto* node_5 = new Node<SequencingState>(5, new SequencingState{{0, 2}, 2});
    auto* arc_1_5 = new Arc<SequencingState>(node_1, node_5, 2);
    node_1->out_arcs.push_back(arc_1_5); node_5->in_arcs.push_back(arc_1_5);
    auto* arc_3_5 = new Arc<SequencingState>(node_3, node_5, 0);
    node_3->out_arcs.push_back(arc_3_5); node_5->in_arcs.push_back(arc_3_5);
    graph->add_node(node_5);

    // {1,2}_2 kept (id=7), {1,2}_1 merged into it
    auto* node_7 = new Node<SequencingState>(7, new SequencingState{{1, 2}, 2});
    auto* arc_2_7 = new Arc<SequencingState>(node_2, node_7, 2);
    node_2->out_arcs.push_back(arc_2_7); node_7->in_arcs.push_back(arc_2_7);
    auto* arc_3_7 = new Arc<SequencingState>(node_3, node_7, 1);
    node_3->out_arcs.push_back(arc_3_7); node_7->in_arcs.push_back(arc_3_7);
    graph->add_node(node_7);

    graph->add_new_layer();
    auto* node_10 = new Node<SequencingState>(10, new SequencingState{{}, -1});
    auto* arc_4_10 = new Arc<SequencingState>(node_4, node_10, 2);
    node_4->out_arcs.push_back(arc_4_10); node_10->in_arcs.push_back(arc_4_10);
    auto* arc_5_10 = new Arc<SequencingState>(node_5, node_10, 1);
    node_5->out_arcs.push_back(arc_5_10); node_10->in_arcs.push_back(arc_5_10);
    auto* arc_7_10 = new Arc<SequencingState>(node_7, node_10, 0);
    node_7->out_arcs.push_back(arc_7_10); node_10->in_arcs.push_back(arc_7_10);
    graph->add_node(node_10);

    return graph;
}

Graph<SequencingState>* GetRestrictedDDSequencing() {
    auto* node_0 = new Node<SequencingState>(0, new SequencingState{{}, -1});
    auto* graph = new Graph<SequencingState>(node_0);

    graph->add_new_layer();
    auto* node_1 = new Node<SequencingState>(1, new SequencingState{{0}, 0});
    auto* arc_0_1 = new Arc<SequencingState>(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1); node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    auto* node_2 = new Node<SequencingState>(2, new SequencingState{{1}, 1});
    auto* arc_0_2 = new Arc<SequencingState>(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2); node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    auto* node_3 = new Node<SequencingState>(3, new SequencingState{{2}, 2});
    auto* arc_0_3 = new Arc<SequencingState>(node_0, node_3, 2);
    node_0->out_arcs.push_back(arc_0_3); node_3->in_arcs.push_back(arc_0_3);
    graph->add_node(node_3);

    // Layer 2: width=3, keep lowest committed processing: {0,2}(3), {0,2}(3), {1,2}(4)
    graph->add_new_layer();
    auto* node_5 = new Node<SequencingState>(5, new SequencingState{{0, 2}, 2});
    auto* arc_1_5 = new Arc<SequencingState>(node_1, node_5, 2);
    node_1->out_arcs.push_back(arc_1_5); node_5->in_arcs.push_back(arc_1_5);
    graph->add_node(node_5);

    auto* node_7 = new Node<SequencingState>(7, new SequencingState{{1, 2}, 2});
    auto* arc_2_7 = new Arc<SequencingState>(node_2, node_7, 2);
    node_2->out_arcs.push_back(arc_2_7); node_7->in_arcs.push_back(arc_2_7);
    graph->add_node(node_7);

    auto* node_8 = new Node<SequencingState>(8, new SequencingState{{0, 2}, 0});
    auto* arc_3_8 = new Arc<SequencingState>(node_3, node_8, 0);
    node_3->out_arcs.push_back(arc_3_8); node_8->in_arcs.push_back(arc_3_8);
    graph->add_node(node_8);

    graph->add_new_layer();
    auto* node_9 = new Node<SequencingState>(9, new SequencingState{{}, -1});
    auto* arc_5_9 = new Arc<SequencingState>(node_5, node_9, 1);
    node_5->out_arcs.push_back(arc_5_9); node_9->in_arcs.push_back(arc_5_9);
    auto* arc_7_9 = new Arc<SequencingState>(node_7, node_9, 0);
    node_7->out_arcs.push_back(arc_7_9); node_9->in_arcs.push_back(arc_7_9);
    auto* arc_8_9 = new Arc<SequencingState>(node_8, node_9, 1);
    node_8->out_arcs.push_back(arc_8_9); node_9->in_arcs.push_back(arc_8_9);
    graph->add_node(node_9);

    return graph;
}

Graph<SequencingState>* GetRelaxedDDSequencing() {
    // Graph equality matches nodes/arcs by state (not id or order), so ids and
    // arc order below are only for readability.
    auto* node_0 = new Node<SequencingState>(0, new SequencingState{{}, -1});
    auto* graph = new Graph<SequencingState>(node_0);

    graph->add_new_layer();
    auto* node_1 = new Node<SequencingState>(1, new SequencingState{{0}, 0});
    auto* arc_0_1 = new Arc<SequencingState>(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1); node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    auto* node_2 = new Node<SequencingState>(2, new SequencingState{{1}, 1});
    auto* arc_0_2 = new Arc<SequencingState>(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2); node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    auto* node_3 = new Node<SequencingState>(3, new SequencingState{{2}, 2});
    auto* arc_0_3 = new Arc<SequencingState>(node_0, node_3, 2);
    node_0->out_arcs.push_back(arc_0_3); node_3->in_arcs.push_back(arc_0_3);
    graph->add_node(node_3);

    // Layer 2: width=3 after merging by committed-processing priority.
    graph->add_new_layer();
    // {2}_2 : merge of {0,2}_2 (from 1 via 2) and {1,2}_1 (from 3 via 1)
    auto* node_4 = new Node<SequencingState>(4, new SequencingState{{2}, 2});
    auto* arc_1_4 = new Arc<SequencingState>(node_1, node_4, 2);
    node_1->out_arcs.push_back(arc_1_4); node_4->in_arcs.push_back(arc_1_4);
    auto* arc_3_4 = new Arc<SequencingState>(node_3, node_4, 1);
    node_3->out_arcs.push_back(arc_3_4); node_4->in_arcs.push_back(arc_3_4);
    graph->add_node(node_4);

    // {1}_2 : merge of {0,1}_1, {0,1}_0, {1,2}_2
    auto* node_5 = new Node<SequencingState>(5, new SequencingState{{1}, 2});
    auto* arc_1_5 = new Arc<SequencingState>(node_1, node_5, 1);
    node_1->out_arcs.push_back(arc_1_5); node_5->in_arcs.push_back(arc_1_5);
    auto* arc_2_5_v0 = new Arc<SequencingState>(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5_v0); node_5->in_arcs.push_back(arc_2_5_v0);
    auto* arc_2_5_v2 = new Arc<SequencingState>(node_2, node_5, 2);
    node_2->out_arcs.push_back(arc_2_5_v2); node_5->in_arcs.push_back(arc_2_5_v2);
    graph->add_node(node_5);

    auto* node_6 = new Node<SequencingState>(6, new SequencingState{{0, 2}, 0});
    auto* arc_3_6 = new Arc<SequencingState>(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6); node_6->in_arcs.push_back(arc_3_6);
    graph->add_node(node_6);

    graph->add_new_layer();
    auto* node_terminal = new Node<SequencingState>(7, new SequencingState{{}, -1});
    auto* arc_4_t0 = new Arc<SequencingState>(node_4, node_terminal, 0);
    node_4->out_arcs.push_back(arc_4_t0); node_terminal->in_arcs.push_back(arc_4_t0);
    auto* arc_4_t1 = new Arc<SequencingState>(node_4, node_terminal, 1);
    node_4->out_arcs.push_back(arc_4_t1); node_terminal->in_arcs.push_back(arc_4_t1);
    auto* arc_5_t0 = new Arc<SequencingState>(node_5, node_terminal, 0);
    node_5->out_arcs.push_back(arc_5_t0); node_terminal->in_arcs.push_back(arc_5_t0);
    auto* arc_5_t2 = new Arc<SequencingState>(node_5, node_terminal, 2);
    node_5->out_arcs.push_back(arc_5_t2); node_terminal->in_arcs.push_back(arc_5_t2);
    auto* arc_6_t = new Arc<SequencingState>(node_6, node_terminal, 1);
    node_6->out_arcs.push_back(arc_6_t); node_terminal->in_arcs.push_back(arc_6_t);
    graph->add_node(node_terminal);

    return graph;
}

Graph<SequencingState>* GetFalseDDSequencing() {
    // Same structure as exact but with swapped arc values (0<->1) on root arcs
    auto* node_0 = new Node<SequencingState>(0, new SequencingState{{}, -1});
    auto* graph = new Graph<SequencingState>(node_0);

    graph->add_new_layer();
    auto* node_1 = new Node<SequencingState>(1, new SequencingState{{0}, 0});
    auto* arc_0_1 = new Arc<SequencingState>(node_0, node_1, 1);  // wrong: should be 0
    node_0->out_arcs.push_back(arc_0_1); node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    auto* node_2 = new Node<SequencingState>(2, new SequencingState{{1}, 1});
    auto* arc_0_2 = new Arc<SequencingState>(node_0, node_2, 0);  // wrong: should be 1
    node_0->out_arcs.push_back(arc_0_2); node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    auto* node_3 = new Node<SequencingState>(3, new SequencingState{{2}, 2});
    auto* arc_0_3 = new Arc<SequencingState>(node_0, node_3, 2);
    node_0->out_arcs.push_back(arc_0_3); node_3->in_arcs.push_back(arc_0_3);
    graph->add_node(node_3);

    graph->add_new_layer();
    auto* node_4 = new Node<SequencingState>(4, new SequencingState{{0, 1}, 1});
    auto* arc_1_4 = new Arc<SequencingState>(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4); node_4->in_arcs.push_back(arc_1_4);
    graph->add_node(node_4);

    auto* node_5 = new Node<SequencingState>(5, new SequencingState{{0, 2}, 2});
    auto* arc_1_5 = new Arc<SequencingState>(node_1, node_5, 2);
    node_1->out_arcs.push_back(arc_1_5); node_5->in_arcs.push_back(arc_1_5);
    graph->add_node(node_5);

    auto* node_6 = new Node<SequencingState>(6, new SequencingState{{0, 1}, 0});
    auto* arc_2_6 = new Arc<SequencingState>(node_2, node_6, 0);
    node_2->out_arcs.push_back(arc_2_6); node_6->in_arcs.push_back(arc_2_6);
    graph->add_node(node_6);

    auto* node_7 = new Node<SequencingState>(7, new SequencingState{{1, 2}, 2});
    auto* arc_2_7 = new Arc<SequencingState>(node_2, node_7, 2);
    node_2->out_arcs.push_back(arc_2_7); node_7->in_arcs.push_back(arc_2_7);
    graph->add_node(node_7);

    auto* node_8 = new Node<SequencingState>(8, new SequencingState{{0, 2}, 0});
    auto* arc_3_8 = new Arc<SequencingState>(node_3, node_8, 0);
    node_3->out_arcs.push_back(arc_3_8); node_8->in_arcs.push_back(arc_3_8);
    graph->add_node(node_8);

    auto* node_9 = new Node<SequencingState>(9, new SequencingState{{1, 2}, 1});
    auto* arc_3_9 = new Arc<SequencingState>(node_3, node_9, 1);
    node_3->out_arcs.push_back(arc_3_9); node_9->in_arcs.push_back(arc_3_9);
    graph->add_node(node_9);

    graph->add_new_layer();
    auto* node_10 = new Node<SequencingState>(10, new SequencingState{{}, -1});
    auto* arc_4_10 = new Arc<SequencingState>(node_4, node_10, 2);
    node_4->out_arcs.push_back(arc_4_10); node_10->in_arcs.push_back(arc_4_10);
    auto* arc_5_10 = new Arc<SequencingState>(node_5, node_10, 1);
    node_5->out_arcs.push_back(arc_5_10); node_10->in_arcs.push_back(arc_5_10);
    auto* arc_6_10 = new Arc<SequencingState>(node_6, node_10, 2);
    node_6->out_arcs.push_back(arc_6_10); node_10->in_arcs.push_back(arc_6_10);
    auto* arc_7_10 = new Arc<SequencingState>(node_7, node_10, 0);
    node_7->out_arcs.push_back(arc_7_10); node_10->in_arcs.push_back(arc_7_10);
    auto* arc_8_10 = new Arc<SequencingState>(node_8, node_10, 1);
    node_8->out_arcs.push_back(arc_8_10); node_10->in_arcs.push_back(arc_8_10);
    auto* arc_9_10 = new Arc<SequencingState>(node_9, node_10, 0);
    node_9->out_arcs.push_back(arc_9_10); node_10->in_arcs.push_back(arc_9_10);
    graph->add_node(node_10);

    return graph;
}
