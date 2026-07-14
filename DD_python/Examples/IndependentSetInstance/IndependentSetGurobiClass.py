from gurobipy import GRB
from Examples.AbstractGurobiProblems.AbstractProblemGurobiClass import AbstractProblemGurobi
from Examples.IndependentSetInstance.IndependentSetInstance import IndependentSetStructure

class IndependentSetGurobi(AbstractProblemGurobi):
    '''
    SourceCode to solve the maximum independent set problem using Gurobi.
    '''
    def __init__(self, parameters: 'IndependentSetStructure', continuous: bool=False) -> None:
        '''
        Initialize the IndependentSetGurobi instance.

        Parameters:
        - parameters (IndependentSetStructure): Instance of the independent set problem, containing:
            * variables (list[tuple[str, list[int]]]): variable names and their domains.
            * objective_weights (list[int]): weight/value of each vertex in the objective.
            * edges (list[tuple[int, int]]): list of edges (i, j) in the graph.
        - continuous (bool): Indicates if the variables should be continuous (True) or binary (False).
        '''
        super().__init__(variable_length=len(parameters.variables),
                         objective_weights=parameters.objective_weights,
                         continuous=continuous,
                         min_max=GRB.MAXIMIZE)

        # PARAMS
        self.params: 'IndependentSetStructure' = parameters
        self.edges: list[tuple[int, int]] = parameters.edges


    def _add_constraints(self) -> None:
        # (1) Edge exclusion: x[i] + x[j] ≤ 1 for each edge (i,j) — adjacent vertices cannot both be selected
        self.model.addConstrs(self.x[i-1] + self.x[j-1] <= 1 for i, j in self.edges)

    def show_results(self) -> None:
        ''' Display the results of the optimization. '''
        if self.model.status == GRB.OPTIMAL:
            print("\n------------------------------")
            print(f'Maximum size of the independent set:  {self.model.objVal:.6g}')
            print('Vertices in the independent set: ')
            for i in self.set_variable_length:
                if self.x[i].x != 0:
                    print(f'Vertex {i}: fraction  {self.x[i].x:.6g}')
        else:
            print("No optimal solution found.")
