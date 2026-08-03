# Decision Diagram Program

The Decision Diagram Program is a versatile implementation designed to solve optimization problems using Decision Graphs. This program facilitates the construction, reduction, restriction, relaxation, visualization and other alghorithms of decision diagrams, which are powerful tools in optimization.

## Table of Contents

1. [Requisites](#requisites)
2. [Installation](#installation)
3. [Usage guide for the programs](#usage-guide-for-the-programs)
4. [Getting Started in the Creation of a New Problem](#getting-started-in-the-creation-of-a-new-problem)
5. [Features](#features)
6. [Heuristics](#heuristics)
7. [Examples](#examples)
8. [Test casses](#test-casses)
9. [Extending the code](#extending_the_code)

## Requisites

Make sure you have in your system

* python > 3.11
* Gurobi python Api version > 11.0.0
* bitarray > 3.0.0

For a complete use of the program, it is recommended to have a tool that can read documents with a .gml extension. yED is a suitable program for this purpose.

## Installation

Clone this repository and navigate to the project directory to implement the main program. To review examples, go to the '/Examples/' directory, where you'll find three folders with different examples to execute from de main, main_cuts and main_gurobi files.

## Usage Guide for the Programs

For the three programs you must to run the program from the command line using the following syntax:

```
python program_name [filename] [flags]
```

Parameters:

* **filename**: The input file containing problem data.
* **flags**: Optional flags to specify the problem class, decision diagram type, and other options.

#### Error Handling

For all the programs if no valid problem class is specified in, the program will display an error message:

```
Error: A valid problem class was not specified.
```

Ensure that at least one of the problem class flags (`-SetCover`, `-Knapsack`, or `-IndependentSet`) is included in the command.

### main

In this executable, you can choose to create the decision diagram in three different forms (Exact, Restricted, and Relaxed), then reduce it, export it, or obtain the minimum or maximum path. Additionally, all this information, along with execution times, can be saved in an output file specified via the command line.

#### Command-Line Flags

Below is a list of available flags and their descriptions:

| Flag                | Description                                          |
| ------------------- | ---------------------------------------------------- |
| `-SetCover`       | Specifies the problem class as Set Cover             |
| `-Knapsack`       | Specifies the problem class as Knapsack Instance     |
| `-IndependentSet` | Specifies the problem class as Independent Set       |
| `-SOCKnapsack`    | Specifies the problem class as SOC Knapsack          |
| `-Sequencing`     | Specifies the problem class as Sequencing            |
| `-Exact`          | Specifies the decision diagram type as Exact         |
| `-Restricted`     | Specifies the decision diagram type as Restricted    |
| `-RelaxPriority`  | Specifies the DD type as Relaxed (merge nodes by priority)              |
| `-RelaxGrouping`  | Specifies the DD type as Relaxed (merge nodes in groups by priority difference) |
| `-Width_`         | Sets the maximum width for Restricted/Relaxed DD     |
| `-Reduce`         | Enables reduction of the decision diagram            |
| `-Verbose`        | Enables verbose mode for detailed output             |
| `-Export`         | Enables export of the decision diagram               |
| `-Max`            | Computes the longest path (default is shortest path) |
| `-Min`            | Computes the shortest path                           |
| `-NoSort`         | Disables the variable-ordering heuristic             |
| `-Output_`        | Specifies the output file path                       |

Flags are matched by exact string, so the capitalisation above matters: `-width_100` or
`-output_x.txt` are silently ignored.

#### Example Execution

```
python main.py example_input.txt -SetCover -Exact -Width_100 -Reduce -Verbose -Output_result.txt
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

Notes

* The program appends the current directory to the Python path to ensure that the necessary modules can be imported.
* Default values are used for unspecified parameters (e.g., decision diagram type defaults to Exact, and maximum width defaults to a large integer value).
* If the `-output_`flag is omitted, no output file will be generated.

### main_gurobi

With this executable, you can choose to create Gurobi optimization models for each of the three specified examples. Additionally, all this information, along with execution times, can be saved in an output file specified via the command line.

#### Command-Line Flags

Below is a list of available flags and their descriptions:

| Flag                | Description                                            |
| ------------------- | ------------------------------------------------------ |
| `-SetCover`       | Specifies the problem class as Set Cover               |
| `-Knapsack`       | Specifies the problem class as Knapsack Instance       |
| `-IndependentSet` | Specifies the problem class as Independent Set         |
| `-SOCKnapsack`    | Specifies the problem class as SOC Knapsack            |
| `-Sequencing`     | Specifies the problem class as Sequencing              |
| `-Verbose`        | Enables verbose mode for detailed output               |
| `-Continuous`     | Uses continuous variables instead of integer variables |
| `-Output_`        | Specifies the output file path                         |

#### Example Execution

Here is an example command to run the program:

```
python main_gurobi.py example_input.txt -SetCover -Verbose -Continuous -Output_solution.txt
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

* The program automatically appends the current directory to the Python path to ensure all necessary modules are accessible.
* Default behavior uses integer variables unless the `-Continuous` flag is provided.
* If the `-output_`flag is omitted, no output file will be generated.

### main_cuts

In this executable, an integer solution (integer variable values) can be obtained for each of the examples. This is achieved by generating, strengthening, and adding cuts (Flow Cuts or JointFlow Cuts) to the diagrams and their Gurobi equivalents. Additionally, all this information, along with execution times, can be saved in an output file specified via the command line.

#### Command-Line Flags

Below is a list of available flags and their descriptions:

| Flag                | Description                                            |
| ------------------- | ------------------------------------------------------ |
| `-SetCover`       | Specifies the problem class as Set Cover               |
| `-Knapsack`       | Specifies the problem class as Knapsack Instance       |
| `-IndependentSet` | Specifies the problem class as Independent Set         |
| `-SOCKnapsack`    | Specifies the problem class as SOC Knapsack            |
| `-Sequencing`     | Specifies the problem class as Sequencing              |
| `-Exact`          | Builds the cut-generating DD as Exact                  |
| `-Restricted`     | Builds the cut-generating DD as Restricted             |
| `-RelaxPriority`  | Builds the cut-generating DD as Relaxed (by priority)  |
| `-RelaxGrouping`  | Builds the cut-generating DD as Relaxed (by grouping)  |
| `-Width_`         | Sets the maximum width of the cut-generating DD        |
| `-Verbose`        | Enables verbose mode for detailed output               |
| `-Continuous`     | Uses continuous variables instead of integer variables |
| `-FlowCuts`       | Select combinatorial cuts to the model.                |
| `-JointFlowCuts`  | Select dual cuts to the model                          |
| `-TargetCuts`     | Select target cuts from relaxed MDD (all problems)     |
| `-CutStrengthening` | Lifts every generated inequality into a tighter one  |
| `-Output_`        | Specifies the output file path                         |

#### Example Execution

```
python main_cuts.py example_file.txt -Knapsack -Continuous -FlowCuts -Verbose -Output_result.txt
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

* The program automatically appends the current directory to the Python path to ensure all necessary modules are accessible.
* If the `-output_`flag is omitted, no output file will be generated.
* Default behavior uses integer variables unless the `-Continuous` flag is provided.
* Default behavior uses Flow Cuts unless de `-JointFlow` flag is provided

## Getting Started in the Creation of a New Problem

First, proceed with constructing of a **Problem** class, which must inherit from **AbstractProblem** (imported from `SourceCode.Problems.AbstractProblemClass`). It is crucial to define the following functions for this class:

### Required Functions for the **Problem** Class

#### **transition_function**

This function defines the transition logic between states. It takes as inputs:

* `previus_state` ('State'): The previous state of the problem.
* `variable_id` (str): The identifier of the variable (e.g., "x_2").
* `variable_value` (int): The domain that was assigned to this variable.

It returns a tuple containing a new state and a boolean value indicating whether the new state is feasible or not.

If you want to obtain the restricted diagram, it is necessary to implement the following function when constructing your **Problem** class.

#### **get_priority_for_discard_node**

This function determines the priority for discarding a node. It takes:

* `state` ('State'): The state of the node.

It returns an integer representing the priority for discarding the node.

Furthermore, to obtain the relaxed diagram, it is necessary to add the following two functions in the **Problem** class.

#### **get_priority_for_merge_nodes**

This function specifies the priority for merging nodes. It takes:

* `id` (int): The ID of the node.
* `state` ('State'): The state of the node.

It returns an integer representing the priority for merging the node with others.

#### **merge_operator**

This function defines how two nodes should be merged. It takes:

* `state_one` ('State'): The first state.
* `state_two` ('State'): The second state.

It returns a new state resulting from the merge of the two input states.

Additional  Neccesary Functions

#### **get_state_as_string**

This function converts a state to its string representation. It takes:

* `state` ('State'): The state of a node.

It returns a string representation of the state.

#### **get_state_copy**

This function provides a copy of a given state. It takes:

* `state` ('State'): The state to copy.

It returns a new, independent copy of the state, ensuring it is not linked to the original by references.

#### get_final_state

This function retrieves the final state of the graph. If it is not implemented, the initial state will be used as the default

It returns a new 'State' corresponding to the final node of the graph.

### Summary of Steps to Solve the Problem

1. **Construct the ProblemClass:** Define the class and implement the required functions.
2. **Create an Instance of ProblemClass** Instantiate the class by passing the initial state, list of variables and domains, and other neccesary params.
3. **Create an instance of the DD class**: to which the instance of the created problem should be provided.
4. **Pass the Instance to the DD Class:** Use this instance to generate a decision diagram. The DD class offers functions to create, reduce, restrict, relax, and visualize the diagram.

By following these steps, you can construct and solve the problem efficiently using the provided framework.

## Features

#### Create Decision Diagram

To create the decision diagram, is necessary to use *create_decision_diagram()* from the **DD** class, which saved the graph class object  when creating the diagram. This new version is then stored within the **DD** class. It is important to note that for each decision diagram (DD), only one graph can be created in either exact, restricted, or relaxed form. If a graph already exists, creating a new one of a different type will not be allowed.

#### Create Restricted Decision Diagram

To create the restricted decision diagram, is necesary use *create_restricted_decision_diagram()* from the **DD** class, which saved the graph class object  when creating the diagram. This new version is then stored within the **DD** class. It is important to note that for each decision diagram (DD), only one graph can be created in either exact, restricted, or relaxed form. If a graph already exists, creating a new one of a different type will not be allowed.

#### Create Relax Decision Diagram

To create the relax decision diagram, is necesary use *create_relaxed_decision_diagram()* from the **DD** class, which transforms the graph class object saved when creating the diagram into its relax form. This new version is then stored within the **DD** class. It is important to note that for each decision diagram (DD), only one graph can be created in either exact, restricted, or relaxed form. If a graph already exists, creating a new one of a different type will not be allowed.

#### Reduce Decision Diagram

To create the reduced decision diagram, is necesary use *reduce_decision_diagram()* from the **DD** class, which transforms the graph class object saved when creating the diagram into its reduced form. This new version is then stored within the **DD** class. To use this feature, a graph must already be created beforehand.

#### Export Decision Diagram

To export the created decision diagram, whether it's reduced or in its original format, should be use the *export_graph_file()* method of the **DD** class. This generates a *.gml* file that can be visualized in programs such as *yED. To use this feature, a graph must already be created beforehand.*

#### Get he Decision Diagram

To obtain the graph instance, being a pointer to the original object, you should use the *get_decision_diagram()* method of the **DD** class. This feature can be useful to apply other alghorithm to the graph.

#### Get a Copy Of the Decision Diagram

To obtain a copy of the Graph instance without it being a pointer to the original object, you should use the *get_decision_diagram_copy()* method of the **DD** class. This feature can be useful for testing different constructors or functions to solve the diagram.

#### Shortest/Longest Path of Decision Diagram

The first step in using this feature is to create an instance of **ShortestLongestPath**, providing the previously created dd_instance as an argument. Next, you need to set the parameters for solving the graph using the method  *ShortestLongestPath.set_parameters()* . This method expects two inputs: a list containing the weights of each variable in the objective function and a string indicating whether to maximize or minimize the objective. Acceptable string values are `'min'` or `'max'`.

Once the parameters are set, you can solve the shortest or longest path on the decision diagram by calling the method  *ShortestLongestPath.solve_path()* . After solving, you can retrieve the solution using the *get_path()* method of the same class. This method returns a structure containing the following attributes:

* path_print: A string representation of the path.
* path_arcs: A list of the arcs in the path.
* final_value: The final computed value of the solution.

#### Obtaining the Maximum Flow of the Decision Diagram

The **MaxFlow** class is responsible for solving the maximum flow problem in a graph using the Ford-Fulkerson algorithm. It seeks to find the maximum flow that can pass through a graph from a starting node to an end node.

To obtain the maximum flow of the decision diagram, it is first necessary to create an instance of the **MaxFlow** class, providing the dd_instance instance created earlier. Then, by using the *solve_max_flow()* method and providing the maximum capacities for each arc, you will get a float that represents the maximum flow of the diagram. It's also possible to obtain the remaining capacities of the arcs using *get_remaining_arcs_capacities()*. Once the maximum flow has been calculated, the value can be retrieved using the *get_max_flow()* method.

#### Obtaining the Maximum Flow of the Gurobi Model representation

The *MaxFlowGurobi* class is designed to solve the maximum flow problem using a Gurobi model representation of a decision diagram. Its objective is to determine the maximum flow that can traverse the graph from a source node to a target node. Unlike the **MaxFlow** class, which uses a custom algorithm implementation, this class leverages Gurobi's optimization engine to compute the solution.

To obtain the maximum flow of the decision diagram, it is first necessary to create an instance of the **MaxFlow** class, providing the dd_instance instance created earlier. Then, by using the *solve_max_flow()* method and providing the maximum capacities for each arc, you will get a float that represents the maximum flow of the diagram. It's also possible to obtain the remaining capacities of the arcs using *get_remaining_arcs_capacities()*. Once the maximum flow has been calculated, the value can be retrieved using the *get_max_flow()* method.

#### Get a Cut of the Decision Diagram

Three cut generators are available, all sharing the same interface:

- **FlowCuts**: combinatorial cuts derived from a min-cut on the DD. Requires a binary DD (BDD).
- **JointFlowCuts**: dual flow cuts, typically tighter than FlowCuts. Requires a binary DD (BDD).
- **TargetCut**: target cuts from a relaxed MDD (Tjandraatmadja & van Hoeve, 2019). Works for any MDD, including problems with multi-valued domains (e.g., Sequencing). Computes the geometric center ω of conv(S) and solves an LP to separate x̄ from the set.

To use any of them, instantiate the class with the `dd_instance` created earlier, then call `generate_cut(x_values)`. The method returns `True` if a cut was found. Retrieve it with `get_cut()` (returns `(coefficients, constant)`). For `FlowCuts`, `get_min_cut()` returns the minimum cut value.

`TargetCut` also exposes `is_equality_cut()` to distinguish between inequality cuts (LP optimal) and equality cuts (LP unbounded, occurs when x̄ is infeasible for the MDD).

#### Get a CutStrengthening of the Decision Diagram

To strengthen a cut, it is necessary to use the **CutStrengthening** class, which should be provided the dd_instance created earlier during instantiation. Then, the *cut_strengthening()* method should be used, to which a valid cut for the diagram must be provided. This method returns a boolean indicating whether the strengthening of the cut was successful. If so, the *get_cut_strengthening()* method should be used to obtain the new inequality.

#### Get the Time of the Algorithms

To get the time taken by the diagram construction use `get_building_time()` from the **DD** class. For all other algorithms, use `get_time()` from the respective class. This is valid for `ShortestLongestPath`, `MaxFlow`, `MaxFlowGurobi`, `FlowCuts`, `JointFlowCuts`, `TargetCut`, and `CutStrengthening`.

## Heuristics

The program provides the capability to implement different heuristics, which can be either ordering, elimination, or merging heuristics. To achieve this, the variables given to the DD (Decision Diagram) must be arranged in a specific order, meaning the **variables** vector of the problem should be in the desired order to traverse the different levels.

Additionally, for implementing the elimination and merge heuristics, the Problem class must include the **get_priority_for_discard_node** and **get_priority_for_merge_nodes** functions, respectively. The first function should return a numerical value representing the priority for eliminating a node, while the second should return a numerical value representing the priority for merging two nodes. It is crucial to emphasize that these heuristics must be static, as the objective function for each decision will not be known in advance.

### Ordering Heuristic and `dd_to_original_index`

When `sort=True`, the problem class reorders variables internally via `sort_variables()`. After construction, DD layer 0 corresponds to the variable at position 0 in the new order, which may differ from the original input order.

`AbstractProblem` exposes a public attribute `dd_to_original_index: list[int]` that maps each DD position to its original variable index:

```
dd_to_original_index[i]  →  original index of the variable at DD layer i
```

**Any parameter that depends on variable order must be reindexed before use.** For example, if you have `objective_weights` aligned with the original variable order:

```python
sorted_weights = [objective_weights[i] for i in problem.dd_to_original_index]
```

This is done automatically inside `create_and_solve_dd` for the objective weights passed to `ShortestLongestPath`. If you implement your own solving loop, you are responsible for applying the same reindexing to any order-dependent parameters.

`sort_variables()` must update `self.dd_to_original_index` to reflect the new order. The base class initializes it as the identity `[0, 1, ..., n-1]`, which is correct when no reordering occurs.

## Examples

For a more comprehensive understanding of these classes, it is recommended to thoroughly review the examples available in the **`Examples/`** folder within the code. Reference implementations are in:

| Folder                      | Problem          | State type              |
|-----------------------------|------------------|-------------------------|
| `KnapsackInstance/`         | Knapsack         | `int`                   |
| `IndependentSetInstance/`   | Independent Set  | `bitarray`              |
| `SetCoverInstance/`         | Set Cover        | `bitarray`              |
| `SOCKnapsack/`              | SOCKnapsack      | `list[float]`           |
| `SequencingInstance/`        | Sequencing        | `tuple(frozenset,…)`    |

Each folder contains a `*Problem` (DD logic), `*Instance` (file parser), and `*GurobiClass` (Gurobi model).

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
