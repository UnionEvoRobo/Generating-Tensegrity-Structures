"""Node class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 2.0
"""
class Node:
    
    def __init__(self, label):
        self.label=str(label)
        self.bracket=None
        self.num_edges=0

    def set_label(self, new_label):
        """Set self.label to the given string.

        Args:
            new_label (string): Intended new label string
        """
        self.label=new_label
        
    def get_label(self):
        """Return the label string.

        Returns:
            self.label(string): string indicating the node number
        """
        return str(self.label) 

    def set_bracket(self, bracket_type):
        """Set the bracket field type.

        Args:
            bracket_type (string): bracket type indicated by a rule
        """
        self.bracket=bracket_type

    def get_bracket(self):
        """Return the bracket string

        Returns:
            self.bracket(string): string indicating the bracket type
        """
        return str(self.bracket)
    
    def get_num_edges(self):
        return self.num_edges
    
    def add_edge(self):
        self.num_edges+=1
        
    def remove_edge(self):
        self.num_edges-=1
 
    def is_extraneous(self):
        return self.get_num_edges()<3
        