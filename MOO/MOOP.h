#ifndef MOOP_H
#define MOOP_H
#include <vector.h>

#include "MOOL.h"
class MOOP
{
 private:
 public:
  MOOP();
  MOOP(MOOP *);
  MOOP(vector<MOOL *> genome); //decode constructor
  virtual ~MOOP();
  virtual void print();
  virtual void init() = 0;
  virtual double evaluate() = 0;
};
#endif
