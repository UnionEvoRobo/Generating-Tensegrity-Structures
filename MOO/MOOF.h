#ifndef _MOOF_H
#define _MOOF_H
#include <vector.h>
#include <math.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "MOOG.h"
#include "MOOM.h"
#include "myutils.h"

// Multi Objective Optimization Framework
// This is a generic superclass111001000101


class MOOF
{
 protected:

  int _do_logging;
  unsigned int _randseed;
  char *_appname;

  int _InitPopSize;  //initial population size
  int _MaxPopSize;   //max allowable pop size
  int _NewPopSize;    //new members to add
  int _MaxGens;      //max number of generations

  int _InitMoogLen;  //initial length of genotype
  int _MaxMoogLen;
  int _MinMoogLen;
  int _XoverRate;    //Crossover Rate
  int _MutRate;      //per-locus mutation rate
  int _Add_Del_Rate; //per-locus grow/shrink rate
  int _Layers;
  int _SampleRate;

  int _num_P_Evals; // number of phenotype evaluations

  
  int _Do_Crowding;  //whether to do crowding on objectives
  int _Crowding_Bin_Size; //how many to allow in each bin

  unsigned int _MoMask;     //mask to select objectives
  unsigned int _MoMask2;     //mask to select objectives

  vector<MOOG *> _pop; //population
  vector<MOOM *> _modules;
  
  char *_outfilename;

  virtual void init() = 0;           //initialize pop
  virtual int delete_ith_member(int i) = 0;

  // Some Populatioin-specific things


  //  virtual void init_pop = 0;
  virtual void add_new_members() = 0;
 
  virtual int cull_below_layer(int n);
 
  // some useful tools
  virtual int fscan_config() = 0;
  virtual int fscan_config(char *infilename) = 0;

  //virtual int fprint_header(char *outfilename,randseed) = 0;
  virtual void fprint_best(char *outfilename) = 0;
  
  void calc_pop_distances();

 
  int get_pop_mass();

  virtual int pareto_dominates(vector<float> v1, vector<float> v2, unsigned int mask);
  virtual int nuanced_dominates(MOOG *p1, MOOG *p2, unsigned int mask) = 0;

 public:

  MOOF();                       //base constructor
  MOOF(char *configfilename);   // read config file
  virtual ~MOOF();
  virtual void reset_ranks();
  void print_pop_stats();
  //return the best with a given priority ranking for objectives
  MOOG *best_by_priority(vector<int> ranks);
  vector<MOOG *> with_max_obj_val(vector<MOOG *>subpop, vector<int> indexes);
  vector<MOOG *> with_max_obj_val(vector<MOOG *>subpop, int objindex);
 
  virtual void do_crowding(int num_per_bin = 1);
  int pareto_rank_pop(int nuanced = 0);    // pareto-rank population using fscanned vals 
  int rank_to_layer(int n);    // pareto-rank population to n layers
  int rank_to_layer(int n,unsigned int mask, int nuanced = 0);    // pareto-rank population to n layers
  virtual int pareto_rank(int, unsigned int mask, int nuanced = 0); //rank at a given level
  void cull_all_below__Layer(); // cull members below layer n
  void cull_all_below_layer(int n); // cull members below layer n
  virtual void evaluate_pop();
  void run();
  int get_pop_size();
  unsigned int get_MoMask();

  MOOG * get_ith_member(int i);

  virtual void print_pop() = 0;
  virtual void  fprint_obj_titles_as_xml(char *infilename) = 0;
  virtual int fprint_config(char *outfilename) = 0;
  virtual int fprint_pop_stats(char *outfilename,int,int);
  virtual int fprint_pop_stats(char *outfilename,int);
  void sort_pop_by_ith_objective(int); // what it sez.
  
};

#endif
