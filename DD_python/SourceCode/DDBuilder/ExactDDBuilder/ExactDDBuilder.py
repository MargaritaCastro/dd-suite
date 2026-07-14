import os
import sys

current_dir = os.path.dirname(os.path.abspath(__file__))
parent_dir = os.path.abspath(os.path.join(current_dir, os.pardir))
sys.path.append(parent_dir)

from SourceCode.DDBuilder.AbstractDDBuilder import AbstractDDBuilder

class ExactDDBuilder(AbstractDDBuilder):
    '''
    Concrete class that inherits from AbstractDDBuilder, generating the basic construction of a decision diagram.
    '''

    def _specific_end_of_layer_function(self) -> None:
        ''' 
        Specific end of layer function for the construction of decision diagrams.
        Not implemented
        '''
        pass

    def _specific_end_of_construction_function(self) -> None:
        ''' 
        Specific en of construction function for the construction of decision diagrams.
        Not implemented
        '''
        if self._bottom_up_pruner():
            self.adjust_node_number()