# Generating-Tensegrity-Structures
recreating John's 2009 paper

I have now updated the code to perform a second mutation on the graph. To alter the number of passes (between 1 and 2) you must only change self.number_of_muta on line 21 between 1 and 2. Then just run Main.py and a .gv file will be created that will visualize a graph of the final state of the graph.

At the very bottom of Main.py on lines 88 and 89 you can change the name of the .gv file that gets created. You MUST change the name of the .gv on BOTH lines after 'doctest-output/'

Next steps will be to eliminate 44-46 from Mutator.py and figure out how to accomodate multi-digit long node_id numbers as well as not assume string lengths.
