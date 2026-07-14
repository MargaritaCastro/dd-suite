#include <gtest/gtest.h>
#include "../../Examples/SchedulerInstance/SchedulerInstance.h"
#include "../../Examples/SchedulerInstance/SchedulerProblem.h"
#include "../../SourceCode/DD.h"
#include "../../SourceCode/DDCutGenerators/TargetCut.h"

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

class TargetCutSchedulerTest : public ::testing::Test {
protected:
    void SetUp() override {
        source_directory = fs::current_path().parent_path().string();
        string test_file = source_directory + "/Test/txt_files/scheduler_test_instance.txt";

        scheduler_params   = new SchedulerInstance(test_file);
        problem_instance   = new SchedulerProblem(*scheduler_params);
        dd_instance        = new DD<SchedulerState>(*problem_instance);
        dd_instance->create_decision_diagram(false);
        tc                 = new TargetCut<SchedulerState>(dd_instance);
    }

    void TearDown() override {
        delete tc;
        delete dd_instance;
        delete problem_instance;
        delete scheduler_params;
    }

    string source_directory;
    SchedulerInstance*         scheduler_params;
    SchedulerProblem*          problem_instance;
    DD<SchedulerState>*        dd_instance;
    TargetCut<SchedulerState>* tc;
};

TEST_F(TargetCutSchedulerTest, TestInstantiation) {
    ASSERT_TRUE(tc != nullptr);
    ASSERT_EQ((int)tc->omega.size(), scheduler_params->n_jobs);
}

TEST_F(TargetCutSchedulerTest, TestGetName) {
    EXPECT_EQ(tc->get_name(), "Target Cut (MDD)");
}

TEST_F(TargetCutSchedulerTest, TestOmegaInDomainRange) {
    // Domain is {0, 1, ..., n_jobs-1}, so ω[k] ∈ [0, n_jobs-1]
    double max_label = static_cast<double>(scheduler_params->n_jobs - 1);
    for (int k = 0; k < (int)tc->omega.size(); ++k) {
        EXPECT_GE(tc->omega[k], 0.0) << "omega[" << k << "] < 0";
        EXPECT_LE(tc->omega[k], max_label) << "omega[" << k << "] > max_label";
    }
}

TEST_F(TargetCutSchedulerTest, TestGetCutBeforeGenerateRaises) {
    EXPECT_THROW(tc->get_cut(), NoCutsFound);
}

TEST_F(TargetCutSchedulerTest, TestCutFoundAllZeros) {
    // All slots assigned job 0 → infeasible → cut expected
    vector<double> x_bar(scheduler_params->n_jobs, 0.0);
    EXPECT_TRUE(tc->generate_cut(x_bar));
}

TEST_F(TargetCutSchedulerTest, TestCutValidity) {
    // The returned cut must be violated by x̄=(0,0,0)
    vector<double> x_bar(scheduler_params->n_jobs, 0.0);
    ASSERT_TRUE(tc->generate_cut(x_bar));
    auto [coeffs, constant] = tc->get_cut();
    double lhs = 0.0;
    for (int k = 0; k < (int)coeffs.size(); ++k)
        lhs += coeffs[k] * x_bar[k];
    EXPECT_GT(lhs, constant) << "Cut must be violated by x̄: LHS > RHS expected";
}

TEST_F(TargetCutSchedulerTest, TestCutCoefficientsShape) {
    vector<double> x_bar(scheduler_params->n_jobs, 0.0);
    ASSERT_TRUE(tc->generate_cut(x_bar));
    auto [coeffs, constant] = tc->get_cut();
    EXPECT_EQ((int)coeffs.size(), scheduler_params->n_jobs);
}

TEST_F(TargetCutSchedulerTest, TestNoCutAtInteriorPoint) {
    // ω itself is the geometric center of conv(S) → no cut expected
    EXPECT_FALSE(tc->generate_cut(tc->omega));
}

TEST_F(TargetCutSchedulerTest, TestNoCutClearsStoredCut) {
    vector<double> x_bar(scheduler_params->n_jobs, 0.0);
    tc->generate_cut(x_bar);          // stores a cut
    tc->generate_cut(tc->omega);      // clears it
    EXPECT_THROW(tc->get_cut(), NoCutsFound);
}

TEST_F(TargetCutSchedulerTest, TestGetTimeNonNegative) {
    vector<double> x_bar(scheduler_params->n_jobs, 0.0);
    tc->generate_cut(x_bar);
    EXPECT_GE(tc->get_time(), 0.0);
}

TEST_F(TargetCutSchedulerTest, TestIsEqualityCutForInfeasible) {
    // For the Scheduler MDD, infeasible points yield an equality cut (unbounded LP)
    vector<double> x_bar(scheduler_params->n_jobs, 0.0);
    ASSERT_TRUE(tc->generate_cut(x_bar));
    EXPECT_TRUE(tc->is_equality_cut());
}
