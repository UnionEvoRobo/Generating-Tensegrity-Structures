"""YOU DONT MATTER
WEIGHTS_PER_NET=9 #REPLACE
DAMP_CONSTANT=6 #REPLACE
ANG_DAMP_CONSTANT=15 #REPLACE
DAMP_CONSTANT=1 #REPLACE
ANG_DAMP_CONSTANT=10 #REPLACE
"""

#from tensegrity import Tensegrity
from element import Element
from tens_string import Tens_String
from MyObject import My_Object

MAX_INT=2147483647


class Evaluator:
    """ASDF"""

    def __init__ (self):
        self.elements=[]
        self.strings=[]
        self.print_lens=0
        self.num_string=1
        self.curr_com=[]
        self.objvect=[]
        self.contact_group=[] #REPLACE
        self.com_vector_x=[]
        self.com_vector_y=[]
        self.com_vector_z=[]

    def evaluate(self, tens):
        """ADSF"""
        #tens.print_genome("curnetwork.net")
        result=1
        results=[]
        result=self.make_from_tensegrity(tens)      #NO ODE
        #self.reset_tens()                           #NO ODE
        self.update_string_labels(tens)             #NO ODE

        if result!=0:
            print("BLACK BOX BLACK BOX BLACK BOX BLACK BOX BLACK BOX BLACK BOX")
            #self.run_tensegrity(stop_on_flag,render)
            #dt=self.distance_traveled(self.curr_com)
            #print(f"distance: {dt}")
            results.append(0.)
            results.append("Something from the evaluation goes here")
            #results.append(dt)
        else:
            results.append(0.)
            results.append(0)
        return results

    def reset(self):
        """ADSF"""
        self.com_vector_x=[]
        self.com_vector_y=[]
        self.com_vector_z=[]
        self.contact_group=[]
        self.elements=[]
        #self.remove_all_objects()
        #self.d_world_set_gravity(0,0,0)
        return 0

    def make_from_tensegrity(self, ints):
        """ADSF"""
        return self.make_from_graph(ints.get_graph(),ints.pairs)

    def make_from_graph(self, graph, rods):
        """ADSF"""
        self.elements=[]
        self.strings=[]
        already_made=[]
        graph_nodes=graph.get_nodes()
        graph_edges=graph.get_edges()
        node_element_matches=[]

        if len(rods)!=len(graph_nodes):
            return 0
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
                elem1 = Element()
                self.elements.append(elem1)
                graph_nodes[top].set_element_num(len(self.elements))
                graph_nodes[bottom].set_element_num(len(self.elements))
            i+=1

        #self.d_body_disable(self.elements[0].body)          #REPLACE
        for e in graph_edges:
            index_of_from=-1
            index_of_to=-1

            for n in node_element_matches:
                if n==e.get_start():
                    index_of_from=node_element_matches.index(n)
                if n==e.get_end():
                    index_of_to=node_element_matches.index(n)
                if index_of_from!=-1 and index_of_to!=-1:
                    break
            if index_of_from==-1 or index_of_to==-1:
                print("couldn't find self.elements")
                return 0
            from_loc=index_of_from%2
            to_loc=index_of_to%2
            from_el_index=index_of_from//2
            to_el_index=index_of_to//2


            #new_string=Tens_String(self.elements[from_el_index],
            #                       from_loc,self.elements[to_el_index],to_loc,nodestub)
            new_string=Tens_String(self.elements[from_el_index],self.elements[to_el_index],self.num_string)
            self.num_string+=1
            new_string.set_rod_number(int(e.get_label()))
            if from_loc==0:
                self.elements[from_el_index].attach_string_to_bottom(new_string)
            else:
                self.elements[from_el_index].attach_string_to_top(new_string)
            if to_loc==0:
                self.elements[to_el_index].attach_string_to_bottom(new_string)
            else:
                self.elements[to_el_index].attach_string_to_top(new_string)
            self.strings.append(new_string)
        return 1

    """def reset_tens(self):  #FEELS LIKE THIS SHOULD BE PASSING A SPECIFIC TENSEGRITY TO BE RESET!!!!!
        if len(self.elements)==15:
            self.reset_t15()
        elif len(self.elements)==3:
            self.reset_t3()
        else:
            self.reset_t4()

    def reset_t15(self):
        return None

    def reset_t3(self):
        return None

    def reset_t4(self):
        return None"""

    def update_string_labels(self, tens):
        """ADSF"""
        t_graph=tens.get_graph()
        graph_edges=t_graph.get_edges()
        for s in self.strings:
            s.set_rod_number(int(graph_edges[self.strings.index(s)].get_label()))
