#ifndef _MULTIPOP_H
#define _MULTIPOP_H

#define MODE 1
#define MAX_NOISE 0.03
#include "myutils.h"
#include "Pop.h"
#include "ODEWorld.h"
#include "Tensegrity.h"

static float prev_best;

class TPop
{
 protected:

  int _InitPopSize;  //initial population size
  int _MaxGens;      //max number of generations

  int _XoverRate;    //Crossover Rate
  int _MutRate;      //per-locus mutation rate
  int _LSMutRate;      //per-locus mutation rate

  int _SampleRate;

  
  int _Fitness_Index;
  // these are actually in Pop.h
  //int _Do_Crowding;
  //int _Crowding_Bin_Size;
  
  int _do_logging;
  char *_appname;
  char *_configfilename;
  int _render;
  unsigned int _randseed;
  int _debug;

  int _directEncodingSize;

  int _maxGraphSize; //for lsystems - how large they can go

  vector<int> _indexes_to_ignore;
  vector<Tensegrity *> _pop;

  ODEWorld *_world;


 public:
  TPop(char *infilename, char *appname, int do_log, unsigned int randseed, int render, int direct=0, int maxgraphsize = 18, Tensegrity *seed = NULL);
  ~TPop();
  
  void add_new_members(int curgen);
  void evaluate_pop();

  void sort_pop_by_fitness();
  void cull_bottom_half();

  void maintainDiversity();

  void init(Tensegrity *seed = NULL);
  void print_pop();
  void fprint_pop_stats(char *fname, int gen);
  
  void run();
  
  void fprint_best(char *fname);
  
  void evaluate_member(Tensegrity *ts);
   
  
  int delete_ith_member(int i);
  
  vector<int> make_roulette(int ind); //based on index ind
  
  int fscan_config();
  int fscan_config(char *);

  int is_in_pop(Tensegrity *ts);
  
};

#endif
