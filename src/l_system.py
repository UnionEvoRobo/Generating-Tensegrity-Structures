"""L-System class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 2.0
"""

import random

MAX_LEN=3
MAX_INT=2147483647
TERMS=[1,2,3,4,5,-1]

class LSystem:
    """L-System class for the generation of complex tensegrity structures."""


    def __init__(self, rules):
        self.rule_dict=rules

    def fscan(self, file_name):
        """Read an input file with the 
        rules for the desired L-System

        Args:
            file_name (string): name of the file to be read
        """
        with open(file_name,"r",encoding="utf-8") as out_file:
            if not out_file:
                print(f"cannot open file {file_name} for input")
                return
            else:
                for result in out_file:
                    if result!=EOFError:
                        result=result.split(" ")
                        rule_num=result[0]
                        rhs=result[1].split('[')
                        rhs=rhs[1].split(',')
                        rhs[2]=rhs[2].split(']')[0]
                        #r_type=result[2].split('(')[1]
                        #spin=result[3].split(')')[0]
                        self.rule_dict[rule_num]=f"{rule_num}>{rhs[0]}"+"{"+rhs[1]+"}"+rhs[2]
            #printf("I read: %d [%d,%d,%d] (%d,%d);%s\n",rulenum,rhs[0],rhs[1],rhs[2],type,spin);
            # newrule->print();
            #      getchar();
        #  printf("read %d items\n",result);
        #  getchar();
        out_file.close()

    def get_rule(self, key):
        rule=[]
        for i in self.rule_dict[key]:
            rule.append(i)
        return rule
        # return self.rule_dict[key]

    def set_rule(self, key, rule):
        self.rule_dict[key]=rule

    def new_rule(self, key):
        rule=self.get_rule(key)
        rule[0]=random.choice(TERMS)
        rule[1]=random.choice(TERMS)
        rule[2]=-1

        #prevent creation of an empty rule
        if (rule[0]==-1 and rule[1]==-1) or (rule[1]!=-1):
            while rule[2]==-1:
                rule[2]=random.choice(TERMS)
        else:
            rule[2]=-1
        self.set_rule(key,rule)

    def __str__(self):
        to_ret=[]
        for i in self.rule_dict:
            rule=self.rule_dict[i]
            if rule[0]==-1 and rule[2]==-1:
                to_ret.append("BAAADD")
            elif rule[0]!=-1 and rule[2]!=-1 and rule[1]==-1:
                to_ret.append("BAAAD")
            elif rule[0]==-1 and rule[2]==-1 and rule[1]==-1:
                to_ret.append("BAAAD")
            else:
                to_ret.append(self.rule_dict[i])
        return str(to_ret)

    def size(self):
        return len(self.rule_dict)

    def int_rand_in_range(self, num_range):
        """Helper function for random seed generation"""
        return (num_range*random.randint(0,MAX_INT)/(MAX_INT+1.0))
