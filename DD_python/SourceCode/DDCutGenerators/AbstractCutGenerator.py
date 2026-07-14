from abc import ABC, abstractmethod

class AbstractCutGenerator(ABC):
    '''
    Abstract class that provides the foundation for generating cuts.
    '''
    def __init__(self, tolerance: float=0.0001) -> None:
        '''
        Constructor for the AbstractCutGenerator class.
        '''
        self.tolerance: float = tolerance
    
    @abstractmethod
    def get_time(self) -> float:
        """
        Returns the execution time of the cut algorithm.

        Returns:
        - Execution time of the cut algorithm.
        """
        pass

    @abstractmethod
    def get_cut(self) -> tuple[list[float], float]:
        """
        Returns the cut found by the algorithm.

        Returns:
        - Tuple with the coefficients of the cut and the constant.
        """
        pass

    @abstractmethod
    def get_name(self) -> str:
        """
        Returns the name of the cut.

        Returns:
        - String representation of the cut.
        """
        pass

    @abstractmethod
    def generate_cut(self, x_values: list[float], verbose:bool =False) -> bool:
        """
        Generates a cut for the optimization problem.

        Parameters:
        x_values (list): Decision variable values.
        verbose (bool): Boolean indicating whether to print additional information.

        Returns:
        - Boolean indicating whether the cut is feasible.
        """
        pass

    def _get_inequality_string_representation(self) -> str:
        '''
        Returns the inequality of the cut.

        Returns:
        - Inequality of the cut.
        '''
        inequality: str = f"{' + '.join(f'{x}x_{i}' if (x > 1 or x < 0) else f'x_{i}' for i, x in enumerate(self.coefficients) if x != 0)} <= {self.constant}"
        return inequality
    