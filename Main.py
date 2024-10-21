"""Main module for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 5.0
"""

from graph import Graph
#from make_tr3 import Tr3
import graphviz  # doctest: +NO_EXE
from secondary_mutations import SecondaryMutations
dot = graphviz.Digraph(comment='Tensegrity Object Graph')

class Main:
    """Main class for the generation of complex tensegrity structures."""


    def __init__(self, rules, edges):
        self.graph=Graph(rules,edges)
    def draw_graph(self):
        """Reads through the node_list and edge_list and generates nodes
        and edges respectively using the information contained within
        the node and edge objects.
        """
        for i in self.graph.node_list:
            if i!=[]:
                dot.node(i.get_label())
        for i in self.graph.edge_list:
            dot.edge(i.get_start_label(), i.get_end_label(), i.get_label())

    def remove_extraneous_nodes(self):
        """Removes all nodes that have less than 3 edges 
        beginning and ending at its location.
        """
        loop=True
        while loop and len(self.graph.node_list)!=0:
            goodbye_edges=[]
            goodbye_nodes=[]
            loop=False
            for node in self.graph.node_list:
                if node.is_extraneous():
                    loop=True
                    goodbye_nodes.append(node)
                    for edge in self.graph.edge_list:
                        if edge.contains(node) and goodbye_edges.count(edge)==0:
                            goodbye_edges.append(edge)
            for i in goodbye_nodes:
                self.graph.remove_node(i)
            for i in goodbye_edges:
                self.graph.remove_edge(i.get_start(),i.get_end())
    def mutate_graph(self, edge1_ind, edge2_ind):
        """Takes two edges and passes them to the secondary_mutations class
        to undergo a secondary mutation

        Args:
            edge1_ind (integer): The index of the first desired edge in the edge_list
            edge2_ind (integer): The index of the second desired edge in the edge_list
        """
        mutator=SecondaryMutations(self.graph.edge_list[edge1_ind],self.graph.edge_list[edge2_ind])
        mutator.mutate_graph()
if __name__=='__main__':
    rule_dict={"A":"A>B{a}E", "B":"B>D{a}A", "C":"C>D", "D":"D>C{d}D", "E":"E>C"}
    NUM_TRANS=4
    edge_types=["A","B","C","D","E"]
    OUTPUT_NAME='dfs'
    main = Main(rule_dict,edge_types)
    N=20
    main.graph.create_graph()
    main.graph.transform(NUM_TRANS)
    main.graph.generate_bracket_edges()
    main.remove_extraneous_nodes()
    main.draw_graph()
    dot.render(f'doctest-output/{OUTPUT_NAME}.gv').replace('\\', '/')
