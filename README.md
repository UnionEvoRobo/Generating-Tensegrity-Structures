# Generating-Tensegrity-Structures
recreating John's 2009 paper

Further modularized the program. Now has separate Node and Edge classes. Also now capable of performing as many passes through the mutator as you desire (with the caveat of the built in graphviz render timeout as well as your machines computing capailities). To alter the number of passes you just need to change self.number_of_muta on line 23. My laptop was capable of 7-8 passes starting with the graph from the paper. Then just run Main.py and a  .gv file will be created that will visualize a graph of the final state of the graph. All edges created are wholly unique. No two nodes will have more than one edge between them.

As per lab meeting, I have noticed that some of the nodes in graphs generated with more passes do NOT have at least the desired minimum of 3 edges. To be looked into.

Node number labels are capable of staying numbers and can be as many characters long as desired.

At the very bottom of Main.py on lines 88 and 89 you can change the name of the .gv file that gets created. You MUST change the name of the .gv on BOTH lines after 'doctest-output/'