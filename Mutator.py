"""Mutator class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 3.0
"""

from Edge import Edge
from Node import Node

class Mutator:

    def __init__(self):
        self.rule_dict={"A":"A>B{a}E", "B":"B>D{a}A", "C":"C>D", "D":"D>C{d}D", "E":"E>C"}
        self.node_index=0

    def mutating(self, main):
        """Controls all operations of the mutator class.

        Args:
            main (object): oject of the Main class
        """
        old_edges=main.edge_list
        main.edge_list=[]
        for i in old_edges:
            for r in self.rule_dict:
                if i.get_label()==r:
                    rule=self.rule_dict[r]
                    self.rule_size(main, i, rule)
            
    def rule_size(self, main, i, rule):
        """Checks whether the accessed rule creates a bracketed node (long_rule)
        or if it just swaps the letter label of the edge (short_rule).

        Args:
            main (object): oject of the Main class
            i (object): edge object
            rule (string): string of the rule being followed
        """   
        if len(rule)==7:
            self.long_rule(main, i, rule)
        elif len(rule)==3:
            self.short_rule(main, i, rule)

    def long_rule(self, main, i, rule):
        """Mutates A, B, and D type edges.

        Args:
            main (object): oject of the Main class
            i (object): edge object
            rule (string): string of the rule being followed
        """
        self.edge_new_start(main, i, rule)
        self.edge_new_end(main, i, rule)

    def short_rule(self, main, i, rule):
        """Mutates C and E type edges.

        Args:
            main (object): oject of the Main class
            i (object): edge object
            rule (string): string of the rule being followed
        """
        new_edge=Edge(rule[-1], i.get_start(), i.get_end())
        main.edge_list.append(new_edge)

    def new_node(self, main, rule):
        """Creates a new node object.

        Args:
            main (object): oject of the Main class
            rule (string): the rule being followed to extract the bracket type

        Returns:
            node (object): an object containing the label and bracket suffix of a node
        """
        node=Node(main.node_number)
        main.node_number+=1
        node.set_bracket(rule[4])
        return node
    
    def edge_new_start(self, main, i, rule):
        """Creates the first edge in a long rule.

        Args:
            main (object): oject of the Main class
            i (object): edge object
            rule (string): string of the rule being followed
        """
        new_node=self.new_node(main, rule)
        main.node_list.append(new_node)
        new_node.set_label(new_node.get_label())
        new_edge=Edge(rule[2], i.get_start(), new_node)
        main.edge_list.append(new_edge)
        main.bracket_nodes.append(new_node)
        
    def edge_new_end(self, main, i, rule):
        """Creates the second edge in a long rule.

        Args:
            main (object): oject of the Main class
            i (object): edge object
            rule (string): string of the rule being followed
        """
        node=main.node_list[-1]
        new_edge=Edge(rule[-1], node, i.get_end())
        main.edge_list.append(new_edge)
