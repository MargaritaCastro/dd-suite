from gurobipy import GRB, quicksum
from Examples.AbstractGurobiProblems.AbstractProblemGurobiClass import AbstractProblemGurobi
from Examples.SetCoverInstance.SetCoverInstance import SetCoverStructure

class SetCoverGurobi(AbstractProblemGurobi):
    '''
    SourceCode to solve the set cover problem using Gurobi
    '''
    def __init__(self, parameters: 'SetCoverStructure', continuous: bool=False) -> None:
        '''
        Initialize the SetCoverGurobi instance.

        Parameters:
        - parameters (SetCoverStructure): Instance of the set cover problem, containing:
            * variables (list[tuple[str, list[int]]]): variable names and their domains.
            * objective_weights (list[int]): cost of selecting each subset.
            * matrix_of_weight (list[list[int]]): coverage matrix (one row per restriction,
              one column per variable).
        - continuous (bool): Indicates if the variables should be continuous (True) or binary (False).
        '''
        super().__init__(variable_length=len(parameters.variables),
                         objective_weights=parameters.objective_weights,
                         continuous=continuous,
                         min_max=GRB.MINIMIZE)

        # PARAMS
        self.params: 'SetCoverStructure' = parameters
        self.restrictions: list[list[int]] = parameters.matrix_of_weight
        self.restrictions_length: int = len(parameters.matrix_of_weight)

        # SETS
        self.R: 'range' = range(self.restrictions_length)


    def _add_constraints(self) -> None:
        # (1) Coverage: Σₛ A[r][s]*x[s] ≥ 1 for each element r — every element must be covered
        for r in self.R:
            self.model.addConstr(quicksum(self.restrictions[r][s]*self.x[s] for s in self.set_variable_length) >= 1, f"Restriction_{r}")

    def show_results(self) -> None:
        ''' Show the results of the optimization. '''
        if self.model.status == GRB.OPTIMAL:
            print("\n------------------------------")
            print(f'Minimum value of the selected subsets:  {self.model.objVal:.6g}')
            print('Selected variables: ')
            for s in self.set_variable_length:
                if self.x[s].x != 0:
                    print(f'Variable  {s + 1} - Fraction {self.x[s].x:.6g} - Value {self.objective_weights[s]}')
        else:
            print("No optimal solution found.")
