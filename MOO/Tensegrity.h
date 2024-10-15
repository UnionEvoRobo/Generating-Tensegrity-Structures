
#ifndef _TENSEGRITY_H 
#define _TENSEGRITY_H
#pragma once
#include "Graph.h"
#include "LSystem.h"
#include "Element.h"
#include "Network.h"

#define WEIGHTS_PER_NET 12 

class Tensegrity 
{

 protected:
 
  Graph *_graph; //the graph
  LSystem *_ls;  //the LSystem
  vector<int> _pairs;   //matching pairs of vertices
  vector<float> _objectives;
 
  int numElements;
  //shuffing elements is easy - just switch corresponding 
  //vals in "pairs"
  vector<int> _elements_to_shuffle; //takes the form of ([v1a,v1b], v2a, v2b,...)

  vector<int> _strings_to_shuffle;  //takes the form of ([s1,s2,v-s1,v-s2]...
                                    //where v-sn is either 0 (from) or 1 (to)
  
  vector<float> _genome;          //set of weights to the ANN;
  vector<int> _stringLabels; 
  //shuffling 

  vector<int> _indexes_to_ignore;
  
  

 public:
  Tensegrity(int numEls = 3);
  Tensegrity(Tensegrity *from);
  ~Tensegrity();

  vector<float> genome();
  void crossover(Tensegrity *p1, Tensegrity *p2, int px);
  void init();

  int lengthOfGenotype();

  void make_tr3();
  void make_tr4();
  void make_tr15();
  //grow graph
  // returning -1 if it fails
  int grow(int size ,int print_intermed = 0);


  void simplify();

  void print();
  void print_graph();
  void print_graphviz();
  void print_lsystem();

  void findPairs();

  int shufflePair();  //these are mutations for the "direct" encoding.
  int shuffleStrings();

  void shufflePairs();

  int mutate();

  Graph *graph();
  LSystem *lsystem();

  vector<int> pairs();
 
  void resetStringLabels();
  void emptyStringLabels();
  void applyLabelsToStrings();
  void mutateStringLabels();
  void findRandomStringLabels();
  void getStringLabelsFromGraph();
  
  void printStringLabels();
  void printGraphRodNumbers();

  vector<int> getStringLabels(){return _stringLabels;};

  int equals(Tensegrity *t1);

  int newgrow(int size);

  void fprintGenome(char *filename);
  void fscanGenome(char *filename);
  void fprintStringLabels(char *filename);
  void fscanStringLabels(char *filename);
  void printGenome();
  void fprint(char *filename);
  void fscan(char *filename);
  void fprint_graphviz(char *filename){_graph->fprint_graphviz(filename);}

  //these are inherited from MOOG
  void reset_rank() {_objectives[0] = 0;}
  void print_obj_vals(){
    print_vector_of_floats(_objectives);
  }
  void fprint_obj_vals(FILE *outfile)
  {
    fprint_vector_of_floats(outfile,_objectives);
  }

  void set_objectives(vector<float> inobj)
  {
    _objectives.clear();
    for (unsigned int i =0; i < inobj.size(); i++)
      _objectives.push_back(inobj[i]);
  }
 
  vector<float> get_objective_vals()
    { return objectives();}

  vector<float> objectives()
    { return _objectives; }

  
};

#endif
