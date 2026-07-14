from bitarray import bitarray

class IndependentSetStructure:
    def __init__(self, file_path: str) -> None:
        """
        SourceCode to store the structure of the independent set problem
        """
        self.variables: list[tuple[str, list[int]]] = None
        self.objective_weights: list[int] = None
        self.dict_node_neighbors: dict[str, list[int]] = None
        self.initial_state: 'bitarray' = None
        self.edges: list[tuple[int, int]] = None

        self.read_file(file_path)
    
    def read_file(self, file_path: str) -> None:
        try:
            with open(file_path, 'r') as input_file:
                variable_length: int = int(input_file.readline().strip())
                self.variables: list[tuple[str, list[int]]] = [("x_" + str(i + 1), [0, 1]) for i in range(variable_length)]

                self.initial_state = bitarray(variable_length)
                self.initial_state.setall(1)

                self.objective_weights: list[tuple[str, list[int]]] = list(map(int, input_file.readline().strip().split()))

                self.dict_node_neighbors: dict[str, list[int]] = {f"x_{i+1}": [] for i in range(variable_length)}
                self.edges: list[tuple[int, int]] = []

                for _ in range(variable_length):
                    line = input_file.readline().strip()
                    parts = line.split(':')
                    key = parts[0].rstrip()

                    values = list(map(int, parts[1][2:-1].split(',')))
                    self.dict_node_neighbors[key] = values

                    child_node = int(key[2:]) 
                    for parent_node in values:
                        if child_node < parent_node:
                            self.edges.append((child_node, parent_node))

        except IOError:
            print(f"Error: could not open file {file_path}")
            exit(1)
    
    def __str__(self):
        return f"IndependentSetStructure; variables:{self.variables}, objective_weights:{self.objective_weights}, dict_node_neighbors:{self.dict_node_neighbors}, initial_stet:{self.initial_state}, edges:{self.edges}"