#include <gtest/gtest.h>
#include "../../Examples/KnapsackInstance/KnapsackProblem.h"
#include "../../SourceCode/DD.h"
#include "GraphAlgorithms/ShortestLongestPath/ShortestLongestPath.h"
#include "dd_controller_generators/DDKnapsack.cpp"

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <filesystem>


namespace fs = filesystem;
using namespace std;


class KnapsackProblemTest : public ::testing::Test {
protected:
    void SetUp() override {
        vector<pair<string, vector<int>>> variables = {
                make_pair("item_oro", vector<int>{0, 1}),
                make_pair("item_plata", vector<int>{0, 1}),
                make_pair("item_bronce", vector<int>{0, 1}),
                make_pair("item_hierro", vector<int>{0, 1})
        };

        vector<int> matrix_of_weight = {3, 3, 4, 6};
        int right_side_of_restrictions = 6;

        knapsack_params = new KnapsackInstance(new int(0), variables, matrix_of_weight, {}, right_side_of_restrictions);
        problem_instance = new KnapsackProblem(*knapsack_params, false);
        dd_instance = new DD(*problem_instance);

        source_directory = fs::current_path().parent_path().string();
    }

    void TearDown() override {
        delete problem_instance;
        delete dd_instance;
        delete knapsack_params;
    }

    PathStructureSolution<int> getLinearDpSolution() {
        vector<double> objective_weights = {-5, 1, 18, 15};
        ShortestLongestPath objective_function_instance = ShortestLongestPath<int>(*dd_instance);
        objective_function_instance.set_parameters(objective_weights, "max");


        return objective_function_instance.solve();
    }

    KnapsackInstance* knapsack_params;
    KnapsackProblem* problem_instance;
    DD<int>* dd_instance;
    string source_directory;
};

TEST_F(KnapsackProblemTest, TestOrderedVariables) {
    vector<string> ordered_variables_test = {"item_oro", "item_plata", "item_bronce", "item_hierro"};
    ASSERT_EQ(problem_instance->ordered_variables, ordered_variables_test);
}

TEST_F(KnapsackProblemTest, TestVariablesDomain) {
    map<string, vector<int>> variables_domain_test = {
            {"item_oro",    {0, 1}},
            {"item_plata",  {0, 1}},
            {"item_bronce", {0, 1}},
            {"item_hierro", {0, 1}}
    };
    ASSERT_EQ(problem_instance->variables_domain, variables_domain_test);

}

TEST_F(KnapsackProblemTest, TransitionFunction) {
    int* inter_state = new int(0);
    int* scratch_state = new int(0);
    bool feasibility = problem_instance->transition_function(inter_state, 0, 1, scratch_state);
    EXPECT_EQ(*scratch_state, int(3));
    EXPECT_TRUE(feasibility);
    delete inter_state;

    int* inter_state_2 = new int(4);
    feasibility = problem_instance->transition_function(inter_state_2, 2, 1, scratch_state);
    EXPECT_EQ(*scratch_state, int(8));
    EXPECT_FALSE(feasibility);
    delete inter_state_2;
    delete scratch_state;
}

TEST_F(KnapsackProblemTest, GetPriorityForDiscardNode) {
    auto state = new int(4);
    int priority = problem_instance->get_priority_for_discard_node(state);
    EXPECT_EQ(priority, -4);
    delete state; 

    state = new int(1);
    priority = problem_instance->get_priority_for_discard_node(state);
    EXPECT_EQ(priority, -1);
    delete state; 

}

TEST_F(KnapsackProblemTest, GetPriorityForMergeNodes) {
    auto state = new int(1);
    int priority = problem_instance->get_priority_for_merge_nodes(1, state);
    EXPECT_EQ(priority, -1);
    delete state; 

    state = new int(4);
    priority = problem_instance->get_priority_for_merge_nodes(3, state);
    EXPECT_EQ(priority, -4);
    delete state;

}

TEST_F(KnapsackProblemTest, MergeOperator) {
    auto state1 = new int(2);
    auto state2 = new int(1);
    auto result = problem_instance->merge_operator(state1, state2);
    EXPECT_EQ(*result, int(1));
    delete state1; 
    delete state2; 
    delete result; 

    state1 = new int(7);
    state2 = new int(6);
    result = problem_instance->merge_operator(state1, state2);
    EXPECT_EQ(*result, int(6));
    delete state1; 
    delete state2; 
    delete result; 

}

TEST_F(KnapsackProblemTest, GetStateAsString) {
    auto state = new int(1);
    string state_string = problem_instance->get_state_as_string(state);
    EXPECT_EQ(typeid(state_string).name(), typeid(string).name());
    EXPECT_EQ(state_string, "1");
    delete state; 

}

TEST_F(KnapsackProblemTest, GetStateCopy) {
    auto state = new int(1);
    auto state_copy = problem_instance->get_state_copy(state);
    EXPECT_NE(state, state_copy);
    EXPECT_EQ(*state, *state_copy);

    *state = 10;
    EXPECT_NE(*state, *state_copy);
    delete state;
    delete state_copy;

}

TEST_F(KnapsackProblemTest, GetFinalState) {
    auto final_state = problem_instance->get_final_state();
    EXPECT_EQ(*final_state, int(0));
    delete final_state;

}


TEST_F(KnapsackProblemTest, TestIsDDCreated) {
    dd_instance->create_decision_diagram(false);
    ASSERT_FALSE(dd_instance->get_decision_diagram()->structure.empty());
}

TEST_F(KnapsackProblemTest, TestVerboseCreateDD) {
    ofstream out("createDDKnapsack.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_decision_diagram(true);
    cout.rdbuf(coutbuf);
    string full_file_path = source_directory + "/Test/txt_files/createDDKnapsack.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createDDKnapsack.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();
    cout << actual_output << endl;
    cout << "here" << endl;
    cout << expected_output << endl;

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackProblemTest, TestVerboseCreateReduceDD) {
    ofstream out("createReduceDDKnapsack.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram(true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createReduceDDKnapsack.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createReduceDDKnapsack.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackProblemTest, TestVerboseCreateRestrictedDD) {
    ofstream out("createRestrictedDDKnapsack.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_restricted_decision_diagram(3, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRestrictedDDKnapsack.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRestrictedDDKnapsack.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackProblemTest, TestVerboseCreateRelaxedDD) {
    ofstream out("createRelaxedDDKnapsack.txt");
    streambuf *coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_relax_priority_decision_diagram(3, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRelaxedDDKnapsack.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRelaxedDDKnapsack.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackProblemTest, TestCreateDDGraphEqual) {
    Graph<int>* expected_graph = GetExactDDKnapsack();
    dd_instance->create_decision_diagram(false);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(KnapsackProblemTest, TestCreateReduceDDGraphEqual) {
    Graph<int>* expected_graph = GetReduceDDKnapsack();
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(KnapsackProblemTest, TestCreateRestrictedDDGraphEqual) {
    Graph<int>* expected_graph = GetRestrictedDDKnapsack();
    dd_instance->create_restricted_decision_diagram(3);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}   

TEST_F(KnapsackProblemTest, CompareTwoDifferentGraphs) {
    Graph<int>* not_expected_graph = GetFalseDDKnapsack();
    dd_instance->create_decision_diagram(false);

    ASSERT_FALSE(*dd_instance->get_decision_diagram() == *not_expected_graph);
    delete not_expected_graph;
}

TEST_F(KnapsackProblemTest, TestCreateRelaxedDDGraphEqual) {
    Graph<int>* expected_graph = GetRelaxedDDKnapsack();
    dd_instance->create_relax_priority_decision_diagram(3);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}


TEST_F(KnapsackProblemTest, TestGetCopy) {
    dd_instance->create_decision_diagram(false);
    Graph<int>* original_graph = dd_instance->get_decision_diagram();
    Graph<int>* copied_graph = dd_instance->get_decision_diagram_copy();


    ASSERT_NE(&original_graph, &copied_graph); // Check if they are different objects
    ASSERT_TRUE(*original_graph==*copied_graph); // Check if they are equal in values

    original_graph->structure[0].front()->set_id(100);
    ASSERT_FALSE(original_graph->structure[0].front()->to_string() == copied_graph->structure[0].front()->to_string());

    Node<int>* new_node = new Node<int>(11, new int(100));
    copied_graph->add_node(new_node);

    ASSERT_FALSE(original_graph->structure.back().size() == copied_graph->structure.back().size());
    delete copied_graph;
}


TEST_F(KnapsackProblemTest, TestGetDDBuilderTime) {
    dd_instance->create_decision_diagram(false);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(KnapsackProblemTest, TestGetReduceDDBuilderTime) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();
    ASSERT_GT(stof(dd_instance->get_reduction_time()), 0);
}

TEST_F(KnapsackProblemTest, TestGetRestrictedDDBuilderTime) {
    dd_instance->create_restricted_decision_diagram(3);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(KnapsackProblemTest, TestGetRelaxedDDBuilderTime) {
    dd_instance->create_relax_priority_decision_diagram(3, true);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(KnapsackProblemTest, GetSolutionForDD) {
    dd_instance->create_decision_diagram(false);

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 18;
    string expected_path = " arc_0_1(0)-> arc_1_3(0)-> arc_3_7(1)-> arc_7_10(0)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(KnapsackProblemTest, GetSolutionForReduceDD) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 18;
    string expected_path = " arc_0_1(0)-> arc_1_3(0)-> arc_3_6(1)-> arc_6_7(0)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(KnapsackProblemTest, GetSolutionForRestrictedeDD) {
    dd_instance->create_restricted_decision_diagram(3);

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 18;
    string expected_path = " arc_0_1(0)-> arc_1_3(0)-> arc_3_6(1)-> arc_6_9(0)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(KnapsackProblemTest, GetSolutionForRelaxedeDD) {
    dd_instance->create_relax_priority_decision_diagram(3);

    PathStructureSolution solution = getLinearDpSolution();
    int expected_value = 18;
    string expected_path = " arc_0_1(0)-> arc_1_3(0)-> arc_3_6(1)-> arc_6_9(0)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(KnapsackProblemTest, TestCompareGMLDDGraph) {
    dd_instance->create_decision_diagram(false);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/exact_dd_knapsack.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackProblemTest, TestCompareGMLReduceDDGraph) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/reduce_dd_knapsack.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackProblemTest, TestCompareGMLRestrictedDDGraph) {
    dd_instance->create_restricted_decision_diagram(3);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/restricted_dd_knapsack.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}

TEST_F(KnapsackProblemTest, TestCompareGMLRelaxedDDGraph) {
    dd_instance->create_relax_priority_decision_diagram(3);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/relax_dd_knapsack.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output==expected_output);
}
