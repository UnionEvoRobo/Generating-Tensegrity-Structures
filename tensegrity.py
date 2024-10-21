"""asdf"""
import random
from graph import Graph
from element import Element


L_SYSTEM={"1":"1>2{1}5", "2":"2>4{1}1", "3":"3>4", "4":"4>3{4}4", "5":"5>3"}

class Tensegrity:
    """asdf"""
    def __init__(self, edges, seed=None):
        self.graph=None
        self.l_system=L_SYSTEM
        self.pairs=[]
        self.objectives=[]
        self.num_elements=0
        self.elements_to_shuffle=[]
        self.strings_to_shuffle=[]
        self.genome=[]
        self.string_labels=[]
        self.indexes_to_ignore=[]
        self.edge_types=edges
        #if rule_dict is None:
        #    self.l_system=seed.l_system
        #else:
        #    self.l_system=rule_dict
        #if edges is None:
        #    self.edge_types=seed.edge_types
        #else:
        #self.edge_types=edges
        #self.graph = Graph(rule_dict,self.edge_types)
        #self.graph.create_graph()
        #if transformations is None:
        #    self.graph.transform(seed.transformations)
        #else:
        #    self.graph.transform(transformations)
        #self.graph.generate_bracket_edges()

    def f_print_genome(self, f_name):
        """Print the genome to a file

        Args:
            f_name (string): File path that we are printing the genome to
        """
        with open(f_name,"w+",encoding="utf-8") as out_file:
            for g in self.genome:
                out_file.write(g)
            out_file.close()

    def get_genome(self):
        """Getter for the genome attribute of a tensegrity

        Returns:
            list: a list containing the genome attribute of a tensegrity
        """
        return self.genome

    def print_genome(self):
        """Print the genome"""
        print(self.genome)

    def crossover(self, parent1, parent2, another_var):
        """Perform a crossover mutation using two parent tensegrities

        Args:
            parent1 (Tensegrity): the first parent tensegrity used in the mutation
            parent2 (Tensegrity): the second parent tensegrity used in the mutation
            another_var (int): _description_
        """
        self.genome=[]
        p1gen=parent1.get_genome()
        p2gen=parent2.get_genome()
        for i in range (another_var):
            self.genome.append(p1gen[i])
        for i in range(another_var,len(p2gen)):
            self.genome.append(p2gen[i])

    def find_random_string_labels(self):
        """asdf"""
        self.graph.find_random_string_labels()
        self.get_string_labels_from_graph()

    def get_string_labels_from_graph(self):
        """Get a list of the labels of all strings in the graph
        """
        self.string_labels=[]
        for i in self.graph.get_string_labels():
            self.string_labels.append(i)

    def f_print_string_labels(self, f_name):
        """Print the list of string labels to a file

        Args:
            file_name (string): File path that we are printing the labels to
        """
        with open(f_name,"w+",encoding="utf-8") as out_file:
            out_file.write(self.string_labels)
            out_file.close()

    def length_of_genotype(self):
        """Get the length of the genome

        Returns:
            int: length of the genome attribute
        """
        return len(self.genome)

    def fprint_obj_vals(self, out_file):
        """I HAVE NO IDEA

        Args:
            out_file (_type_): _description_
        """
        None

    def set_objectives(self, result):
        """Set the objectives of the tensegrity

        Args:
            result (list): list containing new objectives
        """
        self.objectives=result

    def get_objective_vals(self):
        """Getter for the objectives attribute

        Returns:
            list: the list of objectives
        """
        return self.objectives

    def get_graph(self):
        """Getter for the tensegrity's graph

        Returns:
            Graph: the graph object representing the structure of the tensegrity
        """
        return self.graph

    def make_tr3(self):
        """Make a three-bar tensegrity
        """
        self.graph=Graph(self.l_system, self.edge_types)
        self.graph.make_tr3()
        self.pairs=[]
        self.pairs.append(3)
        self.pairs.append(4)
        self.pairs.append(5)
        self.pairs.append(0)
        self.pairs.append(1)
        self.pairs.append(2)

    def new_grow(self, size):
        """Grows the contained graph using the transformer

        Args:
            size (int): number of transformations the graph will undergo

        Returns:
            Graph: the resultant graph after transformation
        """
        return self.graph.grow_while_connecting_rods(size, False)

    def simplify(self):
        """Removes all nodes that have less than 3 edges 
        beginning and ending at its location.
        """
        self.graph.simplify_graph()

    def grow(self, size, print_intermed):
        """Grows the contained graph using the transformer

        Args:
            size (int): number of transformations the graph will undergo
            print_intermed (boolean): Whether or not it is desired for the graph to be 
                                        printed intermediate stages of graph growth

        Returns:
            Graph: the resultant graph after transformation
        """
        print("This is a second, different grow method to transform a graph")

    def mutate(self):
        """Mutate a tensegrity
        """
        if random.randint(0,2):
            mut_loc=random.randint(0,len(self.genome))
            self.genome[mut_loc]=random.random()*2.0
        self.mutate_string_labels()

    def mutate_string_labels(self):
        """Changes labels of individual strings within the 
        tensegrity as a means of mutating it
        """
        self.graph.mutate_string_labels()
        self.get_string_labels_from_graph()
