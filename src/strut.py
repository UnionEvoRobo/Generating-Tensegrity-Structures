"""Strut module for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 1.0
"""

class Strut:
    """Strut class for the generation of complex tensegrity structures."""

    ELEM_UNIT_LEN=12
    ELEM_UNIT_RAD=12


    def __init__(self):
        self.top_strings=[]
        self.bot_strings=[]

    def attach_string_to_bottom(self, new_string):
        """Attaches a string to the bottom of a strut

        Args:
            new_string (tens_string): the string to attach to the bottom of the element
        """
        self.bot_strings.append(new_string)

    def attach_string_to_top(self, new_string):
        """Attaches a string to the top of a strut

        Args:
            new_string (tens_string): the string to attach to the top of the element
        """
        self.top_strings.append(new_string)
