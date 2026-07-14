#include <gtest/gtest.h>
#include "../../Examples/KnapsackInstance/KnapsackProblem.h"
#include "../../Examples/KnapsackInstance/KnapsackGurobiClass.h"
#include "../../Examples/GuiStructure.h"
#include "../../SourceCode/DD.h"
#include "../../SourceCode/DDCutGenerators/FlowCuts.h"
#include "../../SourceCode/DDCutGenerators/CutStrengthening.h"
#include "../../SourceCode/DDCutGenerators/JointFlowCuts.h"
#include "../../SourceCode/DDCutGenerators/TargetCut.h"
#include "GraphAlgorithms/MaxFlow/MaxFlowGurobi.h"

#include <string>
#include <vector>
#include <memory>

namespace fs = filesystem;
using namespace std;

class CutTest : public ::testing::Test {
protected:
    void SetUp() override {
        variable_length = 4;
        int_weights = {7, 3, 2, 1};
        values = {1, 1, 1, 1};
        capacity = 8;
        variables = {
                make_pair("x_1", vector<int>{0, 1}),
                make_pair("x_2", vector<int>{0, 1}),
                make_pair("x_3", vector<int>{0, 1}),
                make_pair("x_4", vector<int>{0, 1})
        };

        knapsack_params = new KnapsackInstance(new int(0), variables, int_weights, values, capacity);
        knapsack_instance = new KnapsackProblem(*knapsack_params);
        dd_instance = new DD(*knapsack_instance);
        dd_instance->create_decision_diagram(false);

    }

    void TearDown() override {
        delete knapsack_instance;
        delete dd_instance;
        delete knapsack_params;
    }

    int variable_length;
    vector<int> int_weights;
    vector<double> values;
    int capacity;
    vector<pair<string, vector<int>>> variables;
    KnapsackInstance* knapsack_params;
    KnapsackProblem* knapsack_instance;
    DD<int>* dd_instance;
};

TEST_F(CutTest, TestOptimizeWithJointFlowCuts) {
    KnapsackGurobiClass* gurobiInstance = nullptr;
    ASSERT_NO_THROW(gurobiInstance = new KnapsackGurobiClass(*knapsack_params, true));
    ASSERT_TRUE(gurobiInstance != nullptr);

    gurobiInstance->create_model();
    gurobiInstance->optimize_with_cuts(vector<DD<int>*>{dd_instance}, CutType::JointFlow);

    vector<double> expected_solution = {0.2857142857142857, 1.0, 1.0, 1.0};
    for (size_t i = 0; i < gurobiInstance->x.size(); ++i) {
        double value = gurobiInstance->x[i].get(GRB_DoubleAttr_X);
        EXPECT_NEAR(expected_solution[i], value, 1e-6);
    }
    EXPECT_FALSE(gurobiInstance->model->get(GRB_DoubleAttr_ObjVal) == static_cast<int>(gurobiInstance->model->get(GRB_DoubleAttr_ObjVal)));
    delete gurobiInstance;
}

TEST_F(CutTest, TestMaxFlowGurobiInstance) {
    MaxFlowGurobi<int>* maxFlowInstance = nullptr;
    ASSERT_NO_THROW(maxFlowInstance = new MaxFlowGurobi<int>(dd_instance));
    ASSERT_TRUE(maxFlowInstance != nullptr);

    ASSERT_THROW(maxFlowInstance->get_max_flow(), MaxFlowNotExecuted);
    EXPECT_TRUE(maxFlowInstance->solve_max_flow({1.0 , 0, 0, 1.0}));
    EXPECT_TRUE(maxFlowInstance->solve_max_flow({0.2857142857142857, 1.0, 1.0, 1.0}));
    EXPECT_DOUBLE_EQ(0.7142857142857143, maxFlowInstance->get_max_flow());

    EXPECT_FALSE(maxFlowInstance->solve_max_flow({1, 1, 1, 1}));
    delete maxFlowInstance;
}

TEST_F(CutTest, TestMaxFlowInstance) {
    MaxFlow<int>* maxFlowInstance = nullptr;
    ASSERT_NO_THROW(maxFlowInstance = new MaxFlow<int>(dd_instance));
    ASSERT_TRUE(maxFlowInstance != nullptr);

    EXPECT_THROW(maxFlowInstance->get_max_flow(), MaxFlowNotExecuted);
    EXPECT_TRUE(maxFlowInstance->solve_max_flow({1.0 , 0, 0, 1.0}));
    EXPECT_TRUE(maxFlowInstance->solve_max_flow({0.2857142857142857, 1.0, 1.0, 1.0}));
    EXPECT_DOUBLE_EQ(0.7142857142857143, maxFlowInstance->get_max_flow());

    EXPECT_FALSE(maxFlowInstance->solve_max_flow({1, 1, 1, 1}));
    delete maxFlowInstance;
}

TEST_F(CutTest, TestCreateFlowCutsInstance) {
    FlowCuts<int>* cuts_instance = nullptr;
    ASSERT_NO_THROW(cuts_instance = new FlowCuts<int>(dd_instance));
    delete cuts_instance;
}

TEST_F(CutTest, TestCreateJointFlowCutsInstance) {
    JointFlowCuts<int>* cuts_instance = nullptr;
    ASSERT_NO_THROW(cuts_instance = new JointFlowCuts<int>(dd_instance));
    delete cuts_instance;
}

TEST_F(CutTest, TestCreateTargetCutInstance) {
    TargetCut<int>* tc = nullptr;
    ASSERT_NO_THROW(tc = new TargetCut<int>(dd_instance));
    ASSERT_TRUE(tc != nullptr);
    ASSERT_EQ((int)tc->omega.size(), 4);
    delete tc;
}

TEST_F(CutTest, TestGetFlowCuts) {
    FlowCuts<int>* cuts_instance = nullptr;
    KnapsackGurobiClass* gurobiInstance = nullptr;

    cuts_instance = new FlowCuts<int>(dd_instance);
    gurobiInstance = new KnapsackGurobiClass(*knapsack_params, true);
    gurobiInstance->create_model();
    gurobiInstance->optimize_with_cuts(vector<DD<int>*>{dd_instance}, CutType::JointFlow);

    EXPECT_THROW(cuts_instance->get_cut(), NoCutsFound);

    vector<double> x_values = {};
    for (size_t i = 0; i < gurobiInstance->x.size(); ++i) {
        x_values.push_back(gurobiInstance->x[i].get(GRB_DoubleAttr_X));
    }

    EXPECT_TRUE(cuts_instance->generate_cut(x_values));

    auto [coefficients, constant] = cuts_instance->get_cut();
    EXPECT_EQ(coefficients, (vector<double>{1, 1, 0, 0}));
    EXPECT_DOUBLE_EQ(constant, 1.0);

    EXPECT_DOUBLE_EQ(cuts_instance->get_min_cut(), 0.7142857142857143);

    delete cuts_instance;
    delete gurobiInstance;
}

TEST_F(CutTest, TestGetJointFlowCuts) {
    JointFlowCuts<int>* cuts_instance = nullptr;
    KnapsackGurobiClass* gurobiInstance = nullptr;

    cuts_instance = new JointFlowCuts<int>(dd_instance);
    gurobiInstance = new KnapsackGurobiClass(*knapsack_params, true);
    gurobiInstance->create_model();
    gurobiInstance->optimize_with_cuts(vector<DD<int>*>{dd_instance}, CutType::JointFlow);

    EXPECT_THROW(cuts_instance->get_cut(), NoCutsFound);

    vector<double> x_values = {};
    for (size_t i = 0; i < gurobiInstance->x.size(); ++i) {
        x_values.push_back(gurobiInstance->x[i].get(GRB_DoubleAttr_X));
    }

    EXPECT_TRUE(cuts_instance->generate_cut(x_values));

    auto [coefficients, constant] = cuts_instance->get_cut();
    EXPECT_EQ(coefficients, (vector<double>{1, 1, 0, 0}));
    EXPECT_DOUBLE_EQ(constant, 1.0);

    delete cuts_instance;
    delete gurobiInstance;
}

TEST_F(CutTest, TestGetTargetCut) {
    TargetCut<int>* tc = new TargetCut<int>(dd_instance);

    EXPECT_THROW(tc->get_cut(), NoCutsFound);

    vector<double> x_bar = {1.0, 1.0, 1.0, 1.0};
    EXPECT_TRUE(tc->generate_cut(x_bar));

    auto [coefficients, constant] = tc->get_cut();
    ASSERT_EQ((int)coefficients.size(), 4);
    EXPECT_NEAR(coefficients[0], 2.5, 1e-9);
    EXPECT_NEAR(coefficients[1], 2.5, 1e-9);
    EXPECT_NEAR(coefficients[2], 0.0, 1e-9);
    EXPECT_NEAR(coefficients[3], 0.0, 1e-9);
    EXPECT_NEAR(constant, 2.5, 1e-9);

    delete tc;
}

TEST_F(CutTest, TestOptimizeWithTargetCuts) {
    KnapsackGurobiClass* gurobiInstance = new KnapsackGurobiClass(*knapsack_params, true);
    gurobiInstance->create_model();
    gurobiInstance->optimize_with_cuts(vector<DD<int>*>{dd_instance}, CutType::Target);

    vector<double> expected_solution = {0.2857142857142857, 1.0, 1.0, 1.0};
    for (size_t i = 0; i < gurobiInstance->x.size(); ++i) {
        double value = gurobiInstance->x[i].get(GRB_DoubleAttr_X);
        EXPECT_NEAR(expected_solution[i], value, 1e-6);
    }
    EXPECT_FALSE(gurobiInstance->model->get(GRB_DoubleAttr_ObjVal) == static_cast<int>(gurobiInstance->model->get(GRB_DoubleAttr_ObjVal)));
    delete gurobiInstance;
}

TEST_F(CutTest, TestCreateCutStrengtheningInstance) {
    CutStrengthening<int>* cut_strengthening_instance = nullptr;
    ASSERT_NO_THROW(cut_strengthening_instance = new CutStrengthening<int>(dd_instance));
    delete cut_strengthening_instance;
}

TEST_F(CutTest, TestGetCutStrengthened) {
    CutStrengthening<int>* cut_strengthening_instance = nullptr;
    cut_strengthening_instance = new CutStrengthening<int>(dd_instance);

    EXPECT_THROW(cut_strengthening_instance->get_cut_strengthening(), NoCutStrengthening);

    EXPECT_FALSE(cut_strengthening_instance->cut_strengthening({1, 1, 0, 0}, 1));
    EXPECT_TRUE(cut_strengthening_instance->cut_strengthening({0.5, 1, 1, 0}, 2));


    auto strengthened_cut = cut_strengthening_instance->get_cut_strengthening();
    const vector<double>& expected_coefficients = {1, 0, 1, 0};
    EXPECT_EQ(strengthened_cut.first.size(), expected_coefficients.size());
    for (size_t i = 0; i < expected_coefficients.size(); ++i) {
        EXPECT_DOUBLE_EQ(strengthened_cut.first[i], expected_coefficients[i]);
    }

    double expected_constant = 1.0;
    EXPECT_DOUBLE_EQ(strengthened_cut.second, expected_constant);

    delete cut_strengthening_instance;
}
