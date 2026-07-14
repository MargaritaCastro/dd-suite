#include <gtest/gtest.h>
#include "../../SourceCode/DDStructure/Node.h"
#include "../../SourceCode/DDStructure/Arc.h"
#include "../../SourceCode/DDStructure/Graph.h"
#include "../Examples/KnapsackInstance/KnapsackProblemState.h"

#include <memory>

using namespace std;

class GraphEqualOperation : public ::testing::Test {
protected:
    void SetUp() override {
        // Layer 1
        Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
        original_graph = new Graph(node_0);
        original_graph->add_new_layer();

        // Layer 2
        Node<State>* node_1 = new Node<State>(1, new State({1, 1}));
        Arc<State>* arc_0_1 = new Arc(node_0, node_1, 1);
        node_0->out_arcs.push_back(arc_0_1);
        node_1->in_arcs.push_back(arc_0_1);
        original_graph->add_node(node_1);

        Node<State>* node_2 = new Node<State>(2, new State({4, 4}));
        Arc<State>* arc_0_2 = new Arc(node_0, node_2, 0);
        node_0->out_arcs.push_back(arc_0_2);
        node_2->in_arcs.push_back(arc_0_2);
        original_graph->add_node(node_2);

        original_graph->add_new_layer();

        // Layer 3
        Node<State>* node_3 = new Node<State>(3, new State({3, 4}));
        Arc<State>* arc_1_3 = new Arc(node_1, node_3, 1);
        node_1->out_arcs.push_back(arc_1_3);
        node_3->in_arcs.push_back(arc_1_3);
        Arc<State>* arc_2_3 = new Arc(node_2, node_3, 1);
        node_2->out_arcs.push_back(arc_2_3);
        node_3->in_arcs.push_back(arc_2_3);
        original_graph->add_node(node_3);

        Node<State>* node_4 = new Node<State>(4, new State({1, 4}));
        Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
        node_1->out_arcs.push_back(arc_1_4);
        node_4->in_arcs.push_back(arc_1_4);
        Arc<State>* arc_2_4 = new Arc(node_2, node_4, 0);
        node_2->out_arcs.push_back(arc_2_4);
        node_4->in_arcs.push_back(arc_2_4);
        original_graph->add_node(node_4);

        Node<State>* node_5 = new Node<State>(5, new State({2, 2}));
        Arc<State>* arc_2_5 = new Arc(node_2, node_5, 0);
        node_2->out_arcs.push_back(arc_2_5);
        node_5->in_arcs.push_back(arc_2_5);
        original_graph->add_node(node_5);

        original_graph->add_new_layer();

        // Layer 4
        Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
        Arc<State>* arc_3_6 = new Arc(node_3, node_6, 0);
        node_3->out_arcs.push_back(arc_3_6);
        node_6->in_arcs.push_back(arc_3_6);
        Arc<State>* arc_4_6_op0 = new Arc(node_4, node_6, 0);
        node_4->out_arcs.push_back(arc_4_6_op0);
        node_6->in_arcs.push_back(arc_4_6_op0);
        Arc<State>* arc_4_6_op1 = new Arc(node_4, node_6, 1);
        node_4->out_arcs.push_back(arc_4_6_op1);
        node_6->in_arcs.push_back(arc_4_6_op1);
        Arc<State>* arc_5_6 = new Arc(node_5, node_6, 1);
        node_5->out_arcs.push_back(arc_5_6);
        node_6->in_arcs.push_back(arc_5_6);
        original_graph->add_node(node_6);
    }

    void TearDown() override {
        delete original_graph;
    }

    Graph<State>* original_graph;
};

TEST_F(GraphEqualOperation, TestSameGraphEquality) {
    Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
    Graph<State>* graph = new Graph(node_0);
    graph->add_new_layer();

    Node<State>* node_1 = new Node<State>(1, new State({1, 1}));
    Arc<State>* arc_0_1 = new Arc(node_0, node_1, 1);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<State>* node_2 = new Node<State>(2, new State({4, 4}));
    Arc<State>* arc_0_2 = new Arc(node_0, node_2, 0);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();

    Node<State>* node_3 = new Node<State>(3, new State({3, 4}));
    Arc<State>* arc_1_3 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    Arc<State>* arc_2_3 = new Arc(node_2, node_3, 1);
    node_2->out_arcs.push_back(arc_2_3);
    node_3->in_arcs.push_back(arc_2_3);
    graph->add_node(node_3);

    Node<State>* node_4 = new Node<State>(4, new State({1, 4}));
    Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<State>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<State>* node_5 = new Node<State>(5, new State({2, 2}));
    Arc<State>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();

    Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
    Arc<State>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    Arc<State>* arc_4_6_op0 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6_op0);
    node_6->in_arcs.push_back(arc_4_6_op0);
    Arc<State>* arc_4_6_op1 = new Arc(node_4, node_6, 1);
    node_4->out_arcs.push_back(arc_4_6_op1);
    node_6->in_arcs.push_back(arc_4_6_op1);
    Arc<State>* arc_5_6 = new Arc(node_5, node_6, 1);
    node_5->out_arcs.push_back(arc_5_6);
    node_6->in_arcs.push_back(arc_5_6);
    graph->add_node(node_6);


    EXPECT_TRUE(*original_graph == *graph);
    delete graph;
}

TEST_F(GraphEqualOperation, TestEquivalentGraphs) {
    Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
    Graph<State>* graph = new Graph(node_0);
    graph->add_new_layer();

    Node<State>* node_1 = new Node<State>(1, new State({4, 4}));
    Arc<State>* arc_0_1 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<State>* node_2 = new Node<State>(2, new State({1, 1}));
    Arc<State>* arc_0_2 = new Arc(node_0, node_2, 1);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();

    Node<State>* node_3 = new Node<State>(3, new State({2, 2}));
    Arc<State>* arc_1_3 = new Arc(node_1, node_3, 0);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    graph->add_node(node_3);

    Node<State>* node_4 = new Node<State>(4, new State({3, 4}));
    Arc<State>* arc_2_4 = new Arc(node_2, node_4, 1);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    graph->add_node(node_4);

    Node<State>* node_5 = new Node<State>(5, new State({1, 4}));
    Arc<State>* arc_2_5 = new Arc(node_2, node_5, 1);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    Arc<State>* arc_1_5 = new Arc(node_1, node_5, 0);
    node_1->out_arcs.push_back(arc_1_5);
    node_5->in_arcs.push_back(arc_1_5);
    graph->add_node(node_5);

    graph->add_new_layer();

    Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
    Arc<State>* arc_4_6 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6);
    node_6->in_arcs.push_back(arc_4_6);
    Arc<State>* arc_5_6_op0 = new Arc(node_5, node_6, 0);
    node_5->out_arcs.push_back(arc_5_6_op0);
    node_6->in_arcs.push_back(arc_5_6_op0);
    Arc<State>* arc_5_6_op1 = new Arc(node_5, node_6, 1);
    node_5->out_arcs.push_back(arc_5_6_op1);
    node_6->in_arcs.push_back(arc_5_6_op1);
    Arc<State>* arc_3_6 = new Arc(node_3, node_6, 1);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    graph->add_node(node_6);

    EXPECT_TRUE(*original_graph == *graph);
    delete graph;
}

TEST_F(GraphEqualOperation, TestDifferentNumberLayers) {
    Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
    Graph<State>* graph = new Graph(node_0);
    graph->add_new_layer();

    Node<State>* node_1 = new Node<State>(1, new State({1, 1}));
    Arc<State>* arc_0_1 = new Arc(node_0, node_1, 1);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<State>* node_2 = new Node<State>(2, new State({4, 4}));
    Arc<State>* arc_0_2 = new Arc(node_0, node_2, 0);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();

    Node<State>* node_3 = new Node<State>(3, new State({3, 4}));
    Arc<State>* arc_1_3 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    Arc<State>* arc_2_3 = new Arc(node_2, node_3, 1);
    node_2->out_arcs.push_back(arc_2_3);
    node_3->in_arcs.push_back(arc_2_3);
    graph->add_node(node_3);

    Node<State>* node_4 = new Node<State>(4, new State({1, 4}));
    Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<State>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<State>* node_5 = new Node<State>(5, new State({2, 2}));
    Arc<State>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();

    Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
    Arc<State>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    Arc<State>* arc_4_6_op0 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6_op0);
    node_6->in_arcs.push_back(arc_4_6_op0);
    Arc<State>* arc_4_6_op1 = new Arc(node_4, node_6, 1);
    node_4->out_arcs.push_back(arc_4_6_op1);
    node_6->in_arcs.push_back(arc_4_6_op1);
    Arc<State>* arc_5_6 = new Arc(node_5, node_6, 1);
    node_5->out_arcs.push_back(arc_5_6);
    node_6->in_arcs.push_back(arc_5_6);
    graph->add_node(node_6);

    graph->add_new_layer();
    Node<State>* node_7 = new Node<State>(7, new State({1, 7}));
    graph->add_node(node_7);


    EXPECT_FALSE(*original_graph == *graph);
    delete graph;
}

TEST_F(GraphEqualOperation, TestSameGraphDifferentNodeId) {
    Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
    Graph<State>* graph = new Graph(node_0);
    graph->add_new_layer();

    Node<State>* node_1 = new Node<State>(1, new State({1, 1}));
    Arc<State>* arc_0_1 = new Arc(node_0, node_1, 1);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<State>* node_2 = new Node<State>(2, new State({4, 4}));
    Arc<State>* arc_0_2 = new Arc(node_0, node_2, 0);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();

    Node<State>* node_3 = new Node<State>(3, new State({3, 4}));
    Arc<State>* arc_1_3 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    Arc<State>* arc_2_3 = new Arc(node_2, node_3, 1);
    node_2->out_arcs.push_back(arc_2_3);
    node_3->in_arcs.push_back(arc_2_3);
    graph->add_node(node_3);

    Node<State>* node_4 = new Node<State>(15, new State({1, 4})); // Different NodeId
    Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<State>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<State>* node_5 = new Node<State>(5, new State({2, 2}));
    Arc<State>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();

    Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
    Arc<State>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    Arc<State>* arc_4_6_op0 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6_op0);
    node_6->in_arcs.push_back(arc_4_6_op0);
    Arc<State>* arc_4_6_op1 = new Arc(node_4, node_6, 1);
    node_4->out_arcs.push_back(arc_4_6_op1);
    node_6->in_arcs.push_back(arc_4_6_op1);
    Arc<State>* arc_5_6 = new Arc(node_5, node_6, 1);
    node_5->out_arcs.push_back(arc_5_6);
    node_6->in_arcs.push_back(arc_5_6);
    graph->add_node(node_6);


    EXPECT_TRUE(*original_graph == *graph);
    delete graph;
}

TEST_F(GraphEqualOperation, TestSameGraphDifferentVariable) {
    Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
    Graph<State>* graph = new Graph(node_0);
    graph->add_new_layer();

    Node<State>* node_1 = new Node<State>(1, new State({1, 1}));
    Arc<State>* arc_0_1 = new Arc(node_0, node_1, 1);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<State>* node_2 = new Node<State>(2, new State({4, 4}));
    Arc<State>* arc_0_2 = new Arc(node_0, node_2, 0); // Different variable
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();

    Node<State>* node_3 = new Node<State>(3, new State({3, 4}));
    Arc<State>* arc_1_3 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    Arc<State>* arc_2_3 = new Arc(node_2, node_3, 1);
    node_2->out_arcs.push_back(arc_2_3);
    node_3->in_arcs.push_back(arc_2_3);
    graph->add_node(node_3);

    Node<State>* node_4 = new Node<State>(4, new State({1, 4}));
    Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<State>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<State>* node_5 = new Node<State>(5, new State({2, 2}));
    Arc<State>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();

    Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
    Arc<State>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    Arc<State>* arc_4_6_op0 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6_op0);
    node_6->in_arcs.push_back(arc_4_6_op0);
    Arc<State>* arc_4_6_op1 = new Arc(node_4, node_6, 1);
    node_4->out_arcs.push_back(arc_4_6_op1);
    node_6->in_arcs.push_back(arc_4_6_op1);
    Arc<State>* arc_5_6 = new Arc(node_5, node_6, 1);
    node_5->out_arcs.push_back(arc_5_6);
    node_6->in_arcs.push_back(arc_5_6);
    graph->add_node(node_6);


    EXPECT_TRUE(*original_graph == *graph);
    delete graph;

}


TEST_F(GraphEqualOperation, TestDifferentGraphMoreNodes) {
    Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
    Graph<State>* graph = new Graph(node_0);
    graph->add_new_layer();

    Node<State>* node_1 = new Node<State>(1, new State({1, 1}));
    Arc<State>* arc_0_1 = new Arc(node_0, node_1, 1);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<State>* node_2 = new Node<State>(2, new State({4, 4}));
    Arc<State>* arc_0_2 = new Arc(node_0, node_2, 0);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();

    Node<State>* node_3 = new Node<State>(3, new State({3, 4}));
    Arc<State>* arc_1_3 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    Arc<State>* arc_2_3 = new Arc(node_2, node_3, 1);
    node_2->out_arcs.push_back(arc_2_3);
    node_3->in_arcs.push_back(arc_2_3);
    graph->add_node(node_3);

    Node<State>* node_4 = new Node<State>(4, new State({1, 4}));
    Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<State>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<State>* node_5 = new Node<State>(5, new State({2, 2}));
    Arc<State>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();

    Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
    Arc<State>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    Arc<State>* arc_4_6_op0 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6_op0);
    node_6->in_arcs.push_back(arc_4_6_op0);
    Arc<State>* arc_4_6_op1 = new Arc(node_4, node_6, 1);
    node_4->out_arcs.push_back(arc_4_6_op1);
    node_6->in_arcs.push_back(arc_4_6_op1);
    Arc<State>* arc_5_6 = new Arc(node_5, node_6, 1);
    node_5->out_arcs.push_back(arc_5_6);
    node_6->in_arcs.push_back(arc_5_6);
    graph->add_node(node_6);

    Node<State>* node_7 = new Node<State>(7, new State({1, 7}));
    graph->add_node(node_7);


    EXPECT_FALSE(*original_graph == *graph);
    delete graph;
}

TEST_F(GraphEqualOperation, TestDifferentGraphLessNodes) {
    Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
    Graph<State>* graph = new Graph(node_0);
    graph->add_new_layer();

    Node<State>* node_1 = new Node<State>(1, new State({1, 1}));
    Arc<State>* arc_0_1 = new Arc(node_0, node_1, 1);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<State>* node_2 = new Node<State>(2, new State({4, 4}));
    Arc<State>* arc_0_2 = new Arc(node_0, node_2, 0);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();

    Node<State>* node_3 = new Node<State>(3, new State({3, 4}));
    Arc<State>* arc_1_3 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    Arc<State>* arc_2_3 = new Arc(node_2, node_3, 1);
    node_2->out_arcs.push_back(arc_2_3);
    node_3->in_arcs.push_back(arc_2_3);
    graph->add_node(node_3);

    Node<State>* node_4 = new Node<State>(4, new State({1, 4}));
    Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<State>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    graph->add_new_layer();

    Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
    Arc<State>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    Arc<State>* arc_4_6_op0 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6_op0);
    node_6->in_arcs.push_back(arc_4_6_op0);
    Arc<State>* arc_4_6_op1 = new Arc(node_4, node_6, 1);
    node_4->out_arcs.push_back(arc_4_6_op1);
    node_6->in_arcs.push_back(arc_4_6_op1);
    graph->add_node(node_6);

    Node<State>* node_7 = new Node<State>(7, new State({1, 7}));
    graph->add_node(node_7);


    EXPECT_FALSE(*original_graph == *graph);
    delete graph;
}

TEST_F(GraphEqualOperation, TestDifferentGraphMoreArcs) {
    Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
    Graph<State>* graph = new Graph(node_0);
    graph->add_new_layer();

    Node<State>* node_1 = new Node<State>(1, new State({1, 1}));
    Arc<State>* arc_0_1_op0 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1_op0);
    node_1->in_arcs.push_back(arc_0_1_op0);
    Arc<State>* arc_0_1_op1 = new Arc(node_0, node_1, 1); // New arc
    node_0->out_arcs.push_back(arc_0_1_op1);
    node_1->in_arcs.push_back(arc_0_1_op1);
    graph->add_node(node_1);

    Node<State>* node_2 = new Node<State>(2, new State({4, 4}));
    Arc<State>* arc_0_2 = new Arc(node_0, node_2, 0);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();

    Node<State>* node_3 = new Node<State>(3, new State({3, 4}));
    Arc<State>* arc_1_3 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    Arc<State>* arc_2_3 = new Arc(node_2, node_3, 1);
    node_2->out_arcs.push_back(arc_2_3);
    node_3->in_arcs.push_back(arc_2_3);
    graph->add_node(node_3);

    Node<State>* node_4 = new Node<State>(4, new State({1, 4}));
    Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<State>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<State>* node_5 = new Node<State>(5, new State({2, 2}));
    Arc<State>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();

    Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
    Arc<State>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    Arc<State>* arc_4_6_op0 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6_op0);
    node_6->in_arcs.push_back(arc_4_6_op0);
    Arc<State>* arc_4_6_op1 = new Arc(node_4, node_6, 1);
    node_4->out_arcs.push_back(arc_4_6_op1);
    node_6->in_arcs.push_back(arc_4_6_op1);
    Arc<State>* arc_5_6 = new Arc(node_5, node_6, 1);
    node_5->out_arcs.push_back(arc_5_6);
    node_6->in_arcs.push_back(arc_5_6);
    graph->add_node(node_6);


    EXPECT_FALSE(*original_graph == *graph);
    delete graph;

}
    
TEST_F(GraphEqualOperation, TestDifferentPointsOfArc) {

    Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
    Graph<State>* graph = new Graph(node_0);
    graph->add_new_layer();

    Node<State>* node_1 = new Node<State>(1, new State({1, 1}));
    Arc<State>* arc_0_1_op0 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1_op0);
    node_1->in_arcs.push_back(arc_0_1_op0);
    graph->add_node(node_1);

    Node<State>* node_2 = new Node<State>(2, new State({4, 4}));
    Arc<State>* arc_0_2 = new Arc(node_0, node_2, 0);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();

    Node<State>* node_3 = new Node<State>(3, new State({3, 4}));
    Arc<State>* arc_1_3 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    Arc<State>* arc_2_3 = new Arc(node_2, node_3, 1);
    node_2->out_arcs.push_back(arc_2_3);
    node_3->in_arcs.push_back(arc_2_3);
    graph->add_node(node_3);

    Node<State>* node_4 = new Node<State>(4, new State({1, 4}));
    Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<State>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<State>* node_5 = new Node<State>(5, new State({2, 2}));
    Arc<State>* arc_1_5 = new Arc(node_1, node_5, 0);  // Different in node of arc
    node_1->out_arcs.push_back(arc_1_5);
    node_5->in_arcs.push_back(arc_1_5);
    graph->add_node(node_5);

    graph->add_new_layer();

    Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
    Arc<State>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    Arc<State>* arc_4_6_op0 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6_op0);
    node_6->in_arcs.push_back(arc_4_6_op0);
    Arc<State>* arc_4_6_op1 = new Arc(node_4, node_6, 1);
    node_4->out_arcs.push_back(arc_4_6_op1);
    node_6->in_arcs.push_back(arc_4_6_op1);
    Arc<State>* arc_5_6 = new Arc(node_5, node_6, 1);
    node_5->out_arcs.push_back(arc_5_6);
    node_6->in_arcs.push_back(arc_5_6);
    graph->add_node(node_6);


    EXPECT_FALSE(*original_graph == *graph);
    delete graph;

}

TEST_F(GraphEqualOperation, TestDifferentGraphLessArcs) {
    Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
    Graph<State>* graph = new Graph(node_0);
    graph->add_new_layer();

    Node<State>* node_1 = new Node<State>(1, new State({1, 1}));
    Arc<State>* arc_0_1_op0 = new Arc(node_0, node_1, 0);
    node_0->out_arcs.push_back(arc_0_1_op0);
    node_1->in_arcs.push_back(arc_0_1_op0);
    graph->add_node(node_1);

    Node<State>* node_2 = new Node<State>(2, new State({4, 4}));
    Arc<State>* arc_0_2 = new Arc(node_0, node_2, 0);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();

    Node<State>* node_3 = new Node<State>(3, new State({3, 4}));
    Arc<State>* arc_1_3 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    Arc<State>* arc_2_3 = new Arc(node_2, node_3, 1);
    node_2->out_arcs.push_back(arc_2_3);
    node_3->in_arcs.push_back(arc_2_3);
    graph->add_node(node_3);

    Node<State>* node_4 = new Node<State>(4, new State({1, 4}));
    Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<State>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<State>* node_5 = new Node<State>(5, new State({2, 2}));
    Arc<State>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();

    Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
    Arc<State>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    Arc<State>* arc_4_6_op1 = new Arc(node_4, node_6, 1); // Less arcs
    node_4->out_arcs.push_back(arc_4_6_op1);
    node_6->in_arcs.push_back(arc_4_6_op1);
    Arc<State>* arc_5_6 = new Arc(node_5, node_6, 1);
    node_5->out_arcs.push_back(arc_5_6);
    node_6->in_arcs.push_back(arc_5_6);
    graph->add_node(node_6);


    EXPECT_FALSE(*original_graph == *graph);
    delete graph;
}

TEST_F(GraphEqualOperation, TestDifferentArcValue) {
    Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
    Graph<State>* graph = new Graph(node_0);
    graph->add_new_layer();

    Node<State>* node_1 = new Node<State>(1, new State({1, 1}));
    Arc<State>* arc_0_1 = new Arc(node_0, node_1, 1);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<State>* node_2 = new Node<State>(2, new State({4, 4}));
    Arc<State>* arc_0_2 = new Arc(node_0, node_2, 0);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();

    Node<State>* node_3 = new Node<State>(3, new State({3, 4}));
    Arc<State>* arc_1_3 = new Arc(node_1, node_3, 0); // Different value
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    Arc<State>* arc_2_3 = new Arc(node_2, node_3, 1);
    node_2->out_arcs.push_back(arc_2_3);
    node_3->in_arcs.push_back(arc_2_3);
    graph->add_node(node_3);

    Node<State>* node_4 = new Node<State>(4, new State({1, 4}));
    Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<State>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<State>* node_5 = new Node<State>(5, new State({2, 2}));
    Arc<State>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();

    Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
    Arc<State>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    Arc<State>* arc_4_6_op0 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6_op0);
    node_6->in_arcs.push_back(arc_4_6_op0);
    Arc<State>* arc_4_6_op1 = new Arc(node_4, node_6, 1);
    node_4->out_arcs.push_back(arc_4_6_op1);
    node_6->in_arcs.push_back(arc_4_6_op1);
    Arc<State>* arc_5_6 = new Arc(node_5, node_6, 1);
    node_5->out_arcs.push_back(arc_5_6);
    node_6->in_arcs.push_back(arc_5_6);
    graph->add_node(node_6);


    EXPECT_FALSE(*original_graph == *graph);
    delete graph;
}

TEST_F(GraphEqualOperation, TestDifferentState) {
    Node<State>* node_0 = new Node<State>(0, new State({0, 0}) );
    Graph<State>* graph = new Graph(node_0);
    graph->add_new_layer();

    Node<State>* node_1 = new Node<State>(1, new State({1, 1}));
    Arc<State>* arc_0_1 = new Arc(node_0, node_1, 1);
    node_0->out_arcs.push_back(arc_0_1);
    node_1->in_arcs.push_back(arc_0_1);
    graph->add_node(node_1);

    Node<State>* node_2 = new Node<State>(2, new State({1, 4})); // Different State
    Arc<State>* arc_0_2 = new Arc(node_0, node_2, 0);
    node_0->out_arcs.push_back(arc_0_2);
    node_2->in_arcs.push_back(arc_0_2);
    graph->add_node(node_2);

    graph->add_new_layer();

    Node<State>* node_3 = new Node<State>(3, new State({3, 4}));
    Arc<State>* arc_1_3 = new Arc(node_1, node_3, 1);
    node_1->out_arcs.push_back(arc_1_3);
    node_3->in_arcs.push_back(arc_1_3);
    Arc<State>* arc_2_3 = new Arc(node_2, node_3, 1);
    node_2->out_arcs.push_back(arc_2_3);
    node_3->in_arcs.push_back(arc_2_3);
    graph->add_node(node_3);

    Node<State>* node_4 = new Node<State>(4, new State({1, 4}));
    Arc<State>* arc_1_4 = new Arc(node_1, node_4, 1);
    node_1->out_arcs.push_back(arc_1_4);
    node_4->in_arcs.push_back(arc_1_4);
    Arc<State>* arc_2_4 = new Arc(node_2, node_4, 0);
    node_2->out_arcs.push_back(arc_2_4);
    node_4->in_arcs.push_back(arc_2_4);
    graph->add_node(node_4);

    Node<State>* node_5 = new Node<State>(5, new State({2, 2}));
    Arc<State>* arc_2_5 = new Arc(node_2, node_5, 0);
    node_2->out_arcs.push_back(arc_2_5);
    node_5->in_arcs.push_back(arc_2_5);
    graph->add_node(node_5);

    graph->add_new_layer();

    Node<State>* node_6 = new Node<State>(6, new State({1, 6}));
    Arc<State>* arc_3_6 = new Arc(node_3, node_6, 0);
    node_3->out_arcs.push_back(arc_3_6);
    node_6->in_arcs.push_back(arc_3_6);
    Arc<State>* arc_4_6_op0 = new Arc(node_4, node_6, 0);
    node_4->out_arcs.push_back(arc_4_6_op0);
    node_6->in_arcs.push_back(arc_4_6_op0);
    Arc<State>* arc_4_6_op1 = new Arc(node_4, node_6, 1);
    node_4->out_arcs.push_back(arc_4_6_op1);
    node_6->in_arcs.push_back(arc_4_6_op1);
    Arc<State>* arc_5_6 = new Arc(node_5, node_6, 1);
    node_5->out_arcs.push_back(arc_5_6);
    node_6->in_arcs.push_back(arc_5_6);
    graph->add_node(node_6);


    EXPECT_FALSE(*original_graph == *graph);
    delete graph;
}

