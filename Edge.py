"""Edge class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 2.0
"""
class Edge:
    
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
        self.start=new_start
        
    def set_end(self, new_end):
        """Setter for the ending node property of an edge

        Args:
            new_start (Node): The node intended as the 
            ending location of the edge
        """
        self.end=new_end
    
    def get_label(self):
        """Return the label string.

        Returns:
            self.label(string): string indicating the edge type
        """
        return self.label

    def get_start(self):
        """Return the start node.

        Returns:
            self.start(object): Returns the node object pointed 
            to by self.start
        """
        return self.start

    def get_end(self):
        """Return the end node.

        Returns:
            self.end(object): Returns the node object pointed 
            to by self.end
        """
        return self.end
        
    def get_start_label(self):
        """Return the start node's label.

        Returns:
            self.start.label(string): The label string for the node
            pointed to by self.start
        """
        return str(self.start.get_label())

    def get_end_label(self):
        """Return the end node's label.

        Returns:
            self.end.label(string): The label string for the node
            pointed to by self.end
        """
        return str(self.end.get_label())
    
    def equals(self, edge2):
        """Compares to edge objects based on their start and end nodes. 
        
        If two edges are the reverse of each other they are still equal.

        Args:
            edge2 (object): the second edge object to compare against.

        Returns:
            boolean: Returns true if the two edges are equal. False if not.
        """
        if (self.get_start()==edge2.get_start() or self.get_start()==edge2.get_end()):
            return (self.get_end()==edge2.get_end() or self.get_end()==edge2.get_start())
        else:
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
    
    def contains(self, node_label):
        """Checks an edge for whether or not it contains a given node.

        Args:
            node_label (string): the label of the node being checked for

        Returns:
            boolean: Returns true if the edge's start or end node's label 
            matches the given nodeLable. False if not.
        """
        return node_label==self.get_start_label() or node_label==self.get_end_label()
    
    def swap_ends(self, new_end):
        """Replaces the current end node of the edge with 
        the designated new_end

        Args:
            new_end (Node): the intended new end location of the edge

        Returns:
            Node: Returns the former ending node of the edge
        """
        cur_end=self.get_end()
        self.set_end(new_end)
        return cur_end