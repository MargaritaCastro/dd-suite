#include <gtest/gtest.h>
#include "../Examples/KnapsackInstance/KnapsackProblemState.h"
#include "../../SourceCode/DD.h"
#include "GraphAlgorithms/ShortestLongestPath/ShortestLongestPath.h"
#include "dd_controller_generators/DDKnapsackState.cpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;


class KnapsackStateProblemTest : public ::testing::Test {
protected:
    void SetUp() override {
        initial_state = new State({0, 0});
        vector<pair<string, vector<int>>> variables = {
                make_pair("item_oro", vector<int>{0, 1}),
                make_pair("item_plata", vector<int>{0, 1}),
                make_pair("item_bronce", vector<int>{0, 1}),
                make_pair("item_hierro", vector<int>{0, 1})
        };

        vector<int> matrix_of_weight = {3, 3, 4, 6};
        int right_side_of_restrictions = 6;

        knapsack_params = new KnapsackInstance(new int(0), variables, matrix_of_weight, {}, right_side_of_restrictions);
        problem_instance = new KnapsackProblemState(initial_state, *knapsack_params, false);
        dd_instance = new DD(*problem_instance);

        source_directory = fs::current_path().parent_path().string();
    }

    void TearDown() override {
        delete problem_instance;
        delete dd_instance;
        delete knapsack_params;
    }

    PathStructureSolution<State> getLinearDpSolution() {
        vector<double> objective_weights = {-5, 1, 18, 17};
        ShortestLongestPath objective_function_instance = ShortestLongestPath<State>(*dd_instance);
        objective_function_instance.set_parameters(objective_weights, "max");

        return objective_function_instance.solve();
    }

    State* initial_state;
    KnapsackInstance* knapsack_params;
    KnapsackProblemState* problem_instance;
    DD<State>* dd_instance;
    string source_directory;
};

TEST_F(KnapsackStateProblemTest, TestOrderedVariables) {
    vector<string> ordered_variables_test = {"item_oro", "item_plata", "item_bronce", "item_hierro"};
    ASSERT_EQ(problem_instance->ordered_variables, ordered_variables_test);
    delete initial_state;
}

TEST_F(KnapsackStateProblemTest, TestVariablesDomain) {
    map<string, vector<int>> variables_domain_test = {
            {"item_oro",    {0, 1}},
            {"item_plata",  {0, 1}},
            {"item_bronce", {0, 1}},
            {"item_hierro", {0, 1}}
    };
    ASSERT_EQ(problem_instance->variables_domain, variables_domain_test);
    delete initial_state;

}

TEST_F(KnapsackStateProblemTest, TransitionFunction) {
    State* inter_state = new State({0, 0});
    State* scratch_state = new State({0, 0});
    bool feasibility = problem_instance->transition_function(inter_state, 0, 1, scratch_state);
    EXPECT_EQ(*scratch_state, State({3, 3}));
    EXPECT_TRUE(feasibility);
    delete inter_state;

    State* inter_state_2 = new State({4, 4});
    feasibility = problem_instance->transition_function(inter_state_2, 2, 1, scratch_state);
    EXPECT_EQ(*scratch_state, State({8, 8}));
    EXPECT_FALSE(feasibility);
    delete inter_state_2;
    delete scratch_state;

    delete initial_state;
}


TEST_F(KnapsackStateProblemTest, GetPriorityForDiscardNode) {
    auto state = new State({1, 4});
    int priority = problem_instance->get_priority_for_discard_node(state);
    EXPECT_EQ(priority, -5);
    delete state;

    state = new State({1, 1});
    priority = problem_instance->get_priority_for_discard_node(state);
    EXPECT_EQ(priority, -2);
    delete state;
    delete initial_state;
}

TEST_F(KnapsackStateProblemTest, GetPriorityForMergeNodes) {
    auto state = new State({1, 4});
    int priority = problem_instance->get_priority_for_merge_nodes(1, state);
    EXPECT_EQ(priority, -numeric_limits<int>::infinity());
    delete state;

    state = new State({4, 4});
    priority = problem_instance->get_priority_for_merge_nodes(3, state);
    EXPECT_EQ(priority, -3);
    delete state;
    delete initial_state;
}

TEST_F(KnapsackStateProblemTest, MergeOperator) {
    auto state1 = new State({2, 4});
    auto state2 = new State({1, 1});
    auto result = problem_instance->merge_operator(state1, state2);
    EXPECT_EQ(*result, State({1, 4}));
    delete state1;
    delete state2;
    delete result;

    state1 = new State({7, 10});
    state2 = new State({6, 7});
    result = problem_instance->merge_operator(state1, state2);
    EXPECT_EQ(*result, State({6, 10}));
    delete state1;
    delete state2;
    delete result;
    delete initial_state;
}

TEST_F(KnapsackStateProblemTest, GetStateAsString) {
    auto state = new State({1, 4});
    string state_string = problem_instance->get_state_as_string(state);
    EXPECT_EQ(typeid(state_string).name(), typeid(string).name());
    EXPECT_EQ(state_string, "1,4");
    delete state;
    delete initial_state;
}

TEST_F(KnapsackStateProblemTest, GetStateCopy) {
    auto state = new State({1, 4});
    auto state_copy = problem_instance->get_state_copy(state);
    EXPECT_NE(state, state_copy);
    EXPECT_EQ(*state, *state_copy);

    (state->value)[0] = 10;
    EXPECT_NE(state, state_copy);
    delete state;
    delete state_copy;
    delete initial_state;
}

TEST_F(KnapsackStateProblemTest, GetFinalState) {
    State* final_state = problem_instance->get_final_state();
    EXPECT_EQ(*final_state, State({0, 0}));
    delete final_state;
    delete initial_state;
}
    

TEST_F(KnapsackStateProblemTest, TestIsDDCreated) {
    dd_instance->create_decision_diagram(false);
    ASSERT_FALSE(dd_instance->get_decision_diagram()->structure.empty());
}

TEST_F(KnapsackStateProblemTest, TestVerboseCreateDD) {
    ofstream out("createDDKnapsackState.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_decision_diagram(true);

    cout.rdbuf(coutbuf);
    string full_file_path = source_directory + "/Test/txt_files/createDDKnapsackState.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createDDKnapsackState.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackStateProblemTest, TestVerboseCreateReduceDD) {
    ofstream out("createReduceDDKnapsackState.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram(true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createReduceDDKnapsackState.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createReduceDDKnapsackState.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackStateProblemTest, TestVerboseCreateRestrictedDD) {
    ofstream out("createRestrictedDDKnapsackState.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_restricted_decision_diagram(3, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRestrictedDDKnapsackState.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRestrictedDDKnapsackState.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackStateProblemTest, TestVerboseCreateRelaxedDD) {
    ofstream out("createRelaxedDDKnapsackState.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_relax_priority_decision_diagram(3, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRelaxedDDKnapsackState.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRelaxedDDKnapsackState.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackStateProblemTest, TestVerboseCreateRelaxedDD2) {
    ofstream out("createRelaxedDDKnapsackState.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_relax_priority_decision_diagram(3, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRelaxedDDKnapsackState.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRelaxedDDKnapsackState.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackStateProblemTest, TestCreateDDGraphEqual) {
    dd_instance->create_decision_diagram(false);
    Graph<State>* expected_graph = GetExactDDKnapsackState();
    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(KnapsackStateProblemTest, TestCreateReduceDDGraphEqual) {
    Graph<State>* expected_graph = GetReduceDDKnapsackState();
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(KnapsackStateProblemTest, TestCreateRestrictedDDGraphEqual) {
    Graph<State>* expected_graph = GetRestrictedDDKnapsackState();
    dd_instance->create_restricted_decision_diagram(3);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(KnapsackStateProblemTest, CompareTwoDifferentGraphs) {
    Graph<State>* not_expected_graph = GetFalseDDKnapsackState();
    dd_instance->create_decision_diagram(false);

    ASSERT_FALSE(*dd_instance->get_decision_diagram() == *not_expected_graph);
    delete not_expected_graph;
}

TEST_F(KnapsackStateProblemTest, TestCreateRelaxedDDGraphEqual) {
    Graph<State>* expected_graph = GetRelaxedDDKnapsackState();
    dd_instance->create_relax_priority_decision_diagram(3);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(KnapsackStateProblemTest, TestGetCopy) {
    dd_instance->create_decision_diagram(false);
    Graph<State>* original_graph = dd_instance->get_decision_diagram();
    Graph<State>* copied_graph = dd_instance->get_decision_diagram_copy();

    ASSERT_NE(&original_graph, &copied_graph); // Check if they are different objects
    ASSERT_TRUE(*original_graph==*copied_graph); // Check if they are equal in values

    original_graph->structure[0].front()->set_id(100);
    ASSERT_FALSE(original_graph->structure[0].front()->to_string() == copied_graph->structure[0].front()->to_string());


    Node<State>* new_node = new Node<State>(11, new State({1,2,3,4,5}));
    copied_graph->add_node(new_node);

    ASSERT_FALSE(original_graph->structure.back().size() == copied_graph->structure.back().size());
    delete copied_graph;
}

TEST_F(KnapsackStateProblemTest, TestGetDDBuilderTime) {
    dd_instance->create_decision_diagram(false);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(KnapsackStateProblemTest, TestGetReduceDDBuilderTime) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();
    ASSERT_GT(stof(dd_instance->get_reduction_time()), 0);
}

TEST_F(KnapsackStateProblemTest, TestGetRestrictedDDBuilderTime) {
    dd_instance->create_restricted_decision_diagram(3);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(KnapsackStateProblemTest, TestGetRelaxedDDBuilderTime) {
    dd_instance->create_relax_priority_decision_diagram(3, true);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(KnapsackStateProblemTest, GetSolutionForDD) {
    dd_instance->create_decision_diagram(false);

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 18;
    string expected_path = " arc_0_1(0)-> arc_1_3(0)-> arc_3_7(1)-> arc_7_10(0)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(KnapsackStateProblemTest, GetSolutionForReduceDD) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 18;
    string expected_path = " arc_0_1(0)-> arc_1_3(0)-> arc_3_6(1)-> arc_6_7(0)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(KnapsackStateProblemTest, GetSolutionForRestrictedeDD) {
    dd_instance->create_restricted_decision_diagram(3);

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 18;
    string expected_path = " arc_0_1(0)-> arc_1_3(0)-> arc_3_6(1)-> arc_6_9(0)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(KnapsackStateProblemTest, GetSolutionForRelaxedeDD) {
    dd_instance->create_relax_priority_decision_diagram(3);

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 35;
    string expected_path = " arc_0_1(0)-> arc_1_3(0)-> arc_3_6(1)-> arc_6_9(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(KnapsackStateProblemTest, TestCompareGMLDDGraph) {
    dd_instance->create_decision_diagram(false);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/exact_dd_knapsack_state.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackStateProblemTest, TestCompareGMLReduceDDGraph) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/reduce_dd_knapsack_state.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackStateProblemTest, TestCompareGMLRestrictedDDGraph) {
    dd_instance->create_restricted_decision_diagram(3);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/restricted_dd_knapsack_state.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackStateProblemTest, TestCompareGMLRelaxedDDGraph) {
    dd_instance->create_relax_priority_decision_diagram(3);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/relax_dd_knapsack_state.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}