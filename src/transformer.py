"""Transformer module for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 7.0
"""

from edge import Edge


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
        edge:Edge
        for edge in old_edges:
            for r in self.graph.get_edge_types():
                if edge.get_label()==r:
                    self.act_edge=edge
                    rule=self.graph.get_rule(r)
                    self.rule_type(rule)

    def apply_rule(self, edge:Edge, rule):
        """Apply the grammar of a rule to a single edge

        Args:
            edge (Edge): the edge upon which the grammar will be applied
            rule (set): the rule containing the grammar being applied
        """
        self.act_edge=edge
        self.rule_type(rule)

    def rule_type(self, rule):
        """Determines what type of rule is being used
        
        Args:
            rule (set): the rule being applied
        """
        if rule.count(-1)==2 and rule[1]==-1:
            self.relabel(rule)
        elif rule.count(-1)==1 and rule[2]==-1:
            self.post_branch(rule)
        elif rule.count(-1)==1 and rule[0]==-1:
            self.pre_branch(rule)
        elif rule.count(-1)==0:
            self.split(rule)
        else:
            print("Bad")

    def split(self, rule):
        """Executes a split rule
        
        Args:
            rule (set): the rule being applied
        """
        new_node=self.graph.add_node(rule[1])
        self.graph.add_edge(rule[0],self.act_edge.get_start(),new_node)
        self.graph.add_edge(rule[-1],self.graph.node_list[-1],self.act_edge.get_end())

    def relabel(self, rule):
        """Executes a relabel rule
        
        Args:
            rule (set): the rule being applied
        """
        if rule[0]==-1:
            self.graph.add_edge(rule[-1], self.act_edge.get_start(), self.act_edge.get_end())
        else:
            self.graph.add_edge(rule[0], self.act_edge.get_start(), self.act_edge.get_end())

    def pre_branch(self, rule):
        """Executes a pre-branch stub rule

        Args:
            rule (set): the rule being applied
        """
        self.graph.add_edge(rule[1],None,self.act_edge.get_start())
        self.relabel([rule[-1],-1,-1])

    def post_branch(self, rule):
        """Executes a post-branch stub rule

        Args:
            rule (set): the rule being applied
        """
        self.graph.add_edge(rule[1],self.act_edge.get_end(),None)
        self.relabel([rule[0],-1,-1])
