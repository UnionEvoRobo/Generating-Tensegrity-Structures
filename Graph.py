"""Graph class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 2.0
"""
from transformer import Transformer
from node import Node
from edge import Edge

class Graph:
    """Graph class for the generation of complex tensegrity structures."""

    def __init__(self, rules, edge_types):
        self.edge_types = edge_types
        self.rules = rules
        self.transformer = Transformer(self)
        self.edge_list = []
        self.node_list = []
        self.bracket_nodes = []
        self.node_number = 0

    def get_rule(self, key):
        """Getter for individual rules in the rule dictionary

        Args:
            key (string): string containing the dictionary 
            key for the desired rule

        Returns:
            string: string containing the desired rule
        """
        return self.rules[key]

    def get_edge_types(self):
        """Getter for the list of different edge types

        Returns:
            list: list containing strings of the different 
            possible edge types
        """
        return self.edge_types

    def order(self):
        """Gives the order, or number of nodes, of the graph

        Returns:
            int: number of nodes in the graph
        """
        return len(self.node_list)

    def size(self):
        """Gives the size, or number of edges, of the graph

        Returns:
            int: number of edges in the graph
        """
        return len(self.edge_list)

    def create_graph(self):
        """Generates a dictionary representing the initial graph to be
        fed into the transformer and then runs the dictionary (edge_dict) 
        through the transformer however many times is desired.
        """
        while self.node_number < 4:
            self.add_node(None)
        self.add_edge("A", self.node_list[0], self.node_list[1])
        self.add_edge("B", self.node_list[1], self.node_list[2])
        self.add_edge("C", self.node_list[2], self.node_list[3])
        self.add_edge("D", self.node_list[3], self.node_list[0])
        self.add_edge("E", self.node_list[1], self.node_list[3])

    def transform(self, num_trans):
        """Passes the graph through the transformer a number of times

        Args:
            num_trans (int): the desired number of passes through 
            the transformer
        """
        transformations = 0
        while transformations < (num_trans):
            self.transformer.transform()
            transformations += 1

    def swap_rule(self, key, new_rule):
        """Changes the definition of the rule at the indicated key

        Args:
            key (string): string containing the dictionary 
            key for the rule that will be changed
            
            new_rule (string): string containing the new rule
        """
        self.rules[key] = new_rule

    def add_node(self, bracket):
        """Creates and adds a new node to the graph

        Args:
            bracket (string): string indicating what 
            the bracket property will be

        Returns:
            Node: returns the newly created node
        """
        node = Node(self.node_number)
        self.node_number += 1
        self.node_list.append(node)
        node.set_bracket(bracket)
        if node.get_bracket() is not None:
            self.bracket_nodes.append(node)
        return node

    def add_edge(self, label, start, end):
        """Creates and adds a new edge to the graph
        
        Args:
            label (string): _description_
            start (Node): the origin node of the edge
            end (Node): the destination node for the edge
        """
        edge = Edge(label, start, end)
        self.edge_list.append(edge)
        start.add_edge(end)
        end.add_edge(start)

    def generate_bracket_edges(self):
        """Creates new connecting edges between 
           nodes with matching bracket types
        """
        for node1 in self.bracket_nodes:
            for node2 in self.bracket_nodes:
                if node1 != node2 and self.bracket_nodes.index(
                        node1) < self.bracket_nodes.index(
                            node2) and node1.get_bracket(
                            ) == node2.get_bracket():
                    edge = Edge(node1.get_bracket().capitalize(), node1, node2)
                    if edge.is_unique(self.edge_list):
                        self.edge_list.append(edge)
                        node1.add_edge(node2)
                        node2.add_edge(node1)

    def adjacent(self, node1, node2):
        """Determines whether two nodes in the graph have an 
        edge connecting them (are adjacent).

        Args:
            node1 (Node): One of the two nodes being checked
            node2 (Node): One of the two nodes being checked

        Returns:
            boolean: True if the two nodes are adjacent and false if not
        """
        return node1.is_adjacent(node2)

    def neighbors(self, node1):
        """Return a list of all nodes adjacent to node1.

        Args:
            node1 (Node): The node whose list of 
            adjacent nodes is desired

        Returns:
            list: list of all adjacent nodes
        """
        return node1.get_adjacent()

    def remove_node(self, node1):
        """Removes a node from the graph.

        Args:
            node1 (Node): The node that is being removed
        """
        self.node_list.remove(node1)

    def remove_edge(self, start_node, end_node):
        """Removes an edge from the graph 

        Args:
            start_node (Node): The starting node of the desired edge
            end_node (Node): The end node of the desired edge

        Returns:
            boolean: True if the edge was deleted and false if not
        """
        for edge in self.edge_list:
            if edge.contains(start_node) and edge.contains(end_node):
                self.edge_list.remove(edge)
                start_node.remove_edge(end_node)
                end_node.remove_edge(start_node)
                return True
        return False

    def clear_nodes(self):
        """Resets the degree of all nodes in the graph.
        """
        for node in self.node_list:
            node.clear_deg()
