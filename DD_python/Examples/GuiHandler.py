import os
import sys
import math
from pathlib import Path

current_dir = os.path.dirname(os.path.abspath(__file__))
root_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(root_dir)

from SourceCode.DD import DD
from SourceCode.GraphAlgorithms.ShortestLongestPath.ShortestLongestPath import ShortestLongestPath

from Examples.GuiStructure import ProblemClass, CutType, DDType, GuiStructure, GuiCutsStructure, GuiGurobiStructure
from Examples.KnapsackInstance.KnapsackInstance import KnapsackStructure
from Examples.KnapsackInstance.KnapsackProblem import KnapsackProblem
from Examples.KnapsackInstance.KnapsackGurobiClass import KnapsackGurobi
from Examples.SetCoverInstance.SetCoverInstance import SetCoverStructure
from Examples.SetCoverInstance.SetCoverProblem import SetCoverProblem
from Examples.SetCoverInstance.SetCoverGurobiClass import SetCoverGurobi
from Examples.IndependentSetInstance.IndependentSetInstance import IndependentSetStructure
from Examples.IndependentSetInstance.IndependentSetProblem import IndependentSetProblem
from Examples.IndependentSetInstance.IndependentSetGurobiClass import IndependentSetGurobi
from Examples.SOCKnapsack.SOCKnapsackInstance import SOCKnapsackInstance
from Examples.SOCKnapsack.SOCKnapsackProblem import SOCKnapsackProblem
from Examples.SOCKnapsack.SOCKnapsackGurobiClass import SOCKnapsackGurobi
from Examples.SchedulerInstance.SchedulerInstance import SchedulerStructure
from Examples.SchedulerInstance.SchedulerProblem import SchedulerProblem
from Examples.SchedulerInstance.SchedulerPathSolver import SchedulerPathSolver
from Examples.SchedulerInstance.SchedulerGurobiClass import SchedulerGurobi

tolerance = 1e-6

def to_string_problem_class(problem_class: ProblemClass) -> str:
    """
    Get the string representation of a ProblemClass value.
    """
    if problem_class == ProblemClass.SET_COVER:
        return "Set Cover"
    elif problem_class == ProblemClass.KNAPSACK:
        return "Knapsack"
    elif problem_class == ProblemClass.INDEPENDENT_SET:
        return "Independent Set"
    elif problem_class == ProblemClass.SOC_KNAPSACK:
        return "SOC Knapsack"
    elif problem_class == ProblemClass.SCHEDULER:
        return "Scheduler"
    else:
        return "None"

def to_string_dd_type(dd_type: DDType) -> str:
    """
    Get the string representation of a DDType value.
    """
    if dd_type == DDType.EXACT:
        return "Exact"
    elif dd_type == DDType.RESTRICTED:
        return "Restricted"
    elif dd_type == DDType.RELAX_PRIORITY:
        return "RelaxPriority"
    elif dd_type == DDType.RELAX_GROUPING:
        return "RelaxGrouping"
    else:
        return "None"

def is_integer(val) -> bool:
    """
    Verify if a value is an integer considering a tolerance (tolerance).
    """
    return abs(math.floor(val) - val) < tolerance or abs(math.ceil(val) - val) < tolerance

def are_all_variables_integer(model) -> bool:
    """
    Verifies if all variables in a Gurobi model have integer values.
    """
    for var in model.getVars():
        value = var.X
        if not is_integer(value):
            return False
    return True

def write_statistics(file_path, statistics, headers) -> None:
    """
    Write statistics in an specified file.
    """

    try:
        if os.path.exists(file_path):
            with open(file_path, "r") as file:

                if file.readline().strip() != headers.strip():
                    print(f"Error: this file is already in use by another program, please erease the file {file_path} or give another output_file.")
                    exit(1)

        write_header = not os.path.exists(file_path) or os.path.getsize(file_path) == 0
        with open(file_path, "a") as file:
            if write_header:
                file.write(headers)
                file.write("\n")

            file.write(statistics)

    except IOError:
        print(f"Error: could not open file {file_path}")
        exit(1)

def create_and_solve_dd(problem_instance: 'AbstractProblem', objective_weights: list[int], gui_structure: GuiStructure, solver_class=None) -> None:
    '''
    Create and solve a decision diagram for a given problem instance.
    '''
    sorted_obj = [objective_weights[i] for i in problem_instance.dd_to_original_index]
    dd_instance: DD = DD(problem_instance)
    headers: str = "File_name,Problem type,Variables number,DD Type,Max Width,Is Reduced,Objective Type,Optimization Value,Construction Time,Reduce Time,Solution Time,Number Node,Number Arcs,Actual Max Width,Sort"

    construction_time:float = 0.0
    if gui_structure.dd_type == DDType.EXACT:
        dd_instance.create_decision_diagram(gui_structure.verbose)
    
    elif gui_structure.dd_type == DDType.RESTRICTED:
        dd_instance.create_restricted_decision_diagram(gui_structure.maximum_width, gui_structure.verbose)
    
    elif gui_structure.dd_type == DDType.RELAX_PRIORITY:
        dd_instance.create_relax_priority_decision_diagram(gui_structure.maximum_width, gui_structure.verbose)

    elif gui_structure.dd_type == DDType.RELAX_GROUPING:
        dd_instance.create_relax_grouping_decision_diagram(gui_structure.maximum_width, gui_structure.verbose)

    reduction_time: str = 'na'
    if gui_structure.reduce_flag:
        dd_instance.reduce_decision_diagram(gui_structure.verbose)
        reduction_time = dd_instance.get_reduction_time()
        
    if gui_structure.export_flag:
        dd_instance.export_graph_file("graph")

    path_solver_class = solver_class if solver_class is not None else ShortestLongestPath
    shortest_longest_path_instance = path_solver_class(dd_instance)
    shortest_longest_path_instance.set_parameters(sorted_obj, gui_structure.minmax)
    shortest_longest_path_instance.solve()

    statistics: str = (f'{Path(gui_structure.input_file_path).name}'
                f',{to_string_problem_class(gui_structure.problem_class)}'
                f',{len(problem_instance.ordered_variables)}'
                f',{to_string_dd_type(gui_structure.dd_type)}'
                f',{gui_structure.maximum_width}'
                f',{gui_structure.reduce_flag}'
                f',{gui_structure.minmax}'
                f',{shortest_longest_path_instance.get_solution().value}'
                f',{dd_instance.get_building_time()}'
                f',{reduction_time}'
                f',{shortest_longest_path_instance.get_time()}'
                f',{str(dd_instance.get_decision_diagram().get_node_count())}'
                f',{str(dd_instance.get_decision_diagram().get_arc_count())}'
                f',{str(dd_instance.get_decision_diagram().get_actual_max_width())}'
                f',{gui_structure.sort}\n')

    if gui_structure.output_file_path:
        write_statistics(gui_structure.output_file_path, statistics, headers)

def run_dd_code(gui_structure: GuiStructure) -> None:
    '''
    Run the decision diagram params for a given problem instance.
    '''
    if gui_structure.problem_class == ProblemClass.SET_COVER:
        set_cover_parameters: SetCoverStructure = SetCoverStructure(gui_structure.input_file_path)
        problem_instance: SetCoverProblem = SetCoverProblem(set_cover_parameters, sort=gui_structure.sort)
        create_and_solve_dd(problem_instance, set_cover_parameters.objective_weights, gui_structure)

    elif gui_structure.problem_class == ProblemClass.INDEPENDENT_SET:
        independent_set_parameters: IndependentSetStructure = IndependentSetStructure(gui_structure.input_file_path)
        problem_instance: IndependentSetProblem = IndependentSetProblem(independent_set_parameters, sort=gui_structure.sort)
        create_and_solve_dd(problem_instance, independent_set_parameters.objective_weights, gui_structure)

    elif gui_structure.problem_class == ProblemClass.KNAPSACK:
        knapsack_parameters: KnapsackStructure = KnapsackStructure(gui_structure.input_file_path)
        problem_instance: KnapsackProblem = KnapsackProblem(knapsack_parameters, sort=gui_structure.sort)
        create_and_solve_dd(problem_instance, knapsack_parameters.objective_weights, gui_structure)

    elif gui_structure.problem_class == ProblemClass.SOC_KNAPSACK:
        sock_params: SOCKnapsackInstance = SOCKnapsackInstance(gui_structure.input_file_path)
        problem_instance: SOCKnapsackProblem = SOCKnapsackProblem(sock_params, 0, sort=gui_structure.sort)
        create_and_solve_dd(problem_instance, sock_params.objective_weights, gui_structure)

    elif gui_structure.problem_class == ProblemClass.SCHEDULER:
        scheduler_parameters: SchedulerStructure = SchedulerStructure(gui_structure.input_file_path)
        problem_instance: SchedulerProblem = SchedulerProblem(scheduler_parameters, sort=gui_structure.sort)
        create_and_solve_dd(problem_instance, scheduler_parameters.objective_weights, gui_structure, solver_class=SchedulerPathSolver)

    elif gui_structure.problem_class == ProblemClass.NONE:
        print("Error: No problem class specified")
        exit(1)

def _build_dd_for_cuts(dd_instance: 'DD', gui_cuts_structure: GuiCutsStructure) -> None:
    '''
    Build the decision diagram for cut generation according to the requested DD type.
    Restricted/relaxed are width-limited; exact ignores the width. The DD is reduced afterwards.
    '''
    if gui_cuts_structure.dd_type == DDType.RESTRICTED:
        dd_instance.create_restricted_decision_diagram(gui_cuts_structure.maximum_width, gui_cuts_structure.verbose)
    elif gui_cuts_structure.dd_type == DDType.RELAX_PRIORITY:
        dd_instance.create_relax_priority_decision_diagram(gui_cuts_structure.maximum_width, gui_cuts_structure.verbose)
    elif gui_cuts_structure.dd_type == DDType.RELAX_GROUPING:
        dd_instance.create_relax_grouping_decision_diagram(gui_cuts_structure.maximum_width, gui_cuts_structure.verbose)
    else:  # DDType.EXACT
        dd_instance.create_decision_diagram(gui_cuts_structure.verbose)
    dd_instance.reduce_decision_diagram(False)

def gurobi_dd_cutting_planes(gui_cuts_structure: GuiCutsStructure, gurobi_instance: 'GurobiClass', problem_instance: 'AbstractProblem') -> None:
    '''
    Run the Gurobi code with DD-based cutting planes (callback approach).
    '''
    headers: str = "File_name,Problem type,Variables number,DD Type,Max Width,Cut Type,CutStrengthening,Best solution,Root Gap,Gap,Solution Time,DD Time,Number Nodes,Number Arcs,NodeBB,Num Cuts"

    dd_instance: DD = DD(problem_instance)
    _build_dd_for_cuts(dd_instance, gui_cuts_structure)

    cut_type: CutType = gui_cuts_structure.cut_type
    cut_strengthening: bool = gui_cuts_structure.cut_strengthening_flag

    gurobi_instance.create_model()

    gurobi_instance.optimize_with_cuts([dd_instance], cut_type=cut_type, cut_strengthening=cut_strengthening)

    root_gap: float = gurobi_instance.get_root_gap()
    final_gap: float = gurobi_instance.get_final_gap()

    statistics: str = (
        f'{Path(gui_cuts_structure.input_file_path).name},'
        f'{to_string_problem_class(gui_cuts_structure.problem_class)},'
        f'{len(problem_instance.ordered_variables)},'
        f'{to_string_dd_type(gui_cuts_structure.dd_type)},'
        f'{gui_cuts_structure.maximum_width},'
        f'{cut_type.value},'
        f'{cut_strengthening},'
        f'{gurobi_instance.model.objVal},'
        f'{root_gap},'
        f'{final_gap},'
        f'{gurobi_instance.get_optimization_time()},'
        f'{dd_instance.get_building_time() + dd_instance.get_reduction_time()},'
        f'{dd_instance.get_decision_diagram().get_node_count()},'
        f'{dd_instance.get_decision_diagram().get_arc_count()},'
        f'{int(gurobi_instance.model.NodeCount)},'
        f'{gurobi_instance.num_cuts_added}\n'
    )
    if gui_cuts_structure.output_file_path:
        write_statistics(gui_cuts_structure.output_file_path, statistics, headers)

def gurobi_dd_cutting_planes_soc(gui_cuts_structure: GuiCutsStructure, gurobi_instance: SOCKnapsackGurobi, sock_params: SOCKnapsackInstance) -> None:
    '''
    Run the Gurobi code with DD-based cutting planes for SOC Knapsack.
    Builds one DD per SOC constraint, then optimizes with cuts and saves statistics.
    '''
    headers: str = "File_name,Problem type,Variables number,DD Type,Max Width,Num SOC constraints,Cut Type,CutStrengthening,Best solution,Root Gap,Gap,Solution Time,DD Time,Total Number Nodes,Total Number Arcs,NodeBB,Num Cuts"

    num_constraints: int = len(sock_params.right_side_of_restrictions)
    dds: list = []
    for i in range(num_constraints):
        prob: SOCKnapsackProblem = SOCKnapsackProblem(sock_params, i)
        dd: DD = DD(prob)
        _build_dd_for_cuts(dd, gui_cuts_structure)
        dds.append(dd)

    gurobi_instance.create_model()
    gurobi_instance.optimize_with_cuts(
        dds,
        cut_type=gui_cuts_structure.cut_type,
        cut_strengthening=gui_cuts_structure.cut_strengthening_flag
    )

    total_dd_time: float = sum(dd.get_building_time() + dd.get_reduction_time() for dd in dds)
    total_nodes: int = sum(dd.get_decision_diagram().get_node_count() for dd in dds)
    total_arcs: int = sum(dd.get_decision_diagram().get_arc_count() for dd in dds)

    root_gap: float = gurobi_instance.get_root_gap()
    final_gap: float = gurobi_instance.get_final_gap()

    statistics: str = (
        f'{Path(gui_cuts_structure.input_file_path).name},'
        f'{to_string_problem_class(gui_cuts_structure.problem_class)},'
        f'{len(sock_params.variables)},'
        f'{to_string_dd_type(gui_cuts_structure.dd_type)},'
        f'{gui_cuts_structure.maximum_width},'
        f'{num_constraints},'
        f'{gui_cuts_structure.cut_type.value},'
        f'{gui_cuts_structure.cut_strengthening_flag},'
        f'{gurobi_instance.model.objVal},'
        f'{root_gap},'
        f'{final_gap},'
        f'{gurobi_instance.get_optimization_time()},'
        f'{total_dd_time},'
        f'{total_nodes},'
        f'{total_arcs},'
        f'{int(gurobi_instance.model.NodeCount)},'
        f'{gurobi_instance.num_cuts_added}\n'
    )
    if gui_cuts_structure.output_file_path:
        write_statistics(gui_cuts_structure.output_file_path, statistics, headers)


def run_gurobi_dd(gui_cuts_structure: GuiCutsStructure) -> None:
    '''
    Create the instances necessary for gurobi_dd_cutting_planes     
    '''

    if gui_cuts_structure.problem_class == ProblemClass.SET_COVER:
        set_cover_parameters: SetCoverStructure = SetCoverStructure(gui_cuts_structure.input_file_path)
        gurobi_instance: SetCoverGurobi = SetCoverGurobi(set_cover_parameters, gui_cuts_structure.continuous_flag)
        problem_instance: SetCoverProblem = SetCoverProblem(set_cover_parameters)
        gurobi_dd_cutting_planes(gui_cuts_structure, gurobi_instance, problem_instance)

    elif gui_cuts_structure.problem_class == ProblemClass.INDEPENDENT_SET:
        independent_set_parameters: IndependentSetStructure = IndependentSetStructure(gui_cuts_structure.input_file_path)
        gurobi_instance: IndependentSetGurobi = IndependentSetGurobi(independent_set_parameters, gui_cuts_structure.continuous_flag)
        problem_instance: IndependentSetProblem = IndependentSetProblem(independent_set_parameters)
        gurobi_dd_cutting_planes(gui_cuts_structure, gurobi_instance, problem_instance)

    elif gui_cuts_structure.problem_class == ProblemClass.KNAPSACK:
        knapsack_parameters: KnapsackStructure = KnapsackStructure(gui_cuts_structure.input_file_path)
        gurobi_instance: KnapsackGurobi = KnapsackGurobi(knapsack_parameters, gui_cuts_structure.continuous_flag)
        problem_instance: KnapsackProblem = KnapsackProblem(knapsack_parameters)
        gurobi_dd_cutting_planes(gui_cuts_structure, gurobi_instance, problem_instance)

    elif gui_cuts_structure.problem_class == ProblemClass.SOC_KNAPSACK:
        sock_params: SOCKnapsackInstance = SOCKnapsackInstance(gui_cuts_structure.input_file_path)
        gurobi_instance: SOCKnapsackGurobi = SOCKnapsackGurobi(sock_params, gui_cuts_structure.continuous_flag)
        gurobi_dd_cutting_planes_soc(gui_cuts_structure, gurobi_instance, sock_params)

    elif gui_cuts_structure.problem_class == ProblemClass.SCHEDULER:
        scheduler_parameters: SchedulerStructure = SchedulerStructure(gui_cuts_structure.input_file_path)
        gurobi_instance: SchedulerGurobi = SchedulerGurobi(scheduler_parameters, gui_cuts_structure.continuous_flag)
        problem_instance: SchedulerProblem = SchedulerProblem(scheduler_parameters)
        gurobi_dd_cutting_planes(gui_cuts_structure, gurobi_instance, problem_instance)

    elif gui_cuts_structure.problem_class == ProblemClass.NONE:
        print("Error: No problem class specified")
        exit(1)

def gurobi_ilp(gui_gurobi_structure: GuiGurobiStructure) -> None:
    '''
    Create de gurobi problem for a given problem instance.
    '''
    gurobi_instance: 'AbstractProblemGurobi' = None
    headers: str = "File_name,Problem type,Variables number,Best solution,Dual bound,Root Gap,Gap,Solution Time,NodeBB"

    if gui_gurobi_structure.problem_class == ProblemClass.SET_COVER:
        set_cover_parameters: SetCoverStructure = SetCoverStructure(gui_gurobi_structure.input_file_path)
        gurobi_instance: SetCoverGurobi = SetCoverGurobi(set_cover_parameters, gui_gurobi_structure.continuous)

    elif gui_gurobi_structure.problem_class == ProblemClass.INDEPENDENT_SET:
        independent_set_parameters: IndependentSetStructure = IndependentSetStructure(gui_gurobi_structure.input_file_path)
        gurobi_instance: IndependentSetGurobi = IndependentSetGurobi(independent_set_parameters, gui_gurobi_structure.continuous)

    elif gui_gurobi_structure.problem_class == ProblemClass.KNAPSACK:
        knapsack_parameters: KnapsackStructure = KnapsackStructure(gui_gurobi_structure.input_file_path)
        gurobi_instance: KnapsackGurobi = KnapsackGurobi(knapsack_parameters, gui_gurobi_structure.continuous)
        
    elif gui_gurobi_structure.problem_class == ProblemClass.SOC_KNAPSACK:
        sock_params: SOCKnapsackInstance = SOCKnapsackInstance(gui_gurobi_structure.input_file_path)
        gurobi_instance: SOCKnapsackGurobi = SOCKnapsackGurobi(sock_params, gui_gurobi_structure.continuous)

    elif gui_gurobi_structure.problem_class == ProblemClass.SCHEDULER:
        scheduler_parameters: SchedulerStructure = SchedulerStructure(gui_gurobi_structure.input_file_path)
        gurobi_instance: SchedulerGurobi = SchedulerGurobi(scheduler_parameters, gui_gurobi_structure.continuous)

    elif gui_gurobi_structure.problem_class == ProblemClass.NONE:
        print("Error: No problem class specified")
        exit(1)

    gurobi_instance.create_model()
    gurobi_instance.optimize_model()

    root_gap: float = gurobi_instance.get_root_gap()
    final_gap: float = gurobi_instance.get_final_gap()

    # At the time limit Gurobi may stop with no feasible incumbent; in that case
    # objVal raises. Write the row anyway (best solution = 'na') so unfinished
    # instances still produce a record, just like the cut-based runs.
    best_solution = gurobi_instance.model.objVal if gurobi_instance.model.SolCount > 0 else 'na'

    statistics: str = (
        f'{Path(gui_gurobi_structure.input_file_path).name},'
        f'{to_string_problem_class(gui_gurobi_structure.problem_class)},'
        f'{gurobi_instance.variable_length},'
        f'{best_solution},'
        f'{gurobi_instance.model.ObjBound},'
        f'{root_gap},'
        f'{final_gap},'
        f'{gurobi_instance.get_optimization_time()},'
        f'{int(gurobi_instance.model.NodeCount)}\n'
    )

    if gui_gurobi_structure.output_file_path:
        write_statistics(gui_gurobi_structure.output_file_path, statistics, headers)
