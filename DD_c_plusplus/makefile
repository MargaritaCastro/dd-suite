BUILD_DIR := cmake-build-debug

# ── Build & Infrastructure ────────────────────────────────────────────────────

.PHONY: all build test clean \
        main_knapsack main_independent_set main_set_cover main_socknapsack main_sequencing \
        gurobi_knapsack gurobi_independent_set gurobi_set_cover gurobi_socknapsack gurobi_sequencing \
        cuts_knapsack cuts_independent_set cuts_set_cover cuts_socknapsack cuts_sequencing \
        leaks_main leaks_cuts leaks_gurobi \
        run_knapsack run_socknapsack

all: build

build:
	cmake -S . -B $(BUILD_DIR) -DCMAKE_BUILD_TYPE=Debug
	cmake --build $(BUILD_DIR)

test: build
	cd $(BUILD_DIR) && ./DD_optimization_program_tests

clean:
	rm -rf $(BUILD_DIR)


# ── Main ──────────────────────────────────────────────────────────────────────

main_knapsack: build
	@./$(BUILD_DIR)/Main DataInstances/Knapsack/Custom/knapsack_instance_v4_d4_seed1.txt -Knapsack -Exact -Max -Output_results.csv -Width_200 -RelaxGrouping -NoSort

main_independent_set: build
	@./$(BUILD_DIR)/Main DataInstances/IndependentSet/Custom/independent_set_instance_v100_d75_seed1.txt -IndependentSet -Exact -Output_results.csv -Max -Width_200 -Reduce -NoSort

main_set_cover: build
	@./$(BUILD_DIR)/Main DataInstances/SetCover/Custom/set_cover_v50_r30_seed1.txt -SetCover -Exact -Output_results.csv -Min -Width_200 -Reduce

main_socknapsack: build
	@./$(BUILD_DIR)/Main DataInstances/SOCKnapsack/knapsack_n5m2o1b5_0.txt -SOCKnapsack -Exact -Max

main_sequencing: build
	@./$(BUILD_DIR)/Main DataInstances/Sequencing/Custom/sequencing_n5_s025_seed1.txt -Sequencing -Exact


# ── Main_knapsack / Main_socknapsack (standalone scripts) ─────────────────────

run_knapsack: build
	@./$(BUILD_DIR)/Main_knapsack

run_socknapsack: build
	@./$(BUILD_DIR)/Main_socknapsack


# ── MainGurobi ────────────────────────────────────────────────────────────────

gurobi_knapsack: build
	@./$(BUILD_DIR)/MainGurobi DataInstances/Knapsack/Custom/knapsack_instance_v4_d4_seed1.txt -Knapsack

gurobi_independent_set: build
	@./$(BUILD_DIR)/MainGurobi DataInstances/IndependentSet/Custom/independent_set_instance_v100_d75_seed1.txt -IndependentSet

gurobi_set_cover: build
	@./$(BUILD_DIR)/MainGurobi DataInstances/SetCover/Custom/set_cover_v50_r30_seed1.txt -SetCover

gurobi_socknapsack: build
	@./$(BUILD_DIR)/MainGurobi DataInstances/SOCKnapsack/knapsack_n5m2o1b5_0.txt -SOCKnapsack -Verbose

gurobi_sequencing: build
	@./$(BUILD_DIR)/MainGurobi DataInstances/Sequencing/Custom/sequencing_n5_s025_seed1.txt -Sequencing -Verbose


# ── MainCuts ──────────────────────────────────────────────────────────────────

cuts_knapsack: build
	@./$(BUILD_DIR)/MainCuts DataInstances/Knapsack/Custom/knapsack_instance_v4_d4_seed1.txt -Knapsack -JointFlowCuts -Verbose -Strength -Continuous

cuts_independent_set: build
	@./$(BUILD_DIR)/MainCuts DataInstances/IndependentSet/Custom/independent_set_instance_v100_d75_seed1.txt -IndependentSet -FlowCuts -Verbose -Strength -Continuous

cuts_set_cover: build
	@./$(BUILD_DIR)/MainCuts DataInstances/SetCover/Custom/set_cover_v50_r30_seed1.txt -SetCover -TargetCuts -Verbose -Strength -Continuous

cuts_socknapsack: build
	@./$(BUILD_DIR)/MainCuts DataInstances/SOCKnapsack/knapsack_n5m2o1b5_0.txt -SOCKnapsack -JointFlowCuts -Verbose -Strength -Continuous

cuts_sequencing: build
	@./$(BUILD_DIR)/MainCuts DataInstances/Sequencing/Custom/sequencing_n5_s025_seed1.txt -Sequencing -TargetCuts -Verbose -Continuous


# ── Memory leaks ──────────────────────────────────────────────────────────────

leaks_main: build
	cd $(BUILD_DIR) && leaks --atExit -- ./Main ../DataInstances/SetCover/Standard/set_cover_n100_m63_d0.1_b_w38_seed1.txt -SetCover -Restricted -Min -Output_set_covering_results.csv -Width_1000

leaks_cuts: build
	cd $(BUILD_DIR) && leaks --atExit -- ./MainCuts ../DataInstances/Knapsack/Custom/knapsack_instance_v4_d4_seed1.txt -Knapsack -JointFlowCuts -Verbose -Strength

leaks_gurobi: build
	cd $(BUILD_DIR) && leaks --atExit -- ./MainGurobi ../DataInstances/IndependentSet/Custom/independent_set_instance_v5_d3_seed1.txt -IndependentSet
