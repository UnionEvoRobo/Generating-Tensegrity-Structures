from Graph import Graph

class Tensegrity:
    
    def __init__(self, rule_dict, edges, transformations, seed=None):
        if rule_dict is None:
            self.l_system=seed.l_system
        else:
            self.l_system=rule_dict
        if edges is None:
            self.edge_types=seed.edge_types
        else:
            self.edge_types=edges
        self.edge_types=edges
        self.graph = Graph(rule_dict,self.edge_types)
        self.graph.create_graph()
        if transformations is None:
            self.graph.transform(seed.transformations)
        else:
            self.graph.transform(transformations)
        self.graph.generate_bracket_edges()
    
    def f_print_genome(self, f_name):
        None