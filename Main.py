"""Main class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 3.0
"""

import graphviz  # doctest: +NO_EXE
from Graph import Graph
from Edge import Edge
dot = graphviz.Digraph(comment='Tensegrity Object Graph')
dot

class Main:

    def __init__(self, num_transformations, rule_dict):
        self.graph=Graph(rule_dict)
        self.num_transformations=num_transformations

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
        for i in self.graph.node_list:
            dot.node(i.get_label())
        for i in self.graph.edge_list:
            dot.edge(i.get_start_label(), i.get_end_label(), i.get_label())

    def draw_bracket_edges(self):
        """Reads through the bracket_nodes list and checks the bracket field
        of each node. If any two unique nodes have the same bracket type, 
        it will generate a graphviz edge between the two of that edge type.
        """
        for i in self.graph.bracket_nodes:
            for r in self.graph.bracket_nodes:
                if i!=r and self.graph.bracket_nodes.index(i)<self.graph.bracket_nodes.index(r) and i.get_bracket()==r.get_bracket():
                    edge=Edge(i.get_bracket().capitalize(), i, r)
                    if edge.is_unique(self.graph.edge_list):
                        self.graph.edge_list.append(edge)
                        dot.edge(edge.get_start_label(), edge.get_end_label(), edge.get_label())



if __name__=='__main__':
    rule_dict={"A":"A>B{a}E", "B":"B>D{a}A", "C":"C>D", "D":"D>C{d}D", "E":"E>C"}
    num_trans=1
    output_name='standardOutput'
    main = Main(num_trans,rule_dict)
    main.create_graph()
    main.draw_graph()
    dot.render('doctest-output/%s.gv' %(output_name)).replace('\\', '/')
    ('doctest-output/%s.gv.pdf' %(output_name))