#ifndef _MOOM_H
#define _MOOM_H
#include <vector.h>
#include <iostream.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "myutils.h"

#include "Instruction.h"  //MultiObjectiveOpt Locus
#include "Assembly.h"
#include "MOOP.h"
#include "World.h"

//Multi-Objective Optimzation Module
//may contain other mooms
class MOOM : public MOOL
{
 protected:

  int _NumModuleEvals;
  vector<MOOL *> _contents;
  World *_result;
  void init();
 
 public:

  MOOM();
  MOOM(MOOM *other);
  MOOM(Assembly *assembly);
  ~MOOM();

  void set_contents(vector<MOOL *> new_contents);
  int is_viable();
  
  World *get_result();

  virtual vector<MOOL *> get_contents();
  virtual void print();
  void print_result();
  virtual void print_verbose();
  virtual void randomize();
  virtual void randomize(int);
  virtual int is_leaf();
  virtual double distance_to_locus(MOOL *otherL);

};

#endif
