"""Main class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 3.0
"""

import graphviz  # doctest: +NO_EXE
from Mutator import Mutator
from Edge import Edge
from Node import Node
dot = graphviz.Digraph(comment='Tensegrity Object Graph')
dot

class Main:

    def __init__(self):
        self.edge_types=["A","B","C","D","E"]
        self.mutator=Mutator()
        self.edge_list=[]
        self.node_list=[]
        self.bracket_nodes=[]
        self.node_number=0
        self.number_of_muta=1

    def create_graph(self):
        """Generates a dictionary representing the initial graph to be
        fed into the mutator and then runs the dictionary (edge_dict) 
        through the mutator however many times is desired.
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
        muta=0
        while muta<(self.number_of_muta-1):
            self.mutator.mutating(self)
            muta+=1
        self.mutator.mutating(self)

    def draw_graph(self):
        """Calls drawing helper methods to generate nodes and edges
        from the contents of the lists called node_list, edge_list,
        and bracket_nodes.
        """
        self.draw_normal_edges()
        self.draw_bracket_edges()

    def draw_normal_edges(self):
        """Reads through the node_list and edge_list and generates nodes
        and edges respectively using the information contained within
        the node and edge objects.
        """
        for i in self.node_list:
            dot.node(i.get_label())
        for i in self.edge_list:
            dot.edge(i.get_start_label(), i.get_end_label(), i.get_label())

    def draw_bracket_edges(self):
        """Reads through the bracket_nodes list and checks the bracket field
        of each node. If any two unique nodes have the same bracket type, 
        it will generate a graphviz edge between the two of that edge type.
        """
        for i in self.bracket_nodes:
            for r in self.bracket_nodes:
                if i!=r and self.bracket_nodes.index(i)<self.bracket_nodes.index(r):
                    if i.get_bracket()==r.get_bracket():
                        dot.edge(i.get_label(), r.get_label(), i.get_bracket().capitalize())



if __name__=='__main__':
    graph = Main()
    graph.create_graph()
    graph.draw_graph()
    print(dot.source)
    dot.render('doctest-output/newClasses.gv').replace('\\', '/')
    'doctest-output/newClasses.gv.pdf'