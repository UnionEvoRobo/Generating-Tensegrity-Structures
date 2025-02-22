"""Algorithm object for the generation of complex tensegrity structures.


@author: Daniel Casper
@version: 5.0
"""
import time
import random
from evaluator import Evaluator
from tensegrity import Tensegrity

DEF_L_SYSTEM={1:[2,1,5],2:[4,1,1],3:[4,-1,-1],4:[3,4,4],5:[3,-1,-1]}
MAX_GRAPH_SIZE=4
BO_PR=False
MAX_INT=2147483647
# EDGES=["1","2","3","4","5"]
EDGES=[1,2,3,4,5]
MAX_GENS=5
NUM_BAR=10
INIT_POP_SIZE= 20
X_OVER_RATE = 60
LS_MUT_RATE=60
MUT_RATE = 2
SAMPLE_RATE = 1
NUM_TRANS=0
FITNESS_INDEX = 1 #this is the index we care about


class Algorithm:
    """Algorithm object for the generation of complex tensegrity structures."""
    def __init__ (self, in_file_name, do_log, seed):
        self.graph_num=1
        self.pop=list[Tensegrity]
        self.init_pop_size=None
        self.sample_rate=None
        self.num_trans=None
        self.max_gens=None
        self.x_over_rate=None
        self.ls_mut_rate=None
        self.mut_rate=None
        self.fitness_index=None
        self.num_bar=NUM_BAR
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

        self.pop=[]
        num_added=0

        if BO_PR:
            print(f"init pop size is: {self.init_pop_size}.")
        while num_added<self.init_pop_size:
            tens=None
            made=True
            if seed is None:
                tens=Tensegrity(EDGES,None)
            else:
                tens=Tensegrity(EDGES,seed)
                if self.num_bar==3:
                    tens.make_tr3()
                elif self.num_bar==4:
                    tens.make_tr4()
                else:
                    made=tens.make_trx(self.num_bar)
                if made:
                    tens.find_random_string_labels()
            if made:
                if self.pop.count(tens)!=0:
                    self.pop.remove(tens)
                else:
                    self.pop.append(tens)
                    num_added+=1
                    tens.draw_graph(self.graph_num, DEF_L_SYSTEM, made, self.num_bar)
                    self.graph_num+=1
        self.evaluate_pop()

    def delete_ith_member(self, i):
        """Deletes the ith member of the current population

        Args:
            i (int): index of the to-be-deleted member within the population

        Returns:
            boolean: True if member deleted, False if member not found
        """
        if i<len(self.pop):
            if BO_PR:
                print(f"deleting {i}")
            self.pop.pop(i)
            return True
        if BO_PR:
            print("Pop::delete_ith_member() - null")
        return False

    def add_new_members(self):
        """Add new members to the population
        """
        p1_index = 0
        p2_index = -1
        num_added = 0

        roulette_wheel = self.make_roulette(FITNESS_INDEX)
        #add members to population until back to initial population size
        while len(self.pop) < self.init_pop_size:
            made1=True
            made2=True
            if self.x_over_rate==-1:
                as1=Tensegrity(EDGES,None)
                as2=Tensegrity(EDGES,None)
                if self.num_bar==3:
                    as1.make_tr3()
                    as2.make_tr3()
                elif self.num_bar==4:
                    as1.make_tr4()
                    as2.make_tr4()
                print("none")
            else:   #normal xover or mutation.
                #2 should be missing index or most relevant objective
                #spin the wheel...
                if roulette_wheel:
                    p1_index=random.choice(roulette_wheel)
                    #and again for p2
                    p2_index=random.choice(roulette_wheel)
                #select the parents
                p1:Tensegrity=self.pop[p1_index]
                p2:Tensegrity=self.pop[p2_index]
                #create new population members
                as1 =Tensegrity(EDGES,p1)
                as2 =Tensegrity(EDGES,p2)
                if self.num_bar==3:
                    as1.make_tr3()
                    as2.make_tr3()
                elif self.num_bar==4:
                    as1.make_tr4()
                    as2.make_tr4()
                #if the probability for a crossover hits
                if (self.binom(self.x_over_rate) and (p1_index != p2_index)):
                    p1x1=random.randrange(1,p1.l_system.size())
                    #crossover
                    as1.crossover(p1,p2,p1x1)
                    as2.crossover(p2,p1,p1x1)
                    #then primary mutation
                    as1.prim_mut(self.ls_mut_rate)
                    as2.prim_mut(self.ls_mut_rate)
                else: #we perform a secondary mutation
                    if self.num_bar>=5:
                        made1=as1.make_trx(self.num_bar)
                        made2=as2.make_trx(self.num_bar)
                        as1.mutate()
                        as2.mutate()
            #if as1 and as2 hasnt been added to the population, evaluate and add it
            num_added=self.eval_add(as1, num_added, made1)
            num_added=self.eval_add(as2, num_added, made2)

    def eval_add(self, tens:Tensegrity, add, made):
        """Evaluate a new tensegrity and add it to the population if 
        not already in population

        Args:
            tens (Tensegrity): potential new population
            add (int): number of members added to population so far
            made (bool): whether or not the tensegrities graph was successfully grown 

        Returns:
            int: number of members added to population including {tens}
        """
        if not self.is_in_pop(tens):
            if tens.get_graph() is None:
                made=tens.make_trx(self.num_bar)
            self.evaluate_member(tens) #don't grow!
            self.pop.append(tens)
            add+=1
            if self.num_bar<5:
                tens.new_grow(self.num_trans, self.graph_num)
            tens.draw_graph(self.graph_num, DEF_L_SYSTEM, made, self.num_bar)
            self.graph_num+=1
        return add

    def print_pop(self):
        """Print the population"""
        i:Tensegrity
        for i in self.pop:
            i.print_obj_vals()

    def fprint_pop_stats(self, gennum):
        """Write the generation number and each member 
        of the populations objectives to the out_file

        Args:
            gennum (int): Current generation number
        """

        with open(self.out_file_name,"a+",encoding="utf-8") as out_file:
            i:Tensegrity
            for i in self.pop:
                out_file.write(str(gennum))
                i.fprint_obj_vals(out_file)
            out_file.close()

    def fprint_best(self, fname):
        """Print the objective values of the best performing tensegrity
        """
        member:Tensegrity=self.pop[0]
        dot_file_name=""
        dot_file_name=f"{fname}.net"
        member.f_print_genome(dot_file_name)
        member.print_genome()
        string_file_name=""
        string_file_name=f"{fname}.str"
        member.f_print_string_labels(string_file_name)
        if BO_PR:
            print("hello!")
        result=self.world.evaluate(member)
        if BO_PR:
            print(result)
            print("olleh!")

    def evaluate_member(self, member:Tensegrity):
        """Evaluate an individual member of the population"""
        result = self.world.evaluate(member)
        member.set_objectives(result)

    def evaluate_pop(self):
        """Evaluate the current population
        """
        for i in self.pop:
            self.evaluate_member(i)

    def make_roulette(self,index):
        """makes a roulette wheel by creating N entries 
        for every member of the pop,
        where N == surviving_children()
        the values of the roulette are indeces of the population
        i.e. if individual 5 has 3 surviving children,
        and     individual 6 has 2 
        and     indivitual 7 has 4 then the roulette will look like
        ....555667777...
        same, but for a given index, not just num children"""
        roul=[]
        i:Tensegrity
        num=0
        for i in self.pop:
            #calculate the sum of fitnesses across this index
            objs=i.get_objective_vals()
            roulettechunk=objs[index]*100
            j=0
            while j<roulettechunk:
                roul.append(num)
                j+=1
            num+=1
        #use this for maximizing
        #make a number of roulette slots proportional to your biomass
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
            self.num_trans=NUM_TRANS

            self.fitness_index=FITNESS_INDEX  #this is the index we care about
            if BO_PR:
                print(f"TPop.fscan_config() - filename is {config}")
            if not in_file:
                if BO_PR:
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
                        case "num_trans":
                            self.num_trans=val
                        case _:
                            if BO_PR:
                                print(f"unrecognized input value to parse_config: {in_string}!")
            in_file.close()
            return True

    def run(self):
        """Run evolution and evaluation cycles over the population
        """
        num_gens = 0

        rand_seed = int(time.time())

        if BO_PR:
            print(f"Random Seed is: {rand_seed}")
        random.seed(rand_seed)

        tod=time.localtime(rand_seed)

        num_of_day=0

        root_file_name=""
        self.out_file_name=""
        best_file_name=""

        #log population stats for run cycle
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

            if BO_PR:
                print(f"genfile is: {self.out_file_name}")
                print(f"bestfile is: {best_file_name}")

            #sys_cmd=""

            #sys_cmd=f"ln -s -f {self.out_file_name} gen.latest.$HOSTNAME"
            #if os.system(sys_cmd)!=0:
            #    if BO_PR:
            #        print("Error linking data file!")
            #sys_cmd=f"ln -s -f {best_file_name} best.latest.$HOSTNAME"
            #if os.system(sys_cmd)!=0:
            #    if BO_PR:
            #        print("Error linking data file!")


        while num_gens < self.max_gens:
            self.sort_pop_by_fitness()
            self.maintain_diversity()
            self.cull_bottom_half()
            if (num_gens % self.sample_rate) == 0:
                print(f"************ generation {num_gens} ********************")
                self.fprint_pop_stats(num_gens)
                self.fprint_best(best_file_name)
            self.add_new_members()
            num_gens+=1

    def maintain_diversity(self):
        """Aids in curating members of the current population"""

        if BO_PR:
            print("hello")
        to_delete=[]
        i=0
        size=self.init_pop_size
        #iterate through population
        while (-i)<len(self.pop):
            imember:Tensegrity=self.pop[i]
            vals=imember.get_objective_vals()
            cur_val=vals[self.fitness_index]
            j=i-5
            while -j>=0 and -j<len(self.pop):
                if BO_PR:
                    print(f"maintain: i:{size+i} j:{size+j}")
                jmember:Tensegrity=self.pop[i]
                other_vals=jmember.get_objective_vals()
                other_val=other_vals[self.fitness_index]
                #compare the fitness values of each tensegrity
                #delete one if they are same
                if cur_val==other_val:
                    if BO_PR:
                        print(f"{cur_val} == {other_val}, will delete {size+j}")
                    to_delete.append((self.pop[j],size+j))
                    j-=1
                else:
                    break
            i=j
        if len(to_delete)!=0:
            if BO_PR:
                print("erasing...")
            for i in to_delete:
                to_del=i[0]
                if BO_PR:
                    print(f"deleting {i[1]}")
                self.pop.remove(to_del)
            if BO_PR:
                print("done erasing")

    def is_in_pop(self, ts):
        """Checks if a tensegrity is in the current population

        Args:
            ts (Tensegrity): the tensegrity being searched for

        Returns:
            boolean: True if the tensegrity is found. False if not,
        """
        return ts in self.pop

    def sort_pop_by_fitness(self):
        """Sort the population according to how well the evaluate in 
        relation to the fitness parameters"""
        i:Tensegrity
        for i in self.pop:
            cur_tens=i
            cur_objs=cur_tens.get_objective_vals()
            max_val=cur_objs[self.fitness_index]
            max_index=self.pop.index(i)
            j:Tensegrity
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
        if BO_PR:
            print(f"culling - from_val+1 is {from_val+1}, pop.size is: {len(self.pop)}")
        for i in range (from_val+1,len(self.pop)):
            if BO_PR:
                print(f"deleting {i}")
            self.pop.pop(from_val+1)
        if BO_PR:
            print("done culling")
        self.pop=self.pop[0:from_val]
        if BO_PR:
            print("done resizing")





#############################HELPER FUNCTIONS (my_utils)#############################

    def binom(self, prob):
        """Helper function for random seed generation"""
        t=(100.0*random.randint(0,MAX_INT)/(MAX_INT + 1.0))
        if t < prob:
            return 1
        return 0

#############################Driver#############################

if __name__=='__main__':
    tens_seed=Tensegrity(EDGES)
    if NUM_BAR==3:
        tens_seed.make_tr3()
        tens_seed.new_grow(NUM_TRANS, 0)
    if NUM_BAR==4:
        tens_seed.make_tr4()
        tens_seed.new_grow(NUM_TRANS, 0)
    else:
        MADE=tens_seed.make_trx(NUM_BAR)
    if MADE:
        tens_seed.draw_graph(0, None, MADE, NUM_BAR)
        FILE_NAME="inFileName.txt"
        algo=Algorithm(FILE_NAME,True,tens_seed)
        algo.run()
