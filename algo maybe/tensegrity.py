from graph import Graph

class Tensegrity:
    
    def __init__(self, rule_dict, edges, transformations):
        self.l_system=rule_dict
        self.edge_types=edges
        self.graph = Graph(rule_dict,self.edge_types)
        self.graph.create_graph()
        self.graph.transform(transformations)
        self.graph.generate_bracket_edges()
        
    def f_print_genome(self, f_name):
        None