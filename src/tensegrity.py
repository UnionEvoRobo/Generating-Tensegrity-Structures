"""Tensegrity module for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 4.0
"""

import random
from graph import Graph
from l_system import LSystem
from strut import Strut

MAX_INT=2147483647
# DEF_L_SYSTEM={"1":"2{1}5", "2":"4{1}1", "3":"4", "4":"3{4}4", "5":"3"}
DEF_L_SYSTEM={1:[2,1,5],2:[4,1,1],3:[4,-1,-1],4:[3,4,4],5:[3,-1,-1]}

class Tensegrity:
    """Tensegrity module for the generation of complex tensegrity structures."""

    def __init__(self, edges, seed=None):
        if seed is not None:
            seed_rules=seed.get_l_system()
            rules={}
            for i in edges:
                rules[i]=seed_rules.get_rule(i)
            self.l_system=LSystem(rules)
            self.edge_types=seed.get_edge_types()
            self.comment=f'{seed.get_comment()}\n\n// {self.l_system}'
        else:
            self.l_system=LSystem(DEF_L_SYSTEM)
            self.edge_types=edges
            self.comment=f'{self.l_system}'
        self.graph=None
        self.pairs=[]
        self.struts=[]
        self.objectives=[]
        self.num_elements=0
        self.elements_to_shuffle=[]
        self.strings_to_shuffle=[]
        self.genome=[]
        self.string_labels=[]
        self.indexes_to_ignore=[]

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

    def get_comment(self):
        """Getter for the comment string

        Returns:
            string: the comment string to be printed in the graph .dot file
        """
        return self.comment

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

    def get_struts(self):
        """Getter for the genome attribute of a tensegrity

        Returns:
            list: a list containing the genome attribute of a tensegrity
        """
        return self.struts

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
        self.comment+="\n// Crossed"
        parent1:Tensegrity=parent1
        parent2:Tensegrity=parent2
        l1=parent1.get_l_system()
        l2=parent2.get_l_system()
        l_system=self.get_l_system()

        for i in range (1,int(another_var)+1):
            l_system.set_rule(i,l1.get_rule(i))
        for i in range(int(another_var)+1,l2.size()+1):
            l_system.set_rule(i,l2.get_rule(i))

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
            out_file.write(str(self.string_labels))
            out_file.close()

    def f_print_genome(self, f_name):
        """Print the list of string labels to a file

        Args:
            file_name (string): File path that we are printing the labels to
        """
        with open(f_name,"w+",encoding="utf-8") as out_file:
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
        self.graph=Graph(self.l_system, self.edge_types, 3)
        self.graph.make_tr3()
        self.pairs=[]
        self.struts=[]
        self.struts.append(Strut(self.graph.node_list[0],self.graph.node_list[3]))
        self.struts.append(Strut(self.graph.node_list[1],self.graph.node_list[4]))
        self.struts.append(Strut(self.graph.node_list[2],self.graph.node_list[5]))

    def make_tr4(self):
        """Make a three-bar tensegrity
        """
        self.graph = Graph(self.l_system, self.edge_types, 4)
        self.graph.make_tr4()
        self.pairs=[]
        self.struts=[]
        nodes=self.graph.get_nodes()
        gnomes=self.graph.find_matching_pairs_recursively()
        for x in gnomes:
            edge=[]
            for j in x:
                edge.append(j.get_label())
            print (edge)
        self.struts.append(Strut(nodes[4],nodes[0]))
        self.struts.append(Strut(nodes[5],nodes[1]))
        self.struts.append(Strut(nodes[6],nodes[2]))
        self.struts.append(Strut(nodes[7],nodes[3]))

    def make_trx(self,num_bars):
        """Make an x-bar tensegrity
        """
        self.graph = Graph(self.l_system,self.edge_types,num_bars)
        self.graph.make_tr3()
        grown=self.grow(num_bars*2,0)
        if grown:
            gnomes=self.get_pairs()
            for i in gnomes:
                self.struts.append(Strut(i[0],i[1]))
            return True
        return False

    def new_grow(self, size, graph_num):
        """Grows the contained graph using the transformer

        Args:
            size (int): number of transformations the graph will undergo

        Returns:
            Graph: the resultant graph after transformation
        """
        result = self.graph.grow_while_connecting_rods(size, 1, graph_num, DEF_L_SYSTEM)
        self.simplify()
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
        growth  = self.graph.grow_node_by_node_until_size(size)
        tries = 1
        success = False
        if not growth:
            if self.graph.order() >= size:
                success = True
        else:
            # if it is too small or an odd number of nodes
            #keep trying a few more times
            if ((self.graph.order() < size) or (self.graph.order()%2 != 0)):
                if self.graph.order()>80:
                    self.graph.remove_node(self.graph.get_nodes()[-1])
                    success=True
                else:
                    do=True
                    while do:
                        growth=self.graph.grow_node_by_node_until_size(size+tries)
                        tries+=1
                        if ((self.graph.order() < size) or (self.graph.order()%2 != 0)) and (
                            tries < 6) or (not growth):
                            do=False
                    if self.graph.order() >= size:
                        success = True
            else:# it is good
                success = True
        if success:
            self.find_pairs()
        return success

    def mutate(self):
        """Perform a secondary mutation a tensegrity"""
        self.comment+="\n// Mutated"
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

    def prim_mut(self, rate):
        """Mutate the l_system of the tensegrity

        Args:
            rate (int): mutation rate of the l_system
        """
        for i in self.edge_types:
            if self.binom(rate):
                self.l_system.new_rule(i)

    def binom(self, prob):
        """Helper function for random seed generation"""
        t=(100.0*random.randint(0,MAX_INT)/(MAX_INT + 1.0))
        if t < prob:
            return 1
        return 0

    def draw_graph(self, graph_num, def_l, made, num):
        """Print a unique graph

        Args:
            graph_num (int): number label for graph
            def_l (dict): default l_system to compare against to determine uniqueness
            made (boolean): whether or not the tensegrities graph was successfully grown
            num (int): number of desired struts
        """
        if self.get_l_system().rule_dict!=def_l and len(self.struts)==num and made:
            self.comment+="\n// Rules:"
            com_str=""
            x:Strut
            for x in self.struts:
                edge=[]
                edge.append(x.get_top().get_label())
                edge.append(x.get_bottom().get_label())
                com_str+=f"\n// {edge}"
            com_str+=f"\n// {len(self.struts)}"
            # print (len(self.struts))
            self.comment+=f"{com_str}"
            self.graph.draw_graph(graph_num,self.comment)
