"""Main class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 5.0
"""

import graphviz  # doctest: +NO_EXE
from Graph import Graph
from Secondary_Mutations import Secondary_Mutations
dot = graphviz.Digraph(comment='Tensegrity Object Graph')
dot

class Main:

    def __init__(self, num_transformations, rule_dict):
        self.graph=Graph(rule_dict,num_transformations)
    
    def draw_graph(self):
        """Reads through the node_list and edge_list and generates nodes
        and edges respectively using the information contained within
        the node and edge objects.
        """
        for i in self.graph.node_list:
            dot.node(i.get_label())
        for i in self.graph.edge_list:
            dot.edge(i.get_start_label(), i.get_end_label(), i.get_label())

    def remove_extraneous_nodes(self):
        """Removes all nodes that have less than 3 edges 
        beginning and ending at its location.
        """
        goodbye_edges=[]
        goodbye_nodes=[]
        for node in self.graph.node_list:
            if node.is_extraneous():
                goodbye_nodes.append(node)
                for edge in self.graph.edge_list:
                    if edge.contains(node) and goodbye_edges.count(edge)==0:
                        goodbye_edges.append(edge)
        for i in goodbye_nodes:
            self.graph.node_list.remove(i)
        for i in goodbye_edges:
            self.graph.edge_list.remove(i)
    
    def mutate_graph(self, edge1_ind, edge2_ind):
        """Takes two edges and passes them to the secondary_mutations class
        to undergo a secondary mutation

        Args:
            edge1_ind (integer): The index of the first desired edge in the edge_list
            edge2_ind (integer): The index of the second desired edge in the edge_list
        """
        mutator=Secondary_Mutations(self.edge_list[edge1_ind],self.edge_list[edge2_ind])
        mutator.mutate_graph()
        
                        
if __name__=='__main__':
    rule_dict={"A":"A>B{a}E", "B":"B>D{a}A", "C":"C>D", "D":"D>C{d}D", "E":"E>C"}
    num_trans=1
    output_name='standardOutput'
    main = Main(num_trans,rule_dict)
    main.graph.create_graph()
    main.graph.transform()
    main.graph.generate_bracket_edges()
    main.remove_extraneous_nodes()
    main.draw_graph()
    dot.render('doctest-output/%s.gv' %(output_name)).replace('\\', '/')
    ('doctest-output/%s.gv.pdf' %(output_name))
