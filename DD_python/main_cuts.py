import os
import sys

current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(current_dir)

from Examples.GuiStructure import ProblemClass, CutType, DDType, VALID_CUTS, GuiCutsStructure
from Examples.GuiHandler import to_string_problem_class, to_string_dd_type, run_gurobi_dd

def main():
    if len(sys.argv) < 2:
        print("\nUsage: python main_cuts.py [filename] [flags]\n")
        print("\tProblem class (required):")
        print("\t\t-SetCover        Set Cover")
        print("\t\t-Knapsack        Knapsack")
        print("\t\t-IndependentSet  Independent Set")
        print("\t\t-SOCKnapsack     SOC Knapsack")
        print("\t\t-Scheduler       Scheduler")
        print("\n\tCut type (default: first valid cut for the selected problem):")
        print("\t\t-FlowCuts        Combinatorial cuts from DD")
        print("\t\t-JointFlowCuts   Dual cuts from DD")
        print("\t\t-TargetCuts      Target cuts from relaxed DD")
        print("\n\tDD type (default: Exact):")
        print("\t\t-Exact           Build the exact DD (default)")
        print("\t\t-Restricted      Build a restricted DD (requires -Width_)")
        print("\t\t-RelaxPriority   Build a relaxed DD merging by priority (requires -Width_)")
        print("\t\t-RelaxGrouping   Build a relaxed DD merging in groups by priority difference (requires -Width_)")
        print("\t\t-Width_<n>       Maximum width per layer for restricted/relaxed DD")
        print("\n\tOptions:")
        print("\t\t-CutStrengthening Strengthen the cuts")
        print("\t\t-Verbose         Enable verbose mode")
        print("\t\t-Continuous      Solve LP relaxation (default: binary/MIP)")
        print("\t\t-Output_<name>   Output CSV file")
        print("\n")
        sys.exit(1)

    source_directory: str = os.path.dirname(os.path.abspath(__file__))
    filename: str = sys.argv[1]
    output_name: str = ""

    problem_class: ProblemClass = ProblemClass.NONE
    cut_type: CutType | None = None   # None = not yet specified; defaulted after validation
    verbose_flag: bool = False
    cut_strengthening_flag: bool = False
    continuous_flag: bool = False
    dd_type: DDType = DDType.EXACT
    max_width: int = 2147483646
    width_provided: bool = False

    for arg in sys.argv[2:]:
        if arg == "-SetCover":
            problem_class = ProblemClass.SET_COVER
        elif arg == "-Knapsack":
            problem_class = ProblemClass.KNAPSACK
        elif arg == "-IndependentSet":
            problem_class = ProblemClass.INDEPENDENT_SET
        elif arg == "-SOCKnapsack":
            problem_class = ProblemClass.SOC_KNAPSACK
        elif arg == "-Scheduler":
            problem_class = ProblemClass.SCHEDULER
        elif arg == "-FlowCuts":
            cut_type = CutType.FLOW
        elif arg == "-JointFlowCuts":
            cut_type = CutType.JOINT_FLOW
        elif arg == "-TargetCuts":
            cut_type = CutType.TARGET
        elif arg == "-Exact":
            dd_type = DDType.EXACT
        elif arg == "-Restricted":
            dd_type = DDType.RESTRICTED
        elif arg == "-RelaxPriority":
            dd_type = DDType.RELAX_PRIORITY
        elif arg == "-RelaxGrouping":
            dd_type = DDType.RELAX_GROUPING
        elif arg.startswith("-Width_"):
            max_width = int(arg[7:])
            width_provided = True
        elif arg == "-CutStrengthening":
            cut_strengthening_flag = True
        elif arg == "-Verbose":
            verbose_flag = True
        elif arg == "-Continuous":
            continuous_flag = True
        elif arg.startswith("-Output_"):
            output_name = arg[8:]

    if problem_class == ProblemClass.NONE:
        print("Error: A valid problem class was not specified.")
        sys.exit(1)

    # Restricted/Relaxed DDs are width-limited and require an explicit width.
    if dd_type in (DDType.RESTRICTED, DDType.RELAX_PRIORITY, DDType.RELAX_GROUPING) and not width_provided:
        print(f"Error: -{dd_type.value} requires a width. Add -Width_<n> (e.g. -Width_4000).")
        sys.exit(1)

    # Default to the first valid cut for the selected problem
    if cut_type is None:
        cut_type = VALID_CUTS[problem_class][0]

    # Validate cut against the problem's supported cuts
    if cut_type not in VALID_CUTS[problem_class]:
        valid_flags = [f"-{c.value}" for c in VALID_CUTS[problem_class]]
        print(f"Error: -{cut_type.value} is not valid for {to_string_problem_class(problem_class)}.")
        print(f"  Valid cuts: {', '.join(valid_flags)}")
        sys.exit(1)

    file_path: str = os.path.join(source_directory, filename)
    if output_name:
        output_path: str = output_name if os.path.isabs(output_name) else os.path.join(source_directory, output_name)
    else:
        output_path: str = ""

    print("Running with the following parameters:")
    print(f"Input File: {filename}")
    print(f"Output File: {output_name}")
    print(f"Problem Class: {to_string_problem_class(problem_class)}")
    print(f"Cut Type: {cut_type.value}")
    print(f"DD Type: {to_string_dd_type(dd_type)}")
    print(f"Maximum Width: {max_width}")
    print(f"Strength Cuts: {'Yes' if cut_strengthening_flag else 'No'}")
    print(f"Verbose: {'Yes' if verbose_flag else 'No'}")
    print(f"Continuous: {'Yes' if continuous_flag else 'No'}")

    gui_cuts_structure = GuiCutsStructure(
        input_file_path=file_path,
        output_file_path=output_path,
        problem_class=problem_class,
        cut_type=cut_type,
        cut_strengthening_flag=cut_strengthening_flag,
        verbose=verbose_flag,
        continuous_flag=continuous_flag,
        dd_type=dd_type,
        maximum_width=max_width,
    )

    run_gurobi_dd(gui_cuts_structure)

if __name__ == "__main__":
    main()