#include "World.h"
#include "MOOM.h"
World::World()
{
  init();
}

World::World(World *otherw)
{
  printf("copy constructor!\n");
  for (int i = 0; i< WORLDSIZE; i++)
    {
      for (int j = 0; j< WORLDSIZE; j++)
	{
	  grid[i][j] = otherw->get_grid_val(i,j);
	}
    }
  
  px = otherw->get_px();
  py = otherw->get_py();
  orientation = otherw->get_orientation();
  
}

//comparison operator
// return 1 if two worlds are the same
// return 0 if they differ.
int World::compare_to(World *otherw)
{
  int myval;
  int otherval;
   for (int i = 0; i< WORLDSIZE; i++)
    {
      for (int j = 0; j< WORLDSIZE; j++)
	{
	  myval  = grid[i][j];
	  otherval = otherw->get_grid_val(i,j);
	  if (myval != otherval)
	    return 0;
	}
    }
   
   return 1;
}
//the "decode" constructor
World::World(vector<MOOL *> _genome,
	     int vbodds,
	     int cbodds,
	     int verbose,
	     int ppmmode)
{
  _Vert_B_Odds = vbodds;
  _Canti_B_Odds = cbodds;

  init();
  set_verbose(verbose);
  //Instruction *ins; // = NULL;
  int size = (int)_genome.size();
  for (int i = 0; i<size;i++)
    {
      if (_genome[i] == NULL)
	{
	  printf("null plan in assembly!\n");
	}

      // if it is a leaf, treat it normally.
      Instruction *ins = (Instruction *)_genome[i];
      if (ins == NULL)
	{
	  printf("World::World() -decode constructor  - ins %d out of %d is null!\n",i,(int)_genome.size());
	}
      switch(ins->get_type())
	{
	case Instruction::I_Put:
	  put_brick(ins->get_param());
	  break;
	case Instruction::I_Move:
	  move_pointer(ins->get_param());
	  break;
	case Instruction::I_Take:
	  take();
	  break;
	case Instruction::I_Rotate:
	  rotate(ins->get_param());
	  break;
	default:
	  printf("World::World() - decode constructor unknown instruction type %d at %d\n",ins->get_type(),i);
	}
      if (_verbose)
	{
	  ins->print();
	  print();
	  if (ppmmode)
	    print_ppm(i);
	  else
	    getchar();
	}
    } 
}
World::World(vector<MOOL *> _genome,
	     vector<MOOL *> _modules,
	     int vbodds,
	     int cbodds,
	     int verbose,
	     int ppmmode
	     )
{
  _Vert_B_Odds = vbodds;
  _Canti_B_Odds = cbodds;
 
  int param;

  init();
  set_verbose(verbose);
  //Instruction *ins; // = NULL;
  int size = (int)_genome.size();
  for (int i = 0; i<size;i++)
    {
      if (_genome[i] == NULL)
	{
	  printf("null plan in assembly!\n");
	}

      // if it is a leaf, treat it normally.
      Instruction *ins = (Instruction *)_genome[i];
      if (ins == NULL)
	{
	  printf("World::World() -decode constructor  - ins %d out of %d is null!\n",i,(int)_genome.size());
	}
      switch(ins->get_type())
	{
	case Instruction::I_Put:
	  param = ins->get_param();
	  if (param < 4)
	      put_brick(ins->get_param());
	  else //it's a module!
	    {
	      if ((param < 0) || (param >= 4 + (int)_modules.size()))
		{
		  printf("bad module number %d, only %d modules!\n",
			 param,(int)_modules.size());
		}
	      else
		{
		  //printf("module number is: %d out of %d\n",param-4,(int)_modules.size());
		  MOOM *module = (MOOM *)(_modules[param - 4]);
		  if (module != NULL)
		    {
		      World *resultWorld = module->get_result();
		      if (resultWorld != NULL)
			{
			  //			  printf("result world is:\n");
			  //resultWorld->print();
			  // getchar();
			  int success = paint_module(resultWorld);
			}
		    }
		  else
		    {
		      printf("NULL MODULE in _modules!\n");
		    }
		}
	    } 
	  break;
	  
	case Instruction::I_Move:
	  move_pointer(ins->get_param());
	  break;
	case Instruction::I_Take:
	  take();
	  break;
	case Instruction::I_Rotate:
	  rotate(ins->get_param());
	  break;
	default:
	  printf("World::World() - decode constructor unknown instruction type %d at %d\n",ins->get_type(),i);
	}
      if (_verbose)
	{
	  ins->print();
	  print();
	  if (ppmmode)
	    print_ppm(i);
	  else
	    getchar();
	}
    } 
}

World::~World()
{
}

void World::set_verbose(int newv)
{
  _verbose = newv;
}

void World::init()
{
  for (int i=0;i<WORLDSIZE;i++)
    for (int j=0;j<WORLDSIZE;j++)
      grid[i][j] = 0;
  px = WORLDSIZE/2;
  py = 0;
  _verbose = 0;
  orientation = 3; //facing north
}

//evaluation now occurs explicityly in the phenotype

double World::evaluate()
{
  printf("World::evalute() -- DON'T USE ME!\n");
  //return compare_to_other(_goal_world);
  return -1;

}

void World::evaluate_volume_and_outer_and_mass(float *volume, float *outer, float *mass)
{

  int numfilled = 0;
  int masstmp = 0;
  
  for (int j = 0; j < WORLDSIZE; j++)
    {
      if (is_free(0,j))  //left edge
	{
	  numfilled += boundaryFill(0,j);
	}
      if (is_free((WORLDSIZE - 1),j)) //right edge
	{
	  numfilled += boundaryFill((WORLDSIZE - 1),j);
	}
      //gotta avoid going over twice
      if (j < (WORLDSIZE - 1))            //takes care of rhs
	{
	  if (is_free((j+1),(WORLDSIZE - 1))) //top j+1 takes care of lhs
	    {
	      numfilled += boundaryFill(j+1,(WORLDSIZE - 1));
	    }
	}
    }
  
  
  for (int i = 0;i<WORLDSIZE;i++)
    {
      for (int j = 0; j < WORLDSIZE; j++)
	{
	  // if a brick is there
	  if (! is_free(i,j))
	    {
	      masstmp++;
	    }
	}
    }	
  
  // make volume equal to the sum heights across each column
  *mass = (float)masstmp;
  *volume = (float)(max(((WORLDSIZE * WORLDSIZE) - numfilled) - masstmp,0));
  *outer = (float)numfilled;
  
}

// move pointer now just moves forward.  
// ROTATE takes care of direction

int World::rotate(int param)
{
  param = param%4;
  
  switch (param)
    {
    case 0:
      orientation += 1;
      orientation = orientation%4;
      break;
    case 1:
      orientation -= 1;
      if (orientation < 0)
	orientation = 3;
      break;
    case 2:
      orientation += 2;
      orientation = orientation%4;
      break;
    case 3: //do nothing
      orientation += 0;
      break;
    }
  return 0;
}

// move_pointer now moves either 1 or 2 squares "forward"
// or "backwards" relative to current orientation
int World::move_pointer(int param)
{
  int p = 0; //either move 1 &2
  switch(param%4)
    {
    case 0:
      p = 1;
      break;
    case 1:
      p = 2;
      break;
    case 2:
      p = -1;
      break;
    case 3:
      p = -2;
      break;
    default:
      printf("World::move_pointer() - unknown param %d\n",param);
    }
  
  switch(orientation)
    {
    case 0:
      px+=p;
      break;
    case 1:
      py-=p;
      break;
    case 2:
      px-=p;
      break;
    case 3:
      py+=p;
      break;
    default:
      printf("World::move_pointer -- unknown orientation!\n");
    }
 
  // can't go to either outer side!
  if (px >= (WORLDSIZE -1 ))
    px = (WORLDSIZE - 2);
  if (py >= WORLDSIZE)
    py = (WORLDSIZE - 1);
  if (px <= 0)
    px = 1;
  if (py < 0)
    py = 0;

  return 0;
}

int World::in_world(int x1, int y1)
{
  if ((x1 > (WORLDSIZE - 1)) || (x1 <0))
    return 0;
  if ((y1 >= WORLDSIZE) || (y1 < 0))
    return 0;
  return 1;
}

int World::in_world(int x1, int y1, int x2, int y2)
{
  if ((x1 > (WORLDSIZE - 1)) || (x1 <0))
    return 0;
  if ((y1 >= WORLDSIZE) || (y1 < 0))
    return 0;
  if ((x2 > (WORLDSIZE -1)) || (x2 <0))
    return 0;
  if ((y2 >= WORLDSIZE) || (y2 < 0))
    return 0;
  
  return 1;
}

unsigned char World::get_h_conditions(int x1, int y1, int x2, int y2)
{
  unsigned char cases = 0;
  //first bit is down left
  cases = cases | ((unsigned char)(!is_free(x1, y1 - 1)));
  //second bit is down right
  cases = cases | ((unsigned char)(!(is_free(x2,y2 -1)))) << 1;
  //third bit is above left
  cases = cases | ((unsigned char)!(is_free(x1, y1 + 1))) << 2;
  //fourth bit is above right
  cases = cases | ((unsigned char)!(is_free(x2, y2 + 1))) << 3;
  //fifth bit is directly left
  cases = cases | ((unsigned char)!(is_free(x1 - 1,y1))) << 4;
  //sixth bit is directly right
  cases = cases | ((unsigned char)!(is_free(x2 + 1,y2))) << 5;
  return cases;
  
}

unsigned char World::get_v_conditions(int x1, int y1, int x2, int y2)
{
  unsigned char cases = 0;
  //first bit is bottom
  if (!is_free(x1,y1 -1 ))
    cases = cases | V_BOTTOM;
  //second bit is top
  if (!is_free(x2,y2+1))
    cases = cases |V_TOP;
  //third is on left
  if ((!is_free(x1 - 1,y1)) || (!is_free(x2 - 1,y2)))
    cases = cases | V_ON_LEFT;
  //fourt bit is right
  if ((!is_free(x1 + 1,y1)) || (!is_free(x2 + 1,y2)))
    cases = cases | V_ON_RIGHT;
  
  return cases;

}

int World::settle(int *x1, int *y1, int *x2, int *y2, int *hmode, int glued)
{
  // when "glued", bricks only fall if completely unsupported.
  // should falling bricks also change curser position and orientation?
  // if so, change it here!
  // should replace V_L_Top etc with generic
  // V_ON_L and V_ON_R
  int x1val = *x1;
  int x2val = *x2;
  int y1val = *y1;
  int y2val = *y2;
  int hmodeval = *hmode;
  //printf("Vert B Odds: %d, Canti B Odds: %d\n", _Vert_B_Odds, _Canti_B_Odds);
  if (_verbose)
    {
      //   printf("World::settle %d, %d, %d, %d, %d\n",x1val,y1val, x2val, y2val,hmodeval);
    }
  unsigned char env = 0;
  switch(hmodeval)
    {
    case 0: //horizontal
      env = get_h_conditions(x1val, y1val, x2val, y2val);
      //printf("settle: env is: %d\n",(int)env);
      // is fully supported
      if ((!(env ^ (H_LEFT_BOTTOM  | H_RIGHT_BOTTOM))) ||
	  (!(env ^ (H_LEFT_BOTTOM  | H_LEFT_TOP)))     || 
	  (!(env ^ (H_RIGHT_BOTTOM | H_RIGHT_TOP)))   || 
	  (!(env ^ (H_LEFT_BOTTOM  | H_IMM_RIGHT)))    || 
	  (!(env ^ (H_RIGHT_BOTTOM | H_IMM_LEFT))))
	{
	  //printf("supported!\n");
	  return 1;
	}
      if (!((env & (H_LEFT_BOTTOM | H_RIGHT_BOTTOM))))
	{
	  if(_verbose)
	    printf("World::settle() - unsupported horiz brick falling!\n");
	  
	  *y1 = y1val - 1;
	  *y2 = y2val - 1;
	  
	  return 0;
	}
      // nothing at bottom left
      if (!glued)
	{
	  if ((!(H_LEFT_BOTTOM & env)) && (H_RIGHT_BOTTOM & env))
	    {
	      if (binom(_Canti_B_Odds)) //it fell
		{
		  if(_verbose)
		    printf("World::settle() - cantilever brick fell\n");
		  *x2 = x1val;
		  *y1 = y1val - 1;
		  *hmode = 1; //brick is now vertical
		  return 0;
		}
	      else // it stayed
		{
		  if(_verbose)
		    printf("World::settle() - canti brick stayed\n");
		  return 1;
		}
	    }
	  //nothing at bottom right
	  else if ((H_LEFT_BOTTOM & env) && (!(H_RIGHT_BOTTOM & env)))
	    {
	      if (binom(_Canti_B_Odds))
		{
		  if(_verbose)
		    printf("World::settle() - cantilever brick fell\n");
		  *x1 = x2val;
		  *y1 = y2val - 1;
		  *hmode = 1;
		  return 0;
		}
	      
	      else
		return 1;
	    }
	}
      else
	{
	  if(_verbose)
	    printf("World::settle() - 'glued' brick stays\n");
	  return 1;
	}
      break;
    case 1: //vertical
      env = get_v_conditions(x1val, y1val, x2val, y2val);
      // printf("env is: %d\n",(int)env);
      if (!(V_BOTTOM & env))
	{
	  if(_verbose)
	    printf("World::settle() - vert brick dropped\n");
	  *x2 = x1val;
	  *y2 = y1val;
	  *x1 = x1val;
	  *y1 = y1val - 1;
	  return 0;
	}
      else //brick will not drop
	{
	  //if brick is supported by at least one brick 
	  // on each side:
	  // V_BOT_RIGHT || V_TOP_RIGHT
	  if ((env & V_ON_LEFT) && (env & V_ON_RIGHT))
	    {
	      if(_verbose)
		printf("brick supported on both sides\n");
	      return 1;
	    }
	  // supported on left but not the other
	  if (!glued)
	    {
	  if ((env & V_ON_LEFT) && (!(env & V_ON_RIGHT)))
	    {
	      //printf("brick supported on left\n");
	      if(binom((int)(_Vert_B_Odds/2)))
		{
		  //brick falls to the right and over
		  if (_verbose)
		    printf("semi-supported v brick topples right\n");
		  *x2 = x1val + 1;
		  *y2 = y1val;
		  *hmode = 0;
		  return 0;
		}
	      else
		{
		  if(_verbose)
		    printf("half-supported v brick stays\n");
		  return 1;
		}
	    }
	  // supported on right but not left
	  if ((!(env & V_ON_LEFT)) && (env & V_ON_RIGHT))
	    {
	      if(_verbose) 
		printf("brick supported on right\n");
	      if (binom((int)(_Vert_B_Odds/2)))
		{
		  *x2 = x1val;
		  *y2 = y1val;
		  *x1 = x1val - 1;
		  *y1 = y1val;
		  *hmode = 0;
		  return 0;
		}
	      else
		{
		  if(_verbose)
		    printf("half-supported v brick stays\n");
		  return 1;
		}
	    }
	  // brick is unsupported
	  else
	    {
	      //  printf("v brick is unsupported on either side\n");
	      if (binom(_Vert_B_Odds))
		{
		  //falls one way or the other
		  switch(int_rand_in_range(2))
		    {
		    case 0:
		      if (_verbose)
			printf("v brick topples to right\n");
		      *x2 = x1val + 1;
		      *y2 = y1val;
		      *hmode = 0;
		      return 0;
		      break;
		    case 1:
		      if(_verbose)
			printf("v brick topples to left\n");
		      *x2 = x1val;
		      *y2 = y1val;
		      *x1 = x1val - 1;
		      *y1 = y1val;
		      *hmode = 0;
		      return 0;
		      break;
		    }
		}
	      else
		{
		  //  printf("unsupported v brick stays\n");
		  return 1;
		}
	    }
	    }
	  else
	    {
	      if(_verbose) 
		printf("World::settle() - 'glued' brick stays\n");
	      return 1;
	    }
	}
      break;
    default:
      break;
    }
  return 1;
}

int World::put_brick(int param)
{
  
  int x1, x2, y1, y2, hmode;
  int newx =0;
  int newy = 0;
 
  param = param%4;
  switch(param)
    {
    case PUT_AHEAD:
      break;
    case PUT_TO_RIGHT:
      rotate(ROT_PLUS_90);
      break;
    case PUT_TO_LEFT:
      rotate(ROT_MINUS_90);
      break;
    case PUT_BEHIND:
      rotate(ROT_180);
      break;
    default:
      printf("World::put_brick() - unknown paramater!\n");
      break;
    }
  switch(orientation)
    {
    case 0:
      x1 = px;
      y1 = py;
      x2 = px + 1;
      y2 = py;
      newx = x2 +1;
      newy = y2;
      hmode = 0;
     
      put_brick(x1,y1,x2,y2,hmode); //coords have changed
      break;
      
    case 1:
      x1 = px;
      y1 = py - 1;
      x2 = px;
      y2 = py;
      newx = x1;
      newy = y1 - 1; 
      hmode = 1;
      
      put_brick(x1,y1,x2,y2,hmode); //coords have changed
      break;
      
    case 2:
      x1 = px - 1;
      y1 = py;
      x2 = px;
      y2 = py;
      newx = x1 - 1;
      newy = y1;
      hmode = 0;
      put_brick(x1,y1,x2,y2,hmode); //coords have changed
      break;
      
    case 3:
      x1 = px;
      y1 = py;
      x2 = px;
      y2 = py + 1;
      newx = x2;
      newy = y2 + 1;
      hmode = 1;
      put_brick(x1,y1,x2,y2,hmode); //coords have changed
      break;
    default:
      printf("World::put_brick: unknown orientation!\n");
      break;
    }
 
  //printf("newx: %d, newy: %d\n",newx,newy);
 
  if (newx >= (WORLDSIZE -1))
    newx = WORLDSIZE - 2;
  if (newx <= 0)
    newx = 1;
  if (newy >= WORLDSIZE)
    newy = WORLDSIZE - 1;
  if (newy < 0)
    newy = 0;
  
  px = newx;
  py = newy;

  //printf("newx: %d, newy: %d\n",newx,newy);

  return 0;
}

int World::put_brick(int x1, int y1, int x2, int y2, int hmode,int glued)
{
  
  // printf("put brick: %d %d %d %d %d %d\n",x1,y1,x2,y2,hmode,glued);
  if (_verbose)
    {
    }
  if (in_world(x1,y1,x2,y2))
    {
      if ((is_free(x1,y1)) && (is_free(x2,y2)))
	{
	  if (settle(&x1,&y1,&x2,&y2,&hmode,glued))
	    paint(x1,y1,x2,y2,hmode); //paint a brick
	  else
	    put_brick(x1,y1,x2,y2,hmode,glued); //coords have changed
	}
    } 
  return 0;
}

int World::is_free(int x, int y)
{
  if ((x < 0) || (x >= WORLDSIZE))
    {
      //  printf("World::is_free() - x out of range, but ok;");
      return 0;
    }
  if ((y < 0) || (y >= WORLDSIZE))
    {
      //printf("World::is_free() - y out of range, but ok;");
      return 0;
    }
  if (grid[x][y] == 0)
    return 1;
  if (grid[x][y] == 5)
    return 1;
  else
    return 0;
}

int World::get_grid_val(int x,int y)
{
  return grid[x][y];
}
void World::paint(int x1, int y1, int x2, int y2,int hmode)
{
  if (hmode == 0)
    {
      grid[x1][y1] = 1;
      grid[x2][y2] = 2;
    }
  else
    {
      grid[x1][y1] = 3;
      grid[x2][y2] = 4;
    }
}

void World::print()
{
  //  printf("World::print()\n");
  #ifdef USE_CURSES
  clear();
  (void) initscr();
  (void) nonl();         /* tell curses not to do NL->CR/NL on output */
  #endif
  for (int j = (WORLDSIZE -1); j>=0;j--)
    {
      for (int i = 0; i<WORLDSIZE;i++)
	{
	  if ((i == px) && (j == py))
	    switch(orientation)
	      {
		#ifdef USE_CURSES
	      case 0:
		mvaddch((WORLDSIZE - j),2*i,'>');
		break;
	      case 1:
		mvaddch((WORLDSIZE - j),2*i,'v');
		break;
	      case 2:
		mvaddch((WORLDSIZE - j),2*i,'<');
		break;
	      case 3:
		mvaddch((WORLDSIZE - j),2*i,'^');
		break;
	      default:
		printf("unknown orientation: %d!\n", orientation);
		break;
		#endif
		#ifndef USE_CURSES
	      case 0:
		printf("> ");
		break;
	      case 1:
		printf("v ");
		break;
	      case 2:
		printf("< ");
		break;
	      case 3:
		printf("^ ");
		break;
	      default:
		printf("unknown orientation: %d!\n", orientation);
		break;
		#endif
	      }
	  else
	    { 
	      int ch = '-';
	      switch (grid[i][j])
		{
		case 0:
		  ch = '.';
		  break;
		case 1:
		  ch = '1';
		  break;
		case 2:
		  ch = '2';
		  break;
		case 3:
		  ch = '3';
		  break;
		case 4:
		  ch = '4';
		  break;
		case 5:
		  ch = '-';
		  break;
		case 6:
		  ch = '*';
		  break;
		default:
		  printf("unknown grid contents\n");
		}
	      #ifdef USE_CURSES
	      mvaddch((WORLDSIZE - j),2*i,ch);
	      #endif
	      #ifndef USE_CURSES
	      printf("%c ",ch);
	      #endif
	    // printf("%d ",grid[i][j]);
	    }
	}
      #ifndef USE_CURSES 
      printf("\n");
      #endif
    }
  #ifdef USE_CURSES
  move(WORLDSIZE,0);
  refresh();
  #endif
  printf("\n");
  //endwin();
}

void World::print_ppm(int sequencenum)
{
  
  
  char outfilename[40];
  
  sprintf(outfilename,"%d.ppm", sequencenum);
      
  printf("filename: %s\n",outfilename);
  ofstream fp;
  
  int numprinted = 0;
  
  fp.open(outfilename);
  fp <<"P5" <<endl;
  fp << "# Frame number " << sequencenum <<endl;
  fp << WORLDSIZE*10 << " " <<WORLDSIZE*10 <<endl;
  fp << "255" << endl;
  
  
  for (int j = (WORLDSIZE -1); j>=0;j--)
    {
      for (int m = 0; m< 10; m++)
	{
	  for (int i = 0; i<WORLDSIZE;i++)
	    {
	      int ch;
	      if ((i == px) && (j == py)) 
		{
		switch(orientation) 
		  { 
 		  case 0:
		    /*
		    for (int n = 0;n <10;n++)
		      {
			if (n <m)
			  fp << ((unsigned char) 0);
			else
			  fp << ((unsigned char) 255);
			  
		      }
		    */
		    ch = 90;
		    break;
		  case 1:
		    //printf("v ");
		    ch = 90;
		    break;
		  case 2:
		    //printf("< ");
		    ch = 90;
		    break;
		  case 3:
		    //printf("^ ");
		    ch = 90;
		    break;
		  default:
		    printf("unknown orientation: %d!\n", orientation);
		    break;
		   }
		 for (int k = 0; k < 10; k++)
		    {
		      fp << ((unsigned char) ch);
		    }
		}
 	      else
	 	{ 
	      
		  int ch;
		  
		  switch (grid[i][j])
		    {
		    case 0:
		      ch = 255;
		      break;
		    case 1:
		      ch = 122;
		      break;
		    case 2:
		      ch = 122;
		      break;
		    case 3:
		      ch = 0;
		      break;
		    case 4:
		      ch = 0;
		      break;
		    case 5:
		      ch = 0;
		      break; 
		    default:
		      printf("unknown grid contents\n");
		    }
		  for (int k = 0; k < 10; k++)
		    {
		      fp << ((unsigned char) ch);
		      numprinted++;
		      //fp << ch << " ";
		    }
		  
		}	
	    }
	}
    }
  printf("Num Printed: %d\n",numprinted);
  //fp.close();
}

int World::unpaint(int x1, int y1, int x2, int y2)
{
  if ((x1 < WORLDSIZE) && (y1 < WORLDSIZE))
    grid[x1][y1] = 0;
  if ((x2 < WORLDSIZE) && (y2 <WORLDSIZE))
    grid[x2][y2] = 0;
  
  return 0;
}

// clean up bricks 
//hm. what if we only worry about 1's and 3's?

void World::clean_grid()
{
  int i2 = 0,j2 = 0,val = 0;
  int bhmode = 0;
  int fall = 0;

  for (int j = 0; j < WORLDSIZE; j++)
    {
      for (int i = 0; i < WORLDSIZE; i++)
	{
	  int is_orphan = 0;
	  val = grid[i][j];
	  switch (val)
	    {
	    case 0:
	      fall = 0;
	      break;
	    case 1:
	      i2 = i+1;
	      j2 = j;
	      bhmode = 0;
	      fall = 1; 
	      if (in_world(i2,j2))
		  is_orphan = (grid[i2][j2] != 2);
	      break;
	    case 2:
	      i2 = i - 1;
	      j2 = j;
	      if (in_world(i2,j2))
		  is_orphan = (grid[i2][j2] != 1);
	      fall = 0;
	      break;
	    case 3:
	      i2 = i;
	      j2 = j+1;
	      if (in_world(i2,j2))
		is_orphan = (grid[i2][j2] != 4);
	      fall = 1;
	      bhmode = 1;
	      break;
	    case 4:
	      i2 = i;
	      j2 = j - 1;
	      if (in_world(i2,j2))
		is_orphan = (grid[i2][j2] != 3);
	      fall = 0;
	      break;
	    }

	  // this should get rid of "orphans" 
	  // caused by putting only half of a brick in a module.
	  if (is_orphan)
	    {
	      grid[i][j] = 0;
	    }
	  else if (fall)
	    {
	      int tempi  = i;
	      int tempj  = j;
	      //printf("FIX ME!\n");
	      //exit(-1);
	      //printf("World::clean_grid() - evaluating brick at: (%d,%d)(%d,%d)\n",i,j,i2,j2);
	      unpaint(tempi,tempj,i2,j2);
	      put_brick(tempi,tempj,i2,j2,bhmode,1); //consider bricks glued
	    }
	}
    }
} 


int World::take_block(int x, int y)
{
  if ((x < WORLDSIZE) && (y < WORLDSIZE))
    {
      switch(grid[x][y])
	{
	case 0:
	  break;
	case 1:
	  unpaint(x,y,x+1,y);
	  break;
	case 2:
	  unpaint(x,y,x-1,y);
	  break;
	case 3:
	  unpaint(x,y,x,y+1);
	  break;
	case 4:
	  unpaint(x,y,x,y-1);
	  break;
	}
    }
  //cleangrid();
  return 0;
}
int World::take_block()
{
  int x = get_px();
  int y = get_py();
  if (((x <WORLDSIZE)&& (x>= 0)) && ((y < WORLDSIZE) && (y >= 0)))
    {
      switch(grid[x][y])
	{
	case 0:
	  break;
	case 1:
	  unpaint(x,y,x+1,y);
	  break;
	case 2:
	  unpaint(x,y,x-1,y);
	  break;
	case 3:
	  unpaint(x,y,x,y+1);
	  break;
	case 4:
	  unpaint(x,y,x,y-1);
	  break;
	}
    }
  return 0;
}

int World::take()
{
  take_block();
  clean_grid();
  return 0;
}
int World::get_px()
{
  return px;
}

int World::get_orientation()
{
  return orientation;
}

int World::get_py()
{
  return py;
}

int World::print_world_to_file()
{

  FILE *outfile = fopen("world.out","a+");
  
  if (!outfile) 
    {
      printf("cannot open file world.out for output\n");
      return -1;
    }
  for (int j = (WORLDSIZE-1);j>=0;j--)
    {
    for (int i = 0; i < WORLDSIZE; i++)
      {
	fprintf(outfile,"%d ",grid[i][j]);
      }
    fprintf(outfile,"\n");
    }
  fclose(outfile);
  return 0;
}


int World::print_world_to_file(char *outfilename)
{

  FILE *outfile = fopen(outfilename,"w");
  
  if (!outfile) 
    {
      printf("cannot open file %s for output\n",outfilename);
      return -1;
    }
  for (int j = (WORLDSIZE-1);j>=0;j--)
    {
    for (int i = 0; i < WORLDSIZE; i++)
      {
	fprintf(outfile,"%d ",grid[i][j]);
      }
    fprintf(outfile,"\n");
    }
  fclose(outfile);
  return 0;
}


int World::read_world_from_file()
{
  FILE *infile = fopen("world.in","r");
  for (int j=(WORLDSIZE-1);j>=0;j--)
    {
      for (int i=0;i<WORLDSIZE;i++)
	{
	  fscanf(infile,"%d", &grid[i][j]);
	}
      fscanf(infile,"\n");
    }
  fclose(infile);
  return 0;
}

int World::read_world_from_file(char *infilename)
{
  FILE *infile = fopen(infilename,"r");
  for (int j=(WORLDSIZE-1);j>=0;j--)
    {
      for (int i=0;i<WORLDSIZE;i++)
	{
	  fscanf(infile,"%d", &grid[i][j]);
	}
      fscanf(infile,"\n");
    }
  fclose(infile);
  return 0;
}

int World::boundaryFill(int x, int y)
{
  if ((x < 0) || (x >= WORLDSIZE)) 
    return 0;
  if ((y < 0) || (y >= WORLDSIZE))
    return 0;
  
  int current = grid[x][y];
  int filled = 0;
  if (current == 0)
    {
      filled = 1;
      put_fill(x,y);
      filled += boundaryFill(x+1, y);
      filled += boundaryFill(x, y+1);
      filled += boundaryFill(x-1, y);
      filled += boundaryFill(x, y-1);
  }
  return filled;
}

void World::put_fill(int in_xpos, int in_ypos)
{
  //printf("putting fill at: %d %d!\n",in_xpos, in_ypos);
  grid[in_xpos][in_ypos] = 5;
}

int World::paint_module(World *module)
{
      for (int j = 0; j < WORLDSIZE; j++)
	{
	  for (int i = 0; i < WORLDSIZE; i++)
	    {
	      int mod_val = module->get_grid_val(i,j);
	    
	      int my_x = 0;
	      int my_y = 0;
	      int brick_val = 6;
	      if (mod_val != 0)
		{
		  switch(orientation)
		    {
		    case 3: // facing "north"
		      // so when i = WORLDSIZE/2, my_x = px
		      // and when i = 0, my_x = px - worldsize/2;
		      // and when i = (worldsize - 1)
		      //  myx = myx + worldsize/2 - 1 
		      my_x = px  - ((WORLDSIZE/2) - i);
		      my_y = py + j;
		      //printf("i: %d, j: %d, px: %d, py: %d, myx: %d, myy:%d, wsdiv2: %d\n", i,j,px,py,my_x,my_y,WORLDSIZE/2);
		      // no change
		      brick_val = mod_val;
		      break;
		    case 1: //facig "south"
		      my_y = py - j;
		      my_x = px + ((WORLDSIZE/2) - i);
		      // threes and fours switch. ones and twos switch.
		      if ((mod_val == 3) || (mod_val == 1))
			brick_val = mod_val + 1;
		      else
			brick_val = mod_val - 1;
		      break;
		    case 0: //facing east
		      my_x = px + j;
		      my_y = py + ((WORLDSIZE/2) - i);
		      // 1 becomes 4
		      // 2 becomes 3
		      // 3 becomes 1
		      // 4 becomes 2
		      switch(mod_val)
			{
			case 1:
			  brick_val = 4;
			  break;
			case 2:
			  brick_val = 3;
			  break;
			case 3:
			  brick_val = 1;
			  break;
			case 4:
			  brick_val = 2;
			  break;
			default:
			  printf("bad mod val in paint module\n");
			}
		      break;
		    case 2: //west
		      my_x = px - j;
		      my_y = py - ((WORLDSIZE/2) - i);
		      // 1 becomes 3
		      // 2 becomes 4
		      // 3 becomes 2
		      // 4 becomes 1
		      switch(mod_val)
			{
			case 1:
			  brick_val = 3;
			  break;
			case 2:
			  brick_val = 4;
			  break;
			case 3:
			  brick_val = 2;
			  break;
			case 4:
			  brick_val = 1;
			  break;
			default:
			  printf("bad mod val in paint module\n");
			}
		      break;
		    default:
		      printf("bad orientation in paint_module!\n");
		    }
		  //okay it fits:
		  if ((my_x >= 0) && (my_x < WORLDSIZE)
		      && (my_y >= 0) && (my_y < WORLDSIZE)
		      // don't want to ever put half of a brick!
		      && (! (((my_y == 0) && (brick_val == 4))
			     || ((my_y == (WORLDSIZE - 1) && (brick_val == 3)))
			     || ((my_x == 0) && (brick_val == 2))
			     || ((my_x == (WORLDSIZE - 1) && (brick_val == 1))))))
			     
		    {
		      if (is_free(my_x,my_y))
			{
			  grid[my_x][my_y] = brick_val;
			}
		    }
		}	
	       
	    }
	} 
      int mod_px = module->get_px();
      int mod_py = module->get_py();
      int mod_or = module->get_orientation();
      int new_px;
      int new_py;

      switch(orientation)
	{
	case 3: // facing "north"
	  // so when i = WORLDSIZE/2, my_x = px
	  // and when i = 0, my_x = px - worldsize/2;
	  // and when i = (worldsize - 1)
	  //  myx = myx + worldsize/2 - 1 
	  new_px= px  - ((WORLDSIZE/2) - mod_px);
	  new_py = py + mod_py;
	  //printf("my orientation is: %d, mod_or is: %d\n",orientation,mod_or);
	  orientation = mod_or;
	  //printf("orientation is now: %d\n",orientation);
	  break;
	case 1: //facig "south"
	  new_py = py - mod_py;
	  new_px = px + ((WORLDSIZE/2) - mod_px);
	  //printf("my orientation is: %d, mod_or is: %d\n",orientation,mod_or);
	  orientation = (mod_or+2)%4;
	  //printf("orientation is now: %d\n",orientation);
	
	  break;
	case 0: //facing east
	  new_px = px + mod_py;
	  new_py = py + ((WORLDSIZE/2) - mod_px);
	  //printf("my orientation is: %d, mod_or is: %d\n",orientation,mod_or);
	  orientation = (mod_or+1)%4; //wild guess
	  //printf("orientation is now: %d\n",orientation);
	  break;
	case 2: //west
	  new_px = px - mod_py;
	  new_py = py - ((WORLDSIZE/2) - mod_px);
	  //printf("my orientation is: %d, mod_or is: %d\n",orientation,mod_or);
	  orientation = (mod_or+3)%4;
	  //printf("orientation is now: %d\n",orientation);
	  break;
	default:
	  printf("bad orientation in paint_module!\n");
	}
      //okay it fits:
      
      new_px = max(new_px,0);
      new_py = max(new_py,0);
      new_px = min(new_px,WORLDSIZE-1);
      new_py = min(new_py,WORLDSIZE-1);
      px = new_px;
      py = new_py;
      //orientation = ((orientation + mod_or)%4);
     
      // now clean the grid
      clean_grid();
      return 1;
}
