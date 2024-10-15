#ifndef MOOL_H
#define MOOL_H
//Multi Objective Optimization Locus

#include <vector.h>
class MOOL
{
 private:
 public:
  MOOL();
  MOOL(MOOL *); //copy constructor
  virtual ~MOOL();
  
  virtual vector<MOOL *> get_contents() = 0;
  virtual void randomize() = 0;
  virtual void randomize(int) = 0;
  virtual double distance_to_locus(MOOL *otherL) = 0;
  virtual void print() = 0;
  virtual void print_verbose() = 0;
  virtual int is_leaf() = 0;
};
#endif

