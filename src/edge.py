"""Edge object for the generation of complex tensegrity structures.


@author: Daniel Casper
@version: 3.0
"""

from node import Node
class Edge:
    """Edge class for the generation of complex tensegrity structures."""    
    def __init__(self, label, start, end):
        self.label=label
        self.set_start(start)
        self.set_end(end)

    def set_start(self, new_start):
        """Setter for the starting node property of an edge

        Args:
            new_start (Node): The node intended as the 
            starting location of the edge
        """
        self.start:Node=new_start

    def set_end(self, new_end):
        """Setter for the ending node property of an edge

        Args:
            new_start (Node): The node intended as the 
            ending location of the edge
        """
        self.end:Node=new_end

    def get_label(self):
        """Return the label string.

        Returns:
            self.label(string): string indicating the edge type
        """
        return self.label

    def get_start(self) -> Node:
        """Return the start node.

        Returns:
            self.start(object): Returns the node object pointed 
            to by self.start
        """
        return self.start

    def get_end(self)  -> Node:
        """Return the end node.

        Returns:
            self.end(object): Returns the node object pointed 
            to by self.end
        """
        return self.end

    def get_endpoint(self, side):
        """Return the start or end of the edge

        Args:
            side (int): 1 if getting start, -1 if getting end

        Returns:
            Node: the start or end node of the edge
        """
        if side>0:
            return self.start
        return self.end
    
    def set_endpoint(self, new_node, side):
        """Return the start or end of the edge

        Args:
            side (int): 1 if getting start, -1 if getting end

        Returns:
            Node: the start or end node of the edge
        """
        if side>0:
            return self.set_start(new_node)
        return self.set_end(new_node)

    def get_start_label(self):
        """Return the start node's label.

        Returns:
            self.start.label(string): The label string for the node
            pointed to by self.start
        """
        if self.start is not None:
            return str(self.start.get_label())
        return "NONE"

    def get_end_label(self):
        """Return the end node's label.

        Returns:
            self.end.label(string): The label string for the node
            pointed to by self.end
        """
        if self.end is not None:
            return str(self.end.get_label())
        return "NONE"

    def equals(self, edge2:Node):
        """Compares to edge objects based on their start and end nodes. 
        
        If two edges are the reverse of each other they are still equal.

        Args:
            edge2 (object): the second edge object to compare against.

        Returns:
            boolean: Returns true if the two edges are equal. False if not.
        """
        if (self.start==edge2.get_start() or self.start==edge2.get_end()):
            return (self.end==edge2.get_end() or self.end==edge2.get_start())
        return False

    def is_unique(self, edges):
        """Compares a given edge to all edges in the edge_list from the main module. 
        If it is not equal to any edge in the list then it is unique.

        Args:
            edges (list): the edge_list from main

        Returns:
            boolean: Returns true if the given edge is unique. False if not.
        """
        for i in edges:
            if self.equals(i):
                return False
        return True

    def contains(self, node):
        """Checks an edge for whether or not it contains a given node.

        Args:
            nodeLabel (string): the node being checked for

        Returns:
            boolean: Returns true if the edge's start or end node 
            matches the given nodeLable. False if not.
        """
        return node in (self.start, self.end)

    def change_label(self, new_label):
        """Change the label of the edge to a new value.

        Args:
            new_label (_type_): the new contents for self.label
        """
        self.label=new_label

    def is_terminating(self):
        """Determines whether an edge begins or ends at nothing

        Returns:
            bool: whether an edge begins or ends at nothing
        """
        return self.start is None or self.end is None
