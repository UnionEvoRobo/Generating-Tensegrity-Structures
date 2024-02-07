# Generating-Tensegrity-Structures
recreating John's 2009 paper

Currently just capable of recreating the graph mutation shown at the bottom of page 3 of the paper. All that's needed is to run the Main file and a .gv file will be created that will visualize a graph of the final state of the graph.

At the end of the Main class you can change the name of the .gv file that gets created. You MUST change the name of the .gv on BOTH lines after 'doctest-output/'

Next steps will be to make the mutator class more reusable and not specialized to just the set of rules from pages 3&4 of the paper.
