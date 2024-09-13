"""Transformer class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 3.0
"""

from Edge import Edge
from Node import Node

class Transformer:

    def __init__(self, rule_set):
        self.rule_dict=rule_set
        self.node_index=0

    def transform(self, graph):
        """Controls all operations of the transformer class.

        Args:
            graph (object): oject of the Graph class
        """
        old_edges=graph.edge_list
        graph.edge_list=[]
        for i in old_edges:
            for r in self.rule_dict:
                if i.get_label()==r:
                    rule=self.rule_dict[r]
                    self.rule_size(graph, i, rule)
            
    def rule_size(self, graph, i, rule):
        """Checks whether the accessed rule creates a bracketed node (long_rule)
        or if it just swaps the letter label of the edge (short_rule).

        Args:
            graph (object): oject of the Graph class
            i (object): edge object
            rule (string): string of the rule being followed
        """   
        if len(rule)==7:
            self.long_rule(graph, i, rule)
        elif len(rule)==3:
            self.short_rule(graph, i, rule)

    def long_rule(self, graph, i, rule):
        """Transforms A, B, and D type edges.

        Args:
            graph (object): oject of the Graph class
            i (object): edge object
            rule (string): string of the rule being followed
        """
        self.edge_new_start(graph, i, rule)
        self.edge_new_end(graph, i, rule)

    def short_rule(self, graph, i, rule):
        """Transforms C and E type edges.

        Args:
            graph (object): oject of the Graph class
            i (object): edge object
            rule (string): string of the rule being followed
        """
        new_edge=Edge(rule[-1], i.get_start(), i.get_end())
        graph.edge_list.append(new_edge)

    def new_node(self, graph, rule):
        """Creates a new node object.

        Args:
            graph (object): oject of the Graph class
            rule (string): the rule being followed to extract the bracket type

        Returns:
            node (object): an object containing the label and bracket suffix of a node
        """
        node=Node(graph.node_number)
        graph.node_number+=1
        node.set_bracket(rule[4])
        return node
    
    def edge_new_start(self, graph, i, rule):
        """Creates the first edge in a long rule.

        Args:
            graph (object): oject of the Graph class
            i (object): edge object
            rule (string): string of the rule being followed
        """
        new_node=self.new_node(graph, rule)
        graph.node_list.append(new_node)
        new_node.set_label(new_node.get_label())
        new_edge=Edge(rule[2], i.get_start(), new_node)
        graph.edge_list.append(new_edge)
        graph.bracket_nodes.append(new_node)
        
    def edge_new_end(self, graph, i, rule):
        """Creates the second edge in a long rule.

        Args:
            graph (object): oject of the Graph class
            i (object): edge object
            rule (string): string of the rule being followed
        """
        node=graph.node_list[-1]
        new_edge=Edge(rule[-1], node, i.get_end())
        graph.edge_list.append(new_edge)
