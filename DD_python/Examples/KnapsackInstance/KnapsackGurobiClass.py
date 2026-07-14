from gurobipy import GRB, quicksum
from Examples.AbstractGurobiProblems.AbstractProblemGurobiClass import AbstractProblemGurobi
from Examples.KnapsackInstance.KnapsackInstance import KnapsackStructure

class KnapsackGurobi(AbstractProblemGurobi):
    '''
    SourceCode to solve the knapsack problem using Gurobi.
    '''
    def __init__(self, parameters: 'KnapsackStructure', continuous: bool=False) -> None:
        '''
        Initialize the KnapsackGurobi instance.

        Parameters:
        - parameters (KnapsackStructure): Instance of the knapsack problem, containing:
            * variables (list[tuple[str, list[int]]]): variable names and their domains.
            * weights (list[int]): weights of each item for the capacity constraint.
            * objective_weights (list[int]): profits/values of each item.
            * right_side_of_restrictions (int): maximum capacity of the knapsack.
        - continuous (bool): Indicates if the variables should be continuous (True) or binary (False).
        '''
        super().__init__(variable_length=len(parameters.variables),
                         objective_weights=parameters.objective_weights,
                         continuous=continuous,
                         min_max=GRB.MAXIMIZE)

        self.params: 'KnapsackStructure' = parameters
        self.weights: list[float] = parameters.weights
        self.capacity: float = parameters.right_side_of_restrictions

    def _add_constraints(self) -> None:
        # (1) Capacity constraint: total weight of selected items must not exceed capacity
        self.model.addConstr(quicksum(self.weights[i]*self.x[i] for i in self.set_variable_length) <= self.capacity, "Capacity")

    def show_results(self) -> None:
        '''
        Show the results of the model optimization.
        '''
        if self.model.status == GRB.OPTIMAL:
            print("\n------------------------------")
            print(f"Optimal total value:  {self.model.objVal:.6g}")
            print("Items included in the knapsack: ")
            for i in self.set_variable_length:
                if self.x[i].x != 0:
                    print(f'Article {i} - Weight:  {self.weights[i]}, Benefit: {self.objective_weights[i]}, Fraction:  {self.x[i].x:.6g}')
        else:
            print("No optimal solution found.")
