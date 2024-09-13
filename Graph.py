from Transformer import Transformer
from Node import Node
from Edge import Edge


class Graph:
        
    def __init__(self, rules):
        self.edge_types=["A","B","C","D","E"]
        self.rules_dict=rules
        self.transformer=Transformer(rules)
        self.edge_list=[]
        self.node_list=[]
        self.bracket_nodes=[]
        self.node_number=0
        
    def get_rule(self, key):
        return self.rules_dict[key]
    
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
        transformations=0
        while transformations<(self.num_transformations-1):
            self.transformer.transform(self)
            transformations+=1
        self.transformer.transform(self)