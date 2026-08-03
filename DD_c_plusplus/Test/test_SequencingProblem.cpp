#include <gtest/gtest.h>
#include "../../Examples/SequencingInstance/SequencingProblem.h"
#include "../../Examples/SequencingInstance/SequencingPathSolver.h"
#include "../../SourceCode/DD.h"
#include "dd_controller_generators/DDSequencing.cpp"

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <filesystem>

namespace fs = filesystem;
using namespace std;


class SequencingProblemTest : public ::testing::Test {
protected:
    void SetUp() override {
        source_directory = fs::current_path().parent_path().string();
        string test_file = source_directory + "/Test/txt_files/sequencing_test_instance.txt";

        sequencing_params = new SequencingInstance(test_file);
        problem_instance = new SequencingProblem(*sequencing_params);
        dd_instance = new DD<SequencingState>(*problem_instance);
    }

    void TearDown() override {
        delete problem_instance;
        delete dd_instance;
        delete sequencing_params;
    }

    PathStructureSolution<SequencingState> getSequencingSolution() {
        SequencingPathSolver solver(*dd_instance);
        solver.set_parameters({}, "min");
        return solver.solve();
    }

    SequencingInstance* sequencing_params;
    SequencingProblem* problem_instance;
    DD<SequencingState>* dd_instance;
    string source_directory;
};

TEST_F(SequencingProblemTest, TestOrderedVariables) {
    vector<string> expected = {"slot_0", "slot_1", "slot_2"};
    ASSERT_EQ(problem_instance->ordered_variables, expected);
}

TEST_F(SequencingProblemTest, TestVariablesDomain) {
    map<string, vector<int>> expected = {
        {"slot_0", {0, 1, 2}},
        {"slot_1", {0, 1, 2}},
        {"slot_2", {0, 1, 2}},
    };
    ASSERT_EQ(problem_instance->variables_domain, expected);
}

TEST_F(SequencingProblemTest, TransitionFunctionFeasible) {
    SequencingState* initial = new SequencingState{{}, -1};
    SequencingState* scratch_state = new SequencingState{{}, -1};
    bool feasible = problem_instance->transition_function(initial, 0, 0, scratch_state);
    // state no longer carries time: just (scheduled | {job}, last_job)
    EXPECT_EQ(scratch_state->scheduled, (set<int>{0}));
    EXPECT_EQ(scratch_state->last_job, 0);
    EXPECT_TRUE(feasible);
    delete initial;
    delete scratch_state;
}

TEST_F(SequencingProblemTest, TransitionFunctionInfeasible) {
    SequencingState* already_scheduled = new SequencingState{{0}, 0};
    SequencingState* scratch_state = new SequencingState{{}, -1};
    bool feasible = problem_instance->transition_function(already_scheduled, 1, 0, scratch_state);
    EXPECT_FALSE(feasible);
    delete already_scheduled;
    delete scratch_state;
}

TEST_F(SequencingProblemTest, TransitionFunctionJob2First) {
    SequencingState* initial = new SequencingState{{}, -1};
    SequencingState* scratch_state = new SequencingState{{}, -1};
    bool feasible = problem_instance->transition_function(initial, 0, 2, scratch_state);
    EXPECT_EQ(scratch_state->scheduled, (set<int>{2}));
    EXPECT_EQ(scratch_state->last_job, 2);
    EXPECT_TRUE(feasible);
    delete initial;
    delete scratch_state;
}

TEST_F(SequencingProblemTest, GetPriorityForDiscardNode) {
    // priority = sum of processing times of scheduled jobs (p=[2,3,1])
    SequencingState* state_high = new SequencingState{{1}, 1};
    EXPECT_EQ(problem_instance->get_priority_for_discard_node(state_high), 3);
    delete state_high;

    SequencingState* state_low = new SequencingState{{0}, 0};
    EXPECT_EQ(problem_instance->get_priority_for_discard_node(state_low), 2);
    delete state_low;
}

TEST_F(SequencingProblemTest, GetPriorityForMergeNodes) {
    SequencingState* state_a = new SequencingState{{0}, 0};
    EXPECT_EQ(problem_instance->get_priority_for_merge_nodes(1, state_a), 2);
    delete state_a;

    SequencingState* state_b = new SequencingState{{1}, 1};
    EXPECT_EQ(problem_instance->get_priority_for_merge_nodes(2, state_b), 3);
    delete state_b;
}

TEST_F(SequencingProblemTest, MergeOperatorKeepsLeastCommittedLastJob) {
    // equal committed processing -> keep first state's last_job
    SequencingState* s1 = new SequencingState{{1, 2}, 2};
    SequencingState* s2 = new SequencingState{{1, 2}, 1};
    SequencingState* merged = problem_instance->merge_operator(s1, s2);
    EXPECT_EQ(merged->scheduled, (set<int>{1, 2}));
    EXPECT_EQ(merged->last_job, 2);
    delete s1;
    delete s2;
    delete merged;
}

TEST_F(SequencingProblemTest, MergeOperatorIntersectsScheduled) {
    SequencingState* s1 = new SequencingState{{0, 1}, 1};
    SequencingState* s2 = new SequencingState{{0, 1}, 0};
    SequencingState* merged = problem_instance->merge_operator(s1, s2);
    EXPECT_EQ(merged->scheduled, (set<int>{0, 1}));
    delete s1;
    delete s2;
    delete merged;
}

TEST_F(SequencingProblemTest, GetStateAsString) {
    SequencingState* state = new SequencingState{{0}, 0};
    string result = problem_instance->get_state_as_string(state);
    EXPECT_EQ(typeid(result).name(), typeid(string).name());
    EXPECT_EQ(result, "[0]_0");
    delete state;
}

TEST_F(SequencingProblemTest, GetStateAsStringEmpty) {
    SequencingState* state = new SequencingState{{}, -1};
    EXPECT_EQ(problem_instance->get_state_as_string(state), "[]_-1");
    delete state;
}

TEST_F(SequencingProblemTest, GetStateCopy) {
    SequencingState* state = new SequencingState{{0}, 0};
    SequencingState* copy = problem_instance->get_state_copy(state);
    EXPECT_NE(state, copy);
    EXPECT_EQ(*state, *copy);

    state->last_job = 99;
    EXPECT_NE(state->last_job, copy->last_job);
    delete state;
    delete copy;
}

TEST_F(SequencingProblemTest, GetFinalState) {
    SequencingState* final_state = problem_instance->get_final_state();
    EXPECT_EQ(final_state->scheduled, set<int>{});
    EXPECT_EQ(final_state->last_job, -1);
    delete final_state;
}

TEST_F(SequencingProblemTest, TestIsDDCreated) {
    dd_instance->create_decision_diagram(false);
    ASSERT_FALSE(dd_instance->get_decision_diagram()->structure.empty());
}

TEST_F(SequencingProblemTest, TestVerboseCreateDD) {
    ofstream out("createDDSequencing.txt");
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_decision_diagram(true);
    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createDDSequencing.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createDDSequencing.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SequencingProblemTest, TestVerboseCreateReduceDD) {
    ofstream out("createReduceDDSequencing.txt");
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram(true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createReduceDDSequencing.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createReduceDDSequencing.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SequencingProblemTest, TestVerboseCreateRestrictedDD) {
    ofstream out("createRestrictedDDSequencing.txt");
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_restricted_decision_diagram(3, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRestrictedDDSequencing.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRestrictedDDSequencing.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SequencingProblemTest, TestVerboseCreateRelaxedDD) {
    ofstream out("createRelaxedDDSequencing.txt");
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_relax_priority_decision_diagram(3, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRelaxedDDSequencing.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRelaxedDDSequencing.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SequencingProblemTest, TestCreateDDGraphEqual) {
    Graph<SequencingState>* expected_graph = GetExactDDSequencing();
    dd_instance->create_decision_diagram(false);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(SequencingProblemTest, TestCreateReduceDDGraphEqual) {
    Graph<SequencingState>* expected_graph = GetReduceDDSequencing();
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(SequencingProblemTest, TestCreateRestrictedDDGraphEqual) {
    Graph<SequencingState>* expected_graph = GetRestrictedDDSequencing();
    dd_instance->create_restricted_decision_diagram(3);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(SequencingProblemTest, TestCreateRelaxedDDGraphEqual) {
    Graph<SequencingState>* expected_graph = GetRelaxedDDSequencing();
    dd_instance->create_relax_priority_decision_diagram(3);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(SequencingProblemTest, CompareTwoDifferentGraphs) {
    Graph<SequencingState>* not_expected_graph = GetFalseDDSequencing();
    dd_instance->create_decision_diagram(false);

    ASSERT_FALSE(*dd_instance->get_decision_diagram() == *not_expected_graph);
    delete not_expected_graph;
}

TEST_F(SequencingProblemTest, TestGetCopy) {
    dd_instance->create_decision_diagram(false);
    Graph<SequencingState>* original_graph = dd_instance->get_decision_diagram();
    Graph<SequencingState>* copied_graph = dd_instance->get_decision_diagram_copy();

    ASSERT_NE(&original_graph, &copied_graph);
    ASSERT_TRUE(*original_graph == *copied_graph);

    original_graph->structure[0].front()->set_id(100);
    ASSERT_FALSE(original_graph->structure[0].front()->to_string() == copied_graph->structure[0].front()->to_string());

    Node<SequencingState>* new_node = new Node<SequencingState>(99, new SequencingState{{0, 1, 2}, 2});
    copied_graph->add_node(new_node);

    ASSERT_FALSE(original_graph->structure.back().size() == copied_graph->structure.back().size());
    delete copied_graph;
}

TEST_F(SequencingProblemTest, TestGetDDBuilderTime) {
    dd_instance->create_decision_diagram(false);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(SequencingProblemTest, TestGetReduceDDBuilderTime) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();
    ASSERT_GT(stof(dd_instance->get_reduction_time()), 0);
}

TEST_F(SequencingProblemTest, TestGetRestrictedDDBuilderTime) {
    dd_instance->create_restricted_decision_diagram(3);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(SequencingProblemTest, TestGetRelaxedDDBuilderTime) {
    dd_instance->create_relax_priority_decision_diagram(3, true);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(SequencingProblemTest, GetSolutionForDD) {
    dd_instance->create_decision_diagram(false);

    PathStructureSolution<SequencingState> solution = getSequencingSolution();
    double expected_value = 31;
    string expected_path = " arc_0_3(2)-> arc_3_8(0)-> arc_8_10(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(SequencingProblemTest, GetSolutionForReduceDD) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    PathStructureSolution<SequencingState> solution = getSequencingSolution();
    double expected_value = 31;
    string expected_path = " arc_0_3(2)-> arc_3_5(0)-> arc_5_7(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(SequencingProblemTest, GetSolutionForRestrictedDD) {
    dd_instance->create_restricted_decision_diagram(3);

    PathStructureSolution<SequencingState> solution = getSequencingSolution();
    double expected_value = 31;
    string expected_path = " arc_0_3(2)-> arc_3_6(0)-> arc_6_7(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(SequencingProblemTest, GetSolutionForRelaxedDD) {
    dd_instance->create_relax_priority_decision_diagram(3);

    PathStructureSolution<SequencingState> solution = getSequencingSolution();
    // Relaxed DD gives a lower bound (<= optimum 31). Dropping the time from the
    // state changes which nodes are merged, hence a different (tighter) bound.
    double expected_value = 30;
    string expected_path = " arc_0_3(2)-> arc_3_4(1)-> arc_4_7(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(SequencingProblemTest, TestCompareGMLDDGraph) {
    dd_instance->create_decision_diagram(false);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/exact_dd_sequencing.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SequencingProblemTest, TestCompareGMLReduceDDGraph) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/reduce_dd_sequencing.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SequencingProblemTest, TestCompareGMLRestrictedDDGraph) {
    dd_instance->create_restricted_decision_diagram(3);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/restricted_dd_sequencing.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SequencingProblemTest, TestCompareGMLRelaxedDDGraph) {
    dd_instance->create_relax_priority_decision_diagram(3);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/relax_dd_sequencing.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output == expected_output);
}
