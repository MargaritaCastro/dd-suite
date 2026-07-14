from gurobipy import GRB, quicksum

from Examples.AbstractGurobiProblems.AbstractProblemGurobiClass import AbstractProblemGurobi
from Examples.SOCKnapsack.SOCKnapsackInstance import SOCKnapsackInstance

class SOCKnapsackGurobi(AbstractProblemGurobi):
    '''
    SourceCode to solve the SOCKnapsack problem using Gurobi.
    '''
    def __init__(self, parameters: SOCKnapsackInstance, continuous: bool=False) -> None:
        '''
        Initialize the SOCKnapsackGurobi instance.

        Parameters:
        - variable_length (int): Number of variables.
        - weights (list of float): Weights of the variables.
        - objective_weights (list of float): Weights of the variables for the objective function.
        - capacity (float): Maximum capacity of the knapsack.
        - cuts (list[tuple[list[int]): List of additional cuts for the model. Each cut is represented as a tuple (vector, bound) where vector is a list of coefficients and bound is a limit value.
        - continuous (bool): Indicates if the variables should be continuous (True) or binary (False).
        '''
        super().__init__(variable_length=len(parameters.variables),
                         objective_weights=parameters.objective_weights,
                         continuous=continuous,
                         min_max=GRB.MAXIMIZE)

        self.params = parameters
        self.num_constraints = len(self.params.right_side_of_restrictions)

        self.y = []
        self.z = None

    def _add_constraints(self) -> None:
        for j in range(self.num_constraints):
            self.y.append(self.model.addVars(self.variable_length, name=f'y_{j}'))
        self.z = self.model.addVars(self.num_constraints, name='z')

        for j in range(self.num_constraints):
            # (1) y[j][i] = sqrt_weights[j][i] * x[i] — defines each y auxiliary variable
            self.model.addConstrs(self.params.sqrt_weights[j][i]*self.x[i] == self.y[j][i] for i in self.set_variable_length)
            # (2) omega * z[j] = rhs[j] − Σᵢ linear_weights[j][i]*x[i] — defines each z auxiliary variable
            self.model.addConstr(self.params.right_side_of_restrictions[j] - quicksum(self.params.linear_weights[j][i]*self.x[i] for i in self.set_variable_length) == self.params.omega*self.z[j])
            # (3) Σᵢ y[j][i]² ≤ z[j]² — second-order cone constraint per restriction j
            self.model.addConstr(quicksum(self.y[j][i]*self.y[j][i] for i in self.set_variable_length) <= self.z[j]*self.z[j])

    def _get_variable_values(self, x_sol: list, dd) -> tuple[list, list]:
        '''
        Override: the SOCK model has auxiliary variables (y, z) not present in the DD.
        Maps x_sol values to the DD's variables using the problem's variable name index.

        Parameters:
        - x_sol: Current LP relaxation values for self.x.
        - dd: The DD object whose variables we need to extract.

        Returns:
        - (x_sol_dd, var_ids): values and indices aligned with the DD's variables.
        '''
        var_ids = dd.problem.dd_to_original_index
        x_sol_dd = [x_sol[var_id] for var_id in var_ids]
        return x_sol_dd, var_ids

    def show_results(self) -> None:
        '''
        Show the results of the model optimization.
        '''
        if self.model.status == GRB.OPTIMAL:
            print("\n------------------------------")
            print(f"Optimal total value:  {self.model.objVal:.6g}")
            print("Items in optimal solution: ")
            for i in self.set_variable_length:
                if self.x[i].x != 0:
                    print(f'Article {i}, Benefit: {self.objective_weights[i]}, Fraction:  {self.x[i].x:.6g}')
        else:
            print("No optimal solution found.")
