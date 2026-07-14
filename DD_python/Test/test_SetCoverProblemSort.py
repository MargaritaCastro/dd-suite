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

from Examples.SetCoverInstance.SetCoverProblem import SetCoverProblem
from Test.dd_controlled_generators.DDSetCoverSort import (
    get_exact_dd_set_cover_sort,
    get_relaxed_dd_set_cover_sort,
    get_restricted_dd_set_cover_sort
)

@contextmanager
def assertNoRaise():
    try:
        yield
    except Exception as e:
        raise AssertionError(f"Se generó una excepción: {e}")


class SetCoverSortTest(unittest.TestCase):
    def setUp(self):
        params = SimpleNamespace(
            initial_state=bitarray("111"),
            variables=[('conjunto_A', [0, 1]), ('conjunto_B', [0, 1]), ('conjunto_C', [0, 1]), ('conjunto_D', [0, 1]), ('conjunto_E', [0, 1]), ('conjunto_F', [0, 1])],
            matrix_of_weight=[[1, 1, 1, 0, 0, 0], [1, 0, 0, 1, 1, 0], [0, 1, 0, 1, 0, 1]],
            right_side_of_restrictions=[1, 1, 1],
        )
        self.problem_instance: SetCoverProblem = SetCoverProblem(params, sort=True)
        self.dd_instance: DD = DD(self.problem_instance)

    def get_value_path_solution(self):
        shortest_longest_path_instance: ShortestLongestPath = ShortestLongestPath(self.dd_instance)
        shortest_longest_path_instance.set_parameters([2, 1, 4, 3, 4, 3], "min")
        answer: 'PathStructure' = shortest_longest_path_instance.solve()
        return answer.value, answer.path_print

    def test_ordered_variables(self):
        # sort=True: sorted descending by coverage count
        # A:2, B:2, D:2, C:1, E:1, F:1
        ordered_variables_test: list[str] = ['conjunto_A', 'conjunto_B', 'conjunto_D', 'conjunto_C', 'conjunto_E', 'conjunto_F']
        self.assertEqual(self.problem_instance.ordered_variables, ordered_variables_test)

    def test_variables_domain(self):
        variables_domain_test: dict[str, list[int]] = {'conjunto_A': [0, 1], 'conjunto_B': [0, 1], 'conjunto_C': [0, 1], 'conjunto_D': [0, 1], 'conjunto_E': [0, 1], 'conjunto_F': [0, 1]}
        self.assertEqual(self.problem_instance.variables_domain, variables_domain_test)

    def test_transition_frunction(self):
        previous_state: bitarray = bitarray("111")
        out = [bitarray("111")]
        feasibility = self.problem_instance.transition_function(previous_state, 0, 1, out)
        self.assertEqual(out[0], bitarray("001"))
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
        self.assertEqual(self.problem_instance.get_state_as_string(merged_state), "00000")

        state_one2: bitarray = bitarray("10010")
        state_two2: bitarray = bitarray("01010")
        merged_state2: bitarray = self.problem_instance.merge_operator(state_one2, state_two2)
        self.assertEqual(self.problem_instance.get_state_as_string(merged_state2), "00010")

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
        self.assertEqual(self.problem_instance.get_state_as_string(final_state), "000")

    def test_is_dd_created(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertIsNotNone(self.dd_instance.get_decision_diagram())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_dd(self, mock_stdout):
        self.dd_instance.create_decision_diagram(True)

        file_path: str = os.path.join('Test', 'txt_files', 'createDDSetCoverSort.txt')

        with open(file_path, "r") as file:
            expected_output = file.read()

        actual_output: str = mock_stdout.getvalue()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_create_dd_graph_equal(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_exact_dd_set_cover_sort())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_reduce_dd(self, mock_stdout):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=True)

        file_path: str = os.path.join('Test', 'txt_files', 'createReduceDDSetCoverSort.txt')

        with open(file_path, "r") as file:
            expected_output = file.read()

        actual_output: str = mock_stdout.getvalue()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_create_reduce_dd_graph_equal(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=False)
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_exact_dd_set_cover_sort())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_restricted_dd(self, mock_stdout):
        self.dd_instance.create_restricted_decision_diagram(verbose=True, max_width=3)

        file_path: str = os.path.join('Test', 'txt_files', 'createRestrictedDDSetCoverSort.txt')

        with open(file_path, "r") as file:
            expected_output = file.read()

        actual_output: str = mock_stdout.getvalue()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_create_restricted_dd_graph_equal(self):
        self.dd_instance.create_restricted_decision_diagram(verbose=False, max_width=3)
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_restricted_dd_set_cover_sort())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_relaxed_dd(self, mock_stdout):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=True, max_width=3)

        file_path: str = os.path.join('Test', 'txt_files', 'createRelaxedDDSetCoverSort.txt')

        with open(file_path, "r") as file:
            expected_output = file.read()

        actual_output: str = mock_stdout.getvalue()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_create_relaxed_dd_graph_equal(self):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=False, max_width=3)
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_relaxed_dd_set_cover_sort())

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

        new_node: Node = Node(11, [1, 2, 3, 4, 5])
        copied_graph.add_node(new_node)

        self.assertNotEqual(
            len(original_graph.structure[-1]),
            len(copied_graph.structure[-1])
        )

    def test_get_DDBuilder_time(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertTrue(self.dd_instance.get_building_time() is not None)

    def test_get_ReduceDDBuilder_time(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=False)
        self.assertTrue(self.dd_instance.get_reduction_time() is not None)

    def test_get_RestrictedDDBuilder_time(self):
        self.dd_instance.create_restricted_decision_diagram(verbose=False, max_width=3)
        self.assertTrue(self.dd_instance.get_building_time() is not None)

    def test_get_RelaxedDDBuilder_time(self):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=False, max_width=3)
        self.assertTrue(self.dd_instance.get_building_time() is not None)

    def test_get_solution_for_DD(self):
        self.dd_instance.create_decision_diagram(False)
        value, path = self.get_value_path_solution()
        expected_value: int = 3
        expected_path: str = ' arc_0_2(1)-> arc_2_6(1)-> arc_6_10(0)-> arc_10_13(0)-> arc_13_17(0)-> arc_17_18(0)'

        self.assertEqual(value, expected_value)
        self.assertEqual(path, expected_path)

    def test_get_solution_for_reduceDD(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=False)

        value, path = self.get_value_path_solution()

        expected_value: int = 3
        expected_path: str = ' arc_0_2(1)-> arc_2_6(1)-> arc_6_10(0)-> arc_10_13(0)-> arc_13_17(0)-> arc_17_18(0)'

        self.assertEqual(value, expected_value)
        self.assertEqual(path, expected_path)

    def test_get_solution_for_restrictedDD(self):
        self.dd_instance.create_restricted_decision_diagram(verbose=False, max_width=3)

        value, path = self.get_value_path_solution()

        expected_value: int = 3
        expected_path: str = ' arc_0_2(1)-> arc_2_5(1)-> arc_5_7(0)-> arc_7_10(0)-> arc_10_12(0)-> arc_12_14(0)'

        self.assertEqual(value, expected_value)
        self.assertEqual(path, expected_path)

    def test_get_solution_for_relaxedDD(self):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=False, max_width=3)

        value, path = self.get_value_path_solution()

        expected_value: int = 3
        expected_path: str = ' arc_0_2(1)-> arc_2_5(1)-> arc_5_7(0)-> arc_7_10(0)-> arc_10_12(0)-> arc_12_14(0)'

        self.assertEqual(value, expected_value)
        self.assertEqual(path, expected_path)

    def test_compare_gml_exact_dd_graph(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.export_graph_file('test')

        expected_file_path: str = os.path.join('Test', 'gml_files', 'exact_dd_set_cover_sort.gml')
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

        expected_file_path: str = os.path.join('Test', 'gml_files', 'reduce_dd_set_cover_sort.gml')
        actual_file_path: str = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'test.gml'))

        self.assertTrue(os.path.exists(actual_file_path))
        self.assertTrue(os.path.exists(expected_file_path))

        with open(expected_file_path, "r") as file:
            expected_output = file.read()

        with open(actual_file_path, "r") as file:
            actual_output = file.read()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_compare_gml_restricted_dd_graph(self):
        self.dd_instance.create_restricted_decision_diagram(verbose=False, max_width=3)
        self.dd_instance.export_graph_file('test')

        expected_file_path: str = os.path.join('Test', 'gml_files', 'restricted_dd_set_cover_sort.gml')
        actual_file_path: str = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'test.gml'))

        self.assertTrue(os.path.exists(actual_file_path))
        self.assertTrue(os.path.exists(expected_file_path))

        with open(expected_file_path, "r") as file:
            expected_output = file.read()

        with open(actual_file_path, "r") as file:
            actual_output = file.read()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_compare_gml_relax_dd_graph(self):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=False, max_width=3)
        self.dd_instance.export_graph_file('test')

        expected_file_path: str = os.path.join('Test', 'gml_files', 'relax_dd_set_cover_sort.gml')
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
