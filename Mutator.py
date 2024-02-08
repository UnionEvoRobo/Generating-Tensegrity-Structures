import graphviz  #doctest: +NO_EXE
dot = graphviz.Digraph(comment='Tensegrity Object Graph')
dot

class Mutator:
    
    def __init__(self):
        self.new_edges={}

    def mutating(self, main):
        countA=1
        countB=1
        countC=1
        countD=1
        countE=1
        self.new_edges={}
        for i in main.edge_dict:
            if i[0]=='A':
                #dot.node(str(main.node_number+1)+ "{a}")
                main.node_number+=1
                new_edge=main.edge_dict[i][0]+(str(main.node_number)+ "{a}")
                main.wacky_node_names.append(str(main.node_number)+ "{a}")
                self.new_edges['B' + str(countB)]=new_edge
                countB+=1
                new_edge=(str(main.node_number)+ "{a}")+main.edge_dict[i][1]
                self.new_edges['E' + str(countE)]=new_edge
                countE+=1
            elif i[0]=='B':
                #dot.node(str(main.node_number+1)+ "{a}")
                main.node_number+=1
                new_edge=main.edge_dict[i][0]+(str(main.node_number)+"{a}")
                main.wacky_node_names.append(str(main.node_number)+ "{a}")

                self.new_edges['D' + str(countD)]=new_edge
                countD+=1
                new_edge=(str(main.node_number)+ "{a}")+main.edge_dict[i][1]
                self.new_edges['A' + str(countA)]=new_edge
                countA+=1
            elif i[0]=='C':
                #dot.node(str(main.node_number+1)+"{d}")
                main.node_number+=1
                new_edge=main.edge_dict[i][0]+(str(main.node_number)+"{d}")
                main.wacky_node_names.append(str(main.node_number)+ "{d}")
                self.new_edges['C' + str(countC)]=new_edge
                countC+=1
                new_edge=(str(main.node_number)+ "{d}")+main.edge_dict[i][1]
                self.new_edges['D' + str(countD)]=new_edge
                countD+=1
            elif i[0]=='D':
                #dot.node(str(main.node_number+1)+"{d}")
                main.node_number+=1
                new_edge=main.edge_dict[i][0]+(str(main.node_number)+"{d}")
                main.wacky_node_names.append(str(main.node_number)+ "{d}")
                self.new_edges['C' + str(countC)]=new_edge
                countC+=1
                new_edge=(str(main.node_number)+ "{d}")+main.edge_dict[i][1]
                self.new_edges['D' + str(countD)]=new_edge
                countD+=1
            elif i[0]=='E':
                self.new_edges['C' + str(countC)]=main.edge_dict[i]
                countC+=1
        return(self.new_edges)
        


