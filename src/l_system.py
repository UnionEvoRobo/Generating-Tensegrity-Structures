"""L-System class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 3.0
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
            for result in out_file:
                if result!=EOFError:
                    result=result.split(" ")
                    rule_num=result[0]
                    rhs=result[1].split('[')
                    rhs=rhs[1].split(',')
                    rhs[2]=rhs[2].split(']')[0]
                    self.rule_dict[rule_num]=f"{rule_num}>{rhs[0]}"+"{"+rhs[1]+"}"+rhs[2]
        out_file.close()

    def get_rule(self, key):
        """Getter for a single rule from the l_system

        Args:
            key (int): the key of the desired rule

        Returns:
            set: the rule associated with the given key
        """
        rule=[]
        for i in self.rule_dict[key]:
            rule.append(i)
        return rule
        # return self.rule_dict[key]

    def set_rule(self, key, rule):
        "Set the definition of a rule"
        self.rule_dict[key]=rule

    def new_rule(self, key):
        """Generate a new randomized rule

        Args:
            key (int): the key for the rule being changed
        """
        rule=self.get_rule(key)
        rule[0]=random.choice(TERMS)
        rule[1]=random.choice(TERMS)
        rule[2]=random.choice(TERMS)

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
            to_ret.append(self.rule_dict[i])
        return str(to_ret)

    def size(self):
        "Get the number of rules"
        return len(self.rule_dict)

    def int_rand_in_range(self, num_range):
        """Helper function for random seed generation"""
        return (num_range*random.randint(0,MAX_INT)/(MAX_INT+1.0))
