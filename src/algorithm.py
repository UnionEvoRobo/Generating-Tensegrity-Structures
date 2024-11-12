"""Algorithm object for the generation of complex tensegrity structures.


@author: Daniel Casper
@version: 1.0
"""
import os
import time
import random
from evaluator import Evaluator
from tensegrity import Tensegrity

MAX_GRAPH_SIZE=4
MAX_INT=2147483647
EDGES=["1","2","3","4","5"]
MAX_GENS= 2
INIT_POP_SIZE= 4
X_OVER_RATE = 60
LS_MUT_RATE =5
MUT_RATE = 2
SAMPLE_RATE = 1
FITNESS_INDEX = 1 #this is the index we care about


class Algorithm:
    """Algorithm object for the generation of complex tensegrity structures."""
    def __init__ (self, in_file_name, do_log, seed):
        self.pop=[]
        self.init_pop_size=None
        self.sample_rate=None
        self.max_gens=None
        self.x_over_rate=None
        self.ls_mut_rate=None
        self.mut_rate=None
        self.fitness_index=None
        self.do_logging=do_log
        self.world=None
        self.init(in_file_name,seed)
        self.out_file_name=None

    def init(self, config, seed):
        """Initializes the population and reads in the config file

        Args:
            config (string): path for the config file
            seed (Tensegrity): tensegrity seed to start the population with
        """
        self.fscan_config(config)

        self.world=Evaluator()

        #prev_best=0
        self.pop=[]
        num_added=0

        print(f"init pop size is: {self.init_pop_size}.")
        while num_added<self.init_pop_size:
            tens=None
            if seed is None:
                tens=Tensegrity(EDGES,None)
                tens.make_tr3()
            else:
                tens=Tensegrity(EDGES,seed)
                tens.make_tr3()
                tens.find_random_string_labels()
            if self.pop.count(tens)!=0:
                self.pop.remove(tens)
            else:
                self.pop.append(tens)
                num_added+=1
        self.evaluate_pop()

    def delete_ith_member(self, i):
        """Deletes the ith member of the current population

        Args:
            i (int): index of the to-be-deleted member within the population

        Returns:
            boolean: True if member deleted, False if member not found
        """
        if i<len(self.pop):
        #if self.pop[i] is not None:
            print(f"deleting {i}")


                        #delete pop[to_del]

            self.pop.pop(i)
            #self.pop[i]=None
            return True
        else:
            print("Pop::delete_ith_member() - null")
            return False

    def add_new_members(self):  #add_new_members(self, cur_gen)
        """Add new members to the population
        """
        #orig_pop_size=len(self.pop)
        p1_index = 0
        p2_index = 0
        num_added = 0
        #new_pop_limit=0


        #as1_mut_worked=0
        #as2_mut_worked=0

        #roulette_wheel = self.make_roulette(self.fitness_index)
        roulette_wheel = self.make_roulette()

        while len(self.pop) < self.init_pop_size:
            if self.x_over_rate==-1: #this is for purely random search
                as1=Tensegrity(EDGES,None) #hard coded for now
                as2=Tensegrity(EDGES,None)
                as1.make_tr3()
                as2.make_tr3()
            else:   #normal xover or mutation.
                #2 should be missing index or most relevant objective
                #spin the wheel...
                p1_val=self.int_rand_in_range(len(roulette_wheel)-1)
                #find the index
                p1_index=roulette_wheel[int(p1_val)]
                #select the parent
                p1=self.pop[p1_index]
                #and again for p2
                #random.randrange(len(roulette_wheel))
                p2_val=self.int_rand_in_range(len(roulette_wheel))
                p2index=roulette_wheel[int(p2_val)]
                p2=self.pop[p2index]
                if (self.binom(self.x_over_rate) and (p1_index != p2_index)):
                    p1x1 = 0

                    p1x1=self.int_rand_in_range(p1.length_of_genotype())

                    as1 =Tensegrity(EDGES,p1)
                    as1.make_tr3()
                    as1.crossover(p1,p2,p1x1)

                    as2 =Tensegrity(EDGES,p2)
                    as2.make_tr3()
                    as2.crossover(p2,p1,p1x1)
                else: #we mutate
                    as1 =Tensegrity(EDGES,p1)
                    as1.make_tr3()
                    as1.mutate()    #as1_mut_worked=as1.mutate()
                    as2 =Tensegrity(EDGES,p2)
                    as2.make_tr3()
                    as2.mutate()    #as2_mut_worked=as2.mutate()
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
        """Print the population"""
        #maxit=len(self.pop)
        for i in self.pop:
            i.print_obj_vals()

    def fprint_pop_stats(self, gennum):
        """Write the generation number and each member 
        of the populations objectives to the out_file

        Args:
            gennum (int): Current generation number
        """
        with open(self.out_file_name,"a+",encoding="utf-8") as out_file:
            for i in self.pop:
                #to_wrt="["
                #j=0
                #while j<(len(gennum)-1):
                #    to_wrt+=j+", "
                #to_wrt+=gennum[-1] + "]"
                out_file.write(str(gennum))
                i.fprint_obj_vals(out_file)
            out_file.close()

    def fprint_best(self, fname):
        """UNCLEAR AS OF WRITING"""
        dot_file_name=""
        dot_file_name=f"{fname}.net"
        self.pop[0].f_print_genome(dot_file_name)
        self.pop[0].print_genome()
        string_file_name=""
        string_file_name=f"{fname}.str"
        self.pop[0].f_print_string_labels(string_file_name)
        print("hello!")
        result=self.world.evaluate(self.pop[0])
        print(result)
        print("olleh!")

    def evaluate_member(self, member):
        """Evaluate an individual member of the population"""
        result = self.world.evaluate(member)
        member.set_objectives(result)

    def evaluate_pop(self):
        """Evaluate the current population
        """
        for i in self.pop:
            self.evaluate_member(i)

    def make_roulette(self):
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
        #use_funky_roulette=0 #funky roulette is for objectives that you're trying to minimize
        #min_biomass=0.
        for i in self.pop:
            #calculate the sum of fitnesses across this index
                                                        #objs=i.get_objective_vals()
                                                        #roulettechunk=objs[index]*100

        #use this for maximizing
        #make a number of roulette slots proportional to your biomass
            j=0
            coin=random.randint(0,1)
            while j<coin:
            #while j<roulettechunk:
                roul.append(self.pop.index(i))
                j+=1
        return roul

    def fscan_config(self, config):
        """Scan the given config file and sets attribute parameters for the population

        Args:
            config (string): path for the config file


        Returns:
            boolean: True if configurations are set, False if config file cannot be opened
        """
        with open(config,"r",encoding="utf-8") as in_file:

            # the defaults
            self.max_gens=MAX_GENS
            self.init_pop_size=INIT_POP_SIZE
            self.x_over_rate=X_OVER_RATE
            self.ls_mut_rate=LS_MUT_RATE
            self.mut_rate=MUT_RATE
            self.sample_rate=SAMPLE_RATE
            self.fitness_index=FITNESS_INDEX  #this is the index we care about

            print(f"TPop.fscan_config() - filename is {config}")
            if not in_file:
                print(f"cannot open file {config} for input")
                return False
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
            return True

    def run(self):
        """Run evolution and evaluation cycles over the population
        """
        #num_iters = 0
        num_gens = 0

        rand_seed = int(time.time())

        #if _rand_seed==0:
        #    _rand_seed=self.signed_to_unsigned(rand_seed,4)
        print(f"Random Seed is: {rand_seed}")
        random.seed(rand_seed)

        tod=time.localtime(rand_seed)

        num_of_day=0

        root_file_name=""
        self.out_file_name=""
        best_file_name=""

        if self.do_logging:
            while 1:
                root_file_name=f"data_tmp/{num_of_day}-{tod.tm_hour}.{tod.tm_mday}.{tod.tm_mon}.{tod.tm_min}.{tod.tm_sec}"
                self.out_file_name=f"{root_file_name}.gen"
                best_file_name=f"{root_file_name}.best"

                try:
                    open(self.out_file_name,'r',encoding="utf-8")
                    #if file doesn't exist, good!
                    num_of_day+=1
                except FileNotFoundError:
                    break


            print(f"genfile is: {self.out_file_name}")
            print(f"bestfile is: {best_file_name}")

            sys_cmd=""

            sys_cmd=f"ln -s -f {self.out_file_name} gen.latest.$HOSTNAME"
            if os.system(sys_cmd)!=0:
                print("Error linking data file!")
            sys_cmd=f"ln -s -f {best_file_name} best.latest.$HOSTNAME"
            if os.system(sys_cmd)!=0:
                print("Error linking data file!")

        #iter_Limit = 1000

        while num_gens < self.max_gens:
            self.sort_pop_by_fitness()
            self.maintain_diversity()
            self.cull_bottom_half()
            if (num_gens % self.sample_rate) == 0:
                print(f"************ generation {num_gens} ********************")
                self.fprint_pop_stats(num_gens)
                self.fprint_best(best_file_name)
                self.print_pop()
            self.add_new_members()  #self.add_new_members(num_gens)
            num_gens+=1
            #num_iters=num_gens

    def maintain_diversity(self):
        """Aids in curating members of the current population"""
        print("hello")
        to_delete=[]
        to_delete.clear()
        i=0
        while (i*-1)<len(self.pop):
            vals=self.pop[i].get_objective_vals()
            cur_val=vals[self.fitness_index]
            j=i-5
            while j>=0:
                print(f"maintain: i:{i} j:{j}")
                other_vals=self.pop[j].get_objective_vals()
                other_val=other_vals[self.fitness_index]
                if cur_val==other_val:
                    print(f"{cur_val} == {other_val}, will delete {j}")
                    to_delete.append(j)
                    j-=1
                else:
                    break
            i=j
        print("erasing...")
        for i in to_delete:
            to_del=i
            print(f"deleting {to_del}")
            self.pop.pop(to_del)
            print(f"and erasing {None}")
            #self.delete_ith_member(to_delete[i])
        print("done erasing")

    def is_in_pop(self, ts):
        """Checks if a tensegrity is in the current population

        Args:
            ts (Tensegrity): the tensegrity being searched for

        Returns:
            boolean: True if the tensegrity is found. False if not,
        """
        return self.pop.count(ts)

    def sort_pop_by_fitness(self):
        """Sort the population according to how well the evaluate in 
        relation to the fitness parameters"""
        for i in self.pop:
            cur_tens=i
            cur_objs=cur_tens.get_objective_vals()
            max_val=cur_objs[self.fitness_index]
            max_index=self.pop.index(i)

            for j in self.pop:
                j_tens=j
                job_js=j_tens.get_objective_vals()
                cur_val=job_js[self.fitness_index]
                if cur_val > max_val:
                    max_index=self.pop.index(j)
                    max_val=cur_val
            tmp=i
            i=self.pop[max_index]
            self.pop[max_index]=tmp

    def cull_bottom_half(self):
        """Removes the len(self.pop)/2 least fit members of the population
        """
        from_val=len(self.pop)//2
        #from_val=self.init_pop_size/2
        print(f"culling - from_val+1 is {from_val+1}, pop.size is: {len(self.pop)}")
        for i in range (from_val+1,len(self.pop)):
            self.delete_ith_member(i)
        print("done culling")
        self.pop=self.pop[0:from_val]
        print("done resizing")





#############################HELPER FUNCTIONS (my_utils)#############################

    def binom(self, prob):
        """Helper function for random seed generation"""
        t=(100.0*random.randint(0,MAX_INT)/(MAX_INT + 1.0))
        if t < prob:
            return 1
        return 0

    def int_rand_in_range(self, num_range):
        """Helper function for random seed generation"""
        return (num_range*random.randint(0,MAX_INT)/(MAX_INT+1.0))



#############################Driver#############################

if __name__=='__main__':
    tens_seed=Tensegrity(EDGES)
    tens_seed.make_tr3()
    FILE_NAME="inFileName.txt"
    algo=Algorithm(FILE_NAME,True,tens_seed)
    algo.run()
