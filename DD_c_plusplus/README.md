# Decision Diagram Program (C++)

This program provides a versatile C++ implementation of Decision Diagrams for solving optimization problems, including Knapsack, Independent Set, Set Cover, and SOCKnapsack.

## Table of Contents

1. [Requisites](#requisites)
2. [Build](#build)
3. [Usage guide for the programs](#usage-guide-for-the-programs)
4. [Getting Started in the Creation of a New Problem](#getting-started-in-the-creation-of-a-new-problem)
5. [Features](#features)
6. [Heuristics](#heuristics)
7. [Examples](#examples)
8. [Test cases](#test-cases)
9. [Extending the code](#extending-the-code)

## Requisites

Make sure you have in your system:

* C++ > 17
* CMake > 3.27
* Boost > 1.86.0
* Gurobi C++ API > 13.0.2
* Google Test > 1.15.2 (for unit tests)

For a complete use of the program, it is recommended to have a tool that can read `.gml` files. yED is a suitable program for this purpose.

## Build

Clone this repository and navigate to the project directory to implement the main program. To review examples, go to the '/Examples/' directory, where you'll find three folders with different examples to execute from de main, main_cuts and main_gurobi files.

From the `DD_c_plusplus/` directory, all targets are available through `make`:

```bash
make build                   # Configure and compile everything
make test                    # Run Google Test suite
make clean                   # Remove build directory

# Run main (DD only)
make main_knapsack           # Knapsack
make main_independent_set    # Independent Set
make main_set_cover          # Set Cover
make main_socknapsack        # SOCKnapsack
make main_sequencing          # Sequencing

# Run main_gurobi (Gurobi baseline)
make gurobi_knapsack
make gurobi_independent_set
make gurobi_set_cover
make gurobi_socknapsack
make gurobi_sequencing

# Run main_cuts (DD + Gurobi with cuts)
make cuts_knapsack
make cuts_independent_set
make cuts_set_cover
make cuts_socknapsack
make cuts_sequencing

# Memory leak checks (macOS leaks tool)
make leaks_main
make leaks_cuts
make leaks_gurobi
```

Gurobi is auto-detected from `/Library/gurobi*/`; override with `-DGUROBI_HOME=<path>` if needed.

## Usage Guide for the Programs

The three CLI programs (`main`, `main_gurobi`, `main_cuts`) are run from the command line:

```
./program_name [filename] [flags]
```

* **filename**: path to the input instance file.
* **flags**: control problem class, DD type, and other options.

If no valid problem class is specified, the program exits with:

```
Error: A valid problem class was not specified.
```

Ensure that at least one of the problem class flags (`-SetCover`, `-Knapsack`, or `-IndependentSet`) is included in the command.
-----------------------------------------------------------------------------------------

### main

Creates a decision diagram (Exact, Restricted, or Relaxed), optionally reduces it, exports it, and computes the shortest or longest path. Results can be saved to an output CSV.

#### Command-Line Flags

| Flag                | Description                               |
| ------------------- | ----------------------------------------- |
| `-SetCover`       | Problem class: Set Cover                  |
| `-Knapsack`       | Problem class: Knapsack                   |
| `-IndependentSet` | Problem class: Independent Set            |
| `-SOCKnapsack`    | Problem class: SOC Knapsack               |
| `-Sequencing`     | Problem class: Sequencing                 |
| `-Exact`          | DD type: Exact                            |
| `-Restricted`     | DD type: Restricted                       |
| `-RelaxPriority`  | DD type: Relaxed (merge nodes by priority)              |
| `-RelaxGrouping`  | DD type: Relaxed (merge nodes in groups by priority difference) |
| `-Width_N`        | Maximum width N for Restricted/Relaxed DD |
| `-Reduce`         | Enable reduction of the diagram           |
| `-Verbose`        | Enable verbose output                     |
| `-Export`         | Export the diagram as a `.gml` file     |
| `-Max`            | Compute longest path                      |
| `-Min`            | Compute shortest path                     |
| `-NoSort`         | Disable the variable-ordering heuristic   |
| `-Output_PATH`    | Write statistics to output file PATH      |

Flags are matched by exact string, so the capitalisation above matters.

#### Example

```bash
./Main ../DataInstances/Knapsack/Custom/knapsack_instance_v4_d4_seed1.txt -Knapsack -Exact -Width_200 -Reduce -Max
```

#### Output Example

When the program runs, it displays the following information:

```
Running with the following parameters:
Input File: example_input.txt
Output File: result.txt
Problem Class: Set Cover
DD Type: Exact
Maximum Width: 100
Reduce: Yes
Verbose: Yes
Export: No
Min/Max?: min
```

* The program automatically determines the current directory and appends it to the input and output file paths to ensure all necessary files are correctly located.
* Default values are used for unspecified parameters (e.g., decision diagram type defaults to Exact, and maximum width defaults to a large integer value).
* If the `-Output_` flag is omitted, no output file will be generated.

---

### main_gurobi

With this executable, you can create Gurobi optimization models for any of the three specified problem classes: **Set Cover**, **Knapsack**, and **Independent Set**. Additionally, all the information, including execution times, can be saved to an output file specified via the command line.

#### Command-Line Flags

| Flag                | Description                                            |
| ------------------- | ------------------------------------------------------ |
| `-SetCover`       | Problem class: Set Cover                               |
| `-Knapsack`       | Problem class: Knapsack                                |
| `-IndependentSet` | Problem class: Independent Set                         |
| `-SOCKnapsack`    | Problem class: SOC Knapsack                            |
| `-Sequencing`     | Problem class: Sequencing                              |
| `-Verbose`        | Enable verbose output                                  |
| `-Continuous`     | Use continuous relaxation instead of integer variables |
| `-Output_PATH`    | Write statistics to output file PATH                   |

#### Example

```bash
./MainGurobi ../DataInstances/IndependentSet/Custom/independent_set_instance_v5_d3_seed1.txt -IndependentSet
```

#### Output Example

When the program runs, it displays the following information:

```
Running with the following parameters:
Input File: example_input.txt
Output File: solution.txt
Problem Class: Set Cover
Verbose: Yes
Continuous: Yes
```

#### Notes

* The program automatically determines the current directory and appends it to the input and output file paths to ensure all necessary files are correctly located.
* Default behavior uses integer variables unless the `-Continuous` flag is provided.
* If the `-Output_` flag is omitted, no output file will be generated.

---

### main_cuts

In this executable, an integer solution (integer variable values) can be obtained for each of the examples. This is achieved by generating, strengthening, and adding cuts (Flow Cuts or JointFlow Cuts) to the diagrams and their Gurobi equivalents. Additionally, all this information, along with execution times, can be saved in an output file specified via the command line.

#### Command-Line Flags

| Flag                | Description                                            |
| ------------------- | ------------------------------------------------------ |
| `-SetCover`       | Problem class: Set Cover                               |
| `-Knapsack`       | Problem class: Knapsack                                |
| `-IndependentSet` | Problem class: Independent Set                         |
| `-SOCKnapsack`    | Problem class: SOC Knapsack                            |
| `-Sequencing`     | Problem class: Sequencing                              |
| `-Exact` / `-Restricted` / `-RelaxPriority` / `-RelaxGrouping` | DD type used to generate the cuts |
| `-Width_N`        | Maximum width N of the cut-generating DD               |
| `-Verbose`        | Enable verbose output                                  |
| `-Continuous`     | Use continuous relaxation instead of integer variables |
| `-FlowCuts`       | Use combinatorial flow cuts                            |
| `-JointFlowCuts`  | Use joint (dual) flow cuts                             |
| `-TargetCuts`     | Use target cuts from relaxed MDD (all problems)        |
| `-CutStrengthening` | Apply cut strengthening after generation             |
| `-NoSort`         | Disable the variable-ordering heuristic                |
| `-Output_PATH`    | Write statistics to output file PATH                   |

#### Example

```bash
./MainCuts ../DataInstances/Knapsack/Custom/knapsack_instance_v4_d4_seed1.txt -Knapsack -JointFlowCuts -CutStrengthening
```

#### Example Execution

```
./MainCuts example_file.txt -Knapsack -Continuous -FlowCuts -Verbose -Output_result.txt
```

#### Output Example

When the program runs, it displays the following information:

```
Running with the following parameters:
Input File: example_input.txt
Output File: result.txt
Problem Class: Set Cover
Continuous Variables: yes
Flow Cuts: No
JointFlow Cuts: Yes
Continuous: Yes
Verbose: Yes
```

#### Notes

* The program automatically determines the current directory and appends it to the input and output file paths to ensure all necessary files are correctly located.
* If the `-Output_` flag is omitted, no output file will be generated.
* Default behavior uses integer variables unless the `-Continuous` flag is provided.
* Default behavior uses Flow Cuts unless the `-JointFlowCuts` or `-TargetCuts` flag is provided

---

## Getting Started in the Creation of a New Problem

First, proceed with constructing of a **Problem** class, which must inherit from **AbstractProblem** (imported from `SourceCode.Problems.AbstractProblemClass`). First, when writing the file, it is necessary to define the data type that will be used in the state. This should be done in the inheritance of AbstractProblem (e.g., "AbstractProblem<vector `<int>`>" or "AbstractProblem `<string>`"). If you prefer to use a custom class for the State, it's important to note that it must implement the '==' operator. It is crucial to define the following functions for this class:

The following methods must be implemented:

#### `transition_function`

Defines state transitions. Inputs:

* `previous_state` (`const T*`): the current node state.
* `variable_index` (`int`): 0-based index of the variable being assigned.
* `variable_value` (`int`): value assigned from the variable's domain.

Returns `pair<T*, bool>`: the new state (heap-allocated) and whether it is feasible.

#### `get_priority_for_discard_node`

Priority for node elimination in the Restricted DD. Inputs:

* `state` (`const T*`): node state.

Returns `int`: lower value = higher discard priority.

#### `get_priority_for_merge_nodes`

Priority for node merging in the Relaxed DD. Inputs:

* `node_id` (`int`): node identifier.
* `state` (`const T*`): node state.

Returns `int`.

#### `merge_operator`

Combines two states into one for the Relaxed DD. Inputs:

* `state_one`, `state_two` (`const T*`).

Returns `T*` (heap-allocated merged state).

#### `get_state_as_string`

Converts a state to string (used for node deduplication during construction). Returns `string`.

#### `get_state_copy`

Returns a deep copy of a state. Returns `T*` (heap-allocated).

#### `get_final_state` *(optional)*

Returns the state to assign to the terminal node. Defaults to a copy of `initial_state` if not overridden. Returns `T*` (heap-allocated).

### Steps to Solve a Problem

1. Implement the `AbstractProblem<T>` subclass.
2. Instantiate it with `initial_state`, `variables` (name + domain pairs), and any problem-specific data.
3. Create a `DD<T>` instance passing the problem object.
4. Call `create_decision_diagram()`, `create_restricted_decision_diagram()`, or `create_relaxed_decision_diagram()`.
5. Optionally call `reduce_decision_diagram()`.
6. Use `ShortestLongestPath`, `FlowCuts`, or Gurobi classes to solve.

---

## Features

#### Create Decision Diagram

To create the decision diagram, is necessary to use *create_decision_diagram()* from the **DD** class, which saved the graph class object  when creating the diagram. This new version is then stored within the **DD** class. It is important to note that for each decision diagram (DD), only one graph can be created in either exact, restricted, or relaxed form. If a graph already exists, creating a new one of a different type will not be allowed.

#### Create Restricted Decision Diagram

To create the restricted decision diagram, is necesary use *create_restricted_decision_diagram()* from the **DD** class, which saved the graph class object  when creating the diagram. This new version is then stored within the **DD** class. It is important to note that for each decision diagram (DD), only one graph can be created in either exact, restricted, or relaxed form. If a graph already exists, creating a new one of a different type will not be allowed.

#### Create Relaxed Decision Diagram

To create the relax decision diagram, is necesary use *create_relaxed_decision_diagram()* from the **DD** class, which transforms the graph class object saved when creating the diagram into its relax form. This new version is then stored within the **DD** class. It is important to note that for each decision diagram (DD), only one graph can be created in either exact, restricted, or relaxed form. If a graph already exists, creating a new one of a different type will not be allowed.

#### Reduce Decision Diagram

To create the reduced decision diagram, is necesary use *reduce_decision_diagram()* from the **DD** class, which transforms the graph class object saved when creating the diagram into its reduced form. This new version is then stored within the **DD** class. To use this feature, a graph must already be created beforehand.

#### Export Decision Diagram

To export the created decision diagram, whether it's reduced or in its original format, should be use the *export_graph_file()* method of the **DD** class. This generates a *.gml* file that can be visualized in programs such as *yED. To use this feature, a graph must already be created beforehand.*

#### Get the Decision Diagram

To obtain the graph instance, being a pointer to the original object, you should use the *get_decision_diagram()* method of the **DD** class. This feature can be useful to apply other alghorithm to the graph.

#### Get a Copy of the Decision Diagram

To obtain a copy of the Graph instance without it being a pointer to the original object, you should use the *get_decision_diagram_copy()* method of the **DD** class. This feature can be useful for testing different constructors or functions to solve the diagram.

#### Solve the Decision Diagram

The first step in using this feature is to create an instance of **ShortestLongestPath `<state_type>`**, providing the previously created dd_instance as an argument. Next, you need to set the parameters for solving the graph using the method  *ShortestLongestPath.set_parameters()* . This method expects two inputs: a list containing the weights of each variable in the objective function and a string indicating whether to maximize or minimize the objective. Acceptable string values are `'min'` or `'max'`.

Once the parameters are set, you can solve the shortest or longest path on the decision diagram by calling the method  *ShortestLongestPath.solve()* . After solving, you can retrieve the solution using the *get_solution()* method of the same class. This method returns a structure containing the following attributes:

* path_print: A string representation of the path.
* path_arcs: A list of the arcs in the path.
* value: The final computed value of the solution.

#### Obtaining the Maximum Flow of the Decision Diagram

The **MaxFlow** class is responsible for solving the maximum flow problem in a graph using the Ford-Fulkerson algorithm. It seeks to find the maximum flow that can pass through a graph from a starting node to an end node.

To obtain the maximum flow of the decision diagram, it is first necessary to create an instance of the **MaxFlow** class, providing the dd_instance instance created earlier. Then, by using the *solve_max_flow()* method and providing the maximum capacities for each arc, you will get a float that represents the maximum flow of the diagram. It's also possible to obtain the remaining capacities of the arcs using *get_remaining_arcs_capacities()*. Once the maximum flow has been calculated, the value can be retrieved using the *get_max_flow()* method.

#### Obtaining the Maximum Flow of the Gurobi Model representation

The *MaxFlowGurobi* class is designed to solve the maximum flow problem using a Gurobi model representation of a decision diagram. Its objective is to determine the maximum flow that can traverse the graph from a source node to a target node. Unlike the **MaxFlow** class, which uses a custom algorithm implementation, this class leverages Gurobi's optimization engine to compute the solution.
To obtain the maximum flow of the decision diagram, it is first necessary to create an instance of the **MaxFlow** class, providing the dd_instance instance created earlier. Then, by using the *solve_max_flow()* method and providing the maximum capacities for each arc, you will get a float that represents the maximum flow of the diagram. It's also possible to obtain the remaining capacities of the arcs using *get_remaining_arcs_capacities()*. Once the maximum flow has been calculated, the value can be retrieved using the *get_max_flow()* method.

#### Get a Cut of the Decision Diagram

Three cut generators are available, all inheriting from `AbstractCutGenerator<T>` and sharing the same interface:

- **FlowCuts**: combinatorial cuts derived from a min-cut on the DD. Requires a binary DD (BDD).
- **JointFlowCuts**: dual flow cuts, typically tighter than FlowCuts. Requires a binary DD (BDD).
- **TargetCut**: target cuts from a relaxed MDD (Tjandraatmadja & van Hoeve, 2019). Works for any MDD, including problems with multi-valued domains (e.g., Sequencing). Computes the geometric center ω of conv(S) and solves an LP to separate x̄ from the set.

To use any of them, instantiate the class with the `dd_instance`, then call `generate_cut(x_values)`. The method returns `true` if a cut was found. Retrieve it with `get_cut()` (returns `pair<vector<double>, double>`). For `FlowCuts`, `get_min_cut()` returns the minimum cut value.

`TargetCut` also exposes `is_equality_cut()` to distinguish between inequality cuts (LP optimal) and equality cuts (LP unbounded, occurs when x̄ is infeasible for the MDD).

#### Get a CutStrengthening of the Decision Diagram

To strengthen a cut, it is necessary to use the **CutStrengthening** class, which should be provided the dd_instance created earlier during instantiation. Then, the *cut_strengthening()* method should be used, to which a valid cut for the diagram must be provided. This method returns a boolean indicating whether the strengthening of the cut was successful. If so, the *get_cut_strengthening()* method should be used to obtain the new inequality.

#### Get the Time of the Algorithms

To get the time taken by diagram construction use `get_building_time()` and `get_reduction_time()` from the **DD** class. For all other algorithms, use `get_time()` from the respective class. This is valid for `ShortestLongestPath`, `MaxFlow`, `MaxFlowGurobi`, `FlowCuts`, `JointFlowCuts`, `TargetCut`, and `CutStrengthening`.
--------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------

## Heuristics

The program provides the capability to implement different heuristics, which can be either ordering, elimination, or merging heuristics. To achieve this, the variables given to the DD (Decision Diagram) must be arranged in a specific order, meaning the **variables** vector of the problem should be in the desired order to traverse the different levels.

Additionally, for implementing the elimination and merge heuristics, the Problem class must include the **get_priority_for_discard_node** and **get_priority_for_merge_nodes** functions, respectively. The first function should return a numerical value representing the priority for eliminating a node, while the second should return a numerical value representing the priority for merging two nodes. It is crucial to emphasize that these heuristics must be static, as the objective function for each decision will not be known in advance.

### Variable Ordering Heuristic

To support variable ordering, the derived problem class must override `sort_variables` **and** call the following snippet at the end of its own constructor:

```cpp
if (sort) {
    apply_variable_order(sort_variables(variables));
}
```

This call **cannot** be placed in the `AbstractProblem` base constructor. In C++, virtual dispatch is not available during base-class construction — the derived class vtable is not yet active, so calling `sort_variables()` from `AbstractProblem`'s constructor would invoke the base implementation instead of the overridden one. Additionally, because `AbstractProblem` is a template whose implementation lives in a `.tpp` file, the compiler instantiates all template code at compile time; without the call in the derived constructor the overridden `sort_variables` is never instantiated for that concrete type.

The `sort` parameter is typically a `bool` received by the derived constructor so that callers can opt in or out of the reordering at construction time.

### `dd_to_original_index` — adjusting order-dependent parameters

When `sort=true`, variables are reordered internally. DD layer 0 corresponds to the variable at position 0 in the new order, which may differ from the original input order.

`AbstractProblem` exposes a public member `vector<int> dd_to_original_index` that maps each DD position to its original variable index:

```
dd_to_original_index[i]  →  original index of the variable at DD layer i
```

**Any parameter that depends on variable order must be reindexed before use.** For example, if you have `objective_weights` aligned with the original variable order:

```cpp
vector<double> sorted_weights(problem->dd_to_original_index.size());
for (int i = 0; i < (int)problem->dd_to_original_index.size(); ++i)
    sorted_weights[i] = objective_weights[problem->dd_to_original_index[i]];
```

This is done automatically inside `create_and_solve_dd` for the objective weights passed to `ShortestLongestPath`. If you implement your own solving loop, you are responsible for applying the same reindexing to any order-dependent parameters.

`sort_variables()` must populate `dd_to_original_index` to reflect the new order. The base class initializes it as the identity `{0, 1, ..., n-1}`, which is correct when no reordering occurs.
--------------------------------------------------------------------------------------------------------------------------------------------

## Examples

For a more comprehensive understanding of these classes, it is recommended to thoroughly review the examples available in the **"/Examples/"** folder within the code, and execute **Knapsack/KnapsackMain, IndependentSet/IndependentSetMain** or **SetCover/SetCoverMain.** These examples are generic and open for testing different values. In light of the above, the examples will be explained below.

### Knapsack

Within the **KnapsackMain** file, it can be noticed that it is a generalized version of a linear problem. For this reason, it is possible to input the weights of the variables in the different constraints. This should be done in the *weights* parameter, and it is also necessary to provide the value on the right side of the constraints in the *capacity* parameter. With these two values, an instance of Knapsack can be created. However, to generate a decision diagram, it is necessary to provide the initial state in *initial_state* and the variables with their domain in *variables.* After defining all the parameters and delivering them in the creation of an instance of the KnapsackProblem, it is ready to test the various features that were explained earlier. Importantly, it should be noted that there are some checks in place to ensure there is no inconsistency in the provided data.

### IndependentSet

Similar to the previous case, it can be observed that in the IndependentSetMain file, it corresponds to a generalization of the IndependentSet problem. Therefore, it is possible to test different values. To do so, it is necessary to provide a dictionary in which the *key* is the variable's ID, and the *value* corresponds to a list of all nodes, that can be reached in a single arc, formatted as integers. Once the variables are selected to instantiate the IndependentSet, it is necessary to create the variables to build a decision diagram. These include *initial_state* as a bitarray on one hand and, on the other hand, the variable IDs with their domain in *variables.* After providing all the parameters to the instance of the constructed problem class, you can test the other features.

### SetCover

This example represents a generalization of the set cover problem, allowing for different values to test various cases. To construct it, various values need to be provided. First, the initial state should be bitarray where active bit represents the number of constraints to be given, for example, "111". Additionally, a dictionary of variables along with their domains must be provided. Specifically for this problem, the capacity is the minimum value for each constraint, associated through their positions. Lastly, weights is a list of lists where the values represent the probability of each variable (associated by position) being within the constraint. After providing all these parameters to the instance of the constructed problem class, you can test the other features.

## Test Casses

Test cases have been created for the examples of problems explained earlier. These test implemented all the features and can be used to verify that different changes implemented continue to provide correct responses.

## Extending the Code

It is important to emphasize that all classes along with their modules have been documented for a more detailed understanding. This can be useful if it's want to create new objective functions or diagram constructors, which are expected to follow the same directory structure.
