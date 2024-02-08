import graphviz  # doctest: +NO_EXE
from Mutator import Mutator
dot = graphviz.Digraph(comment='Tensegrity Object Graph')
dot


class Main:

    def __init__(self):
        self.edge_dict={}
        self.wacky_node_names=[]
        self.node_number=-1
        self.mutator=Mutator()   

    def create_graph(self):
        while self.node_number<3:
            dot.node(str(self.node_number+1))
            self.node_number+=1
        self.edge_dict={'A':'01', 'B':'12', 'C':'23', 'D':'30', 'E':'13'}
        new_edges=self.mutator.mutating(self)

        for i in new_edges:
                if int(new_edges[i][0])>3:
                    dot.edge(new_edges[i][0], new_edges[i][4], i[0])
                elif int(new_edges[i][1]):
                    dot.edge(new_edges[i][0], new_edges[i][1], i[0])
                else:
                    dot.edge(new_edges[i][0], new_edges[i][1], i[0])

        for i in range (len(self.wacky_node_names)-1):
            if self.wacky_node_names[i][2]==self.wacky_node_names[i+1][2]:
                dot.edge(self.wacky_node_names[i][0],self.wacky_node_names[i+1][0],self.wacky_node_names[i][2].capitalize())

        print(dot.source)
        dot.render('doctest-output/finalGraph.gv').replace('\\', '/')
        'doctest-output/finalGraph.gv.pdf'



if __name__=='__main__':
    graph = Main()
    graph.create_graph()