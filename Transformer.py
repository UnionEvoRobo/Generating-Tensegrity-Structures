"""Transformer module for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 4.0
"""

class Transformer:
    """Transformer class for the generation of complex tensegrity structures."""

    def __init__(self, graph):
        self.act_edge=None
        self.rule=None
        self.graph=graph
        self.node_index=0

    def transform(self):
        """Controls all operations of the transformer class."""
        self.graph.clear_nodes()
        old_edges=self.graph.edge_list
        self.graph.edge_list=[]
        for edge in old_edges:
            for r in self.graph.get_edge_types():
                if edge.get_label()==r:
                    self.act_edge=edge
                    self.rule=self.graph.get_rule(r)
                    self.rule_size()
    def rule_size(self):
        """Checks whether the accessed rule creates a bracketed node (long_rule)
        or if it just swaps the letter label of the edge (short_rule)."""
        if len(self.rule)==7:
            self.long_rule()
        elif len(self.rule)==3:
            self.short_rule()

    def long_rule(self):
        """Transforms A, B, and D type edges."""
        new_node=self.graph.add_node(self.rule[4])
        self.graph.add_edge(self.rule[2],self.act_edge.get_start(),new_node)
        self.graph.add_edge(self.rule[-1],self.graph.node_list[-1],self.act_edge.get_end())

    def short_rule(self):
        """Transforms C and E type edges."""
        self.graph.add_edge(self.rule[-1], self.act_edge.get_start(), self.act_edge.get_end())
