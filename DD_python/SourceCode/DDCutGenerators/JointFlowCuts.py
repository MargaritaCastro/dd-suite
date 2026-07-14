from Exceptions.MyExceptions import NoCutsFound, GurobiModelNotInitialized
from SourceCode.DD import DD
from SourceCode.DDCutGenerators.AbstractCutGenerator import AbstractCutGenerator

import time
from gurobipy import GRB, Model, LinExpr, GurobiError

class JointFlowCuts(AbstractCutGenerator):
    '''
    JointFlowCuts class, responsible for solving the cut problem in a graph, utilizing the dual of the linear programming
    model to find the cut.

    This algorithm can only be used to solve binary decision diagrams (BDDs) and was specifically designed
    to address certain max-flow problems from the paper "A combinatorial cut-and-lift procedure with an
    application to 0-1 second-order conic programming. Mathematical Programming, section 5.2."

    Castro, M. P., Cire, A. A., & Beck, J. C. (2022). A combinatorial cut-and-lift procedure with an
    application to 0-1 second-order conic programming. Mathematical Programming, 196(1), 115-171.
    '''
    
    def __init__(self, DD: DD, tolerance: float=0.0001) -> None:
        '''
        Constructor for the Dual Cut class.

        Parameters:
        DD (DD): Decision diagram to be solved.
        tolerance (float): tolerance value for the linear programming model, to avoid numeric errors.
        verbose (bool): Indicates if additional information will be shown on the console.
        '''
        super().__init__(tolerance)

        self.graph_DD: 'Graph' = DD.get_decision_diagram()
        self.time: float = 0
        self.verbose: bool = False

        try:
            self.model: 'Model' = Model("bdd_lp_model")
        except GurobiError as e:
            self.model = None
            raise GurobiModelNotInitialized(
                f"Gurobi model not initialized (possible license issue): {e}"
            ) from e

        if self.model is None:
            raise GurobiModelNotInitialized(
                "Gurobi model not initialized (possible license issue)"
            )

        self._set_variables()
        self._set_constraints()

        if not self.verbose:
            self.model.setParam('OutputFlag', 0)

    
    def _set_variables(self) -> None:
        ''' Sets the variables for the linear programming model. '''
        num_vars: int = len(self.graph_DD.structure) -1 

        self.mu = self.model.addVars(num_vars, lb=0, name="mu")
        self.lambda_vars = self.model.addVars(num_vars, lb=0, name="lambda")
        self.w = {}
        
        self.nodes_to_vars = [{} for _ in range(num_vars + 1)]

        for layer_index in range(len(self.graph_DD.structure)):
            nodes_in_layer: int = len(self.graph_DD.structure[layer_index])

            for node_index, node in enumerate(self.graph_DD.structure[layer_index]):
                self.nodes_to_vars[layer_index][node.get_id()] = node_index

            self.w[layer_index] = self.model.addVars(nodes_in_layer, lb=-GRB.INFINITY, ub=GRB.INFINITY, name=f"w_{layer_index}")
       
    def _set_constraints(self) -> None:
        ''' Sets the constraints for the linear programming model. '''

        num_vars: int = len(self.graph_DD.structure)-1
        self.model.addConstr(self.w[0][0] == 1, "initial_constraint")
        self.model.addConstr(self.w[num_vars][0] == 0, "final_constraint")

        for layer_index, layer in enumerate(self.graph_DD.structure[:-1]):
            for node in layer:
                for arc in node.out_arcs:
                    source_var_index: int = self.nodes_to_vars[layer_index][arc.parent_node.get_id()]
                    target_var_index: int = self.nodes_to_vars[layer_index + 1][arc.child_node.get_id()]

                    if arc.variable_value == 0:
                        self.model.addConstr(self.w[layer_index+1][target_var_index] - self.w[layer_index][source_var_index] + self.mu[layer_index] >= 0)
                    else:
                        self.model.addConstr(self.w[layer_index+1][target_var_index] - self.w[layer_index][source_var_index] + self.lambda_vars[layer_index] >= 0)
    
    def get_time(self) -> float:
        ''' 
        Returns the execution time of the cut algorithm. 
        
        Returns:
        - float: Execution time of the cut algorithm.
        '''
        return self.time
    
    def get_name(self) -> str:
        ''' 
        Returns the name of the cut. 

        Returns:
        - String representation of the cut.
        '''
        return "Joint Flow Cuts (Dual Cuts)"
    
    def get_cut(self) -> tuple[list[float], float]:
        ''' 
        Returns the precomputed values from the generate_cut() method. 

        Returns:
        - tuple[list[float], float]: Tuple with the coefficients of the cut and the constant.
        '''
        if not hasattr(self, "coefficients"):
            raise NoCutsFound()
        
        return (self.coefficients, self.constant)
    
    def generate_cut(self, x_values: list[float], verbose:bool =False) -> bool:
        '''
        Solves the decision diagram, finding a cut. Returns this value.

        Parameters:
        x_values (list): List of x values in the graph. (Equivalent to capacities)
        verbose (bool): Indicates if additional information will be shown on the console.

        Returns:
        - Bool: indicating whether the cut is feasible.
        '''
        self.verbose = verbose
        start_time: float = time.time()
        find_cut: bool = self._compute_cut(x_values)
        end_time: float = time.time() 
        self.time = end_time - start_time

        if find_cut:
            if self.verbose:
                print(f"\n '''A cut was found in the graph.''' ")
                print(f"    The cut it is:  {self._get_inequality_string_representation()}\n")
        else:
            if self.verbose:
                print(f"\n '''No cut was found in the graph.''' \n")
            del self.coefficients
            del self.constant

        return find_cut
    
    def _compute_cut(self, x_values: list[float]) -> bool:
        '''
        Solves the decision diagram, finding a cut. Returns this value.

        Parameters:
        x_values (list): List of x values in the graph. (Equivalent to capacities)

        Returns:
        - Bool: indicating whether the cut is feasible.
        '''
        self.coefficients: list[int] = [0] * (len(self.graph_DD.structure) -  1 )
        self.constant: int = -1
        self.x_values: list[float] = x_values

        if self.verbose:
            print("\nCalculating cut...\n")
        
        self._update_lp_model()

        return self._solve_lp_model()

    def _update_lp_model(self) -> None:
        '''
        Creates the linear programming model to be solved.
        '''
        self._set_objective()
        self.model.update()

    def _solve_lp_model(self) -> bool:
        ''' 
        Solves the linear programming model. 
        
        Returns:
        - bool: if the model was solved.
        '''
        self.model.setParam(GRB.Param.PreCrush, 1)
        self.model.setParam(GRB.Param.Threads, 1)
        self.model.optimize()

        for i in range(len(self.graph_DD.structure)-1):
            self.coefficients[i] = self.mu[i].X - self.lambda_vars[i].X
            self.constant += self.mu[i].X
        
        return abs(self.model.ObjVal - 1) >= self.tolerance

    def _set_objective(self) -> None:
        ''' Updates the objective function for the linear programming model. '''
        objExpr: 'LinExpr' = LinExpr()

        for var_index in range(len(self.graph_DD.structure)-1):
            objExpr += self.x_values[var_index] * self.lambda_vars[var_index] + (1 - self.x_values[var_index]) * self.mu[var_index]
        
        self.model.setObjective(objExpr, GRB.MINIMIZE)
        self.model.update()

    
    