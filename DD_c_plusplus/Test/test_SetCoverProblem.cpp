#include <gtest/gtest.h>
#include "../../Examples/SetCoverInstance/SetCoverProblem.h"
#include "../../SourceCode/DD.h"
#include "GraphAlgorithms/ShortestLongestPath/ShortestLongestPath.h"
#include "dd_controller_generators/DDSetCover.cpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>

using namespace std;

class SetCoverProblemTest : public ::testing::Test {
protected:
    void SetUp() override {
        vector<vector<int>> matrix_of_wheight = {{1, 1, 1, 0, 0, 0},
                                                 {1, 0, 0, 1, 1, 0},
                                                 {0, 1, 0, 1, 0, 1}};
        vector<int> right_side_of_restrictions = {1, 1, 1};

        vector<pair<string, vector<int>>> variables = {
                make_pair("conjunto_A", vector<int>{0, 1}),
                make_pair("conjunto_B", vector<int>{0, 1}),
                make_pair("conjunto_C", vector<int>{0, 1}),
                make_pair("conjunto_D", vector<int>{0, 1}),
                make_pair("conjunto_E", vector<int>{0, 1}),
                make_pair("conjunto_F", vector<int>{0, 1})
        };

        set_cover_params = new SetCoverInstance(new dynamic_bitset<>(3, 0b111), variables, matrix_of_wheight, right_side_of_restrictions);
        problem_instance = new SetCoverProblem(*set_cover_params, false);
        dd_instance = new DD<dynamic_bitset<>>(*problem_instance);

        source_directory = fs::current_path().parent_path().string();
    }

    void TearDown() override {
        delete problem_instance;
        delete dd_instance;
        delete set_cover_params;
    }

    PathStructureSolution<dynamic_bitset<>> getLinearDpSolution() {
        vector<double> objective_weights = {2, 1, 4, 3, 4, 3};
        ShortestLongestPath objective_function_instance = ShortestLongestPath<dynamic_bitset<>>(*dd_instance);
        objective_function_instance.set_parameters(objective_weights, "min");

        return objective_function_instance.solve();
    }
    SetCoverInstance* set_cover_params;
    SetCoverProblem* problem_instance;
    DD<dynamic_bitset<>>* dd_instance;
    string source_directory;
};


TEST_F(SetCoverProblemTest, TestOrderedVariables) {
    vector<string> ordered_variables_test = {"conjunto_A", "conjunto_B", "conjunto_C", "conjunto_D", "conjunto_E", "conjunto_F"};
    ASSERT_EQ(problem_instance->ordered_variables, ordered_variables_test);
}

TEST_F(SetCoverProblemTest, TestVariablesDomain) {
    map<string, vector<int>> variables_domain_test = {
            {"conjunto_A", {0, 1}},
            {"conjunto_B", {0, 1}},
            {"conjunto_C", {0, 1}},
            {"conjunto_D", {0, 1}},
            {"conjunto_E", {0, 1}},
            {"conjunto_F", {0, 1}}
    };
    ASSERT_EQ(problem_instance->variables_domain, variables_domain_test);

}

TEST_F(SetCoverProblemTest, TransitionFunction) {
    auto previous_state = new dynamic_bitset<>(3, 0b111);
    auto scratch_state = new dynamic_bitset<>(3);
    bool feasibility = problem_instance->transition_function(previous_state, 0, 1, scratch_state);

    EXPECT_EQ(*scratch_state, dynamic_bitset<>(3, 0b100));
    EXPECT_TRUE(feasibility);
    delete previous_state;
    delete scratch_state;
}

TEST_F(SetCoverProblemTest, GetPriorityForDiscardNode) {
    auto state = new dynamic_bitset<>(5, 0b01100);
    int priority = problem_instance->get_priority_for_discard_node(state);
    EXPECT_EQ(priority, 2);
    delete state;


    auto state2 = new dynamic_bitset<>(5, 0b01110);
    int priority2 = problem_instance->get_priority_for_discard_node(state2);
    EXPECT_EQ(priority2, 3);
    delete state2;

}

TEST_F(SetCoverProblemTest, GetPriorityForMergeNodes) {
    auto state = new dynamic_bitset<>(5, 0b01100);
    int priority = problem_instance->get_priority_for_merge_nodes(2, state);
    EXPECT_EQ(priority, 2);
    delete state;

    auto state2 = new dynamic_bitset<>(5, 0b01110);
    int priority2 = problem_instance->get_priority_for_merge_nodes(4, state2);
    EXPECT_EQ(priority2, 3);
    delete state2;

}

TEST_F(SetCoverProblemTest, MergeOperator) {
    auto state_one = new dynamic_bitset<>(5, 0b10100);
    auto state_two = new dynamic_bitset<>(5, 0b01010);
    auto merged_state = problem_instance->merge_operator(state_one, state_two);
    EXPECT_EQ(problem_instance->get_state_as_string(merged_state), "00000");
    delete state_one;
    delete state_two;
    delete merged_state;

    auto state_one2 = new dynamic_bitset<>(5, 0b10010);
    auto state_two2 = new dynamic_bitset<>(5, 0b01010);
    auto merged_state2 = problem_instance->merge_operator(state_one2, state_two2);
    EXPECT_EQ(problem_instance->get_state_as_string(merged_state2), "00010");
    delete state_one2;
    delete state_two2;
    delete merged_state2;

}

TEST_F(SetCoverProblemTest, GetStateAsString) {
    auto state = new dynamic_bitset<>(5, 0b10100);
    string state_string = problem_instance->get_state_as_string(state);
    EXPECT_EQ(state_string, "10100");
    delete state;

}

TEST_F(SetCoverProblemTest, GetStateCopy) {
    auto state = new dynamic_bitset<>(5, 0b10100);
    auto state_copy = problem_instance->get_state_copy(state);

    EXPECT_NE(state, state_copy);
    EXPECT_EQ(problem_instance->get_state_as_string(state_copy), "10100");

    state->set(4, false);
    EXPECT_NE(problem_instance->get_state_as_string(state), problem_instance->get_state_as_string(state_copy));
    delete state;
    delete state_copy;

}

TEST_F(SetCoverProblemTest, GetFinalState) {
    auto final_state = problem_instance->get_final_state();

    EXPECT_EQ(problem_instance->get_state_as_string(final_state), "000");
    delete final_state;

}


TEST_F(SetCoverProblemTest, TestIsDDCreated) {
    dd_instance->create_decision_diagram(false);
    ASSERT_FALSE(dd_instance->get_decision_diagram()->structure.empty());
}

TEST_F(SetCoverProblemTest, TestVerboseCreateDD) {
    ofstream out("createDDSetCover.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_decision_diagram(true);
    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createDDSetCover.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createDDSetCover.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(SetCoverProblemTest, TestVerboseCreateReduceDD) {
    ofstream out("createReduceDDSetCover.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram(true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createReduceDDSetCover.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createReduceDDSetCover.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(SetCoverProblemTest, TestVerboseCreateRestrictedDD) {
    ofstream out("createRestrictedDDSetCover.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_restricted_decision_diagram(3, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRestrictedDDSetCover.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRestrictedDDSetCover.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(SetCoverProblemTest, TestVerboseCreateRelaxedDD) {
    ofstream out("createRelaxedDDSetCover.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_relax_priority_decision_diagram(3, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRelaxedDDSetCover.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRelaxedDDSetCover.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(SetCoverProblemTest, TestCreateDDGraphEqual) {
    Graph<dynamic_bitset<>>* expected_graph = GetExactDDSetCover();
    dd_instance->create_decision_diagram(false);
    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(SetCoverProblemTest, TestCreateReduceDDGraphEqual) {
    Graph<dynamic_bitset<>>* expected_graph = GetReduceDDSetCover();
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(SetCoverProblemTest, TestCreateRestrictedDDGraphEqual) {
    Graph<dynamic_bitset<>>* expected_graph = GetRestrictedDDSetCover();
    dd_instance->create_restricted_decision_diagram(3);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(SetCoverProblemTest, CompareTwoDifferentGraphs) {
    dd_instance->create_decision_diagram(false);
    Graph<dynamic_bitset<>>* not_expected_graph = GetFalseDDSetCover();

    ASSERT_FALSE(*dd_instance->get_decision_diagram() == *not_expected_graph);
    delete not_expected_graph;
}

TEST_F(SetCoverProblemTest, TestCreateRelaxedDDGraphEqual) {
    Graph<dynamic_bitset<>>* expected_graph = GetRelaxedDDSetCover();
    dd_instance->create_relax_priority_decision_diagram(3);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(SetCoverProblemTest, TestGetCopy) {
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

TEST_F(SetCoverProblemTest, TestGetDDBuilderTime) {
    dd_instance->create_decision_diagram(false);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(SetCoverProblemTest, TestGetReduceDDBuilderTime) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();
    ASSERT_GT(stof(dd_instance->get_reduction_time()), 0);
}

TEST_F(SetCoverProblemTest, TestGetRestrictedDDBuilderTime) {
    dd_instance->create_restricted_decision_diagram(2);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(SetCoverProblemTest, TestGetRelaxedDDBuilderTime) {
    dd_instance->create_relax_priority_decision_diagram(2);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(SetCoverProblemTest, GetSolutionForDD) {
    dd_instance->create_decision_diagram(false);

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 3;
    string expected_path = " arc_0_2(1)-> arc_2_6(1)-> arc_6_10(0)-> arc_10_12(0)-> arc_12_16(0)-> arc_16_17(0)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(SetCoverProblemTest, GetSolutionForReduceDD) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 3;
    string expected_path = " arc_0_2(1)-> arc_2_6(1)-> arc_6_10(0)-> arc_10_12(0)-> arc_12_16(0)-> arc_16_17(0)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(SetCoverProblemTest, GetSolutionForRestrictedeDD) {
    dd_instance->create_restricted_decision_diagram(3, false);

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 3;
    string expected_path = " arc_0_2(1)-> arc_2_5(1)-> arc_5_8(0)-> arc_8_10(0)-> arc_10_12(0)-> arc_12_14(0)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(SetCoverProblemTest, GetSolutionForRelaxedeDD) {
    dd_instance->create_relax_priority_decision_diagram(3);

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 3;
    string expected_path = " arc_0_1(0)-> arc_1_3(0)-> arc_3_6(0)-> arc_6_10(1)-> arc_10_12(0)-> arc_12_14(0)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(SetCoverProblemTest, TestCompareGMLDDGraph) {
    dd_instance->create_decision_diagram(false);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/exact_dd_set_cover.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(SetCoverProblemTest, TestCompareGMLReduceDDGraph) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/reduce_dd_set_cover.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(SetCoverProblemTest, TestCompareGMLRestrictedDDGraph) {
    dd_instance->create_restricted_decision_diagram(3);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/restricted_dd_set_cover.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(SetCoverProblemTest, TestCompareGMLRelaxedDDGraph) {
    dd_instance->create_relax_priority_decision_diagram(3);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/relax_dd_set_cover.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}
