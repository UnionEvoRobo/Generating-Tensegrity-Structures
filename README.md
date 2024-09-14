# Generating-Tensegrity-Structures
Daniel Casper Computer Science Thesis 2024/25

How to use:
Further modularized the program. Now has separate Node and Edge classes. Also now capable of performing as many passes through the mutator as you desire (with the caveat of the built in graphviz render timeout as well as your machines computing capailities). To alter the number of passes you just need to change self.number_of_muta on line 23. My laptop was capable of 7-8 passes starting with the graph from the paper. Then just run Main.py and a  .gv file will be created that will visualize a graph of the final state of the graph. All edges created are wholly unique. No two nodes will have more than one edge between them.

At the very bottom of Main.py on lines 88 and 89 you can change the name of the .gv file that gets created. You MUST change the name of the .gv on BOTH lines after 'doctest-output/'

--------------------------------------------------------------------------------------------------------------------------------------------------------

To Do/Implement (not ordered by importance):

2: Tensegrity form finding function 

2: Primary mutations

3: Crossover mutations

5: Actual evolutionary algorithm

--------------------------------------------------------------------------------------------------------------------------------------------------------

In Progress:

4: Secondary mutations
    i: Swap string endpoints
    ii: Swap strut endpoints
    
    NOTE: I have created the class/basic functionality but have yet to implement this formally into the graph making process.

--------------------------------------------------------------------------------------------------------------------------------------------------------

Fixed/Finished:

Node number labels are capable of staying numbers and can be as many characters long as desired.

SOLVED (kinda, it doesn't work if it's only been transformed one time. 2 and up seems to work fine) I have noticed that some of the nodes in graphs generated with more passes do NOT have at least the desired minimum of 3 edges.