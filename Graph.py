"""Graph class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 2.0
"""

from Transformer import Transformer
from Node import Node
from Edge import Edge


class Graph:
        
    def __init__(self, rules, edge_types):
        self.edge_types=edge_types
        self.rules=rules
        self.transformer=Transformer(self)
        self.edge_list=[]
        self.node_list=[]
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
        while self.node_number<4:
            node=Node(self.node_number)
            self.node_number+=1
            self.node_list.append(node)
        edge=Edge("A", self.node_list[0], self.node_list[1])
        self.edge_list.append(edge)
        edge=Edge("B", self.node_list[1], self.node_list[2])
        self.edge_list.append(edge)
        edge=Edge("C", self.node_list[2], self.node_list[3])
        self.edge_list.append(edge)
        edge=Edge("D", self.node_list[3], self.node_list[0])
        self.edge_list.append(edge)
        edge=Edge("E", self.node_list[1], self.node_list[3])
        self.edge_list.append(edge)
        
    def transform(self, num_trans):
        """Passes the graph through the transformer a number of times

        Args:
            num_trans (int): the desired number of passes through 
            the transformer
        """
        transformations=0
        while transformations<(num_trans):
            self.transformer.transform()
            transformations+=1
    
    def swap_rule(self, key, new_rule):
        """Changes the definition of the rule at the indicated key

        Args:
            key (string): string containing the dictionary 
            key for the rule that will be changed
            
            new_rule (string): string containing the new rule
        """
        self.rules[key]=new_rule
        
    def add_node(self, bracket):
        """Creates and adds a new node to the graph

        Args:
            bracket (string): string indicating what 
            the bracket property will be

        Returns:
            Node: returns the newly created node
        """
        node=Node(self.node_number,bracket)
        self.node_number+=1
        self.node_list.append(node)
        self.bracket_nodes.append(node)
        return node
    
    def add_edge(self, label, start, end):
        """Creates and adds a new edge to the graph

        Args:
            label (string): _description_
            start (Node): the origin node of the edge
            end (Node): the destination node for the edge
        """
        edge=Edge(label, start, end)
        self.edge_list.append(edge)
        start.add_edge()
        end.add_edge()
    
    def generate_bracket_edges(self):
        """Creates new connecting edges between 
           nodes with matching bracket types
        """
        for i in self.bracket_nodes:
            for r in self.bracket_nodes:
                if i!=r and self.bracket_nodes.index(i)<self.bracket_nodes.index(r) and i.get_bracket()==r.get_bracket():
                    edge=Edge(i.get_bracket().capitalize(), i, r)
                    if edge.is_unique(self.edge_list):
                        self.edge_list.append(edge)   
                        i.add_edge()
                        r.add_edge() 
                            
    """def adjacent(self, x, y):
        #tests whether there is an edge from the vertex x to the vertex y;
        adjacent=False
        for e in self.edge_list:
            if e.contains(x):
                adjacent=

    def neighbors(self, x):
        #lists all vertices y such that there is an edge from the vertex x to the vertex y;


    def remove_vertex(self, x):
        #removes the vertex x, if it is there;


    def remove_edge(self, x, y):
        #removes the edge from the vertex x to the vertex y, if it is there;
"""
