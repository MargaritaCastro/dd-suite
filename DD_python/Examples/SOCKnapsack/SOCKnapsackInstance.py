class SOCKnapsackInstance:
    def __init__(self, file_path: str) -> None:
        """
        SourceCode to store the structure of the knapsack problem.
        """

        self.initial_state: list[int] = None
        self.variables: list[tuple[str, list[int]]] = None
        self.right_side_of_restrictions: list[int] = None
        self.linear_weights: list[list[int]] = []
        self.sqrt_weights: list[list[int]] = []
        self.omega: int = None
        self.objective_weights: list[int] = None


        self.read_file(file_path)

    def read_file(self, file_path: str) -> None:
        try:
            with open(file_path, 'r') as input_file:
                [variable_length, num_constraints] = list(map(int, input_file.readline().strip().split()))
                self.omega = int(input_file.readline().strip())

                self.objective_weights = list(map(int, input_file.readline().strip().split()))
                self.right_side_of_restrictions = list(map(int, input_file.readline().strip().split()))

                for i in range(num_constraints):
                    self.linear_weights.append(list(map(int, input_file.readline().strip().split())))

                for i in range(num_constraints):
                    self.sqrt_weights.append(list(map(int, input_file.readline().strip().split())))

                self.variables = [("x_" + str(i + 1), [0, 1]) for i in range(variable_length)]
                self.initial_state = [0, 0]

        except IOError:
            print(f"Error: could not open file {file_path}")
            exit(1)

    def __str__(self):
        num_vars = len(self.variables) if self.variables else 0
        num_constraints = len(self.linear_weights)

        return (
            f"SOCKnapsackInstance\n"
            f"-------------------\n"
            f"Variables (n): {num_vars}\n"
            f"Constraints (m): {num_constraints}\n"
            f"Omega: {self.omega}\n\n"
            
            f"Initial State:\n{self.initial_state}\n\n"
            
            f"Objective Weights:\n{self.objective_weights}\n\n"
            
            f"Right-hand Side (b):\n{self.right_side_of_restrictions}\n\n"
            
            f"Linear Weights (A):\n"
            + "\n".join([str(row) for row in self.linear_weights]) + "\n\n"
            
            f"Quadratic Weights (Q):\n"
            + "\n".join([str(row) for row in self.sqrt_weights])
        )