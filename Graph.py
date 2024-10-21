"""Graph class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 3.0
"""

import random
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
        self.bracket_nodes=[]
        self.node_number=0

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

    def get_nodes(self):
        """Getter for the list of the graphs nodes

        Returns:
            list: list containing all nodes in the graph
        """
        return self.node_list

    def get_edges(self):
        """Getter for the list of the graphs edges

        Returns:
            list: list containing all edges in the graph
        """
        return self.edge_list

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

    def get_edge(self,node1,node2):
        """Getter for a specific edge in the graph

        Args:
            node1 (Node): One of the nodes that the desired edge must contain
            node2 (Node): One of the nodes that the desired edge must contain

        Returns:
            Edge: the edge in the graph containing both of the specified nodes.
            If not found, returns None
        """
        for i in self.edge_list:
            if i.contains(node1) and i.contains(node2):
                return i
        return None

    def make_tr3(self):
        """Create the graph of a starter 3 bar tensegrity"""
        while self.node_number < 6:
            self.add_node(None)

        self.add_edge('3',self.node_list[0],self.node_list[1])
        self.add_edge('3',self.node_list[1],self.node_list[2])
        self.add_edge('3',self.node_list[2],self.node_list[0])
        self.add_edge('1',self.node_list[3],self.node_list[4])
        self.add_edge('3',self.node_list[4],self.node_list[5])
        self.add_edge('5',self.node_list[5],self.node_list[3])
        self.add_edge('2',self.node_list[4],self.node_list[0])
        self.add_edge('4',self.node_list[5],self.node_list[1])
        self.add_edge('1',self.node_list[3],self.node_list[2])

    def grow_while_connecting_rods(self, size, print_intermed):
        """Grows an initial graph using the indicated l_system and size parameters

        Args:
            size (int): Number of iterations through the transformer
            print_intermed (boolean): Whether or not the method should 
                                    print intermediate stages of graph growth

        Returns:
            boolean: _description_
        """
        ####    USE THE TRANSFORMER HERE!!!!!   ####
        if print_intermed:
            print("INTERMEDIATE GRAPH PRINTS HERE {size} times")
            return size
        return False

    def simplify_graph(self):
        """Removes all nodes that have less than 3 edges 
        beginning and ending at its location.
        """

        ####    SEE IF THIS CAN BE IMPROVED!!!!!   ####

        loop=True
        while loop and len(self.node_list)!=0:
            goodbye_edges=[]
            goodbye_nodes=[]
            loop=False
            for node in self.node_list:
                if node.is_extraneous():
                    loop=True
                    goodbye_nodes.append(node)
                    for edge in self.edge_list:
                        if edge.contains(node) and goodbye_edges.count(edge)==0:
                            goodbye_edges.append(edge)
            for i in goodbye_nodes:
                self.remove_node(i)
            for i in goodbye_edges:
                self.remove_edge(i.get_start(),i.get_end())

    def find_random_string_labels(self):
        """Reassigns the lables of strings in the graph
        """
        for i in self.node_list:
            self.assign_random_label_for_edges_to_node(i)

    def assign_random_label_for_edges_to_node(self, node):
        """Reassigns the lables of strings connected to the given node.
        """
        edges=self.get_node_edges(node)
        if edges is not None:
            stay_in=True
            for e in edges:
                if int(e.get_label())==node.get_label():
                    e.change_label(-1)
            tries=0
            while stay_in and tries<=5*len(edges):
                tries+=1
                rand_ex=random.randint(0,(len(edges)-1))
                rand_edge=edges[rand_ex]
                if int(rand_edge.get_label()==-1):
                    rand_edge.change_label(-1)
                stay_in=False

    def get_node_edges(self, node):
        """Get the list of all edges containing the given node

        Args:
            node (Node): the node being inspected

        Returns:
            list: the list of all edges in graph that contains the given node
        """
        ret=[]
        if node!=[]:
            for i in self.neighbors(node):
                edge=self.get_edge(node,i)
                if edge is not None:
                    ret.append(edge)
            if not ret:
                return None
            return ret

    def get_string_labels(self):
        """Return a list of the labels of all edges
        """
        ret=[]
        for i in self.edge_list:
            ret.append(i.get_label())
        return ret

    def mutate_string_labels(self):
        """Change the label of a string in the graph at random
        """
        randnode = random.randint(0,self.node_number)
        self.assign_random_label_for_edges_to_node(self.node_list[randnode])
