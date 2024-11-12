"""Tens_String class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 1.0
"""

class TensString:
    """Tens_String class for the generation of complex tensegrity structures."""


    def __init__ (self, elem1, elem2,num):
        self.elem1=elem1
        self.elem2=elem2
        #self.to_loc=to_loc
        #self.from_loc=from_loc
        self.node_stub=None
        self.rod_number=num

    def set_rod_number(self, label):
        """Sets a string's number label

        Args:
            label (string): the label for the string
        """
        self.node_stub=label

    def get_from(self):
        """Gets one end of the string

        Returns:
            element: 
        """
        return self.elem1

    def get_to(self):
        """_summary_

        Returns:
            _type_: _description_
        """
        return self.elem2
    