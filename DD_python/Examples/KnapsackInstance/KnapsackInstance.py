class KnapsackStructure:
    def __init__(self, file_path: str) -> None:
        """
        SourceCode to store the structure of the knapsack problem.
        """

        self.initial_state: int = None
        self.variables: list[tuple[str, list[int]]] = None
        self.right_side_of_restrictions: int = None
        self.weights: list[int] = None
        self.objective_weights: list[int] = None

        self.read_file(file_path)
    
    def read_file(self, file_path: str) -> None:
        try:
            with open(file_path, 'r') as input_file:
                variable_length: int = int(input_file.readline().strip())
                self.right_side_of_restrictions = int(input_file.readline().strip())

                self.weights = list(map(int, input_file.readline().strip().split()))

                self.objective_weights = list(map(int, input_file.readline().strip().split()))

                self.variables = [("x_" + str(i + 1), [0, 1]) for i in range(variable_length)]
                self.initial_state = 0

        except IOError:
            print(f"Error: could not open file {file_path}")
            exit(1)
    
    def __str__(self):
        return f"KnapsackStructure; Initial State: {self.initial_state}\nVariables: {self.variables}\nRight Side of Restrictions: {self.right_side_of_restrictions}\nWeights: {self.weights}\nObjective Weights: {self.objective_weights}"