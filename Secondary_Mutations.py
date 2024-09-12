"""Mutator Class that facilitates the processes of a secondary mutation

@author: Daniel Casper
@version: 1.0
"""

class Secondary_Mutations:
    
    def __init__(self, edge1, edge2):
        self.edge1=edge1
        self.edge2=edge2

    def mutate_graph(self):
        """Swaps the destination nodes of edge1 and edge2.
        """
        new_end2=self.edge1.swap_ends(self.edge2.get_end())
        self.edge2.swap_ends(new_end2)