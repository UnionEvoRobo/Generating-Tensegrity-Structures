"""Main module for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 5.0
"""

from edge import Edge
from graph import Graph
import graphviz # doctest: +NO_EXE
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
        i:Edge
        for i in self.graph.edge_list:
            dot.edge(str(i.get_start_label()),str(i.get_end_label()),str(i.get_label()))

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
                self.graph.remove_edge1(i.get_start(),i.get_end())

if __name__=='__main__':
    # rule_dict={"A":"A>B{a}E", "B":"B>D{a}A", "C":"C>D", "D":"D>C{d}D", "E":"E>C"}
    NUM_TRANS=2
    # edge_types=["A","B","C","D","E"]
    #rule_dict={"1":"2{1}5", "2":"4{1}1", "3":"4", "4":"3{4}4", "5":"3"}
    rule_dict={1:[2, 1, 5], 2:[4, 1, 1], 3:[3, 1, 3], 4:[3, 1, 1], 5:[3, -1, -1]}
    # rule_dict={1:[2,1,5],2:[-1,1,1],3:[4,-1,-1],4:[3,4,4],5:[3,-1,-1]}
    edge_types=[1,2,3,4,5]
    OUTPUT_NAME='OMG'
    main = Main(rule_dict,edge_types)
    main.graph.make_tr3()
    main.graph.transform(NUM_TRANS)
    main.graph.generate_bracket_edges()
    main.graph.simplify_graph()
    main.draw_graph()
    try:
        dot.render(f'doctest-output/{OUTPUT_NAME}.gv').replace('\\', '/')
    except graphviz.backend.execute.ExecutableNotFound:
        print("Graph error")
