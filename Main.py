"""Main class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 2.0
"""

import graphviz  # doctest: +NO_EXE
from Mutator import Mutator
dot = graphviz.Digraph(comment='Tensegrity Object Graph')
dot

class Main:

    def __init__(self):
        self.alpha=['a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v','w','x','y','z']
        self.mutator=Mutator()
        self.edge_dict={}
        self.final_dict={}
        self.wacky_node_names=[]
        self.node_number=-1
        self.number_of_muta=2
        self.node_limit=0   

    def create_graph(self):
        """Generates a dictionary representing the initial graph to be
        fed into the mutator and then runs the dictionary (edge_dict) 
        through the mutator however many times is desired.
        """
        if self.number_of_muta==1:
            self.node_limit=3
        elif self.number_of_muta==2:
            self.node_limit=7
        while self.node_number<3:
            dot.node(str(self.node_number+1))
            self.node_number+=1
        self.edge_dict={'A':'01', 'B':'12', 'C':'23', 'D':'30', 'E':'13'}
        muta=0
        while muta<(self.number_of_muta-1):
            self.edge_dict=self.mutator.mutating(self)
            self.clean_dict()
            muta+=1
        self.final_dict=self.mutator.mutating(self)

    def draw_graph(self):
        """Reads through the contents of final_dict and interfaces
        with graphviz to actually create the edges of the actual 
        graph. Then creates additional edges between any paired 
        bracket nodes.
        """
        for i in self.final_dict:
            if (self.final_dict[i][0]) in self.alpha or int(self.final_dict[i][0])>self.node_limit:
                dot.edge(self.final_dict[i][0], self.final_dict[i][4], i[0])
            elif (self.final_dict[i][1]) in self.alpha or int(self.final_dict[i][1]):
                dot.edge(self.final_dict[i][0], self.final_dict[i][1], i[0])
            else:
                dot.edge(self.final_dict[i][0], self.final_dict[i][1], i[0])

        for i in range (len(self.wacky_node_names)-1):
            if self.wacky_node_names[i][2]==self.wacky_node_names[i+1][2]:
                dot.edge(self.wacky_node_names[i][0],self.wacky_node_names[i+1][0],self.wacky_node_names[i][2].capitalize())

    
    def clean_dict(self):
        """Cleans and returns the dictionary of current edges, 
        removing all of the bracketed node lables generated 
        by the rules for A, B, and D edges.

        NOT to be used on the final_dict dictionary of the 
        final structure. As of this version, the draw graph
        method requires the bracketed node labels.
        """
        for i in self.edge_dict:
            if len(self.edge_dict[i])>2:
                if self.edge_dict[i][2]=="{":
                    self.edge_dict[i]=self.edge_dict[i][0:2]
                elif self.edge_dict[i][1]=="{":
                    self.edge_dict[i]=self.edge_dict[i][0]+self.edge_dict[i][4]


if __name__=='__main__':
    graph = Main()
    graph.create_graph()
    graph.draw_graph()
    print(dot.source)
    dot.render('doctest-output/finalGraph.gv').replace('\\', '/')
    'doctest-output/finalGraph.gv.pdf'
