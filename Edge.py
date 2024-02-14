"""Edge class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 1.0
"""
class Edge:
    
    def __init__(self, label, start, end):
        self.label=label
        self.start=start
        self.end=end

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
        
        