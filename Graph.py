"""Graph class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 2.0
"""

from Transformer import Transformer
from Node import Node
from Edge import Edge


class Graph:
        
    def __init__(self, rules, num_trans):
        self.edge_types=["A","B","C","D","E"]
        self.rules_dict=rules
        self.transformer=Transformer(self)
        self.edge_list=[]
        self.node_list=[]
        self.bracket_nodes=[]
        self.node_number=0
        self.num_transformations=num_trans

    def get_rule(self, key):
        return self.rules_dict[key]
    
    def get_edge_types(self):
        return self.edge_types
    
    def order(self):
        return len(self.node_list)
    
    def size(self):
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
        
    def transform(self):
        transformations=0
        while transformations<(self.num_transformations):
            self.transformer.transform()
            transformations+=1
    
    def swap_rule(self,key,new_rule):
        self.rules_dict[key]=new_rule
        
    def add_node(self,rule):
        node=Node(self.node_number)
        self.node_number+=1
        node.set_bracket(rule[4])
        self.node_list.append(node)
        self.bracket_nodes.append(node)
        return node
    
    def add_edge(self, label, start, end):
        
        edge=Edge(label, start, end)
        self.edge_list.append(edge)
        start.add_edge()
        end.add_edge()
    
    def generate_bracket_edges(self):
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
