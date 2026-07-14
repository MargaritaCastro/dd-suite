import os
import sys

current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(current_dir)

from Examples.GuiStructure import ProblemClass, DDType, GuiStructure
from Examples.GuiHandler import to_string_dd_type, to_string_problem_class, run_dd_code

def main(argv):
    if len(argv) < 2:
        print("\nUsage: program_name [filename] [flags] \n")
        print("\tflags:")
        print("\t\t-SetCover = Problem class: Set Cover")
        print("\t\t-Knapsack = Problem class: KnapsackInstance")
        print("\t\t-IndependentSet = Problem class: Independent Set")
        print("\t\t-SOCKnapsack = Problem class: SOC Knapsack")
        print("\t\t-Scheduler = Problem class: Scheduler (single-machine)")
        print("\t\t-Exact = DD Type: Exact")
        print("\t\t-Restricted = DD Type: Restricted")
        print("\t\t-RelaxPriority = DD Type: Relaxed (merge nodes by priority)")
        print("\t\t-RelaxGrouping = DD Type: Relaxed (merge nodes in groups by priority difference)")
        print("\t\t-Width_<n> =  Maximum width for restricted/relaxed DD")
        print("\t\t-Reduce  = Enable reduction of decision diagram")
        print("\t\t-Verbose  = Enable verbose mode")
        print("\t\t-Export  = Enable export of decision diagram")
        print("\t\t-Max = Longest path; if blank, it will be the shortest path")
        print("\t\t-Min  = Shortest path")
        print("\t\t-Output_ = Output file path")
        print("\t\t-NoSort = Disable variable sorting")
        print("\n\n")
        return 1

    source_directory: str = os.path.dirname(os.path.abspath(__file__))
    filename: str = argv[1]
    output_name: str = ""

    problem_class: ProblemClass = ProblemClass.NONE
    dd_type: DDType = DDType.EXACT
    max_width: int = 2147483646
    width_provided: bool = False
    reduce_flag: bool = False
    verbose_flag: bool = False
    export_flag: bool = False
    minmax: str = "min"
    sort_flag: bool = True

    for arg in argv[2:]:
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
        elif arg == "-Reduce":
            reduce_flag = True
        elif arg == "-Verbose":
            verbose_flag = True
        elif arg == "-Export":
            export_flag = True
        elif arg.startswith("-Output_"):
            output_name = arg[8:]
        elif arg == "-Max":
            minmax = "max"
        elif arg == "-Min":
            minmax = "min"
        elif arg == "-NoSort":
            sort_flag = False

    if problem_class == ProblemClass.NONE:
        print("Error: A valid problem class was not specified.")
        sys.exit(1)
    
    # Restricted/Relaxed DDs are width-limited and require an explicit width.
    if dd_type in (DDType.RESTRICTED, DDType.RELAX_PRIORITY, DDType.RELAX_GROUPING) and not width_provided:
        print(f"Error: -{dd_type.value} requires a width. Add -Width_<n> (e.g. -Width_4000).")
        sys.exit(1)

    file_path: str = ""
    if os.path.isabs(filename):
        file_path = filename
    else:
        file_path = f"{source_directory}/{filename}"

    output_path: str = ""
    if output_name != "" :
        if os.path.isabs(output_name):
            output_path = output_name
        else:
            output_path = os.path.join(source_directory, output_name)

    # Mostrar la configuración
    print("Running with the following parameters:")
    print(f"Input File: {filename}")
    print(f"Output File: {output_name}")
    print(f"Problem Class: {to_string_problem_class(problem_class)}")
    print(f"DD Type: {to_string_dd_type(dd_type)}")
    print(f"Maximum Width: {max_width}")
    print(f"Reduce: {'Yes' if reduce_flag else 'No'}")
    print(f"Verbose: {'Yes' if verbose_flag else 'No'}")
    print(f"Export: {'Yes' if export_flag else 'No'}")
    print(f"Min/Max?: {minmax}")
    print(f"Sort: {'Yes' if sort_flag else 'No'}")

    gui_structure = GuiStructure(
        input_file_path=str(file_path),
        output_file_path=str(output_path),
        dd_type=dd_type,
        verbose=verbose_flag,
        export_flag=export_flag,
        reduce_flag=reduce_flag,
        maximum_width=max_width,
        problem_class=problem_class,
        minmax=minmax,
        sort=sort_flag,
    )

    run_dd_code(gui_structure)

if __name__ == "__main__":
    sys.exit(main(sys.argv))
