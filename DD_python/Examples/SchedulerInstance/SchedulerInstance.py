class SchedulerStructure:
    def __init__(self, file_path: str) -> None:
        """
        Structure for the single-machine scheduling problem (minimise Σ w_j * C_j).

        File format:
            n_jobs
            p_0 p_1 ... p_{n-1}          (processing times)
            w_0 w_1 ... w_{n-1}          (weights)
            s_00 s_01 ... s_0{n-1}       (setup times from depot, row 0)
            s_10 s_11 ...                (setup times from job 0, row 1)
            ...
            s_n0 ...                     (setup times from job n-1, row n)
        """
        self.n_jobs: int = None
        self.processing_times: list[int] = None
        self.weights: list[int] = None
        self.setup_times: list[list[int]] = None  
        self.initial_state: tuple = None
        self.variables: list[tuple[str, list[int]]] = None
        self.objective_weights: list[int] = None

        self.read_file(file_path)

    def read_file(self, file_path: str) -> None:
        try:
            with open(file_path, 'r') as f:
                self.n_jobs = int(f.readline().strip())
                self.processing_times = list(map(int, f.readline().strip().split()))
                self.weights = list(map(int, f.readline().strip().split()))
                self.setup_times = []
                for _ in range(self.n_jobs + 1):
                    self.setup_times.append(list(map(int, f.readline().strip().split())))

            self.initial_state = (frozenset(), -1)
            self.variables = [(f"slot_{i}", list(range(self.n_jobs))) for i in range(self.n_jobs)]
            self.objective_weights = self.weights

        except IOError:
            print(f"Error: could not open file {file_path}")
            exit(1)

    def __str__(self) -> str:
        return (
            f"SchedulerStructure; n_jobs={self.n_jobs}\n"
            f"Processing times: {self.processing_times}\n"
            f"Weights: {self.weights}\n"
            f"Setup times: {self.setup_times}"
        )
