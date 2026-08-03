# ── Testing ───────────────────────────────────────────────────────────────────

.PHONY: test pytest coverage report html \
        main_knapsack main_independent_set main_set_cover main_socknapsack main_sequencing \
        gurobi_knapsack gurobi_independent_set gurobi_set_cover gurobi_socknapsack gurobi_sequencing \
        cuts_knapsack cuts_independent_set cuts_set_cover cuts_socknapsack cuts_sequencing

test:
	python -m unittest discover -v -s ./Test -p "test_*.py" -b

pytest:
	pytest ./Test -v

coverage:
	coverage run -m unittest discover -s ./Test -p "test_*.py"

report:
	coverage report -m

html:
	coverage html


# ── main.py ───────────────────────────────────────────────────────────────────

main_knapsack:
	python main.py ../DataInstances/Knapsack/Standard/f1_l-d_kp_10_269.txt -Knapsack -Exact -Output_results.csv -Max -Width_200 -RelaxGrouping

main_independent_set:
	python main.py ../DataInstances/IndependentSet/Custom/independent_set_instance_v100_d75_seed1.txt -IndependentSet -Exact -Output_results.csv -Max -Width_200 -Reduce -NoSort

main_set_cover:
	python main.py ../DataInstances/SetCover/Standard/set_cover_n100_m63_d0.1_b_w38_seed1.txt -SetCover -Relaxed -Output_results.csv -Min -Width_20000 -NoSort

main_socknapsack:
	python main.py ../DataInstances/SOCKnapsack/knapsack_n5m2o1b5_1.txt -SOCKnapsack -Exact -Output_results.csv -Max -Width_20 -Reduce

main_sequencing:
	python main.py ../DataInstances/Sequencing/Custom/sequencing_n5_s025_seed1.txt -Sequencing -Exact -Output_results.csv -Min


# ── main_gurobi.py ────────────────────────────────────────────────────────────

gurobi_knapsack:
	python main_gurobi.py ../DataInstances/Knapsack/Custom/knapsack_instance_v4_d4_seed1.txt -Knapsack -Output_resultsGurobi.csv

gurobi_independent_set:
	python main_gurobi.py ../DataInstances/IndependentSet/Custom/independent_set_instance_v5_d3_seed2.txt -IndependentSet -Output_resultsGurobi.csv

gurobi_set_cover:
	python main_gurobi.py ../DataInstances/SetCover/Standard/set_cover_n100_m63_d0.1_b_w38_seed1.txt -SetCover -Output_resultsGurobi.csv

gurobi_socknapsack:
	python main_gurobi.py ../DataInstances/SOCKnapsack/knapsack_n5m2o1b5_1.txt -SOCKnapsack -Output_resultsGurobiSock.csv

gurobi_sequencing:
	python main_gurobi.py ../DataInstances/Sequencing/Custom/sequencing_n5_s025_seed1.txt -Sequencing


# ── main_cuts.py ──────────────────────────────────────────────────────────────

cuts_knapsack:
	python main_cuts.py ../DataInstances/Knapsack/Custom/knapsack_instance_v4_d4_seed1.txt -Knapsack -JointFlowCuts -Verbose -Strength -Output_resultCuts.csv -Continuous

cuts_independent_set:
	python main_cuts.py ../DataInstances/IndependentSet/Custom/independent_set_instance_v5_d3_seed2.txt -IndependentSet -FlowCuts -Verbose -Strength -Output_resultCuts.csv -Continuous

cuts_set_cover:
	python main_cuts.py ../DataInstances/SetCover/Custom/set_cover_v50_r30_seed1.txt -SetCover -TargetCuts -Verbose -Strength -Output_resultCuts.csv -Continuous

cuts_socknapsack:
	python main_cuts.py ../DataInstances/SOCKnapsack/knapsack_n5m2o1b5_1.txt -SOCKnapsack -JointFlowCuts -Verbose -Strength -Output_resultCutsSock.csv -Continuous

cuts_sequencing:
	python main_cuts.py ../DataInstances/Sequencing/Custom/sequencing_n5_s025_seed1.txt -Sequencing -TargetCuts -Verbose -Output_resultCuts.csv -Continuous
