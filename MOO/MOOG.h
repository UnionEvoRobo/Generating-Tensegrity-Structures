#ifndef _MOOG_H
#define _MOOG_H
#include <vector.h>
#include <iostream.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "myutils.h"

#include "MOOL.h"  //MultiObjectiveOpt Locus
#include "MOOP.h"  //MultiObjectiveOpt Phenotype

//make some global objective indeces into the _objectives matrix
#define MOO_RANK_INDEX 0  //pareto rank
#define MOO_RANK_MASK 0x1
#define MOO_DIV_INDEX 1   //diversity distance
#define MOO_DIV_MASK 0x2

class MOOG
{
 protected:

  vector<MOOL *> _genome;      // a genome is a vector of MOOLs

  vector<float> _objectives;// the MOOs are in a vector!

  MOOP *_phenotype;            //the phenotype is a MOOP
  
  MOOL* random_locus();

  virtual MOOP* decode() = 0;          //pure virtual
                               //genotype to phenotype
                               //with apologies to Oyama
  
  int _Div_Len_Mod;           //how much to reward length as diversity
  
 public:
  
  MOOG();
  //  MOOG(MOOG *copyfrom); //copy constructor
  //MOOG(int len);        //random MOOG of given length
  virtual ~MOOG();

  virtual MOOL *new_MOOL() = 0; 
  virtual MOOL *new_MOOL(MOOL *) = 0; //copy
  virtual int clear_genome() = 0;

  virtual void init();
  virtual void print();
  virtual void print_phenotype();
  virtual void print_genome() = 0;
  virtual void print_obj_vals() = 0;
  virtual void print_obj_titles() = 0;
  virtual void print_obj_vals(unsigned int inmask) = 0;
  virtual void print_obj_titles(unsigned int inmask) = 0;
  virtual void fprint_obj_titles(char *, unsigned int) = 0;

  
  vector<MOOL *> get_genome();
  vector<float> get_objective_vals();  //get values of objectives 
  vector<float> get_objective_vals(unsigned int flags);  //get values of objectives 
  
  void set_genome(vector<MOOL *> newg);
  void append_to_genome(vector<MOOL *> appendage);
  
  virtual int get_rank();
  void set_rank(int);  //set pareto rank
  void set_diversity(float);
  float get_diversity();
  
  int get_div_len_mod();
  int get_len();
  
  float get_ith_objective(int i); 
  void set_ith_objective(int i, float val);

  virtual void evaluate_objectives(unsigned int MOO_MASK) = 0;     

  //void fprint_genome();
  virtual int fprint_obj_vals(FILE *) = 0;
  virtual int fprint_obj_vals(FILE *,unsigned int) = 0;
  virtual int fprint_genome(char *);
  virtual int fprint_genome(FILE *) = 0;
  virtual int fscan_genome(char *);
  
  virtual int randomize_genome();      //make yourself random of lenght l
  
  //  virtual int delete_random_locus();
  //virtual int mutate_random_locus();
  virtual void mutate_with_prob(int prob); //mutate each locus with prob 
  virtual void add_del_with_prob(int prob);//add or delete at each locus with prob

  virtual float distance_to_genome(vector<MOOL *> othergenome);
  virtual int equal_to_genome(vector<MOOL *> othergenome);

  int single_point_crossover(MOOG *p1, int p1x, MOOG *p2, int p2x);
  int two_point_crossover(MOOG *p1, int p1x1, int p1x2, MOOG *p2, int p2x1, int p2x2);


  vector<MOOL *> get_flat_genome();
};
#endif
