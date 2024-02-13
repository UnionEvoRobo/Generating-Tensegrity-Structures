"""Mutator class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 2.0
"""

import graphviz  #doctest: +NO_EXE
dot = graphviz.Digraph(comment='Tensegrity Object Graph')
dot

class Mutator:

    def __init__(self):

        self.rule_dict={"A":"A>B{a}E", "B":"B>D{a}A", "C":"C>D", "D":"D>C{d}D", "E":"E>C"}
        self.c_rule="C>D"        #this and self.d_rule have been left as separate variables 
        self.d_rule="D>C{d}D"    #for the convenience of the final 'if' statement in the self.mutating() 
        self.new_edges={}
        self.node_index=0

    def mutating(self, main):
        """Main functions of the mutator. Excluding the return statement, 
        the last three lines are temporary to properly output the graph 
        in the paper.

        Parameters
        ---
        main: object
            -oject from the Main class

        Returns
        ---
        self.new_edges: dictionary
            -a dictionary containing all edges created through mutation of the 
            input dictionary from main (main.edge_dict)
        """
        count_dict={'A':1, 'B':1, 'C':1, 'D':1, 'E':1, }
        self.new_edges={}
        for i in main.edge_dict:
            for r in self.rule_dict:
                if i[0]==r:
                    rule=self.rule_dict[r]
                    self.rule_size(main, i, rule, count_dict)
                    if r=='C':
                        self.rule_size(main, i, self.d_rule, count_dict)
                        del self.new_edges[self.c_rule[-1]+str(count_dict[self.c_rule[-1]]-2)]
        return(self.new_edges)
        

    def rule_size(self, main, i, rule, count_dict):
        """Checks whether the accessed rule creates a bracketed node (long_rule) 
        or if it just swaps the letter label of the edge (short_rule).

        Parameters
        ---
        main: object
            -object from the Main class
        i: string
            -the key for an item in main.edge_dict
        rule: string
            -the rule for the given edge type
        count_dict: dictionary
            -keeps track of how many edges of each type have been added to 
            self.new_edges so as to not overwrite an already existing key's value
        """    
        if len(rule)==7:
            self.long_rule(main, i, rule, count_dict)
        elif len(rule)==3:
            self.short_rule(main, i, rule, count_dict)


    def long_rule(self, main, i, rule, count_dict):
        """Mutates A, B, and D type edges.

        Parameters
        ---
        main: object
            -object from the Main class
        i: string
            -the key for an item in main.edge_dict
        rule: string
            -the rule for the given edge type
        count_dict: dictionary
            -keeps track of how many edges of each type have been added to 
            self.new_edges so as to not overwrite an already existing key's value
        """
        l=3
        bracket=''
        while (l<6):
            bracket+=rule[l]
            l+=1
        node_id=self.new_node(main)
        new_edge=main.edge_dict[i][0]+node_id+ bracket
        main.wacky_node_names.append(node_id+ bracket)
        self.new_edges[rule[2] + str(count_dict[rule[2]])]=new_edge
        count_dict[rule[2]]+=1
        new_edge=(node_id+ bracket)+main.edge_dict[i][1]
        self.new_edges[rule[-1] + str(count_dict[rule[-1]])]=new_edge
        count_dict[rule[-1]]+=1

    def short_rule(self, main, i, rule, count_dict):
        """Mutates C and E type edges.

        Parameters
        ---
        main: object
            -object from the Main class
        i: string
            -the key for an item in main.edge_dict
        rule: string
            -the rule for the given edge type
        count_dict: dictionary
            -keeps track of how many edges of each type have been added to 
            self.new_edges so as to not overwrite an already existing key's value
        """
        self.new_edges[rule[-1] + str(count_dict[rule[-1]])]=main.edge_dict[i]
        count_dict[rule[-1]]+=1

    def new_node(self, main):
        """Increments the node labels through single digit integers, and 
        exceeding that, labels them alpahbetically.

        Parameters
        ---
        main: object
            -oject from the Main class
        
        Returns
        ---
        node_id: string
            -an identification string pertaining to each new node. 
            Ranges from 1-9 and then lowercase alphabetical letters. 
            len(node_id) should never be more than 1
        """
        if main.node_number>=9:
            node_id=main.alpha[self.node_index]
            self.node_index+=1
            main.node_number+=1
        else:
            main.node_number+=1
            node_id=str(main.node_number)
        return node_id
