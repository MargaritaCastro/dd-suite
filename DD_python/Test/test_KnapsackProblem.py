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
from SourceCode.GraphAlgorithms.ShortestLongestPath.ShortestLongestPath import ShortestLongestPath

from Examples.KnapsackInstance.KnapsackProblem import KnapsackProblem
from Test.dd_controlled_generators.DDKnapsack import (
    get_exact_dd_knapsack,
    get_reduce_dd_knapsack,
    get_relaxed_dd_knapsack,
    get_restricted_dd_knapsack,
    get_false_dd_knapsack,
)


@contextmanager
def assertNoRaise():
    try:
        yield
    except Exception as e:
        raise AssertionError(f"Se generó una excepción: {e}")


class KnapsackProblemTest(unittest.TestCase):
    def setUp(self):

        params = SimpleNamespace(
            initial_state=0,
            variables=[('item_oro', [0, 1]), ('item_plata', [0, 1]), ('item_bronce', [0, 1]), ('item_hierro', [0, 1])],
            weights=[3, 3, 4, 6],
            right_side_of_restrictions=6,
        )

        self.problem_instance: 'AbstractProblem' = KnapsackProblem(params)
        self.dd_instance: DD = DD(self.problem_instance)
    
    def get_value_path_solution(self):
        shortest_longest_path_instance: ShortestLongestPath = ShortestLongestPath(self.dd_instance)
        shortest_longest_path_instance.set_parameters([-5, 1, 18, 15], "max")
        answer: 'PathStructure' = shortest_longest_path_instance.solve()
        return answer.value, answer.path_print

    def test_ordered_variables(self):
        ordered_variables_test: list[str] = ['item_oro', 'item_plata', 'item_bronce', 'item_hierro']
        self.assertEqual(self.problem_instance.ordered_variables, ordered_variables_test)

    def test_variables_domain(self):
        variables_domain_test: dict[str, list[int]] = {'item_oro': [0, 1], 'item_plata': [0, 1], 'item_bronce': [0, 1], 'item_hierro': [0, 1]}
        self.assertEqual(self.problem_instance.variables_domain, variables_domain_test)

    def test_transition_frunction(self):
        out = [0]
        feasibility = self.problem_instance.transition_function(0, 0, 1, out)
        self.assertEqual(3, out[0])
        self.assertTrue(feasibility)

        feasibility = self.problem_instance.transition_function(4, 2, 1, out)
        self.assertEqual(8, out[0])
        self.assertFalse(feasibility)

    def test_get_priority_for_discard_node(self):
        priority: int = self.problem_instance.get_priority_for_discard_node(4)
        self.assertEqual(-4, priority)

        priority: int = self.problem_instance.get_priority_for_discard_node(1)
        self.assertEqual(-1, priority)
    
    def test_get_priority_for_merge_nodes(self):
        priority: int = self.problem_instance.get_priority_for_merge_nodes(1, 1)
        self.assertEqual(-1, priority)

        priority: int = self.problem_instance.get_priority_for_merge_nodes(3, 4)
        self.assertEqual(-4, priority)

    def test_merge_operator(self):
        state: 'State' = self.problem_instance.merge_operator(2, 1)
        self.assertEqual(1, state)

        state: 'State' = self.problem_instance.merge_operator(7, 6)
        self.assertEqual(6, state)

    def test_get_state_as_string(self):
        state: 'State' = 1
        state_string: str = self.problem_instance.get_state_as_string(state)
        self.assertTrue(type(state_string) == str)
        self.assertEqual(state_string, "1")
    
    def test_get_state_copy(self):
        state: 'State' = 1
        state_copy: 'State' = self.problem_instance.get_state_copy(state)
        #self.assertIsNot(state, state_copy)
        self.assertEqual(state, state_copy)

        state = 10
        self.assertNotEqual(state, state_copy)
    
    def test_get_final_state(self):
        final_state: 'State' = self.problem_instance.get_final_state()
        self.assertEqual(0, final_state)

    def test_is_dd_created(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertIsNotNone(self.dd_instance.get_decision_diagram())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_dd(self, mock_stdout):
        self.dd_instance.create_decision_diagram(True)

        file_path: str = os.path.join('Test', 'txt_files', 'createDDKnapsack.txt')
        
        with open(file_path, "r") as file:
            expected_output = file.read()

        actual_output: str = mock_stdout.getvalue()

        self.assertEqual(actual_output.strip(), expected_output.strip())
    
    def test_create_dd_graph_equal(self):
        self.dd_instance.create_decision_diagram()
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_exact_dd_knapsack())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_reduce_dd(self, mock_stdout):
        self.dd_instance.create_decision_diagram()
        self.dd_instance.reduce_decision_diagram(verbose=True)

        file_path: str = os.path.join('Test', 'txt_files', 'createReduceDDKnapsack.txt')
        
        with open(file_path, "r") as file:
            expected_output = file.read()

        actual_output: str = mock_stdout.getvalue()

        self.assertEqual(actual_output.strip(), expected_output.strip())

    def test_create_reduce_dd_graph_equal(self):
        self.dd_instance.create_decision_diagram()
        self.dd_instance.reduce_decision_diagram(verbose=False)

        self.assertTrue(self.dd_instance.get_decision_diagram() == get_reduce_dd_knapsack())

    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_restricted_dd(self, mock_stdout):
        self.dd_instance.create_restricted_decision_diagram(verbose=True, max_width=3)

        file_path: str = os.path.join('Test', 'txt_files', 'createRestrictedDDKnapsack.txt')
        
        with open(file_path, "r") as file:
            expected_output = file.read()

        actual_output: str = mock_stdout.getvalue()

        self.assertEqual(actual_output.strip(), expected_output.strip())
    
    def test_create_restricted_dd_graph_equal(self):
        self.dd_instance.create_restricted_decision_diagram(verbose=False, max_width=3)
        self.assertTrue(self.dd_instance.get_decision_diagram() == get_restricted_dd_knapsack())
    
    @patch('sys.stdout', new_callable=io.StringIO)
    def test_verbose_create_relaxed_dd(self, mock_stdout):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=True, max_width=3)

        file_path: str = os.path.join('Test', 'txt_files', 'createRelaxedDDKnapsack.txt')
        
        with open(file_path, "r") as file:
            expected_output = file.read()

        actual_output: str = mock_stdout.getvalue()

        self.assertEqual(actual_output.strip(), expected_output.strip())
    
    def test_create_relaxed_dd_graph_equal(self):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=False, max_width=3)

        self.assertTrue(self.dd_instance.get_decision_diagram() == get_relaxed_dd_knapsack())

    def test_get_dd_graph(self):
        self.dd_instance.create_decision_diagram(False)
        self.assertIsNotNone(self.dd_instance.get_decision_diagram())

    def test_get_copy(self):
        self.dd_instance.create_decision_diagram(False)
        original_graph = self.dd_instance.get_decision_diagram()
        copied_graph = self.dd_instance.get_decision_diagram_copy()

        # Check if they are different objects
        self.assertIsNot(original_graph, copied_graph)

        # Check if they are equal in values
        self.assertEqual(original_graph, copied_graph)

        # Modify the original graph
        original_graph.structure[0][0].set_id(100)
        self.assertNotEqual(
            original_graph.structure[0][0].to_string(),
            copied_graph.structure[0][0].to_string()
        )

        # Add a new node to the copied graph
        new_node: 'Node' = Node(11, [1, 2, 3, 4, 5])
        copied_graph.add_node(new_node)

        # Check if the sizes of the last layers are different
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
        
        expected_value: int = 18
        expected_path: str = ' arc_0_1(0)-> arc_1_3(0)-> arc_3_7(1)-> arc_7_10(0)'

        self.assertEqual(value, expected_value)
        self.assertEqual(path, expected_path)

    def test_get_solution_for_reduceDD(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=False)

        value, path = self.get_value_path_solution()

        expected_value: int = 18
        expected_path: str = ' arc_0_1(0)-> arc_1_3(0)-> arc_3_6(1)-> arc_6_7(0)'

        self.assertEqual(value, expected_value)
        self.assertEqual(path, expected_path)

    def test_get_solution_for_restrictedDD(self):
        self.dd_instance.create_restricted_decision_diagram(verbose=False, max_width=3)

        value, path = self.get_value_path_solution()

        expected_value: int = 18
        expected_path: str = ' arc_0_1(0)-> arc_1_3(0)-> arc_3_6(1)-> arc_6_9(0)'

        self.assertEqual(value, expected_value)
        self.assertEqual(path, expected_path)

    def test_get_solution_for_relaxedDD(self):
        self.dd_instance.create_relax_priority_decision_diagram(verbose=False, max_width=3)

        value, path = self.get_value_path_solution()

        expected_value: int = 18
        expected_path: str = ' arc_0_1(0)-> arc_1_3(0)-> arc_3_6(1)-> arc_6_9(0)'

        self.assertEqual(value, expected_value)
        self.assertEqual(path, expected_path)
    
    def test_compare_two_diferent_graphs(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.reduce_decision_diagram(verbose=False)

        self.assertFalse(self.dd_instance.get_decision_diagram() == get_false_dd_knapsack())
    
    def test_compare_gml_dd_graph(self):
        self.dd_instance.create_decision_diagram(False)
        self.dd_instance.export_graph_file('test')

        expected_file_path: str = os.path.join('Test', 'gml_files', 'exact_dd_knapsack.gml')
        actual_file_path: str = os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'test.gml'))

        self.assertTrue(os.path.exists(actual_file_path))
        self.assertTrue(os.path.exists(expected_file_path))

        with open(expected_file_path, "r") as file:
            expected_output = file.read()
        
        with open(actual_file_path, "r") as file:
            actual_output = file.read()
        
        self.assertEqual(actual_output.strip(), expected_output.strip())
    
    def test_compare_gml_reduce_dd_graph(self):
        self.dd_instance.create_decision_diagram()
        self.dd_instance.reduce_decision_diagram()
        self.dd_instance.export_graph_file('test')

        expected_file_path: str = os.path.join('Test', 'gml_files', 'reduce_dd_knapsack.gml')
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

        expected_file_path: str = os.path.join('Test', 'gml_files', 'restricted_dd_knapsack.gml')
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

        expected_file_path: str = os.path.join('Test', 'gml_files', 'relax_dd_knapsack.gml')
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