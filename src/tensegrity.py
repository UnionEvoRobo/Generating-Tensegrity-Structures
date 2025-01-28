"""Tensegrity module for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 1.1
"""

import random
from graph import Graph
from l_system import LSystem

#DEF_L_SYSTEM={}
DEF_L_SYSTEM={"1":"1>2{1}5", "2":"2>4{1}1", "3":"3>4", "4":"4>3{4}4", "5":"5>3"}

class Tensegrity:
    """Tensegrity module for the generation of complex tensegrity structures."""

    def __init__(self, edges, seed=None):
        if seed is not None:
            self.graph=seed.get_graph()
            self.l_system=seed.get_l_system()
            self.pairs=seed.get_pairs()
            self.objectives=seed.get_objective_vals()
            self.num_elements=seed.get_num_elements()
            self.elements_to_shuffle=seed.get_elements_to_shuffle()
            self.strings_to_shuffle=seed.get_strings_to_shuffle()
            self.genome=seed.get_genome()
            self.string_labels=seed.get_string_labels()
            self.indexes_to_ignore=seed.get_indexes_to_ignore()
            self.edge_types=seed.get_edge_types()
        else:
            self.graph=None
            self.l_system=LSystem(DEF_L_SYSTEM)
            self.pairs=[]
            self.objectives=[]
            self.num_elements=0
            self.elements_to_shuffle=[]
            self.strings_to_shuffle=[]
            self.genome=[]
            self.string_labels=[]
            self.indexes_to_ignore=[]
            self.edge_types=edges

    def get_graph(self):
        """Getter for the tensegrity's graph

        Returns:
            Graph: the graph object representing the structure of the tensegrity
        """
        return self.graph

    def get_l_system(self):
        """Getter for the l_system attribute

        Returns:
            l_system: the tensegrities l_system
        """
        return self.l_system

    def get_pairs(self):
        """Getter for the pairs attribute

        Returns:
            list: the list of pairs in the tensegrity
        """
        return self.pairs

    def get_objective_vals(self):
        """Getter for the objectives attribute

        Returns:
            list: the list of objectives
        """
        return self.objectives

    def get_num_elements(self):
        """Getter for the num_elements attribute

        Returns:
            int: the number of elements
        """
        return self.num_elements

    def get_elements_to_shuffle(self):
        """Getter for the elements_to_shuffle attribute

        Returns:
            list: the list of elements_to_shuffle
        """
        return self.elements_to_shuffle

    def get_strings_to_shuffle(self):
        """Getter for the strings_to_shuffle attribute

        Returns:
            list: the list of strings_to_shuffle
        """
        return self.strings_to_shuffle

    def get_genome(self):
        """Getter for the genome attribute of a tensegrity

        Returns:
            list: a list containing the genome attribute of a tensegrity
        """
        return self.genome

    def get_string_labels(self):
        """Getter for the string_labels attribute
        Get a list of the labels of all strings in the graph
        
        Returns:
            list: a list of the labels of all edges in the tensegrities graph
        """
        return self.string_labels

    def get_indexes_to_ignore(self):
        """Getter for the indexes_to_ignore attribute

        Returns:
            list: the list of indexes_to_ignore
        """
        return self.indexes_to_ignore

    def get_edge_types(self):
        """Getter for the edge_types attribute

        Returns:
            list: the list of edge_types
        """
        return self.edge_types






    def set_string_labels_from_graph(self):
        """Get a list of the labels of all strings in the graph
        """
        self.string_labels=self.graph.get_string_labels()

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
        for i in range (int(another_var)):
            self.genome.append(p1gen[i])
        for i in range(int(another_var),len(p2gen)):
            self.genome.append(p2gen[i])

    def find_random_string_labels(self):
        """Find the labels of random strings"""
        self.graph.find_random_string_labels()
        self.set_string_labels_from_graph()

    def f_print_string_labels(self, f_name):
        """Print the list of string labels to a file

        Args:
            file_name (string): File path that we are printing the labels to
        """
        with open(f_name,"w+",encoding="utf-8") as out_file:
            #to_wrt="["
            #j=0
            #while j<(len(self.string_labels)-1):
            #    to_wrt+=j+", "
            #to_wrt+=self.string_labels[-1] + "]"
            out_file.write(str(self.string_labels))
            out_file.close()

    def f_print_genome(self, f_name):
        """Print the list of string labels to a file

        Args:
            file_name (string): File path that we are printing the labels to
        """
        with open(f_name,"w+",encoding="utf-8") as out_file:
            #to_wrt="["
            #j=0
            #while j<(len(self.string_labels)-1):
            #    to_wrt+=j+", "
            #to_wrt+=self.string_labels[-1] + "]"
            out_file.write(str(self.genome))
            out_file.close()

    def length_of_genotype(self):
        """Get the length of the genome

        Returns:
            int: length of the genome attribute
        """
        return len(self.genome)

    def fprint_obj_vals(self, out_file):
        """Print the objective values

        Args:
            out_file (string): name of destination file
        """
        out_file.write(str(self.objectives)+"\n")

    def print_obj_vals(self):
        """Print objective values
        """
        print(self.objectives)

    def set_objectives(self, result):
        """Set the objectives of the tensegrity

        Args:
            result (list): list containing new objectives
        """
        self.objectives=result

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

    def make_tr4(self):
        """Make a three-bar tensegrity
        """
        self.graph = Graph(self.l_system, self.edge_types)
        self.graph.make_tr4()
        self.pairs=[]
        self.pairs.append(4)
        self.pairs.append(5)
        self.pairs.append(6)
        self.pairs.append(7)
        self.pairs.append(0)
        self.pairs.append(1)
        self.pairs.append(2)
        self.pairs.append(3)

    def make_tr15(self):
        """Make a fifteen-bar tensegrity
        """
        #self.l_system.fscan("tr15.tens")
        self.graph = Graph(self.l_system,self.edge_types)
        self.graph.make_tr3()
        # getchar();
        #  _ls->print();
        self.grow(30,0)
        #hard-code element numbers
        elnums=[1,2,3,1,4,3,5,2,6,4,7,5,8,6,9,7,10,8,11,9,12,10,13,11,14,12,15,13,15,14]
        gnomes=self.graph.get_nodes()
        for g in (enumerate(gnomes)):
            #printf("setting %d %d\n",g,elnums[g]);
            gnomes[g].setElementNum(elnums[g])


    def new_grow(self, size):
        """Grows the contained graph using the transformer

        Args:
            size (int): number of transformations the graph will undergo

        Returns:
            Graph: the resultant graph after transformation
        """
        result = self.graph.grow_while_connecting_rods(size, self.l_system, 0)
        return result

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
        result  = self.graph.grow_node_by_node_until_size(size)
        # result  = self.graph.grow_node_by_node_until_size(size,self.l_system)
        tries = 1
        success = 0
        if result == -1:
            success = 0
        else:
            # if it is too small or an odd number of nodes
            #keep trying a few more times
            if ((self.graph.order() < size) or (self.graph.order()%2 != 0)):
                do=True
                while do:
                    self.graph.grow_node_by_node_until_size(size+tries)
                    # self.graph.grow_node_by_node_until_size(size+tries,self.l_system,print_intermed)
                    tries+=1
                    if ((self.graph.order() < size) or (self.graph.order()%2 != 0)) and (
                        tries < 10):
                        do=False
                if ((self.graph.order() >= size) and (self.graph.order()%2 == 0)):
                    success = 1
            else:# it is good
                success = 1
        if success:
            self.find_pairs()
        return success


    def mutate(self):
        """Mutate a tensegrity"""
        if random.randint(0,2):
            if len(self.genome)==0:
                self.genome.append(random.random()*2.0)
            if len(self.genome)==1:
                self.genome[0]=random.random()*2.0
            else:
                mut_loc=random.randint(0,len(self.genome)-1)
                self.genome[mut_loc]=random.random()*2.0
        self.mutate_string_labels()

    def mutate_string_labels(self):
        """Changes labels of individual strings within the 
        tensegrity as a means of mutating it
        """
        self.graph.mutate_string_labels()
        self.set_string_labels_from_graph()

    def find_pairs(self):
        """Find pairs of nodes."""
        self.pairs.clear()
        self.pairs=self.graph.find_matching_pairs_recursively()
