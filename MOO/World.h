#ifndef _WORLD_H
#define _WORLD_H
#include <stdio.h>
//#include <fstream.h>
#include <iostream.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <vector.h>
//#include <conio.h>

#ifdef USE_CURSES
#include <curses.h>
#endif
//#include <malloc.h>
#include "myutils.h"
#include "MOOP.h"
#include "MOOL.h"
#include "Instruction.h"

#define WORLDSIZE 12

#define H_LEFT_BOTTOM  0x1
#define H_RIGHT_BOTTOM 0x2 
#define H_LEFT_TOP     0x4
#define H_RIGHT_TOP    0x8
#define H_IMM_LEFT     0x10 //sixteen 
#define H_IMM_RIGHT    0x20 //thirty two

#define V_BOTTOM       0x1
#define V_TOP          0x2
#define V_ON_LEFT     0x4
#define V_ON_RIGHT    0x8


//************************ CLASS WORLD *********************************
class World : public MOOP
{
protected:
  int grid[WORLDSIZE][WORLDSIZE];
  int is_free(int, int);
  void paint(int, int, int, int,int);
  int unpaint(int, int, int, int);
  int paint_module(World *module);
  int px, py;  //pointer positions
  int orientation; //pointer orientation
                   // 0 is east
                   // 1 is south
                   // 2 is west
                   // 3 is north
  unsigned char get_h_conditions(int, int, int,int);
  unsigned char get_v_conditions(int, int, int,int);
  //double compare_to_other(World *);
  

  int _Vert_B_Odds;
  int _Canti_B_Odds;
  
  int _verbose;
  void set_verbose(int);
 
public:

  World();
  World(World *);
  World(vector<MOOL *> _genome,
	int vbodds = 0, int cbodds = 0, int verbose = 0, int ppmmode = 0);
  World(vector<MOOL *> _genome,vector <MOOL *> _modules,
	int vbodds = 0, int cbodds = 0, int verbose = 0, int ppmmode = 0);
  ~World();
  void clean_grid();
  virtual void init();
  virtual double evaluate();
  // void evaluate_both(float *, float *);
  void evaluate_volume_and_outer_and_mass(float *, float *, float *);
  
  int take();
  int take_block();
  int take_block(int, int);
  void print();
  void print_ppm(int);
  int move_pointer(int);
  int put_brick(int param = 0);
  int put_brick(int, int, int, int, int, int glued = 0);
  int rotate(int);
  int in_world(int, int, int, int); //are coords in world?
  int in_world(int, int);
  int settle(int *, int *, int *, int *,int*,int glued = 0); //stochastic place
                                         //return 1 if brick stays
                                         //return 0 (and change vals)
                                         //if brick falls
  int get_px();
  int get_py();
  int get_orientation();

  int get_grid_val(int, int);
 
  int print_world_to_file();
  int print_world_to_file(char *);
  int read_world_from_file();
  int read_world_from_file(char *);

  void put_fill(int, int);
  int boundaryFill(int x, int y);

  int compare_to(World *);
  
};

#endif
