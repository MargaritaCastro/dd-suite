#include <gtest/gtest.h>
#include "../../Examples/SchedulerInstance/SchedulerProblem.h"
#include "../../Examples/SchedulerInstance/SchedulerPathSolver.h"
#include "../../SourceCode/DD.h"
#include "dd_controller_generators/DDScheduler.cpp"

#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <map>
#include <memory>
#include <filesystem>

namespace fs = filesystem;
using namespace std;


class SchedulerProblemTest : public ::testing::Test {
protected:
    void SetUp() override {
        source_directory = fs::current_path().parent_path().string();
        string test_file = source_directory + "/Test/txt_files/scheduler_test_instance.txt";

        scheduler_params = new SchedulerInstance(test_file);
        problem_instance = new SchedulerProblem(*scheduler_params);
        dd_instance = new DD<SchedulerState>(*problem_instance);
    }

    void TearDown() override {
        delete problem_instance;
        delete dd_instance;
        delete scheduler_params;
    }

    PathStructureSolution<SchedulerState> getSchedulerSolution() {
        SchedulerPathSolver solver(*dd_instance);
        solver.set_parameters({}, "min");
        return solver.solve();
    }

    SchedulerInstance* scheduler_params;
    SchedulerProblem* problem_instance;
    DD<SchedulerState>* dd_instance;
    string source_directory;
};

TEST_F(SchedulerProblemTest, TestOrderedVariables) {
    vector<string> expected = {"slot_0", "slot_1", "slot_2"};
    ASSERT_EQ(problem_instance->ordered_variables, expected);
}

TEST_F(SchedulerProblemTest, TestVariablesDomain) {
    map<string, vector<int>> expected = {
        {"slot_0", {0, 1, 2}},
        {"slot_1", {0, 1, 2}},
        {"slot_2", {0, 1, 2}},
    };
    ASSERT_EQ(problem_instance->variables_domain, expected);
}

TEST_F(SchedulerProblemTest, TransitionFunctionFeasible) {
    SchedulerState* initial = new SchedulerState{{}, -1};
    SchedulerState* scratch_state = new SchedulerState{{}, -1};
    bool feasible = problem_instance->transition_function(initial, 0, 0, scratch_state);
    // state no longer carries time: just (scheduled | {job}, last_job)
    EXPECT_EQ(scratch_state->scheduled, (set<int>{0}));
    EXPECT_EQ(scratch_state->last_job, 0);
    EXPECT_TRUE(feasible);
    delete initial;
    delete scratch_state;
}

TEST_F(SchedulerProblemTest, TransitionFunctionInfeasible) {
    SchedulerState* already_scheduled = new SchedulerState{{0}, 0};
    SchedulerState* scratch_state = new SchedulerState{{}, -1};
    bool feasible = problem_instance->transition_function(already_scheduled, 1, 0, scratch_state);
    EXPECT_FALSE(feasible);
    delete already_scheduled;
    delete scratch_state;
}

TEST_F(SchedulerProblemTest, TransitionFunctionJob2First) {
    SchedulerState* initial = new SchedulerState{{}, -1};
    SchedulerState* scratch_state = new SchedulerState{{}, -1};
    bool feasible = problem_instance->transition_function(initial, 0, 2, scratch_state);
    EXPECT_EQ(scratch_state->scheduled, (set<int>{2}));
    EXPECT_EQ(scratch_state->last_job, 2);
    EXPECT_TRUE(feasible);
    delete initial;
    delete scratch_state;
}

TEST_F(SchedulerProblemTest, GetPriorityForDiscardNode) {
    // priority = sum of processing times of scheduled jobs (p=[2,3,1])
    SchedulerState* state_high = new SchedulerState{{1}, 1};
    EXPECT_EQ(problem_instance->get_priority_for_discard_node(state_high), 3);
    delete state_high;

    SchedulerState* state_low = new SchedulerState{{0}, 0};
    EXPECT_EQ(problem_instance->get_priority_for_discard_node(state_low), 2);
    delete state_low;
}

TEST_F(SchedulerProblemTest, GetPriorityForMergeNodes) {
    SchedulerState* state_a = new SchedulerState{{0}, 0};
    EXPECT_EQ(problem_instance->get_priority_for_merge_nodes(1, state_a), 2);
    delete state_a;

    SchedulerState* state_b = new SchedulerState{{1}, 1};
    EXPECT_EQ(problem_instance->get_priority_for_merge_nodes(2, state_b), 3);
    delete state_b;
}

TEST_F(SchedulerProblemTest, MergeOperatorKeepsLeastCommittedLastJob) {
    // equal committed processing -> keep first state's last_job
    SchedulerState* s1 = new SchedulerState{{1, 2}, 2};
    SchedulerState* s2 = new SchedulerState{{1, 2}, 1};
    SchedulerState* merged = problem_instance->merge_operator(s1, s2);
    EXPECT_EQ(merged->scheduled, (set<int>{1, 2}));
    EXPECT_EQ(merged->last_job, 2);
    delete s1;
    delete s2;
    delete merged;
}

TEST_F(SchedulerProblemTest, MergeOperatorIntersectsScheduled) {
    SchedulerState* s1 = new SchedulerState{{0, 1}, 1};
    SchedulerState* s2 = new SchedulerState{{0, 1}, 0};
    SchedulerState* merged = problem_instance->merge_operator(s1, s2);
    EXPECT_EQ(merged->scheduled, (set<int>{0, 1}));
    delete s1;
    delete s2;
    delete merged;
}

TEST_F(SchedulerProblemTest, GetStateAsString) {
    SchedulerState* state = new SchedulerState{{0}, 0};
    string result = problem_instance->get_state_as_string(state);
    EXPECT_EQ(typeid(result).name(), typeid(string).name());
    EXPECT_EQ(result, "[0]_0");
    delete state;
}

TEST_F(SchedulerProblemTest, GetStateAsStringEmpty) {
    SchedulerState* state = new SchedulerState{{}, -1};
    EXPECT_EQ(problem_instance->get_state_as_string(state), "[]_-1");
    delete state;
}

TEST_F(SchedulerProblemTest, GetStateCopy) {
    SchedulerState* state = new SchedulerState{{0}, 0};
    SchedulerState* copy = problem_instance->get_state_copy(state);
    EXPECT_NE(state, copy);
    EXPECT_EQ(*state, *copy);

    state->last_job = 99;
    EXPECT_NE(state->last_job, copy->last_job);
    delete state;
    delete copy;
}

TEST_F(SchedulerProblemTest, GetFinalState) {
    SchedulerState* final_state = problem_instance->get_final_state();
    EXPECT_EQ(final_state->scheduled, set<int>{});
    EXPECT_EQ(final_state->last_job, -1);
    delete final_state;
}

TEST_F(SchedulerProblemTest, TestIsDDCreated) {
    dd_instance->create_decision_diagram(false);
    ASSERT_FALSE(dd_instance->get_decision_diagram()->structure.empty());
}

TEST_F(SchedulerProblemTest, TestVerboseCreateDD) {
    ofstream out("createDDScheduler.txt");
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_decision_diagram(true);
    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createDDScheduler.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createDDScheduler.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SchedulerProblemTest, TestVerboseCreateReduceDD) {
    ofstream out("createReduceDDScheduler.txt");
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram(true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createReduceDDScheduler.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createReduceDDScheduler.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SchedulerProblemTest, TestVerboseCreateRestrictedDD) {
    ofstream out("createRestrictedDDScheduler.txt");
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_restricted_decision_diagram(3, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRestrictedDDScheduler.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRestrictedDDScheduler.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SchedulerProblemTest, TestVerboseCreateRelaxedDD) {
    ofstream out("createRelaxedDDScheduler.txt");
    streambuf* coutbuf = cout.rdbuf();
    cout.rdbuf(out.rdbuf());

    dd_instance->create_relax_priority_decision_diagram(3, true);

    cout.rdbuf(coutbuf);

    string full_file_path = source_directory + "/Test/txt_files/createRelaxedDDScheduler.txt";

    ifstream file(full_file_path);
    string expected_output((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
    file.close();

    ifstream actual_file("createRelaxedDDScheduler.txt");
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());
    actual_file.close();

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SchedulerProblemTest, TestCreateDDGraphEqual) {
    Graph<SchedulerState>* expected_graph = GetExactDDScheduler();
    dd_instance->create_decision_diagram(false);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(SchedulerProblemTest, TestCreateReduceDDGraphEqual) {
    Graph<SchedulerState>* expected_graph = GetReduceDDScheduler();
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(SchedulerProblemTest, TestCreateRestrictedDDGraphEqual) {
    Graph<SchedulerState>* expected_graph = GetRestrictedDDScheduler();
    dd_instance->create_restricted_decision_diagram(3);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(SchedulerProblemTest, TestCreateRelaxedDDGraphEqual) {
    Graph<SchedulerState>* expected_graph = GetRelaxedDDScheduler();
    dd_instance->create_relax_priority_decision_diagram(3);

    ASSERT_TRUE(*dd_instance->get_decision_diagram() == *expected_graph);
    delete expected_graph;
}

TEST_F(SchedulerProblemTest, CompareTwoDifferentGraphs) {
    Graph<SchedulerState>* not_expected_graph = GetFalseDDScheduler();
    dd_instance->create_decision_diagram(false);

    ASSERT_FALSE(*dd_instance->get_decision_diagram() == *not_expected_graph);
    delete not_expected_graph;
}

TEST_F(SchedulerProblemTest, TestGetCopy) {
    dd_instance->create_decision_diagram(false);
    Graph<SchedulerState>* original_graph = dd_instance->get_decision_diagram();
    Graph<SchedulerState>* copied_graph = dd_instance->get_decision_diagram_copy();

    ASSERT_NE(&original_graph, &copied_graph);
    ASSERT_TRUE(*original_graph == *copied_graph);

    original_graph->structure[0].front()->set_id(100);
    ASSERT_FALSE(original_graph->structure[0].front()->to_string() == copied_graph->structure[0].front()->to_string());

    Node<SchedulerState>* new_node = new Node<SchedulerState>(99, new SchedulerState{{0, 1, 2}, 2});
    copied_graph->add_node(new_node);

    ASSERT_FALSE(original_graph->structure.back().size() == copied_graph->structure.back().size());
    delete copied_graph;
}

TEST_F(SchedulerProblemTest, TestGetDDBuilderTime) {
    dd_instance->create_decision_diagram(false);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(SchedulerProblemTest, TestGetReduceDDBuilderTime) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();
    ASSERT_GT(stof(dd_instance->get_reduction_time()), 0);
}

TEST_F(SchedulerProblemTest, TestGetRestrictedDDBuilderTime) {
    dd_instance->create_restricted_decision_diagram(3);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(SchedulerProblemTest, TestGetRelaxedDDBuilderTime) {
    dd_instance->create_relax_priority_decision_diagram(3, true);
    ASSERT_GT(stof(dd_instance->get_building_time()), 0);
}

TEST_F(SchedulerProblemTest, GetSolutionForDD) {
    dd_instance->create_decision_diagram(false);

    PathStructureSolution<SchedulerState> solution = getSchedulerSolution();
    double expected_value = 31;
    string expected_path = " arc_0_3(2)-> arc_3_8(0)-> arc_8_10(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(SchedulerProblemTest, GetSolutionForReduceDD) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();

    PathStructureSolution<SchedulerState> solution = getSchedulerSolution();
    double expected_value = 31;
    string expected_path = " arc_0_3(2)-> arc_3_5(0)-> arc_5_7(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(SchedulerProblemTest, GetSolutionForRestrictedDD) {
    dd_instance->create_restricted_decision_diagram(3);

    PathStructureSolution<SchedulerState> solution = getSchedulerSolution();
    double expected_value = 31;
    string expected_path = " arc_0_3(2)-> arc_3_6(0)-> arc_6_7(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(SchedulerProblemTest, GetSolutionForRelaxedDD) {
    dd_instance->create_relax_priority_decision_diagram(3);

    PathStructureSolution<SchedulerState> solution = getSchedulerSolution();
    // Relaxed DD gives a lower bound (<= optimum 31). Dropping the time from the
    // state changes which nodes are merged, hence a different (tighter) bound.
    double expected_value = 30;
    string expected_path = " arc_0_3(2)-> arc_3_4(1)-> arc_4_7(1)";
    ASSERT_EQ(solution.value, expected_value);
    ASSERT_EQ(solution.path_print, expected_path);
}

TEST_F(SchedulerProblemTest, TestCompareGMLDDGraph) {
    dd_instance->create_decision_diagram(false);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/exact_dd_scheduler.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SchedulerProblemTest, TestCompareGMLReduceDDGraph) {
    dd_instance->create_decision_diagram(false);
    dd_instance->reduce_decision_diagram();
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/reduce_dd_scheduler.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SchedulerProblemTest, TestCompareGMLRestrictedDDGraph) {
    dd_instance->create_restricted_decision_diagram(3);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/restricted_dd_scheduler.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output == expected_output);
}

TEST_F(SchedulerProblemTest, TestCompareGMLRelaxedDDGraph) {
    dd_instance->create_relax_priority_decision_diagram(3);
    dd_instance->export_graph_file("test");

    string expected_file_path = source_directory + "/Test/gml_files/relax_dd_scheduler.gml";
    string actual_file_path = source_directory + "/test.gml";

    ASSERT_TRUE(ifstream(expected_file_path).good());
    ASSERT_TRUE(ifstream(actual_file_path).good());

    ifstream expected_file(expected_file_path);
    ifstream actual_file(actual_file_path);
    string expected_output((istreambuf_iterator<char>(expected_file)), istreambuf_iterator<char>());
    string actual_output((istreambuf_iterator<char>(actual_file)), istreambuf_iterator<char>());

    ASSERT_TRUE(actual_output == expected_output);
}
