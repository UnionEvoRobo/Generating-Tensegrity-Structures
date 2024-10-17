from tensegrity import Tensegrity
from element import Element
from tens_string import Tens_String
from MyObject import My_Object

class Evaluator:
    """asdf"""
    def __init__ (self):
        self.elements=[]
        self.strings=[]
        self.print_lens = 0
        self.curr_COM=[]
        self.objvect=[]
        self.contact_group=None #REPLACE
        self.WEIGHTS_PER_NET=None #REPLACE
        self.horizontal_gravity_vector=None #REPLACE
        self.DAMP_CONSTANT=None #REPLACE
        self.ANG_DAMP_CONSTANT=None #REPLACE

    #def d_joint_group_empty(self, group):
    #    None
    #
    #def d_world_set_gravity(self, x, y, z):
    #    None
    #
    #def d_body_disable(self, body):
    #    None
    #
    #def d_body_enable(self, body):
    #    None
    #
    #def atoi(self, label):
    #    None
    #
    #def resetT15(self):
    #    None
#
    #def resetT3(self):
    #    None
#
    #def resetT4(self):
    #    None
    #
    #def d_body_add_force(self, body, drag0, drag1, drag2):
    #    None
#
    #def d_body_add_torque(self, body, adrag0, adrag1, adrag2):
    #    None
#
    #def center_of_mass(self, cur_pos):
    #    None
    #
    #def d_body_get_linear_vel(self, body):
    #    None
#
    #def d_body_get_angular_vel(self, body):
    #    None


    def evaluate(self, tens, read_from_file, do_render, stop_on_flag):
        tens.f_print_genom("curnetwork.net")
        cur_tens=tens
        use_networks=0
        render=do_render

        self.reset()
        result=1
        is_static=0
        results=[]
        result=self.make_from_tensegrity(tens,is_static)
        self.reset_tens()
        self.update_string_labels(tens)
        self.copy_weights_to_elements(tens)

        if result!=0:
            self.run_tensegrity(stop_on_flag,render)
            dt=self.distance_traveled(self.curr_COM)
            print(f"distance: {dt}")
            vol=self.get_tensegrity_volume()
            results.append(0.)
            results.append(dt)
        else:
            results.append(0.)
            results.append(0)
        return results
    
    def reset(self):
        com_vector_x=[]
        com_vector_y=[]
        com_vector_z=[]
        self.d_joint_group_empty(self.contact_group)
        self.delete_all_elements=[]
        remove_all_objects()
        stop_flag=0
        collisions_enabled=0
        self.d_world_set_gravity(0,0,0)
        return 0
    
    def make_from_tensegrity(self, ints, is_static):
        return self.make_from_graph(ints.graph(),ints.pairs,is_static)
    
    def make_from_graph(self, in_graph, rods, is_static):
        self.elements=[]
        self.strings=[]
        already_made=[]
        graph_nodes=in_graph.get_nodes()
        graph_edges=in_graph.get_edges()
        node_element_matches=[]
        
        if rods.size()!=graph_nodes.size():
            return 0
        for i in range (rods.size()):
            found=already_made[already_made.index(rods[i])]
            
            
            if True:
            #if (found == alreadyMade.end()):
            

                bottom=i
                top=rods[i]
                node_element_matches.append(graph_nodes[bottom])
                node_element_matches.append(graph_nodes[top])
                already_made.append(i)
                already_made.append(rods[i])
                
                if i==0:
                    self.put_capsule(0.5,0.5,((Element.ELEM_UNIT_LEN/2)+(Element.ELEM_UNIT_RAD*2)))
                elif not is_static:
                    self.put_capsule()
                else:
                    print("making in static positions")
                    self.put_capsule((0.5*i+0.5),(0.5*i+0.5),(0.5*i+0.5))
                cur_obj=self.objvect[self.objvect.size()-1]
                elem1=Element(cur_obj.body)
                self.elements.append(elem1)
                graph_nodes[top].set_element_num(self.elements.size())
                graph_nodes[bottom].set_element_num(self.elements.size())
        
        self.d_body_disable(self.elements[0].body)
        for e in range (graph_edges.size()):
            nodestub=graph_edges[e].stub_label()
            e_from=graph_edges[e].start
            e_to=graph_edges[e].end
            
            index_of_from=-1
            index_of_to=-1
            
            for n in range (node_element_matches.size()):
                if node_element_matches[n]==e_from:
                    index_of_from=n
                if node_element_matches[n]==e_to:
                    index_of_to=n
                if index_of_from!=-1 and index_of_to!=-1:
                    break
            if index_of_from==-1 or index_of_to==-1:
                print("couldn't find self.elements")
                return 0
            from_loc=index_of_from%2
            to_loc=index_of_to%2
            from_el_index=(index_of_from//2)
            to_el_index=(index_of_to//2)
            

            new_string=Tens_String(self.elements[from_el_index],from_loc,self.elements[to_el_index],to_loc,nodestub)
            new_string.set_rod_number(self.atoi(graph_edges[e].get_label()))
            if from_loc==0:
                self.elements[from_el_index].attach_string_to_bottom(new_string)
            else:
                self.elements[from_el_index].attach_string_to_top(new_string)
            if to_loc==0:
                self.elements[to_el_index].attach_string_to_bottom(new_string)
            else:
                self.elements[to_el_index].attach_string_to_top(new_string)
            self.strings.append(new_string)        

    def reset_tens(self):
        if self.elements.size()==15:
            self.resetT15()
        elif self.elements.size()==3:
            self.resetT3()
        else:
            self.resetT4()

    def update_string_labels(self, tens):
        t_graph=tens.graph()
        graph_edges=t_graph.get_edges()
        for s in range (self.strings.size()):
            self.strings[s].set_rod_number(self.atoi(graph_edges[s].get_label()))
    
    def copy_weights_to_elements(self, tens):
        t_vals=tens.genome()
        el_vals=[]
        size_of_tens=t_vals.size()/self.WEIGHTS_PER_NET
        if size_of_tens != self.elements.size():
            print("copyWeights has a size problem!")
            for e in range (self.elements.size()):
                el_vals=[]
                cur_el=self.elements[e]
                for w in range (self.WEIGHTS_PER_NET):
                    el_vals.append(t_vals[(e*self.WEIGHTS_PER_NET+w)])
                cur_el.set_weights(el_vals)

    def attatch_strings_to_nets(self):
        for i in range (self.strings.size()):
            st=self.strings[i]
            label=self.strings[i].label
            val=st.rod_number
            if val!=-1 and val >=0 and val <=self.elements.size():
                cur_el=self.elements[val-1]
                if cur_el==st.from_el():
                    cur_el.set_pull_string(st.from_side,st)
                elif cur_el==st.to_el():
                    cur_el.set_pull_string(st.to_side,st)
                else:
                    print("uh oh...")
        
    def run_tensegrity(self, stop_on_flag, render):
        collisions_enabled=1
        use_networks=1
        self.d_body_enable(self.elements[0].body)
        self.d_world_set_gravity(self,self.horizontal_gravity_vector,0,-0.1)
        self.reset_tens()
        self.attatch_strings_to_nets()
        cur_step=0
        enable_ODE=1
        stop_flag=0
        max_force=-1
        
        
        #start_time=                         #GET TIME!!!!!!!!!!!!
        
        
        avg_vel=0
        velmem=[]
        self.center_of_mass(self.curr_COM)
        MAX_STEPS=10000
        
        
        #do{
        #    i++;
        #    cout<<"2 x "<<i<<"="<<2*i<<endl;
        #}while(i<5);

        while(True):
            if render!=0:
                do_graphics_stuff()
                ds_set_color(0,0,1)
            if cur_step==5000:
                None
                #dBodyEnable(_elements[0]->_body)
                #dWorldSetGravity(world,_Horizontal_Gravity_Vector,0,-0.1)
            if enable_ODE:
                reset_forces()
                if use_networks:
                    self.run_element_networks()
                self.pull_strings()
                self.add_drags()
                max_force=get_max_force()
                g_max_force=max_force
                g_max_vel=get_max_velocity()
                velmem.append(g_max_vel)
                if len(velmem)>100:
                    velmem.pop(0)
                    
                    
                #if (render == 1) drawObjects();
                #if (_collisions_enabled){
                #    dJointGroupEmpty (contactgroup);
                #    dSpaceCollide (space,0,&staticNearCallback);
                #}
                
                
                d_world_step(self,0.007)
                if len(velmem)>50:
                    avg_vel=0
                    for i in velmem:
                        avg_vel+=i
                    avg_vel=avg_vel/len(velmem)
            cur_step+=1


            #if (_stopFlag)
            
             
            if(cur_step<20000):
                continue
            else:
                break
            
        dt = self.distance_traveled(self.curr_COM)
        
    def run_element_networks(self):
        for i in range (self.elements.size()):
            self.elements[i].activate_strings(i)
            if self.print_lens:
                
                
                #f=OPEN "stringlens.txt","a+"
                
                
                for s in range (self.strings.size()):
                    if self.strings[s].rod_number>0:
                        
                        None
                        #fprintf(f,"%d ",_strings[s]->is_active)
                
                
                #fprintf(f,"\n")
                #close f
                
    def pull_strings(self):
        for i in self.strings:
            i.pull()

    def add_drags(self):
        for i in self.elements:
            vel=self.d_body_get_linear_vel(i.body)
            avel=self.d_body_get_angular_vel(i.body)


            drag=[]
            #dVector3 drag;
            

            drag[0] = -1*vel[0]*self.DAMP_CONSTANT
            drag[1] = -1*vel[1]*self.DAMP_CONSTANT
            drag[2] = -1*vel[2]*self.DAMP_CONSTANT
            
            
            adrag=[]
            #dVector3 adrag;
            

            adrag[0] = -1*avel[0]*self.ANG_DAMP_CONSTANT
            adrag[1] = -1*avel[1]*self.ANG_DAMP_CONSTANT
            adrag[2] = -1*avel[2]*self.ANG_DAMP_CONSTANT
            
            self.d_body_add_force(i.body,drag[0],drag[1],drag[2])
            self.d_body_add_torque(i.body,adrag[0],adrag[1],adrag[2])

    def distance_traveled(self, init_pos):
        cur_pos=[]
        self.center_of_mass(cur_pos)
        
        out_vec=[]
        out_vec[0]=cur_pos[0]-init_pos[0]
        out_vec[2]=cur_pos[2]-init_pos[2]
        out_vec[1]=cur_pos[1]-init_pos[1]
        
        distance=fabs(magnitude(out_vec))
        return distance
        
    def get_tensegrity_volume(self):
        my_vol=-1
        try:
            
            es=object()
            #exec_stream_t es( "qconvex", "FS" )
            #es.in() << "3\n"; 
            #es.in() << (int)_elements.size()*2 << "\n";
            for i in self.elements:
                cur_el=i
                top=[]
                bot=[]
                cur_el.get_bot_pos(bot)
                cur_el.get_top_pos(top)
                
                
                #es.in() << bot[0] << " " << bot[1] << " " << bot[2] << "\n";
                #es.in() << top[0] <<" " << top[1] <<" " << top[2] << "\n";
            #es.close_in()
            #std::string s1
            #std::string s2
        
            #std::getline( es.out(), s1)    //get the first line
            #std::string something,area,vol,more; 
            #es.out() >> something; // read the first word of output 
            #es.out() >> area; // read the next word of output 
            #es.out() >> vol; // read the next word of output 
            
            #//  std::cerr << "hello!\n";
            #//    std::cerr << area << "\n";
            #// std::cerr << vol << "\n";

            #std::istringstream i(vol);
            
            #i >> myvol;
            
            
        except TypeError:   #catch( std::exception const & e ) {
                                #std::cerr << "error: "  <<  e.what()  <<  "\n";
                            #} 
            None
        
        return my_vol
    
    def put_capsule(self, x, y, z):
        new_obj=My_Object()
        
        
        #memset (&newobj,0,sizeof(MyObject));
        
        
        new_obj.set_body=d_body_create(world)
        new_obj.set_geom([])
        
        
        #size_t i;
        i=None
        #int j,k;
        j,k=None
        #dReal sides[3];
        sides=None
        #dMass m;
        m=None
        #dMatrix3 R;
        R=None
        


        new_obj.rc=0.9*d_rand_real()
        new_obj.gc=d_rand_real()
        new_obj.bc=d_rand_real()
        if x==0. and y==0. and z==0.:
            d_body_set_position(new_obj.set_body,d_rand_real()*2-1,d_rand_real(*2-1),Element.ELEM_UNIT_LEN/2+Element.ELEM_UNIT_RAD*2)
            d_r_from_axis_and_angle (R,d_rand_real()*2.0-1.0,d_rand_real()*2.0-1.0,d_rand_real()*2.0-1.0,d_rand_real()*10.0-5.0)
            d_body_set_rotation(new_obj.body,R)
        else:
            d_body_set_position(new_obj.body,x,y,z)
            d_r_from_axis_and_angle(R,x*10,y*10,z*10,z*10)
            d_body_set_rotation(new_obj.body,R)
        
        
        #for (k=0; k<3; k++) sides[k] = _ELEM_UNIT_LEN;
            

        sides[0]=Element.ELEM_UNIT_RAD
        
        
        #dMassSetCapsule (&m,DENSITY,3,sides[0],sides[1]);
    
    
        new_geom=d_create_capsule(space,sides[0],sides[1])
        
        
        new_obj.geom.append(new_geom)
        for k in len(new_obj.geom):
            if new_obj.geom[k]:
                d_geom_set_body(new_obj.geom[k],new_obj.body)
            else:
                break


        #dBodySetMass (newobj.body,&m)
        
        
        self.objvect.append(new_obj)
