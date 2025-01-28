"""L-System class for the generation of complex tensegrity structures.

@author: Daniel Casper
@version: 1.0
"""

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
        return self.rule_dict[key]
