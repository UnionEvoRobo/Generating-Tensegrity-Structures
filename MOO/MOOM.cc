#include "MOOM.h"

MOOM::MOOM()
{
  init();
}

MOOM::MOOM(MOOM *other)
{
  init();
  vector<MOOL *> new_contents = other->get_contents();

  for (int i = 0; i < (int)new_contents.size(); i++)
    {
      MOOL *ins;
      if (new_contents[i]->is_leaf())
	ins = new Instruction((Instruction *)new_contents[i]);
      else
	ins = new MOOM((MOOM *)new_contents[i]);
	
      _contents.push_back((MOOL *)ins);
    }
}

MOOM::MOOM(Assembly *fromAssembly)
{
  init();
  vector<MOOL *> new_contents = fromAssembly->get_genome();
  
  for (int i = 0; i < (int)new_contents.size(); i++)
    {
      MOOL *ins;
      if (new_contents[i]->is_leaf())
	ins = new Instruction((Instruction *)new_contents[i]);
      else
	ins = new MOOM((MOOM *)new_contents[i]);
	
      _contents.push_back((MOOL *)ins);
    }
 
  
  if (is_viable() == 0)
    {
      printf("You have given me a non-viable assembly to modularize!\n");
    }

}

MOOM::~MOOM()
{
  for (int i = (int)_contents.size() -1 ; i >= 0; i--)
    {
      delete(_contents[i]);
      _contents[i] = NULL;
    }
  if (_result)
    delete(_result);
}


void MOOM::init()
{
  _NumModuleEvals = 1;
  _contents.clear();
  _result = NULL;
}

void MOOM::set_contents(vector<MOOL *> new_contents)
{
  _contents.clear();
  for (int i = 0; i < (int)new_contents.size(); i++)
    {
      //new contents may contain MOOMS! so we can't do it this way...
      MOOL *ins;
      if (new_contents[i]->is_leaf())
	ins = new Instruction((Instruction *)new_contents[i]);
      else
	ins = new MOOM((MOOM *)new_contents[i]);
	
      _contents.push_back((MOOL *)ins);
    }
}

vector<MOOL *> MOOM::get_contents()
{
  return _contents;
}

void MOOM::print()
{
  printf("[ ");
  for (int i = 0; i < (int)_contents.size(); i++)
    {
      _contents[i]->print();
    }
  printf(" ]");
}

void MOOM::print_verbose()
{
    printf("**begin module**\n");
  for (int i = 0; i < (int)_contents.size(); i++)
    {
      _contents[i]->print_verbose();
    }
  printf("**end module**\n");
}

void MOOM::print_result()
{
  if (_result)
    _result->print();
}

int MOOM::is_viable()
{
  int _Vert_B_Odds = 50;
  int _Canti_B_Odds = 50;
  float volume_result = 0; 
  float outer_result = (float)WORLDSIZE*WORLDSIZE;
  float mass_result = (float)WORLDSIZE*WORLDSIZE;
  
  _result = new World(_contents,_Vert_B_Odds,_Canti_B_Odds);
  _result->print();

  for (int i = 0; i < _NumModuleEvals; i++)
    {
      World *world2 = new World(_contents,_Vert_B_Odds,_Canti_B_Odds);
      //   world2->print();
      if (_result->compare_to(world2) == 0)
	{
	  printf("worlds are dissimilar\n");
	  delete(_result);
	  delete(world2);
	  _result = NULL;
	  return 0;
	}
      delete(world2);
    }
  
  
  //  _result->print();
  //printf("Module is viable\n");
   
   return 1;
  

}

void MOOM::randomize()
{
  printf("MOOMs can't be randomized!\n");
}

void MOOM::randomize(int in)
{
  printf("MOOMs can't be randomize(int)'d!\n");
}

double MOOM::distance_to_locus(MOOL *otherL)
{
  printf("MOOMs can't be compared to other locii yet!\n");
  return 0.;
}

int MOOM::is_leaf()
{
  return 0;
}

World *MOOM::get_result()
{
  if (_result == NULL)
    {
      //lazy module evaluation. hehe.
    _result = new World(_contents,50,50);
    }
  return _result;
}
