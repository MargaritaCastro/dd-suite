#include <gtest/gtest.h>
#include "../../Examples/IndependentSetInstance/IndependentSetProblem.h"
#include "../../SourceCode/DD.h"
#include "GraphAlgorithms/ShortestLongestPath/ShortestLongestPath.h"
#include "dd_controller_generators/DDIndependentSet.cpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>

using namespace std;

class IndependentSetProblemTest : public ::testing::Test {
protected:
    void SetUp() override {
        map<string, vector<int>> dict_node_neighbors;
        dict_node_neighbors["x_1"] = {2, 3};
        dict_node_neighbors["x_2"] = {1, 3, 4};
        dict_node_neighbors["x_3"] = {1, 2, 4};
        dict_node_neighbors["x_4"] = {2, 3, 5};
        dict_node_neighbors["x_5"] = {4};

        vector<pair<string, vector<int>>> variables = {
                make_pair("x_1", vector<int>{0, 1}),
                make_pair("x_2", vector<int>{0, 1}),
                make_pair("x_3", vector<int>{0, 1}),
                make_pair("x_4", vector<int>{0, 1}),
                make_pair("x_5", vector<int>{0, 1})
        };

        independent_set_params = new IndependentSetInstance(new dynamic_bitset<>(5, 0b11111), variables, dict_node_neighbors);
        problem_instance = new IndependentSetProblem(*independent_set_params, false);
        dd_instance = new DD<dynamic_bitset<>>(*problem_instance);

        source_directory = fs::current_path().parent_path().string();
    }

    void TearDown() override {
        delete problem_instance;
        delete dd_instance;
        delete independent_set_params;
    }

    PathStructureSolution<dynamic_bitset<>> getLinearDpSolution() {
        vector<double> objective_weights = {3, 4, 2, 2, 7};
        ShortestLongestPath objective_function_instance = ShortestLongestPath<dynamic_bitset<>>(*dd_instance);
        objective_function_instance.set_parameters(objective_weights, "max");
        return objective_function_instance.solve();
    }
    IndependentSetInstance* independent_set_params;
    IndependentSetProblem* problem_instance;
    DD<dynamic_bitset<>>* dd_instance;
    string source_directory;
};


TEST_F(IndependentSetProblemTest, TestOrderedVariables) {
    vector<string> ordered_variables_test = {"x_1", "x_2", "x_3", "x_4", "x_5"};
    ASSERT_EQ(problem_instance->ordered_variables, ordered_variables_test);
}

TEST_F(IndependentSetProblemTest, TestVariablesDomain) {
    map<string, vector<int>> variables_domain_test = {
            {"x_1", {0, 1}},
            {"x_2", {0, 1}},
            {"x_3", {0, 1}},
            {"x_4", {0, 1}},
            {"x_5", {0, 1}}
    };
    ASSERT_EQ(problem_instance->variables_domain, variables_domain_test);
}

TEST_F(IndependentSetProblemTest, TransitionFunction) {
    auto previous_state = new dynamic_bitset<>(5, 0b01111);
    auto scratch_state = new dynamic_bitset<>(5);
    bool feasibility = problem_instance->transition_function(previous_state, 0, 1, scratch_state);

    EXPECT_EQ(*scratch_state, dynamic_bitset<>(5, 0b01000));
    EXPECT_TRUE(feasibility);
    delete previous_state;
    delete scratch_state;
}

TEST_F(IndependentSetProblemTest, GetPriorityForDiscardNode) {
    auto state = new dynamic_bitset<>(5, 0b01100);
    int priority = problem_instance->get_priority_for_discard_node(state);
    EXPECT_EQ(priority, 2);
    delete state;


    auto state2 = new dynamic_bitset<>(5, 0b01110);
    int priority2 = problem_instance->get_priority_for_discard_node(state2);
    EXPECT_EQ(priority2, 3);
    delete state2;
}

TEST_F(IndependentSetProblemTest, GetPriorityForMergeNodes) {
    auto state = new dynamic_bitset<>(5, 0b01100);
    int priority = problem_instance->get_priority_for_merge_nodes(2, state);
    EXPECT_EQ(priority, 2);
    delete state;

    auto state2 = new dynamic_bitset<>(5, 0b01110);
    int priority2 = problem_instance->get_priority_for_merge_nodes(4, state2);
    EXPECT_EQ(priority2, 3);
    delete state2;
}

TEST_F(IndependentSetProblemTest, MergeOperator) {
    auto state_one = new dynamic_bitset<>(5, 0b10100);
    auto state_two = new dynamic_bitset<>(5, 0b01010);
    auto merged_state = problem_instance->merge_operator(state_one, state_two);
    EXPECT_EQ(problem_instance->get_state_as_string(merged_state), "11110");
    delete state_one;
    delete state_two;
    delete merged_state;

    auto state_one2 = new dynamic_bitset<>(5, 0b10010);
    auto state_two2 = new dynamic_bitset<>(5, 0b01010);
    auto merged_state2 = problem_instance->merge_operator(state_one2, state_two2);
    EXPECT_EQ(problem_instance->get_state_as_string(merged_state2), "11010");
    delete state_one2;
    delete state_two2;
    delete merged_state2;

}

TEST_F(IndependentSetProblemTest, GetStateAsString) {
    auto state = new dynamic_bitset<>(5, 0b10100);
    string state_string = problem_instance->get_state_as_string(state);
    EXPECT_EQ(state_string, "10100");
    delete state;
}

TEST_F(IndependentSetProblemTest, GetStateCopy) {
    auto state = new dynamic_bitset<>(5, 0b10100);
    auto state_copy = problem_instance->get_state_copy(state);

    EXPECT_NE(state, state_copy);
    EXPECT_EQ(problem_instance->get_state_as_string(state_copy), "10100");

    state->set(4, false);
    EXPECT_NE(problem_instance->get_state_as_string(state), problem_instance->get_state_as_string(state_copy));
    delete state;
    delete state_copy;
}

TEST_F(IndependentSetProblemTest, GetFinalState) {
    auto final_state = problem_instance->get_final_state();

    EXPECT_EQ(problem_instance->get_state_as_string(final_state), "00000");
    delete final_state;
}

TEST_F(IndependentSetProblemTest, TestIsDDCreated) {
    dd_instance->create_decision_diagram(false);
    ASSERT_FALSE(dd_instance->get_decision_diagram()->structure.empty());
}

TEST_F(IndependentSetProblemTest, TestVerboseCreateDD) {
    ofstream out("createDDIndependentSet.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_decision_diagram(true);
    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createDDIndependentSet.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createDDIndependentSet.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(IndependentSetProblemTest, TestVerboseCreateReduceDD) {
    ofstream out("createReduceDDIndependentSet.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram(true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createReduceDDIndependentSet.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createReduceDDIndependentSet.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();


    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(IndependentSetProblemTest, TestVerboseCreateRestrictedDD) {
    ofstream out("createRestrictedDDIndependentSet.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_restricted_decision_diagram(2, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRestrictedDDIndependentSet.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRestrictedDDIndependentSet.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(IndependentSetProblemTest, TestVerboseCreateRelaxedDD) {
    ofstream out("createRelaxedDDIndependentSet.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_relax_priority_decision_diagram(2, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRelaxedDDIndependentSet.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRelaxedDDIndependentSet.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(IndependentSetProblemTest, TestCreateDDGraphEqual) {
    Graph<dynamic_bitset<>>* expected_graph = GetExactDDIndependentSet();
    dd_instance->create_decision_diagram(false);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(IndependentSetProblemTest, TestCreateReduceDDGraphEqual) {
    Graph<dynamic_bitset<>>* expected_graph = GetReduceDDIndependentSet();
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(IndependentSetProblemTest, TestCreateRestrictedDDGraphEqual) {
    Graph<dynamic_bitset<>>* expected_graph = GetRestrictedDDIndependentSet();
    dd_instance->create_restricted_decision_diagram(2);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(IndependentSetProblemTest, CompareTwoDifferentGraphs) {
    Graph<dynamic_bitset<>>* not_expected_graph = GetFalseDDIndependentSet();
    dd_instance->create_decision_diagram(false);

    ASSERT_FALSE(*dd_instance->get_decision_diagram() == *not_expected_graph);
    delete not_expected_graph;
}

TEST_F(IndependentSetProblemTest, TestCreateRelaxedDDGraphEqual) {
    Graph<dynamic_bitset<>>* expected_graph = GetRelaxedDDIndependentSet();
    dd_instance->create_relax_priority_decision_diagram(2);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(IndependentSetProblemTest, TestGetCopy) {
    dd_instance->create_decision_diagram(false);
    Graph<dynamic_bitset<>>* original_graph = dd_instance->get_decision_diagram();
    Graph<dynamic_bitset<>>* copied_graph = dd_instance->get_decision_diagram_copy();

    ASSERT_NE(&original_graph, &copied_graph); // Check if they are different objects
    ASSERT_TRUE(*original_graph==*copied_graph); // Check if they are equal in values

    original_graph->structure[0].front()->set_id(100);
    ASSERT_FALSE(original_graph->structure[0].front()->to_string() == copied_graph->structure[0].front()->to_string());

    Node<dynamic_bitset<>>* new_node = new Node<dynamic_bitset<>>(11, new dynamic_bitset<>(31));
    copied_graph->add_node(new_node);

    ASSERT_FALSE(original_graph->structure.back().size() == copied_graph->structure.back().size());
    delete copied_graph;
}

TEST_F(IndependentSetProblemTest, TestGetDDBuilderTime) {
    dd_instance->create_decision_diagram(false);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(IndependentSetProblemTest, TestGetReduceDDBuilderTime) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();
    ASSERT_GT(stof(dd_instance->get_reduction_time()), 0);
}

TEST_F(IndependentSetProblemTest, TestGetRestrictedDDBuilderTime) {
    dd_instance->create_restricted_decision_diagram(2);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(IndependentSetProblemTest, TestGetRelaxedDDBuilderTime) {
    dd_instance->create_relax_priority_decision_diagram(2);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(IndependentSetProblemTest, GetSolutionForDD) {
    dd_instance->create_decision_diagram(false);

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 11;
    string expected_path = " arc_0_1(0)-> arc_1_4(1)-> arc_4_7(0)-> arc_7_8(0)-> arc_8_10(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(IndependentSetProblemTest, GetSolutionForReduceDD) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 11;
    string expected_path = " arc_0_1(0)-> arc_1_4(1)-> arc_4_7(0)-> arc_7_8(0)-> arc_8_10(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(IndependentSetProblemTest, GetSolutionForRestrictedeDD) {
    dd_instance->create_restricted_decision_diagram(2, false);

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 11;
    string expected_path = " arc_0_1(0)-> arc_1_3(1)-> arc_3_5(0)-> arc_5_7(0)-> arc_7_9(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(IndependentSetProblemTest, GetSolutionForRelaxedeDD) {
    dd_instance->create_relax_priority_decision_diagram(2);

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 12;
    string expected_path = " arc_0_2(1)-> arc_2_4(0)-> arc_4_5(1)-> arc_5_7(0)-> arc_7_9(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(IndependentSetProblemTest, TestCompareGMLDDGraph) {
    dd_instance->create_decision_diagram(false);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/exact_dd_independent_set.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(IndependentSetProblemTest, TestCompareGMLReduceDDGraph) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/reduce_dd_independent_set.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(IndependentSetProblemTest, TestCompareGMLRestrictedDDGraph) {
    dd_instance->create_restricted_decision_diagram(2);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/restricted_dd_independent_set.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(IndependentSetProblemTest, TestCompareGMLRelaxedDDGraph) {
    dd_instance->create_relax_priority_decision_diagram(2);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/relax_dd_independent_set.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

