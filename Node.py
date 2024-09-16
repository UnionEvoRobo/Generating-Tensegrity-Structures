"""Node class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 2.0
"""

import copy 

class Node:
    
    def __init__(self, label, bracket=None):
        self.label=str(label)
        self.bracket=bracket
        self.adjacent=[]

    def set_label(self, new_label):
        """Set self.label to the given string.

        Args:
            new_label (string): Intended new label string
        """
        self.label=new_label
        
    def get_label(self):
        """Return the label string.

        Returns:
            string: string indicating the node number
        """
        return str(self.label) 
    
    def get_adjacent(self):
        """Return a list of all adjacent nodes.

        Returns:
            list: list of all adjacent nodes
        """
        return copy.deepcopy(self.adjacent)

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
    
    def get_degree(self):
        return len(self.adjacent)
    
    def add_edge(self, node):
        """Increments the degree of the node property by 1
        """
        self.adjacent.append(node)
        
    def remove_edge(self):
        """Decrements the degree of the node property by 1
        """
        self.degree-=1
 
    def is_extraneous(self):
        """Indicates whether or not a node has the desired three edges.

        Returns:
            boolean: returns true if the node's num_edges property is 
            greater than or equal to three. Otherwise returns false.
        """
        return self.get_degree()<3
      
    def is_adjacent(self, node):
        return self.adjacent.__contains__(node)