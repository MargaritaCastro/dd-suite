'''
Quick benchmark comparing the two relaxed DD builders on medium Knapsack instances:
  - RelaxPriority: merges the highest-priority nodes pairwise (RelaxedPriorityDDBuilder)
  - RelaxGrouping: merges nodes in groups by priority difference (RelaxedGroupingDDBuilder)

For each instance it builds the relaxed DD with both methods at a few maximum widths and
reports construction time, node/arc counts and the longest-path bound (which is a valid upper
bound for the max knapsack; both methods stay valid even if they differ).

Usage:
    python benchmark_relax.py
'''
import os
import time

from Examples.KnapsackInstance.KnapsackInstance import KnapsackStructure
from Examples.KnapsackInstance.KnapsackProblem import KnapsackProblem
from SourceCode.DD import DD
from SourceCode.GraphAlgorithms.ShortestLongestPath.ShortestLongestPath import ShortestLongestPath

SOURCE_DIR = os.path.dirname(os.path.abspath(__file__))
INSTANCE_DIR = os.path.join(SOURCE_DIR, "..", "DataInstances", "Knapsack", "Standard")

# Medium instances: 200, 500 and 1000 items.
INSTANCES = [
    "knapPI_1_200_1000_1.txt",
    "knapPI_1_500_1000_1.txt",
    "knapPI_1_1000_1000_1.txt",
]

WIDTHS = [100, 500, 1000]

METHODS = {
    "RelaxPriority": "create_relax_priority_decision_diagram",
    "RelaxGrouping": "create_relax_grouping_decision_diagram",
}


def build_and_measure(path: str, method_name: str, max_width: int):
    '''
    Build the relaxed DD with the given method and return its metrics.
    '''
    params = KnapsackStructure(path)
    problem = KnapsackProblem(params, sort=True)
    sorted_obj = [params.objective_weights[i] for i in problem.dd_to_original_index]

    dd = DD(problem)
    start = time.perf_counter()
    getattr(dd, method_name)(max_width, False)
    build_time = time.perf_counter() - start

    graph = dd.get_decision_diagram()
    nodes = graph.get_node_count()
    arcs = graph.get_arc_count()

    path_solver = ShortestLongestPath(dd)
    path_solver.set_parameters(sorted_obj, "max")
    bound = path_solver.solve().value

    return build_time, nodes, arcs, bound


def main():
    header = f"{'Instance':<24}{'Width':>7}{'Method':>16}{'Build(s)':>11}{'Nodes':>9}{'Arcs':>9}{'Bound':>12}"
    print(header)
    print("-" * len(header))

    for instance in INSTANCES:
        path = os.path.join(INSTANCE_DIR, instance)
        if not os.path.exists(path):
            print(f"  (missing instance {instance}, skipping)")
            continue

        for width in WIDTHS:
            results = {}
            for label, method_name in METHODS.items():
                build_time, nodes, arcs, bound = build_and_measure(path, method_name, width)
                results[label] = (build_time, nodes, arcs, bound)
                print(f"{instance:<24}{width:>7}{label:>16}{build_time:>11.4f}{nodes:>9}{arcs:>9}{bound:>12.1f}")

            tp = results["RelaxPriority"][0]
            tg = results["RelaxGrouping"][0]
            if tg > 0:
                faster = "grouping" if tg < tp else "priority"
                print(f"{'':<24}{width:>7}{'-> speedup':>16}{tp / tg:>11.2f}x  ({faster} faster)")
            print()


if __name__ == "__main__":
    main()
