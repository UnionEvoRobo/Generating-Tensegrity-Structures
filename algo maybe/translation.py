from tensegrity import Tensegrity
from evaluator import Evaluator
import os
import time
import random


MAX_GRAPH_SIZE=4
SAMPLE_RATE=None
INIT_POP_SIZE=None
MAX_GENS=None
MAX_INT=2147483647

class Algo:

    def __init__ (self, in_file_name, do_log, randseed, render, seed):
        """(self, samp_rate, init_pop_size, max_gen, in_file_name, dest_file_name, x_over, fitness, rend):
        daf"""
        self.render=render
        self.config_file=in_file_name
        self.random_seed=randseed
        self.pop=[]
        self.max_int=MAX_INT
        self.init_pop_size=INIT_POP_SIZE
        self.sample_rate=SAMPLE_RATE
        self.max_gens=MAX_GENS
        self.max_graph_size=MAX_GRAPH_SIZE
        self.indexes_to_ignore=[]
        self.x_over_rate=None
        self.ls_mut_rate=None
        self.mut_rate=None
        self.fitness_index=None
        self.do_logging=do_log
        self.world=None
        self.debug=None
        self.init(seed)
        #self.out_file_name=dest_file_name


    
    def init(self, seed):
        self.fscan_config()
        
        self.world=Evaluator()
        
        prev_best=0
        self.pop=[]
        num_added=0
        
        print(f"init pop size is: {self.init_pop_size}.")
        while num_added<self.init_pop_size:
            tens=None
            if seed is None:
                tens=Tensegrity({},[],None)
            else:
                tens=Tensegrity(None,None,None,seed)
                tens.find_random_string_labels()
            if self.pop.count(tens)!=0:
                print("IM CONFUSED!!!!")
                #delete tens !!!!!! delete it from the pop???
            else:
                self.pop.append(tens)
                num_added+=1
        self.debug=0
        self.evaluate_pop()
        
    def delete_ith_member(self, i):
        if self.pop[i] != None:
            print(f"deleting {i}")

            
                        #delete pop[to_del]
            
            
            self.pop[i]=None
            return 0
        else:
            print("Pop::delete_ith_member() - null")
            return -1    
    
    def add_new_members(self, cur_gen):
        orig_pop_size=len(self.pop)
        p1_index = 0
        p2_index = 0
        num_added = 0
        New_Pop_Limit=0
        #Tensegrity *p1
        #Tensegrity *p2
        #Tensegrity *as1
        #Tensegrity *as2
        
        as1_mut_worked=0
        as2_mut_worked=0

        roulette_wheel = self.make_roulette(self.fitness_index)
        
        while self.pop.size() < self.init_pop_size:
            if (self.x_over_rate==-1): #this is for purely random search
                as1=Tensegrity() #hard coded for now
                as2=Tensegrity()
            else:   #normal xover or mutation.
                #2 should be missing index or most relevant objective
                #spin the wheel...
                p1_val=self.int_rand_in_range(len(roulette_wheel))
                #find the index
                p1_index=roulette_wheel[p1_val]
                #select the parent
                p1=self.pop[p1_index]
                #and again for p2
                #random.randrange(len(roulette_wheel))
                p2_val=self.int_rand_in_range(len(roulette_wheel))
                p2index=roulette_wheel[p2_val]
                p2=self.pop[p2index] 
                if (self.binom(self.x_over_rate) and (p1_index != p2_index)):
                    p1x1 = 0

                    p1x1=self.int_rand_in_range(p1.lengthOfGenotype())

                    as1 =Tensegrity(p1)
                    as1.crossover(p1,p2,p1x1)
                    
                    as2 =Tensegrity(p2)
                    as2.crossover(p2,p1,p1x1)
                else: #we mutate
                    as1 =Tensegrity(p1)
                    as1_mut_worked=as1.mutate()
                    as2 =Tensegrity(p2)
                    as_2mut_worked = as2.mutate()
            #if as1 doesn't belong get rid of it.
            if not self.is_in_pop(as1):
                self.evaluate_member(as1) #don't grow!
                self.pop.append(as1)
                num_added+=1
            if not self.is_in_pop(as2):
                self.evaluate_member(as2) # don't grow!
                self.pop.append(as2)
                num_added+=1
    
    def print_pop(self):
        maxit=self.pop.size()
        for i in range (maxit):
            self.pop[i].print_obj_vals()
    
    def fprint_pop_stats(self, gennum):
        out_file=""
        with open(self.out_file_name,"a+",encoding="utf-8") as out_file:
            for i in range (self.size()):
                out_file.append(f"{gennum}")
                self.pop[i].fprint_obj_vals(out_file)
            out_file.close()
        
    def fprint_best(self, fname):
        dot_file_name=""    
        dot_file_name=f"{fname}.net"
        self.pop[0].f_print_genome(dot_file_name)
        self.pop[0].print_genome()
        string_file_name=""
        string_file_name=f"{fname}.str"
        self.pop[0].f_print_string_labels(string_file_name)
        print("hello!")
        result=self.world.evaluate(self.pop[0],0,0,0)
        print(result)
        print("olleh!")
        
    def evaluate_member(self, member):
        result = self.world.evaluate(member,0,self.render,0)
        member.set_objectives(result)
        
    def evaluate_pop(self):
        maxit=self.pop.size()
        for i in range (maxit):
            self.evaluate_member(self.pop[i])
         
    

    def make_roulette(self, index):
        """makes a roulette wheel by creating N entries 
        for every member of the pop,
        where N == surviving_children()
        the values of the roulette are indeces to the pop
        i.e. if individual 5 has 3 surviving children,
        and     individual 6 has 2 
        and     indivitual 7 has 4 then the roulette will look like
        ....555667777...
        same, but for a given index, not just num children"""
        roul=[]
        roul.clear()
        use_funky_roulette=0 #funky roulette is for objectives that you're trying to minimize
        min_biomass=0.
        for i in range (self.pop.size()):
            #calculate the sum of fitnesses across this index
            objs=self.pop[i].get_objective_vals()
            roulettechunk=objs[index]*100
        
        #use this for maximizing    
        #make a number of roulette slots proportional to your biomass    
        for j in range (roulettechunk):
            roul.append(i)
        return roul

    
    
    
    def fscan_config(self):
        with open(self.config_file,"r",encoding="utf-8") as in_file:

            # the defaults
            self.max_gens = 1000
            self.init_pop_size= 100
            self.x_over_rate = 60
            self.ls_mut_rate =5
            self.mut_rate = 2
            self.sample_rate = 1
            self.fitness_index = 3 #this is the index we care about

            print(f"TPop.fscan_config() - filename is {self.config_file}")
            if not in_file:
                print(f"cannot open file {self.config_file} for input")
                return -1
            val=None
            in_string=""
            for line in in_file:
                if line!=EOFError:
                    line_split=line.split("=")
                    in_string=line_split[0]
                    val=line_split[1]
                    match in_string:
                        case "max_gens":
                            self.max_gens=val
                        case "init_pop_size":
                            self.init_pop_size=val
                        case "x_over_rate":
                            self.x_over_rate=val
                        case "ls_mut_rate":
                            self.ls_mut_rate=val
                        case "mut_rate":
                            self.mut_rate=val
                        case "sample_rate":
                            self.sample_rate=val
                        case "fitness_index":
                            self.fitness_index=val
                        case _:
                            print(f"unrecognized input value to parse_config: {in_string}!")
            in_file.close()
            return 0

    def run(self):
        
        num_iters = 0
        num_gens = 0
        
        rand_seed = int(time.time())
        
        #if _rand_seed==0:
        #    _rand_seed=self.signed_to_unsigned(rand_seed,4)
        print(f"Random Seed is: {rand_seed}")
        random.seed(rand_seed)
        
        tod=time.localtime(rand_seed)
        
        num_of_day=0
        
        root_file_name=""
        out_file_name=""
        best_file_name=""
        
        if self.do_logging:
            while 1:
                root_file_name=f"data_tmp/{num_of_day}-{tod.tm_hour}.{tod.tm_mday}.{tod.tm_mon}.{tod.tm_min}.{tod.tm_sec}"
                out_file_name=f"{root_file_name}.gen"
                best_file_name=f"{root_file_name}.best"

                temp_out=open(out_file_name,'r',encoding="utf-8")
                #if file doesn't exist, good!
                if not temp_out:
                    break
                else:
                    num_of_day+=1

            print(f"genfile is: {out_file_name}")
            print(f"bestfile is: {best_file_name}")
            
            sys_cmd=""
            
            sys_cmd=f"ln -s -f {out_file_name} gen.latest.$HOSTNAME"
            if os.system(sys_cmd)!=0:
                print("Error linking data file!")
            sys_cmd=f"ln -s -f {best_file_name} best.latest.$HOSTNAME"
            if os.system(sys_cmd)!=0:
                print("Error linking data file!")
                
        iter_Limit = 1000
        
        while num_gens < self.max_gens:
            self.sort_pop_by_fitness()
            self.maintain_diversity()
            self.cull_bottom_half()
            if ((num_gens % self.sample_rate) == 0):
                print(f"************ generation {num_gens} ********************\n")
                self.fprint_pop_stats(num_gens)
                self.fprint_best(best_file_name) 
                self.print_pop()
            self.add_new_members(num_gens)
            num_gens+=1
            num_iters=num_gens

    def maintain_diversity(self):
        print("hello")
        to_delete=[]
        to_delete.clear()
        for i in range (self.pop.size()):
            vals=self.pop[i].get_objective_vals()
            cur_val=vals[self.fitness_index]
            for j in range ((i-5),0,-1):
                print("maintain: i:{i} j:{j}")
                other_vals=self.pop[j].get_objective_vals
                other_val=other_vals[self.fitness_index]
                if cur_val==other_val:
                    print(f"{cur_val} == {other_val}, will delete {j}")
                    to_delete.append(j)
                else:
                    break
            i=j
        print("erasing...")
        for i in range (len(to_delete)):
            to_del=to_delete[i]
            print(f"deleting {to_del}")            
            self.pop.pop(to_del)
            print(f"and erasing {None}")
            #self.delete_ith_member(to_delete[i])
        print("done erasing")

    def is_in_pop(self, ts):
        for i in range (self.pop.size()):
            if ts.equals(self.pop[i]):
                return 1
        return 0

    def sort_pop_by_fitness(self):
        for i in range (self.pop.size()):
            cur_tens=self.pop[i]
            cur_objs=cur_tens.get_objective_vals()
            max_val=cur_objs[self.fitness_index]
            max_index=i

            for j in range (self.pop.size()):
                j_tens=self.pop[j]
                job_js=j_tens.get_objective_vals()
                cur_val=job_js[self.fitness_index]
                if cur_val > max_val:
                    max_index=j
                    max_val=cur_val
            tmp=self.pop[i]
            self.pop[i]=self.pop[max_index]
            self.pop[max_index]=tmp

    def cull_bottom_half(self):
        from_val=self.init_pop_size/2
        print(f"culling - from_val+1 is {from_val+1}, pop.size is: {self.pop.size()}")
        for i in range (from_val+1,self.pop.size()):
            self.delete_ith_member(i)
        print("done culling")
        self.pop.resize(from_val)
        print("done resizing")

    
        
    
       
        


#############################HELPER FUNCTIONS (my_utils)#############################

    def binom(self, prob):
        t=(100.0*random.randint(0,self.max_int)/(self.max_int + 1.0))
        print(t)
        if (t < prob):
            return 1
        else:
            return 0

    def int_rand_in_range(self, range):
        return (range*random.randint(0,self.max_int)/(self.max_int+1.0))

    def signed_to_unsigned(self, n, byte_count): 
        return int.from_bytes(n.to_bytes(byte_count, 'little', signed=True), 'little', signed=False)
    
    
    
    
    