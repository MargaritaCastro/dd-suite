import os
import sys
current_dir = os.path.dirname(os.path.abspath(__file__))
sys.path.append(current_dir)

from Examples.GuiStructure import ProblemClass, GuiGurobiStructure
from Examples.GuiHandler import to_string_problem_class, gurobi_ilp

def main():
    if len(sys.argv) < 2:
        print("\nUsage: python main.py [filename] [flags]\n")
        print("\tflags:")
        print("\t\t-SetCover = Problem class: Set Cover")
        print("\t\t-Knapsack = Problem class: KnapsackInstance")
        print("\t\t-IndependentSet = Problem class: Independent Set")
        print("\t\t-SOCKnapsack = Problem class: SOC Knapsack")
        print("\t\t-Sequencing = Problem class: Sequencing")
        print("\t\t-Verbose  = Enable verbose mode")
        print("\t\t-Continuous = Continuous variables; if not selected, variables will be integers")
        print("\t\t-Output_ = Output file path")
        print("\n")
        sys.exit(1)

    source_directory: str = os.path.dirname(os.path.abspath(__file__))
    filename: str = sys.argv[1]
    output_name: str = ""

    # Initialize flags and parameters
    problem_class: ProblemClass = ProblemClass.NONE
    verbose_flag: bool = False
    continuous_flag: bool = False

    # Process command-line arguments
    for arg in sys.argv[2:]:
        if arg == "-SetCover":
            problem_class = ProblemClass.SET_COVER
        elif arg == "-Knapsack":
            problem_class = ProblemClass.KNAPSACK
        elif arg == "-IndependentSet":
            problem_class = ProblemClass.INDEPENDENT_SET
        elif arg == "-SOCKnapsack":
            problem_class = ProblemClass.SOC_KNAPSACK
        elif arg == "-Sequencing":
            problem_class = ProblemClass.SEQUENCING
        elif arg == "-Verbose":
            verbose_flag = True
        elif arg == "-Continuous":
            continuous_flag = True
        elif arg.startswith("-Output_"):
            output_name = arg[8:]

    # Validate problem class
    if problem_class == ProblemClass.NONE:
        print("Error: A valid problem class was not specified.")
        sys.exit(1)
    
    file_path: str = os.path.join(source_directory, filename)
    output_path: str = ""
    if output_name != "":
        if os.path.isabs(output_name):
            output_path = output_name
        else:
            output_path = os.path.join(source_directory, output_name)

    # Display configuration
    print("Running with the following parameters:")
    print(f"Input File: {filename}")
    print(f"Output File: {output_name}")
    print(f"Problem Class: {to_string_problem_class(problem_class)}")
    print(f"Verbose: {'Yes' if verbose_flag else 'No'}")
    print(f"Continuous: {'Yes' if continuous_flag else 'No'}")


    # Execute the process using GuiCutsStructure
    gui_gurobi_structure = GuiGurobiStructure(
        input_file_path=file_path,
        output_file_path=output_path,
        problem_class=problem_class,
        verbose=verbose_flag,
        continuous=continuous_flag
    )

    gurobi_ilp(gui_gurobi_structure)

if __name__ == "__main__":
    main()