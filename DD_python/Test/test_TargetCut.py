import os
import sys
import unittest
from types import SimpleNamespace

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

from SourceCode.DD import DD
from SourceCode.DDCutGenerators.TargetCut import TargetCut
from Exceptions.MyExceptions import NoCutsFound
from Examples.SchedulerInstance.SchedulerProblem import SchedulerProblem


class TestTargetCutScheduler(unittest.TestCase):

    @staticmethod
    def _scheduler_params():
        """3 jobs, symmetric setup times → ω = (1.0, 1.0, 1.0) by symmetry."""
        return SimpleNamespace(
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
            variables=[('slot_0', [0, 1, 2]),
                    ('slot_1', [0, 1, 2]),
                    ('slot_2', [0, 1, 2])],
            objective_weights=[1, 2, 3],
        )

    @staticmethod
    def _build_exact_dd(problem_instance) -> DD:
        dd = DD(problem_instance)
        dd.create_decision_diagram(verbose=False)
        return dd

    def setUp(self):
        self.dd = self._build_exact_dd(SchedulerProblem(self._scheduler_params()))
        self.tc = TargetCut(self.dd)

    def test_instantiation(self):
        self.assertIsInstance(self.tc, TargetCut)
        self.assertEqual(len(self.tc.omega), 3)

    def test_get_name(self):
        self.assertEqual(self.tc.get_name(), "Target Cut (MDD)")

    def test_omega_in_domain_range(self):
        """For domain [0,1,2], each ω_k must be in [0, 2]."""
        for k, w in enumerate(self.tc.omega):
            self.assertGreaterEqual(w, 0.0, f"ω[{k}] < 0")
            self.assertLessEqual(w, 2.0, f"ω[{k}] > 2")

    def test_omega_symmetric(self):
        """3-job scheduler with uniform setup structure → ω = (1, 1, 1)."""
        for k, w in enumerate(self.tc.omega):
            self.assertAlmostEqual(w, 1.0, places=10,
                                   msg=f"Expected ω[{k}]=1.0, got {w}")

    def test_get_cut_before_generate_raises(self):
        with self.assertRaises(NoCutsFound):
            self.tc.get_cut()

    def test_cut_found_all_zeros(self):
        """x̄=(0,0,0) means job 0 in every slot, infeasible → cut found."""
        self.assertTrue(self.tc.generate_cut([0.0, 0.0, 0.0]))

    def test_cut_validity(self):
        """The returned cut must be violated by x̄=(0,0,0)."""
        x_bar = [0.0, 0.0, 0.0]
        self.tc.generate_cut(x_bar)
        coeffs, constant = self.tc.get_cut()
        lhs = sum(coeffs[k] * x_bar[k] for k in range(3))
        self.assertGreater(lhs, constant, "Cut must be violated by x̄: LHS > RHS expected")

    def test_cut_coefficients_shape(self):
        """After a successful cut, coefficients have 3 entries."""
        self.tc.generate_cut([0.0, 0.0, 0.0])
        coeffs, constant = self.tc.get_cut()
        self.assertEqual(len(coeffs), 3)
        self.assertIsInstance(constant, float)

    def test_no_cut_interior_point(self):
        """x̄=ω=(1,1,1) is the geometric center of conv(S) → no cut expected."""
        self.assertFalse(self.tc.generate_cut([1.0, 1.0, 1.0]))

    def test_no_cut_clears_stored_cut(self):
        """After a failed generate_cut, get_cut raises NoCutsFound."""
        self.tc.generate_cut([0.0, 0.0, 0.0])  
        self.tc.generate_cut([1.0, 1.0, 1.0])   
        with self.assertRaises(NoCutsFound):
            self.tc.get_cut()

    def test_get_time(self):
        """Execution time is recorded and non-negative."""
        self.tc.generate_cut([0.0, 0.0, 0.0])
        self.assertGreaterEqual(self.tc.get_time(), 0.0)

    def test_is_equality_cut_true_for_infeasible(self):
        """For the scheduler MDD, infeasible points always yield an equality cut (unbounded LP)."""
        self.tc.generate_cut([0.0, 0.0, 0.0])
        self.assertTrue(self.tc.is_equality_cut())


if __name__ == '__main__':
    unittest.main()
