"""Node module for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 5.0
"""

class Node:
    """Node class for the generation of complex tensegrity structures."""

    def __init__(self, label):
        self.label=str(label)
        self.bracket=""
        self.adjacent=[]
        self.pair:Node=None
        self.el_num=None
        self.my_pairs=[]
        self.other=None

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
        return self.adjacent

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
        return self.bracket

    def get_degree(self):
        """Getter for the degree (number of adjacent nodes) of the node

        Returns:
            int: Degree of the node
        """
        return len(self.adjacent)

    def add_edge(self, node):
        """Increments the degree of the node property by 1
        """
        if node is not None:
            self.adjacent.append(node)

    def remove_edge(self, node):
        """Decrements the degree of the node property by 1
        """
        if node in self.adjacent:
            self.adjacent.remove(node)

    def is_extraneous(self):
        """Indicates whether or not a node has the desired three edges.

        Returns:
            boolean: returns true if the node's num_edges property is 
            greater than or equal to three. Otherwise returns false.
        """
        return self.get_degree()<3

    def is_adjacent(self, node):
        """Determines whether another node is adjacent to current node

        Args:
            node (Node): the node whose adjacency is being determined

        Returns:
            boolean: True if the nodes are adjacent and false if not
        """
        return node in self.adjacent

    def clear_deg(self):
        """Sets the degree of the node back to 0
        """
        self.adjacent.clear()

    def get_other(self):
        """Get a nodes paired other

        Returns:
            node: the current nodes linked pair
        """
        return self.pair

    def set_other(self, other):
        """Set a node's linked pair to an indicated other node

        Args:
            other (node): the node to be set as the linked pair
        """
        self.pair=other

    def unpair(self):
        """Unpair a node from it's self.pair if it exists
        """
        if self.pair is not None:
            self.pair.set_other(None)
        self.pair=None

    def __lt__(self, other):
        return int(self.get_label()) < int(other.get_label())
