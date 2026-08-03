#include <gtest/gtest.h>
#include "../../Examples/SequencingInstance/SequencingInstance.h"
#include "../../Examples/SequencingInstance/SequencingProblem.h"
#include "../../SourceCode/DD.h"
#include "../../SourceCode/DDCutGenerators/TargetCut.h"

#include <string>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;
using namespace std;

class TargetCutSequencingTest : public ::testing::Test {
protected:
    void SetUp() override {
        source_directory = fs::current_path().parent_path().string();
        string test_file = source_directory + "/Test/txt_files/sequencing_test_instance.txt";

        sequencing_params   = new SequencingInstance(test_file);
        problem_instance   = new SequencingProblem(*sequencing_params);
        dd_instance        = new DD<SequencingState>(*problem_instance);
        dd_instance->create_decision_diagram(false);
        tc                 = new TargetCut<SequencingState>(dd_instance);
    }

    void TearDown() override {
        delete tc;
        delete dd_instance;
        delete problem_instance;
        delete sequencing_params;
    }

    string source_directory;
    SequencingInstance*         sequencing_params;
    SequencingProblem*          problem_instance;
    DD<SequencingState>*        dd_instance;
    TargetCut<SequencingState>* tc;
};

TEST_F(TargetCutSequencingTest, TestInstantiation) {
    ASSERT_TRUE(tc != nullptr);
    ASSERT_EQ((int)tc->omega.size(), sequencing_params->n_jobs);
}

TEST_F(TargetCutSequencingTest, TestGetName) {
    EXPECT_EQ(tc->get_name(), "Target Cut (MDD)");
}

TEST_F(TargetCutSequencingTest, TestOmegaInDomainRange) {
    // Domain is {0, 1, ..., n_jobs-1}, so ω[k] ∈ [0, n_jobs-1]
    double max_label = static_cast<double>(sequencing_params->n_jobs - 1);
    for (int k = 0; k < (int)tc->omega.size(); ++k) {
        EXPECT_GE(tc->omega[k], 0.0) << "omega[" << k << "] < 0";
        EXPECT_LE(tc->omega[k], max_label) << "omega[" << k << "] > max_label";
    }
}

TEST_F(TargetCutSequencingTest, TestGetCutBeforeGenerateRaises) {
    EXPECT_THROW(tc->get_cut(), NoCutsFound);
}

TEST_F(TargetCutSequencingTest, TestCutFoundAllZeros) {
    // All slots assigned job 0 → infeasible → cut expected
    vector<double> x_bar(sequencing_params->n_jobs, 0.0);
    EXPECT_TRUE(tc->generate_cut(x_bar));
}

TEST_F(TargetCutSequencingTest, TestCutValidity) {
    // The returned cut must be violated by x̄=(0,0,0)
    vector<double> x_bar(sequencing_params->n_jobs, 0.0);
    ASSERT_TRUE(tc->generate_cut(x_bar));
    auto [coeffs, constant] = tc->get_cut();
    double lhs = 0.0;
    for (int k = 0; k < (int)coeffs.size(); ++k)
        lhs += coeffs[k] * x_bar[k];
    EXPECT_GT(lhs, constant) << "Cut must be violated by x̄: LHS > RHS expected";
}

TEST_F(TargetCutSequencingTest, TestCutCoefficientsShape) {
    vector<double> x_bar(sequencing_params->n_jobs, 0.0);
    ASSERT_TRUE(tc->generate_cut(x_bar));
    auto [coeffs, constant] = tc->get_cut();
    EXPECT_EQ((int)coeffs.size(), sequencing_params->n_jobs);
}

TEST_F(TargetCutSequencingTest, TestNoCutAtInteriorPoint) {
    // ω itself is the geometric center of conv(S) → no cut expected
    EXPECT_FALSE(tc->generate_cut(tc->omega));
}

TEST_F(TargetCutSequencingTest, TestNoCutClearsStoredCut) {
    vector<double> x_bar(sequencing_params->n_jobs, 0.0);
    tc->generate_cut(x_bar);          // stores a cut
    tc->generate_cut(tc->omega);      // clears it
    EXPECT_THROW(tc->get_cut(), NoCutsFound);
}

TEST_F(TargetCutSequencingTest, TestGetTimeNonNegative) {
    vector<double> x_bar(sequencing_params->n_jobs, 0.0);
    tc->generate_cut(x_bar);
    EXPECT_GE(tc->get_time(), 0.0);
}

TEST_F(TargetCutSequencingTest, TestIsEqualityCutForInfeasible) {
    // For the Sequencing MDD, infeasible points yield an equality cut (unbounded LP)
    vector<double> x_bar(sequencing_params->n_jobs, 0.0);
    ASSERT_TRUE(tc->generate_cut(x_bar));
    EXPECT_TRUE(tc->is_equality_cut());
}
