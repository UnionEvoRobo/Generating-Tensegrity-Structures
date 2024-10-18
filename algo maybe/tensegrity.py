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
        self.pairs=[]
    
    def f_print_genome(self, f_name):
        None
    def print_genome(self):
        None
    
    def crossover(self, tens1, tens2, another_var):
        None
        
    def mutate(self):
        return "HELLO"
    
    def find_random_string_labels(self):
        None
    
    def f_print_string_labels(self, string_file_name):
        None
        
    def length_of_genotype(self):
        None
    
    def print_obj_vals(self):
        None
    
    def fprint_obj_vals(self, out_file):
        None
        
    def set_objectives(self, result):
        None
    
    def get_objectives(self):
        None
    
    def get_graph(self):
        return self.graph
    