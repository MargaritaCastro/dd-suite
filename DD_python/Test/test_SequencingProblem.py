import os
import sys
import io
import unittest
from types import SimpleNamespace
from unittest.mock import patch
from contextlib import contextmanager

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

from SourceCode.DD import DD
from SourceCode.DDStructure.Node import Node
from Examples.SequencingInstance.SequencingProblem import SequencingProblem
from Examples.SequencingInstance.SequencingPathSolver import SequencingPathSolver
from Test.dd_controlled_generators.DDSequencing import (
    get_exact_dd_sequencing,
    get_reduce_dd_sequencing,
    get_restricted_dd_sequencing,
    get_relaxed_dd_sequencing,
    get_false_dd_sequencing,
)


@contextmanager
def assertNoRaise():
    try:
        yield
    except Exception as e:
        raise AssertionError(f"An exception was raised: {e}")


class SequencingProblemTest(unittest.TestCase):
    def setUp(self):
        params = SimpleNamespace(
            n_jobs=3,
            processing_times=[2, 3, 1],
            weights=[1, 2, 3],
            setup_times=[
                [1, 1, 1],  # from depot
                [0, 2, 1],  # from job 0
                [2, 0, 1],  # from job 1
                [1, 2, 0],  # from job 2
            ],
            initial_state=(frozenset(), -1),
            variables=[
                ('slot_0', [0, 1, 2]),
                ('slot_1', [0, 1, 2]),
                ('slot_2', [0, 1, 2]),
            ],
            objective_weights=[1, 2, 3],
        )

        self.problem_instance: 'AbstractProblem' = SequencingProblem(params)
        self.dd_instance: DD = DD(self.problem_instance)

    def get_value_path_solution(self):
        solver = SequencingPathSolver(self.dd_instance)
        solver.set_parameters(self.problem_instance.weights, "min")
        solution = solver.solve()
        return solution.value, solution.path_print

    def test_ordered_variables(self):
        expected = ['slot_0', 'slot_1', 'slot_2']
        self.assertEqual(self.problem_instance.ordered_variables, expected)

    def test_variables_domain(self):
        expected = {
            'slot_0': [0, 1, 2],
            'slot_1': [0, 1, 2],
            'slot_2': [0, 1, 2],
        }
        self.assertEqual(self.problem_instance.variables_domain, expected)

    def test_transition_function_feasible(self):
        initial = (frozenset(), -1)
        out = [None]
        feasible = self.problem_instance.transition_function(initial, 0, 0, out)
        # state no longer carries time: just (scheduled | {job}, last_job)
        self.assertEqual(out[0], (frozenset({0}), 0))
        self.assertTrue(feasible)

    def test_transition_function_infeasible(self):
        already_scheduled = (frozenset({0}), 0)
        out = [None]
        feasible = self.problem_instance.transition_function(already_scheduled, 1, 0, out)
        self.assertFalse(feasible)

    def test_transition_function_job2_first(self):
        initial = (frozenset(), -1)
        out = [None]
        feasible = self.problem_instance.transition_function(initial, 0, 2, out)
        self.assertEqual(out[0], (frozenset({2}), 2))
        self.assertTrue(feasible)

    def test_get_priority_for_discard_node(self):
        # priority = sum of processing times of scheduled jobs (p=[2,3,1])
        state_high = (frozenset({1}), 1)
        self.assertEqual(self.problem_instance.get_priority_for_discard_node(state_high), 3)

        state_low = (frozenset({0}), 0)
        self.assertEqual(self.problem_instance.get_priority_for_discard_node(state_low), 2)

    def test_get_priority_for_merge_nodes(self):
        state_a = (frozenset({0}), 0)
        self.assertEqual(self.problem_instance.get_priority_for_merge_nodes(1, state_a), 2)

        state_b = (frozenset({1}), 1)
        self.assertEqual(self.problem_instance.get_priority_for_merge_nodes(2, state_b), 3)

    def test_merge_operator_keeps_least_committed_last_job(self):
        # equal committed processing -> keep first state's last_job
        state_a = (frozenset({1, 2}), 2)
        state_b = (frozenset({1, 2}), 1)
        merged = self.problem_instance.merge_operator(state_a, state_b)
        self.assertEqual(merged, (frozenset({1, 2}), 2))

    def test_merge_operator_intersects_scheduled(self):
        state_a = (frozenset({0, 1}), 1)
        state_b = (frozenset({0, 1}), 0)
        merged = self.problem_instance.merge_operator(state_a, state_b)
        self.assertEqual(merged[0], frozenset({0, 1}))

    def test_get_state_as_string(self):
        state = (frozenset({0}), 0)
        result = self.problem_instance.get_state_as_string(state)
        self.assertIsInstance(result, str)
        self.assertEqual(result, "[0]_0")

    def test_get_state_as_string_empty(self):
        state = (frozenset(), -1)
        self.assertEqual(self.problem_instance.get_state_as_string(state), "[]_-1")

    def test_get_state_copy(self):
        state = (frozenset({0}), 0)
        copy = self.problem_instance.get_state_copy(state)
        self.assertEqual(state, copy)

    def test_get_final_state(self):
        final = self.problem_instance.get_final_state()
        self.assertEqual(final, (frozenset(), -1))

    def test_is_dd_created(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertIsNotNone(self.dd_instance.get_decision_diagram())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_dd(self, mock_stdout):
        self.dd_instance.create_decision_diagram(True)

        file_path = os.path.join('Test', 'txt_files', 'createDDSequencing.txt')
        with open(file_path, 'r') as f:
            expected = f.read()

        self.assertEqual(mock_stdout.getvalue().strip(), expected.strip())

    def test_create_dd_graph_equal(self):
        self.dd_instance.create_decision_diagram()
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_exact_dd_sequencing())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_reduce_dd(self, mock_stdout):
        self.dd_instance.create_decision_diagram()
        self.dd_instance.reduce_decision_diagram(verbose=True)

        file_path = os.path.join('Test', 'txt_files', 'createReduceDDSequencing.txt')
        with open(file_path, 'r') as f:
            expected = f.read()

        self.assertEqual(mock_stdout.getvalue().strip(), expected.strip())

    def test_create_reduce_dd_graph_equal(self):
        self.dd_instance.create_decision_diagram()
        self.dd_instance.reduce_decision_diagram(verbose=False)
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_reduce_dd_sequencing())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_restricted_dd(self, mock_stdout):
        self.dd_instance.create_restricted_decision_diagram(3, verbose=True)

        file_path = os.path.join('Test', 'txt_files', 'createRestrictedDDSequencing.txt')
        with open(file_path, 'r') as f:
            expected = f.read()

        self.assertEqual(mock_stdout.getvalue().strip(), expected.strip())

    def test_create_restricted_dd_graph_equal(self):
        self.dd_instance.create_restricted_decision_diagram(3, verbose=False)
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_restricted_dd_sequencing())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_relaxed_dd(self, mock_stdout):
        self.dd_instance.create_relax_priority_decision_diagram(3, verbose=True)

        file_path = os.path.join('Test', 'txt_files', 'createRelaxedDDSequencing.txt')
        with open(file_path, 'r') as f:
            expected = f.read()

        self.assertEqual(mock_stdout.getvalue().strip(), expected.strip())

    def test_create_relaxed_dd_graph_equal(self):
        self.dd_instance.create_relax_priority_decision_diagram(3, verbose=False)
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_relaxed_dd_sequencing())

    def test_get_dd_graph(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertIsNotNone(self.dd_instance.get_decision_diagram())

    def test_get_copy(self):
        self.dd_instance.create_decision_diagram(False)
        original = self.dd_instance.get_decision_diagram()
        copy = self.dd_instance.get_decision_diagram_copy()

        self.assertIsNot(original, copy)
        self.assertEqual(original, copy)

        original.structure[0][0].set_id(999)
        self.assertNotEqual(
            original.structure[0][0].to_string(),
            copy.structure[0][0].to_string(),
        )

        new_node = Node(99, (frozenset({0, 1, 2}), 2))
        copy.add_node(new_node)
        self.assertNotEqual(
            len(original.structure[-1]),
            len(copy.structure[-1]),
        )

    def test_get_DDBuilder_time(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertIsNotNone(self.dd_instance.get_building_time())

    def test_get_ReduceDDBuilder_time(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=False)
        self.assertIsNotNone(self.dd_instance.get_reduction_time())

    def test_get_RestrictedDDBuilder_time(self):
        self.dd_instance.create_restricted_decision_diagram(3, verbose=False)
        self.assertIsNotNone(self.dd_instance.get_building_time())

    def test_get_RelaxedDDBuilder_time(self):
        self.dd_instance.create_relax_priority_decision_diagram(3, verbose=False)
        self.assertIsNotNone(self.dd_instance.get_building_time())

    def test_get_solution_for_DD(self):
        self.dd_instance.create_decision_diagram(False)
        value, path = self.get_value_path_solution()

        self.assertEqual(value, 31)
        self.assertEqual(path, ' arc_0_3(2)-> arc_3_8(0)-> arc_8_10(1)')

    def test_get_solution_for_reduceDD(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=False)
        value, path = self.get_value_path_solution()

        self.assertEqual(value, 31)
        self.assertEqual(path, ' arc_0_3(2)-> arc_3_5(0)-> arc_5_7(1)')

    def test_get_solution_for_restrictedDD(self):
        self.dd_instance.create_restricted_decision_diagram(3, verbose=False)
        value, path = self.get_value_path_solution()

        self.assertEqual(value, 31)
        self.assertEqual(path, ' arc_0_3(2)-> arc_3_6(0)-> arc_6_7(1)')

    def test_get_solution_for_relaxedDD(self):
        self.dd_instance.create_relax_priority_decision_diagram(3, verbose=False)
        value, path = self.get_value_path_solution()

        # Relaxed DD gives a lower bound (<= optimum 31). Dropping the time from
        # the state changes which nodes are merged, hence a different (tighter)
        # bound and path than before.
        self.assertEqual(value, 30)
        self.assertEqual(path, ' arc_0_3(2)-> arc_3_4(1)-> arc_4_7(1)')

    def test_compare_two_different_graphs(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertFalse(self.dd_instance.get_decision_diagram() == get_false_dd_sequencing())

    def test_compare_gml_dd_graph(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.export_graph_file('test')

        expected_path = os.path.join('Test', 'gml_files', 'exact_dd_sequencing.gml')
        actual_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'test.gml'))

        self.assertTrue(os.path.exists(actual_path))
        self.assertTrue(os.path.exists(expected_path))

        with open(expected_path, 'r') as f:
            expected = f.read()
        with open(actual_path, 'r') as f:
            actual = f.read()

        self.assertEqual(actual.strip(), expected.strip())

    def test_compare_gml_reduce_dd_graph(self):
        self.dd_instance.create_decision_diagram()
        self.dd_instance.reduce_decision_diagram()
        self.dd_instance.export_graph_file('test')

        expected_path = os.path.join('Test', 'gml_files', 'reduce_dd_sequencing.gml')
        actual_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'test.gml'))

        with open(expected_path, 'r') as f:
            expected = f.read()
        with open(actual_path, 'r') as f:
            actual = f.read()

        self.assertEqual(actual.strip(), expected.strip())

    def test_compare_gml_restricted_dd_graph(self):
        self.dd_instance.create_restricted_decision_diagram(3, verbose=False)
        self.dd_instance.export_graph_file('test')

        expected_path = os.path.join('Test', 'gml_files', 'restricted_dd_sequencing.gml')
        actual_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'test.gml'))

        with open(expected_path, 'r') as f:
            expected = f.read()
        with open(actual_path, 'r') as f:
            actual = f.read()

        self.assertEqual(actual.strip(), expected.strip())

    def test_compare_gml_relax_dd_graph(self):
        self.dd_instance.create_relax_priority_decision_diagram(3, verbose=False)
        self.dd_instance.export_graph_file('test')

        expected_path = os.path.join('Test', 'gml_files', 'relax_dd_sequencing.gml')
        actual_path = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'test.gml'))

        with open(expected_path, 'r') as f:
            expected = f.read()
        with open(actual_path, 'r') as f:
            actual = f.read()

        self.assertEqual(actual.strip(), expected.strip())


if __name__ == '__main__':
    unittest.main()
