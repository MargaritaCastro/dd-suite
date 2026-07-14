from bitarray import bitarray
class SetCoverStructure:
    def __init__(self, file_path: str) -> None:
        """
        SourceCode for Set Cover Problem
        """
        self.initial_state: 'bitarray' = None
        self.variables: list[tuple[str, list[int]]] = None
        self.objective_weights:list[int] = None
        self.matrix_of_weight: list[list[int]] = None
        self.right_side_of_restrictions:list[int] = None

        self.read_file(file_path)

    def read_file(self, file_path: str) -> None:
        try:
            with open(file_path, 'r') as input_file:
                variable_length: int = int(input_file.readline().strip())
                restrictions_length: int = int(input_file.readline().strip())

                self.initial_state = bitarray(restrictions_length)
                self.initial_state.setall(1)
                self.right_side_of_restrictions: list[int] = [1] * restrictions_length

                self.objective_weights: list[int] = list(map(int, input_file.readline().strip().split()))

                self.matrix_of_weight = [
                    list(map(int, input_file.readline().strip().split()))
                    for _ in range(restrictions_length)
                ]

                self.variables = [("x_" + str(i + 1), [0, 1]) for i in range(variable_length)]

        except IOError:
            print(f"Error: could not open file {file_path}")
            exit(1)
    
    def __str__(self):
        return f"SetCoverStructure; initial_state:{self.initial_state}, variables:{self.variables}, objective_weights:{self.objective_weights}, matrix_of_weight:{self.matrix_of_weight}, right_side_of_restriction:{self.right_side_of_restrictions})"