"""Evaluator module for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 3.0
"""
import os
import random
import matlab.engine
from edge import Edge
from graph import Graph
from node import Node
from strut import Strut
from tens_string import TensString
from tensegrity import Tensegrity

MAX_INT=2147483647


class Evaluator:
    """Evaluator module for the generation of complex tensegrity structures."""

    def __init__ (self):
        self.eng = matlab.engine.start_matlab()
        self.strings=[]
        self.print_lens=0
        self.num_string=1
        self.curr_com=[]
        self.objvect=[]
        self.contact_group=[] #REPLACE
        self.com_vector_x=[]
        self.com_vector_y=[]
        self.com_vector_z=[]

    def evaluate(self, tens: Tensegrity):
        """Evaluates a tensegrity

        Args:
            tens (tensegrity): The tensegrity structure being evaluated

        Returns:
            list: list of results from the evaluation
        """
        # tens.print_genome("curnetwork.net")
        result=1
        results=[]
        result=self.make_from_tensegrity(tens)
        self.update_string_labels(tens)

        if result:
            endpoints=[]
            i:TensString
            for i in self.strings:
                endpoints.append(int(i.get_from().get_label()))
                endpoints.append(int(i.get_to().get_label()))
            #self.black_box(tens.get_struts(),endpoints)
            #self.run_tensegrity(stop_on_flag,render)
            #dt=self.distance_traveled(self.curr_com)
            #print(f"distance: {dt}")
            results.append(0.)
            #Something from the evaluation goes here
            results.append(random.randint(1,20))
            #results.append(dt)
        else:
            results.append(0.)
            results.append(0)
        return results

    def black_box(self, struts, string_ends):
        """Interfaces with the form finding function
        Args:
            struts (int): number of struts in the proposed tensegrity
            strings (list): list of endpoint pairs of strings in proposed tensegrity
                            The 2n'th element is the endpoint/bottom for the string 
                            that originates at the top node indicated by the element 2n-1
        """

        cwd = os.getcwd()
        self.eng.cd(cwd, nargout=0)
        nargout_num=2
        #presuming blackbox expects g to be an array of ints, num_struts is an int, and tk is tbd
        returnvals=self.eng.blackBox(matlab.int32(string_ends),
                                        matlab.int32(len(struts)),nargout=nargout_num)
        print(returnvals)
        i:Strut
        x=[]
        for i in struts:
            x.append((i.get_top().get_label(),i.get_bottom().get_label()))
        print(x)
        x=returnvals[0][0]
        string_list=[]
        for i in x:
            string_list.append(i)
        #print("BLACK BOX BLACK BOX BLACK BOX BLACK BOX BLACK BOX BLACK BOX")

    def make_from_tensegrity(self, ints:Tensegrity):
        """Creates a model from a tensegrity

        Args:
            ints (tensegrity): the tensegrity structure being modeled

        Returns:
            int: indicates whether or not the tensegrity was modeled
        """
        return self.make_from_graph(ints.get_graph(),ints.struts)

    def make_from_graph(self, graph:Graph, rods:list[Node]):
        """Creates a model from a tensegrities graph

        Args:
            graph (graph): the graph of the tensegrity structure being modeled
            rods (list): list of paired nodes in the tensegrity

        Returns:
            int: indicates whether or not the tensegrity was modeled
        """
        self.strings=[]
        self.num_string=1
        graph_nodes=graph.get_nodes()
        graph_edges:list[Edge]=graph.get_edges()
        strut_endpoints=[]
        if len(rods)!=len(graph_nodes)/2:
            return False
        i:Node
        for i in rods:
            strut_endpoints.append(i.get_bottom())
            strut_endpoints.append(i.get_top())
        #create strings between the struts
        for e in graph_edges:
            index_of_from=-1
            index_of_to=-1
            #get the indices of the start and end of the edge in strut_endpoints
            for n in strut_endpoints:
                if n==e.get_start():
                    index_of_from=strut_endpoints.index(n)
                elif n==e.get_end():
                    index_of_to=strut_endpoints.index(n)
                elif index_of_from!=-1 and index_of_to!=-1:
                    break
            if index_of_from==-1 or index_of_to==-1:
                print("couldn't find pair")
                return False
            from_el_index=index_of_from//2
            to_el_index=index_of_to//2
            #create a new string based on the graph edge
            new_string=TensString(rods[from_el_index],
                            rods[to_el_index],self.num_string)
            self.num_string+=1
            new_string.set_rod_number(int(e.get_label()))
            from_strut:Strut=rods[from_el_index]
            to_strut:Strut=rods[to_el_index]
            #attatch the string to the struts
            if (index_of_from%2)==0:
                from_strut.attach_string_to_bottom(new_string)
                new_string.set_from(from_strut.get_bottom())
            else:
                from_strut.attach_string_to_top(new_string)
                new_string.set_from(from_strut.get_top())
            if (index_of_to%2)==0:
                to_strut.attach_string_to_bottom(new_string)
                new_string.set_to(to_strut.get_bottom())
            else:
                to_strut.attach_string_to_top(new_string)
                new_string.set_to(to_strut.get_top())
            self.strings.append(new_string)
        return True

    def update_string_labels(self, tens:Tensegrity):
        """Updates the labels for strings in the tensegrity

        Args:
            tens (tensegrity): the tensegrity being modified
        """
        t_graph=tens.get_graph()
        graph_edges=t_graph.get_edges()
        s:TensString
        for i,s in enumerate(self.strings):
            s.set_rod_number(int(graph_edges[i].get_label()))
