"""Strut module for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 2.0
"""

from node import Node


class Strut:
    """Strut class for the generation of complex tensegrity structures.""" 

    def __init__(self,top_node=None,bottom_node=None):
        self.top:Node=top_node
        self.bottom:Node=bottom_node
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

    def get_top(self) -> Node:
        """Getter for node at the top of the strut

        Returns:
            node: node object at the top of the strut
        """
        return self.top

    def get_bottom(self) -> Node:
        """Getter for node at the bottom of the strut

        Returns:
            node: node object at the bottom of the strut
        """
        return self.bottom
    
    def get_top_label(self) -> str:
        """Getter for node at the top of the strut

        Returns:
            node: node object at the top of the strut
        """
        return self.top.get_label()

    
    def get_bottom_label(self) -> str:
        """Getter for node at the bottom of the strut

        Returns:
            node: node object at the bottom of the strut
        """
        return self.bottom.get_label()
