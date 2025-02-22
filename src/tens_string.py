"""Tens_String class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 2.0
"""

class TensString:
    """Tens_String class for the generation of complex tensegrity structures."""


    def __init__ (self, node1, node2,num):
        self.node1=node1
        self.node2=node2
        self.node_stub=None
        self.rod_number=num

    def set_rod_number(self, label):
        """Sets a string's number label

        Args:
            label (string): the label for the string
        """
        self.node_stub=label

    def get_rod_number(self):
        """Getter for the identifying number of the rod

        Returns:
            int: identifying number of the rod
        """
        return self.rod_number

    def get_from(self):
        """Getter for one end of the string

        Returns:
            node: node that is set as an endpoint for the string
        """
        return self.node1

    def get_to(self):
        """Getter for one end of the string

        Returns:
            node: node that is set as an endpoint for the string
        """
        return self.node2

    def set_from(self, node):
        """Sets one end of the string

        Args:
            node (node): node to be set as an endpoint for the string
        """
        self.node1=node

    def set_to(self, node):
        """Sets the other end of the string

        Args:
            node (node): node to be set as an endpoint for the string
        """
        self.node2=node
