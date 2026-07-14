import os

from SourceCode.DD import DD
from Examples.KnapsackInstance.KnapsackInstance import KnapsackStructure
from Examples.KnapsackInstance.KnapsackProblem import KnapsackProblem
from Examples.KnapsackInstance.KnapsackGurobiClass import KnapsackGurobi
from Examples.GuiStructure import CutType
from SourceCode.GraphAlgorithms.ShortestLongestPath.ShortestLongestPath import ShortestLongestPath

## ============================
## SETUP PROBLEM AND PARAMETERS
## ============================

# Code Parameters
_verbose = False
_width = 3

# Instance file
source_directory: str = os.path.dirname(os.path.abspath(__file__))
filename: str = "../DataInstances/Knapsack/Custom/knapsack_instance_v5_d10_seed1.txt"
file_path: str = f"{source_directory}/{filename}"

# Load instance
knapsack_instance = KnapsackStructure(file_path)
print("Instance loaded:")
print(knapsack_instance)

# Build KnapsackProblem for DD construction
knapsack_problem = KnapsackProblem(knapsack_instance)

## ============================
## CREATE EXACT DD
## ============================

dd_exact = DD(knapsack_problem)
dd_exact.create_decision_diagram(verbose=_verbose)
dd_exact.reduce_decision_diagram(verbose=_verbose)

print("\n-- Exact DD --")
print(f"\tConstruction time (sec): {dd_exact.get_building_time()}")
print(f"\tReduction time   (sec): {dd_exact.get_reduction_time()}")
print(f"\tNodes: {dd_exact.get_decision_diagram().get_node_count()}")
print(f"\tArcs:  {dd_exact.get_decision_diagram().get_arc_count()}")

## ============================
## CREATE RELAXED DD
## ============================

dd_relaxed = DD(knapsack_problem)
dd_relaxed.create_relax_priority_decision_diagram(max_width=_width, verbose=_verbose)
dd_relaxed.reduce_decision_diagram(verbose=_verbose)

print("\n-- Relaxed DD (width={}) --".format(_width))
print(f"\tConstruction time (sec): {dd_relaxed.get_building_time()}")
print(f"\tReduction time   (sec): {dd_relaxed.get_reduction_time()}")
print(f"\tNodes: {dd_relaxed.get_decision_diagram().get_node_count()}")
print(f"\tArcs:  {dd_relaxed.get_decision_diagram().get_arc_count()}")

## ============================
## CREATE RESTRICTED DD
## ============================

dd_restricted = DD(knapsack_problem)
dd_restricted.create_restricted_decision_diagram(max_width=_width, verbose=_verbose)
dd_restricted.reduce_decision_diagram(verbose=_verbose)

print("\n-- Restricted DD (width={}) --".format(_width))
print(f"\tConstruction time (sec): {dd_restricted.get_building_time()}")
print(f"\tReduction time   (sec): {dd_restricted.get_reduction_time()}")
print(f"\tNodes: {dd_restricted.get_decision_diagram().get_node_count()}")
print(f"\tArcs:  {dd_restricted.get_decision_diagram().get_arc_count()}")

# Export graph in .gml format
dd_exact.export_graph_file("knapsack_exact_dd_file")
dd_relaxed.export_graph_file("knapsack_relaxed_dd_file")
dd_restricted.export_graph_file("knapsack_restricted_dd_file")

## ============================
## SHORTEST / LONGEST PATH
## ============================

# Optimal solution via exact DD
longest_path = ShortestLongestPath(dd_exact)
longest_path.set_parameters(knapsack_instance.objective_weights, "max")
answer = longest_path.solve()

print("\n-- Optimal solution (exact DD longest path) --")
print(f"\tOptimal value:  {answer.value}")
print(f"\tPath:           {answer.path_print}")
print(f"\tSolution time:  {longest_path.get_time()} sec")

# Dual (upper) bound via relaxed DD
longest_path_relaxed = ShortestLongestPath(dd_relaxed)
longest_path_relaxed.set_parameters(knapsack_instance.objective_weights, "max")
answer_relaxed = longest_path_relaxed.solve()

print("\n-- Dual bound (relaxed DD longest path) --")
print(f"\tUpper bound:   {answer_relaxed.value}")
print(f"\tSolution time: {longest_path_relaxed.get_time()} sec")

# Primal (lower) bound via restricted DD
longest_path_restricted = ShortestLongestPath(dd_restricted)
longest_path_restricted.set_parameters(knapsack_instance.objective_weights, "max")
answer_restricted = longest_path_restricted.solve()

print("\n-- Primal bound (restricted DD longest path) --")
print(f"\tLower bound:   {answer_restricted.value}")
print(f"\tSolution time: {longest_path_restricted.get_time()} sec")

## ============================
## GUROBI – BASELINE
## ============================

print("\n============================")
print("Gurobi baseline (no DD cuts)")
print("============================")
gurobi_base = KnapsackGurobi(knapsack_instance, continuous=False)
gurobi_base.create_model()
gurobi_base.optimize_model()
print(f"Optimization time: {gurobi_base.get_optimization_time()} sec")

## ============================
## GUROBI – DD CUTS
## ============================

print("\n============================")
print("Gurobi with DD cuts")
print("============================")
gurobi_cuts = KnapsackGurobi(knapsack_instance, continuous=False)
gurobi_cuts.create_model()
gurobi_cuts.optimize_with_cuts([dd_relaxed], cut_type=CutType.JOINT_FLOW, cut_strengthening=False)
print(f"Optimization time: {gurobi_cuts.get_optimization_time()} sec")
