"""Evaluator module for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 2.1   #####DISABLED LINES IN MAKE FROM GRAPH!!! ADDRESS!!!!
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
        self.struts=[]
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
        #tens.print_genome("curnetwork.net")
        result=1
        results=[]
        result=self.make_from_tensegrity(tens)      #NO ODE
        #self.reset_tens()                           #NO ODE
        self.update_string_labels(tens)             #NO ODE

        if result!=0:
            endpoints=[]
            i:TensString
            for i in self.strings:
                endpoints.append(int(i.get_from().get_label()))
                endpoints.append(int(i.get_to().get_label()))
            self.black_box(len(self.struts),endpoints)
            #self.run_tensegrity(stop_on_flag,render)
            #dt=self.distance_traveled(self.curr_com)
            #print(f"distance: {dt}")
            results.append(0.)
            #results.append("Something from the evaluation goes here")
            results.append(random.randint(1,20))
            #results.append(dt)
        else:
            results.append(0.)
            results.append(0)
        return results

    def black_box(self, num_struts, string_ends):
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
                                        matlab.int32(num_struts),nargout=nargout_num)
        #print(returnvals)
        x=returnvals[0][0]
        string_list=[]
        for i in x:
            string_list.append(i)
        #print("BLACK BOX BLACK BOX BLACK BOX BLACK BOX BLACK BOX BLACK BOX")

    def reset(self):
        """Resets a tensegrity"""
        self.com_vector_x=[]
        self.com_vector_y=[]
        self.com_vector_z=[]
        self.contact_group=[]
        self.struts=[]
        #self.remove_all_objects()
        #self.d_world_set_gravity(0,0,0)

    def make_from_tensegrity(self, ints:Tensegrity):
        """Creates a model from a tensegrity

        Args:
            ints (tensegrity): the tensegrity structure being modeled

        Returns:
            int: indicates whether or not the tensegrity was modeled
        """
        return self.make_from_graph(ints.get_graph(),ints.pairs)

    def make_from_graph(self, graph:Graph, rods:list[Node]):
        """Creates a model from a tensegrities graph

        Args:
            graph (graph): the graph of the tensegrity structure being modeled
            rods (list): list of paired nodes in the tensegrity

        Returns:
            int: indicates whether or not the tensegrity was modeled
        """
        self.struts:list[Strut]=[]
        self.strings=[]
        already_made=[]
        self.num_string=1
        graph_nodes=graph.get_nodes()
        graph_edges:list[Edge]=graph.get_edges()
        node_element_matches=[]

                                                #if len(rods)!=len(graph_nodes):
                                                #    return 0
        i=0
        while i<len(rods):
            #for i in rods:

            #ind=rods.index(i)
            temp=rods[i]
            if len(already_made)<temp:
                bottom=i
                top=rods[i]
                node_element_matches.append(graph_nodes[bottom])
                node_element_matches.append(graph_nodes[top])
                already_made.append(bottom)
                already_made.append(top)

                #if ind==0:
                #    self.put_capsule(0.5,0.5,((Element.ELEM_UNIT_LEN/2)+(Element.ELEM_UNIT_RAD*2)))
                #elif not is_static:
                #    self.put_capsule(0,0,0)
                #else:
                #    print("making in static positions")
                #    self.put_capsule((0.5*ind+0.5),(0.5*ind+0.5),(0.5*ind+0.5))         #REPLACE
                #cur_obj=self.objvect[(self.objvect)-1]

                elem1 = Strut(graph_nodes[top],graph_nodes[bottom])
                self.struts.append(elem1)
                #graph_nodes[top].set_label(len(self.elements))
                #graph_nodes[bottom].set_label(len(self.elements))
            i+=1

        #self.d_body_disable(self.elements[0].body)          #REPLACE
        for e in graph_edges:
            index_of_from=-1
            index_of_to=-1

            for n in node_element_matches:
                if n==e.get_start():
                    index_of_from=node_element_matches.index(n)
                elif n==e.get_end():
                    index_of_to=node_element_matches.index(n)
                elif index_of_from!=-1 and index_of_to!=-1:
                    break
                                                        #if index_of_from==-1 and index_of_to==-1:
                                                        #    print("couldn't find self.elements")
                                                        #    return 0
            from_loc=index_of_from%2
            to_loc=index_of_to%2
            from_el_index=index_of_from//2
            to_el_index=index_of_to//2



            new_string=TensString(self.struts[from_el_index],
                            self.struts[to_el_index],self.num_string)
            self.num_string+=1
            new_string.set_rod_number(int(e.get_label()))
            from_strut:Strut=self.struts[from_el_index]
            to_strut:Strut=self.struts[to_el_index]
            if from_loc==0:
                from_strut.attach_string_to_bottom(new_string)
                new_string.set_from(from_strut.get_bottom())
            else:
                from_strut.attach_string_to_top(new_string)
                new_string.set_from(from_strut.get_top())
            if to_loc==0:
                to_strut.attach_string_to_bottom(new_string)
                new_string.set_to(to_strut.get_bottom())
            else:
                to_strut.attach_string_to_top(new_string)
                new_string.set_to(to_strut.get_top())
            self.strings.append(new_string)
        return 1

    def update_string_labels(self, tens:Tensegrity):
        """Updates the labels for strings in the tensegrity

        Args:
            tens (tensegrity): the tensegrity being modified
        """
        t_graph=tens.get_graph()
        graph_edges=t_graph.get_edges()
        s:TensString
        for s in self.strings:
            s.set_rod_number(int(graph_edges[self.strings.index(s)].get_label()))




if __name__=='__main__':
    world=Evaluator()
    struts:int=3
    strings=[0, 1, 1, 2, 2, 0, 3, 4, 4, 5, 5, 3, 4, 0, 5, 1, 3, 2]
    world.black_box(struts,strings)
