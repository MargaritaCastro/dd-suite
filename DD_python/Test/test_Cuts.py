import os
import sys

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

import unittest
from types import SimpleNamespace
from SourceCode.DD import DD
from SourceCode.GraphAlgorithms.MaxFlow.MaxFlow import MaxFlow
from SourceCode.GraphAlgorithms.MaxFlow.MaxFlowGurobi import MaxFlowGurobi
from SourceCode.DDCutGenerators.FlowCuts import FlowCuts
from SourceCode.DDCutGenerators.JointFlowCuts import JointFlowCuts
from SourceCode.DDCutGenerators.CutStrengthening import CutStrengthening
from SourceCode.DDCutGenerators.TargetCut import TargetCut
from Examples.KnapsackInstance.KnapsackProblem import KnapsackProblem
from Examples.KnapsackInstance.KnapsackGurobiClass import KnapsackGurobi
from Examples.GuiStructure import CutType
from Exceptions.MyExceptions import NoCutStrengthening, MaxFlowNotExecuted, NoCutsFound

class CutTest(unittest.TestCase):
    def setUp(self):
        self.variable_length: int = 4
        self.weights: list[int] = [7, 3, 2, 1]
        self.values: list[int] = [1, 1, 1, 1]
        self.capacity: int = 8
        self.initial_state: list[int] = 0
        self.variables: list[str, list[int]] = [(f'x_{i}', [0, 1]) for i in range(1, self.variable_length + 1)]

        # In-memory stand-in for a KnapsackStructure: exposes the attributes
        # that KnapsackGurobi reads (variables, weights, objective_weights,
        # right_side_of_restrictions) without needing to read a file.
        self.knapsack_parameters = SimpleNamespace(
            variables=self.variables,
            weights=self.weights,
            objective_weights=self.values,
            right_side_of_restrictions=self.capacity,
            initial_state=self.initial_state,
        )

        self.knapsack_instance: 'AbstractProblem' = KnapsackProblem(self.knapsack_parameters)
        self.dd_instance: DD = DD(self.knapsack_instance)
        self.dd_instance.create_decision_diagram(False)
    
    def test_optimize_with_joint_flow_cuts(self):
        gurobiInstance: KnapsackGurobi = KnapsackGurobi(self.knapsack_parameters, continuous=True)
        self.assertIsInstance(gurobiInstance, KnapsackGurobi)

        gurobiInstance.create_model()
        gurobiInstance.optimize_with_cuts([self.dd_instance], cut_type=CutType.JOINT_FLOW)
        self.assertEqual([0.2857142857142857, 1.0, 1.0, 1.0], gurobiInstance.model.getAttr('x'))
        self.assertFalse(gurobiInstance.model.ObjVal.is_integer())
    
    def test_max_flow_instance(self):
        max_flow_instance: MaxFlow = MaxFlow(self.dd_instance)
        self.assertIsInstance(max_flow_instance, MaxFlow)
        
        with self.assertRaises(MaxFlowNotExecuted):
            max_flow_instance.get_max_flow()
        self.assertTrue(max_flow_instance.solve_max_flow([0.2857142857142857, 1.0, 1.0, 1.0]))
        self.assertEqual(0.7142857142857143, max_flow_instance.get_max_flow())
        self.assertTrue(max_flow_instance.solve_max_flow([1, 0, 0, 1]))
        self.assertEqual(1, max_flow_instance.get_max_flow())
        self.assertFalse(max_flow_instance.solve_max_flow([1, 1, 1, 1]))
    
    def test_max_flow_gurobi_instance(self):
        max_flow_gurobi_instance: MaxFlowGurobi = MaxFlowGurobi(self.dd_instance)
        self.assertIsInstance(max_flow_gurobi_instance, MaxFlowGurobi)
        with self.assertRaises(MaxFlowNotExecuted):
            max_flow_gurobi_instance.get_max_flow()
        self.assertTrue(max_flow_gurobi_instance.solve_max_flow([0.2857142857142857, 1.0, 1.0, 1.0]))
        self.assertEqual(0.7142857142857143, max_flow_gurobi_instance.get_max_flow())
        self.assertTrue(max_flow_gurobi_instance.solve_max_flow([1, 0, 0, 1]))
        self.assertEqual(1, max_flow_gurobi_instance.get_max_flow())
        self.assertFalse(max_flow_gurobi_instance.solve_max_flow([1, 1, 1, 1]))
    
    def test_create_flow_cuts_instance(self):
        cuts_instance_dd: FlowCuts = FlowCuts(self.dd_instance)
        self.assertIsInstance(cuts_instance_dd, FlowCuts)

    def test_create_joint_flow_cuts_instance(self):
        cuts_instance_dd: JointFlowCuts = JointFlowCuts(self.dd_instance)
        self.assertIsInstance(cuts_instance_dd, JointFlowCuts)

    def test_create_target_cut_instance(self):
        cuts_instance_dd: TargetCut = TargetCut(self.dd_instance)
        self.assertIsInstance(cuts_instance_dd, TargetCut)

    def test_create_cut_strengthening_instance(self):
        cut_strengthening_instance_dd: CutStrengthening = CutStrengthening(self.dd_instance)
        self.assertIsInstance(cut_strengthening_instance_dd, CutStrengthening)

    def test_get_a_flow_cut(self):
        cuts_instance: FlowCuts = FlowCuts(self.dd_instance)
        gurobiInstance: KnapsackGurobi = KnapsackGurobi(self.knapsack_parameters, continuous=True)
        gurobiInstance.create_model()
        gurobiInstance.optimize_with_cuts([self.dd_instance], cut_type=CutType.JOINT_FLOW)

        with self.assertRaises(NoCutsFound):
            cuts_instance.get_cut()
        self.assertTrue(cuts_instance.generate_cut(gurobiInstance.model.getAttr('x')))
        self.assertEqual(([1, 1, 0, 0], 1), cuts_instance.get_cut())
        self.assertEqual(0.7142857142857143, cuts_instance.get_min_cut())

    def test_get_a_joint_flow_cut(self):
        cuts_instance: JointFlowCuts = JointFlowCuts(self.dd_instance)
        gurobiInstance: KnapsackGurobi = KnapsackGurobi(self.knapsack_parameters, continuous=True)
        gurobiInstance.create_model()
        gurobiInstance.optimize_with_cuts([self.dd_instance], cut_type=CutType.JOINT_FLOW)

        with self.assertRaises(NoCutsFound):
            cuts_instance.get_cut()
        self.assertTrue(cuts_instance.generate_cut(gurobiInstance.model.getAttr('x')))
        self.assertEqual(([1, 1, 0, 0], 1), cuts_instance.get_cut())

    def test_get_a_target_cut(self):
        tc: TargetCut = TargetCut(self.dd_instance)
        with self.assertRaises(NoCutsFound):
            tc.get_cut()
        x_bar = [1.0, 1.0, 1.0, 1.0]
        self.assertTrue(tc.generate_cut(x_bar))
        coeffs, constant = tc.get_cut()
        self.assertAlmostEqual(coeffs[0], 2.5, places=10)
        self.assertAlmostEqual(coeffs[1], 2.5, places=10)
        self.assertAlmostEqual(coeffs[2], 0.0, places=10)
        self.assertAlmostEqual(coeffs[3], 0.0, places=10)
        self.assertAlmostEqual(constant, 2.5, places=10)

    def test_optimize_with_target_cuts(self):
        gurobiInstance: KnapsackGurobi = KnapsackGurobi(self.knapsack_parameters, continuous=True)
        gurobiInstance.create_model()
        gurobiInstance.optimize_with_cuts([self.dd_instance], cut_type=CutType.TARGET)
        self.assertEqual([0.2857142857142857, 1.0, 1.0, 1.0], gurobiInstance.model.getAttr('x'))
        self.assertFalse(gurobiInstance.model.ObjVal.is_integer())

    def test_strengthen_a_cut(self):
        cut_strengthening_instance: CutStrengthening = CutStrengthening(self.dd_instance)
        with self.assertRaises(NoCutStrengthening):
            cut_strengthening_instance.get_cut_strengthening()
        self.assertFalse(cut_strengthening_instance.cut_strengthening([1, 1, 0, 0], 1))
        self.assertTrue(cut_strengthening_instance.cut_strengthening([0.5, 1, 1, 0], 2))
        self.assertEqual(([1, 0, 1, 0], 1), cut_strengthening_instance.get_cut_strengthening())

if __name__ == '__main__':
    unittest.main()