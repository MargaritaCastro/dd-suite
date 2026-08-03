import io
import os
import sys
import unittest
from types import SimpleNamespace
from unittest.mock import patch
from contextlib import contextmanager
from bitarray import bitarray

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

from SourceCode.DD import DD
from SourceCode.DDStructure.Node import Node
from SourceCode.GraphAlgorithms.ShortestLongestPath.ShortestLongestPath import ShortestLongestPath

from Examples.IndependentSetInstance.IndependentSetProblem import IndependentSetProblem
from Test.dd_controlled_generators.DDIndependentSetSort import (
    get_exact_dd_independent_set_sort,
    get_relaxed_dd_independent_set_sort,
    get_restricted_dd_independent_set_sort
)

@contextmanager
def assertNoRaise():
    try:
        yield
    except Exception as e:
        raise AssertionError(f"An exception was raised: {e}")


class IndependentSetProblemSortTest(unittest.TestCase):
    def setUp(self):
        params = SimpleNamespace(
            initial_state=bitarray("11111"),
            variables=[('x_1', [0, 1]), ('x_2', [0, 1]), ('x_3', [0, 1]), ('x_4', [0, 1]), ('x_5', [0, 1])],
            dict_node_neighbors={'x_1': [2, 3], 'x_2': [1, 3, 4], 'x_3': [1, 2, 4], 'x_4': [2, 3, 5], 'x_5': [4]},
        )
        self.problem_instance: 'AbstractProblem' = IndependentSetProblem(params, sort=True)
        self.dd_instance: DD = DD(self.problem_instance)

    def get_value_path_solution(self):
        shortest_longest_path_instance = ShortestLongestPath(self.dd_instance)
        shortest_longest_path_instance.set_parameters([3, 4, 2, 2, 7], "max")
        answer = shortest_longest_path_instance.solve()
        return answer.value, answer.path_print

    def test_ordered_variables(self):
        # sort=True: sorted descending by number of neighbors
        # x_2:3, x_3:3, x_4:3, x_1:2, x_5:1
        ordered_variables_test: list[str] = ['x_2', 'x_3', 'x_4', 'x_1', 'x_5']
        self.assertEqual(self.problem_instance.ordered_variables, ordered_variables_test)

    def test_variables_domain(self):
        variables_domain_test: dict[str, list[int]] = {'x_1': [0, 1], 'x_2': [0, 1], 'x_3': [0, 1], 'x_4': [0, 1], 'x_5': [0, 1]}
        self.assertEqual(self.problem_instance.variables_domain, variables_domain_test)

    def test_transition_frunction(self):
        previous_state: bitarray = bitarray("11111")
        out = [bitarray("11111")]
        feasibility = self.problem_instance.transition_function(previous_state, 0, 1, out)
        self.assertEqual(out[0], bitarray("00001"))
        self.assertTrue(feasibility)

    def test_get_priority_for_discard_node(self):
        state: bitarray = bitarray("01100")
        priority: int = self.problem_instance.get_priority_for_discard_node(state)
        self.assertEqual(priority, 2)

        state2: bitarray = bitarray("01110")
        priority2: int = self.problem_instance.get_priority_for_discard_node(state2)
        self.assertEqual(priority2, 3)

    def test_get_priority_for_merge_nodes(self):
        state: bitarray = bitarray("01100")
        priority: int = self.problem_instance.get_priority_for_merge_nodes(2, state)
        self.assertEqual(priority, 2)

        state2: bitarray = bitarray("01110")
        priority2: int = self.problem_instance.get_priority_for_merge_nodes(4, state2)
        self.assertEqual(priority2, 3)

    def test_merge_operator(self):
        state_one: bitarray = bitarray("10100")
        state_two: bitarray = bitarray("01010")
        merged_state: bitarray = self.problem_instance.merge_operator(state_one, state_two)
        self.assertEqual(self.problem_instance.get_state_as_string(merged_state), "11110")

        state_one2: bitarray = bitarray("10010")
        state_two2: bitarray = bitarray("01010")
        merged_state2: bitarray = self.problem_instance.merge_operator(state_one2, state_two2)
        self.assertEqual(self.problem_instance.get_state_as_string(merged_state2), "11010")

    def test_get_state_as_string(self):
        state: bitarray = bitarray("10100")
        state_string: str = self.problem_instance.get_state_as_string(state)
        self.assertEqual(state_string, "10100")

    def test_get_state_copy(self):
        state: bitarray = bitarray("10100")
        state_copy: bitarray = self.problem_instance.get_state_copy(state)

        self.assertNotEqual(id(state), id(state_copy))
        self.assertEqual(self.problem_instance.get_state_as_string(state_copy), "10100")

        state[2] = False
        self.assertNotEqual(self.problem_instance.get_state_as_string(state), self.problem_instance.get_state_as_string(state_copy))

    def test_get_final_state(self):
        final_state: bitarray = self.problem_instance.get_final_state()
        self.assertEqual(self.problem_instance.get_state_as_string(final_state), "00000")

    def test_is_dd_created(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertIsNotNone(self.dd_instance.get_decision_diagram())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_dd(self, mock_stdout):
        self.dd_instance.create_decision_diagram(True)

        file_path: str = os.path.join('Test', 'txt_files', 'createDDIndependentSetSort.txt')

        with open(file_path, "r") as file:
            expected_output = file.read()

        actual_output: str = mock_stdout.getvalue()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_create_dd_graph_equal(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_exact_dd_independent_set_sort())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_reduce_dd(self, mock_stdout):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=True)

        file_path: str = os.path.join('Test', 'txt_files', 'createReduceDDIndependentSetSort.txt')

        with open(file_path, "r") as file:
            expected_output = file.read()

        actual_output: str = mock_stdout.getvalue()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_create_reduce_dd_graph_equal(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=False)
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_exact_dd_independent_set_sort())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_restricted_dd(self, mock_stdout):
        self.dd_instance.create_restricted_decision_diagram(verbose=True, max_width=2)

        file_path: str = os.path.join('Test', 'txt_files', 'createRestrictedDDIndependentSetSort.txt')

        with open(file_path, "r") as file:
            expected_output: str = file.read()

        actual_output: str = mock_stdout.getvalue()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_create_restricted_dd_graph_equal(self):
        self.dd_instance.create_restricted_decision_diagram(verbose=False, max_width=2)
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_restricted_dd_independent_set_sort())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_relaxed_dd(self, mock_stdout):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=True, max_width=2)

        file_path: str = os.path.join('Test', 'txt_files', 'createRelaxedDDIndependentSetSort.txt')

        with open(file_path, "r") as file:
            expected_output: str = file.read()

        actual_output: str = mock_stdout.getvalue()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_create_relaxed_dd_graph_equal(self):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=False, max_width=2)
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_relaxed_dd_independent_set_sort())

    def test_get_dd_graph(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertIsNotNone(self.dd_instance.get_decision_diagram())

    def test_get_copy(self):
        self.dd_instance.create_decision_diagram(False)
        original_graph: 'Graph' = self.dd_instance.get_decision_diagram()
        copied_graph: 'Graph' = self.dd_instance.get_decision_diagram_copy()

        self.assertIsNot(original_graph, copied_graph)
        self.assertEqual(original_graph, copied_graph)

        original_graph.structure[0][0].set_id(100)
        self.assertNotEqual(
            original_graph.structure[0][0].to_string(),
            copied_graph.structure[0][0].to_string()
        )

        new_node: 'Node' = Node(11, [1, 2, 3, 4, 5])
        copied_graph.add_node(new_node)

        self.assertNotEqual(
            len(original_graph.structure[-1]),
            len(copied_graph.structure[-1])
        )

    def test_get_DDBuilder_time(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertTrue(self.dd_instance.get_building_time() >= 0)

    def test_get_ReduceDDBuilder_time(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=False)
        self.assertTrue(self.dd_instance.get_reduction_time() >= 0)

    def test_get_RestrictedDDBuilder_time(self):
        self.dd_instance.create_restricted_decision_diagram(verbose=False, max_width=2)
        self.assertTrue(self.dd_instance.get_building_time() >= 0)

    def test_get_RelaxedDDBuilder_time(self):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=False, max_width=2)
        self.assertTrue(self.dd_instance.get_building_time() >= 0)

    def test_get_solution_for_DD(self):
        self.dd_instance.create_decision_diagram(False)
        value, path = self.get_value_path_solution()

        expected_value: int = 11
        expected_path: str = ' arc_0_1(0)-> arc_1_4(1)-> arc_4_7(0)-> arc_7_8(0)-> arc_8_10(1)'

        self.assertEqual(value, expected_value)
        self.assertEqual(path, expected_path)

    def test_get_solution_for_reduceDD(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=False)

        value, path = self.get_value_path_solution()

        expected_value: int = 11
        expected_path: str = ' arc_0_1(0)-> arc_1_4(1)-> arc_4_7(0)-> arc_7_8(0)-> arc_8_10(1)'

        self.assertEqual(value, expected_value)
        self.assertEqual(path, expected_path)

    def test_get_solution_for_restrictedDD(self):
        self.dd_instance.create_restricted_decision_diagram(verbose=False, max_width=2)

        value, path = self.get_value_path_solution()

        expected_value: int = 11
        expected_path: str = ' arc_0_1(0)-> arc_1_4(1)-> arc_4_6(0)-> arc_6_8(0)-> arc_8_9(1)'

        self.assertEqual(value, expected_value)
        self.assertEqual(path, expected_path)

    def test_get_solution_for_relaxedDD(self):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=False, max_width=2)

        value, path = self.get_value_path_solution()

        expected_value: int = 11
        expected_path: str = ' arc_0_1(0)-> arc_1_3(0)-> arc_3_5(1)-> arc_5_7(1)-> arc_7_8(1)'

        self.assertEqual(value, expected_value)
        self.assertEqual(path, expected_path)

    def test_compare_gml_exact_dd_graph(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.export_graph_file('test')

        expected_file_path: str = os.path.join('Test', 'gml_files', 'exact_dd_independent_set_sort.gml')
        actual_file_path: str = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'test.gml'))

        self.assertTrue(os.path.exists(actual_file_path))
        self.assertTrue(os.path.exists(expected_file_path))

        with open(expected_file_path, "r") as file:
            expected_output = file.read()

        with open(actual_file_path, "r") as file:
            actual_output = file.read()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_compare_gml_reduce_dd_graph(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=False)
        self.dd_instance.export_graph_file('test')

        expected_file_path: str = os.path.join('Test', 'gml_files', 'reduce_dd_independent_set_sort.gml')
        actual_file_path: str = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'test.gml'))

        self.assertTrue(os.path.exists(actual_file_path))
        self.assertTrue(os.path.exists(expected_file_path))

        with open(expected_file_path, "r") as file:
            expected_output = file.read()

        with open(actual_file_path, "r") as file:
            actual_output = file.read()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_compare_gml_restricted_dd_graph(self):
        self.dd_instance.create_restricted_decision_diagram(verbose=False, max_width=2)
        self.dd_instance.export_graph_file('test')

        expected_file_path: str = os.path.join('Test', 'gml_files', 'restricted_dd_independent_set_sort.gml')
        actual_file_path: str = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'test.gml'))

        self.assertTrue(os.path.exists(actual_file_path))
        self.assertTrue(os.path.exists(expected_file_path))

        with open(expected_file_path, "r") as file:
            expected_output = file.read()

        with open(actual_file_path, "r") as file:
            actual_output = file.read()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_compare_gml_relax_dd_graph(self):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=False, max_width=2)
        self.dd_instance.export_graph_file('test')

        expected_file_path: str = os.path.join('Test', 'gml_files', 'relax_dd_independent_set_sort.gml')
        actual_file_path: str = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'test.gml'))

        self.assertTrue(os.path.exists(actual_file_path))
        self.assertTrue(os.path.exists(expected_file_path))

        with open(expected_file_path, "r") as file:
            expected_output = file.read()

        with open(actual_file_path, "r") as file:
            actual_output = file.read()

        self.assertEqual(actual_output.strip(), expected_output.strip())


if __name__ == '__main__':
    unittest.main()
