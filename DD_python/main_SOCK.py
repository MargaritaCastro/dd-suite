import os

from SourceCode.DD import DD
from Examples.SOCKnapsack.SOCKnapsackInstance import SOCKnapsackInstance
from Examples.SOCKnapsack.SOCKnapsackProblem import SOCKnapsackProblem
from Examples.SOCKnapsack.SOCKnapsackGurobiClass import SOCKnapsackGurobi
from Examples.GuiStructure import CutType

## ============================
## SETUP PROBLEM AND PARAMETERS
## ============================

# Code Parameters
_verbose = False
_width = 2

# Input data
source_directory: str = os.path.dirname(os.path.abspath(__file__))
filename: str = "../DataInstances/SOCKnapsack/knapsack_n5m2o1b5_0.txt"
#filename: str = "../DataInstances/SOCKnapsack/Standard/knapsack_n100m10o1b5_0.txt"
file_path: str = f"{source_directory}/{filename}"

# SOCKnapsack instance
sock_instance = SOCKnapsackInstance(file_path)
num_constraints = len(sock_instance.right_side_of_restrictions)

# Create one SOCKnapsackProblem per constraint
sock_constraints = []
for i in range(num_constraints):
    sock_constraints.append(SOCKnapsackProblem(sock_instance, i))

## ============================
## CREATE EXACT DD PER CONSTRAINT
## ============================

dd_exact = []
for i in range(num_constraints):
    dd_exact.append(DD(sock_constraints[i]))
    dd_exact[i].create_decision_diagram(verbose=_verbose)

    print(f"-- Exact DD information (constraint {i}) --")
    print(f"\tConstruction time (sec): {dd_exact[i].get_building_time()}")
    print(f"\tNumber Layers: {dd_exact[i].get_decision_diagram().actual_layer}")
    print(f"\tMax width: {dd_exact[i].get_decision_diagram().get_actual_max_width()}")
    print(f"\tNumber of nodes: {dd_exact[i].get_decision_diagram().get_node_count()}")
    print(f"\tNumber of arcs: {dd_exact[i].get_decision_diagram().get_arc_count()}")

## ============================
## CREATE AND SOLVE GUROBI MODEL
## ============================

gurobi_sock = SOCKnapsackGurobi(sock_instance, continuous=False)
gurobi_sock.create_model()
gurobi_sock.optimize_with_cuts(dd_exact, cut_type=CutType.JOINT_FLOW, cut_strengthening=False)
