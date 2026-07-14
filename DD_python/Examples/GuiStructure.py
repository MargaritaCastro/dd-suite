import os
import sys
from enum import Enum

current_dir = os.path.dirname(os.path.abspath(__file__))
root_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(root_dir)

class ProblemClass(Enum):
    NONE = "None"
    SET_COVER = "SetCoverInstance"
    KNAPSACK = "KnapsackInstance"
    INDEPENDENT_SET = "IndependentSetInstance"
    SOC_KNAPSACK = "SOCKnapsackInstance"
    SCHEDULER = "SchedulerInstance"

class CutType(Enum):
    FLOW       = "FlowCuts"
    JOINT_FLOW = "JointFlowCuts"
    TARGET     = "TargetCuts"

VALID_CUTS: dict[ProblemClass, list[CutType]] = {
    ProblemClass.KNAPSACK:        [CutType.FLOW, CutType.JOINT_FLOW, CutType.TARGET],
    ProblemClass.INDEPENDENT_SET: [CutType.FLOW, CutType.JOINT_FLOW, CutType.TARGET],
    ProblemClass.SET_COVER:       [CutType.FLOW, CutType.JOINT_FLOW, CutType.TARGET],
    ProblemClass.SOC_KNAPSACK:    [CutType.FLOW, CutType.JOINT_FLOW, CutType.TARGET],
    ProblemClass.SCHEDULER:       [CutType.TARGET],
}

class DDType(Enum):
    NONE = "None"
    EXACT = "Exact"
    RESTRICTED = "Restricted"
    RELAX_PRIORITY = "RelaxPriority"
    RELAX_GROUPING = "RelaxGrouping"

class GuiStructure:
    def __init__(
        self,
        input_file_path: str,
        output_file_path: str = None,
        dd_type: DDType = DDType.EXACT,
        verbose: bool = False,
        export_flag: bool = False,
        reduce_flag: bool = False,
        maximum_width: int = 2147483646,
        problem_class: ProblemClass = ProblemClass.KNAPSACK,
        minmax: str = "min",
        sort: bool = True,
    ):
        self.input_file_path: str = input_file_path
        self.output_file_path: str = output_file_path
        self.dd_type: DDType = dd_type
        self.verbose: bool = verbose
        self.export_flag: bool = export_flag
        self.reduce_flag: bool = reduce_flag
        self.maximum_width: int = maximum_width
        self.problem_class: ProblemClass = problem_class
        self.minmax: str = minmax
        self.sort: bool = sort


class GuiCutsStructure:
    def __init__(
        self,
        input_file_path: str,
        output_file_path: str = None,
        problem_class: ProblemClass = ProblemClass.KNAPSACK,
        cut_type: CutType = CutType.FLOW,
        cut_strengthening_flag: bool = False,
        verbose: bool = False,
        continuous_flag: bool = False,
        dd_type: DDType = DDType.EXACT,
        maximum_width: int = 2147483646,
    ):
        self.input_file_path: str = input_file_path
        self.output_file_path: str = output_file_path
        self.problem_class: ProblemClass = problem_class
        self.cut_type: CutType = cut_type
        self.cut_strengthening_flag: bool = cut_strengthening_flag
        self.verbose: bool = verbose
        self.continuous_flag: bool = continuous_flag
        self.dd_type: DDType = dd_type
        self.maximum_width: int = maximum_width

class GuiGurobiStructure:
    def __init__(
        self,
        input_file_path: str,
        output_file_path: str = None,
        problem_class: ProblemClass = ProblemClass.KNAPSACK,
        verbose: bool = False,
        continuous: bool = False
    ):
        self.input_file_path: str = input_file_path
        self.output_file_path: str = output_file_path
        self.problem_class: ProblemClass = problem_class
        self.verbose: bool = verbose
        self.continuous: bool = continuous
