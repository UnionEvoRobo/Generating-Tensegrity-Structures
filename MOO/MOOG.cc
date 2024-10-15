//Multi Objective Optimization Genome
// see MOOG.h for details
#include "MOOG.h"

MOOG::MOOG()
{
  init();
}

MOOG::~MOOG()
{
  // printf("MOOG::~MOOG()!\n");
}


int MOOG::randomize_genome()
{
  int maxit = (int)_genome.size();
  for (int i = 0; i< maxit;i++)
    {
      _genome[i]->randomize();
    }
  return 1;
}

vector<MOOL *> MOOG::get_genome()
{
  return _genome;
}

vector<float> MOOG::get_objective_vals(unsigned int flags)
{
  vector<float> outvect;
  outvect.clear();
  for (int i = 0; i < (int)_objectives.size(); i++)
    {
      if (((unsigned int)(pow(2,i))) & flags)
	outvect.push_back(_objectives[i]);
    }
  return outvect;
}

vector<float> MOOG::get_objective_vals()
{
  return _objectives;
}

float MOOG::get_ith_objective(int i)
{
  if (((int)_objectives.size()) >= (i + 1))
    {
      return _objectives[i];
    }
  else
    {
      printf("MOOG::get_ith_objective(int i) - bad index!\n");
      printf("MOOG::get_ith_objective(int i) - only %d objectives!\n",
	     (int)_objectives.size());
      
      return -1;
    }
}

int MOOG::get_len()
{
  return (int)_genome.size();
}

void MOOG::set_ith_objective(int i,float val)
{
  if (((int)_objectives.size()) >= (i + 1))
    {
      _objectives[i] = val;
    }
}

float MOOG::get_diversity()
{
  return get_ith_objective(MOO_DIV_INDEX);
}

int MOOG::get_div_len_mod()
{
  return _Div_Len_Mod;
}

void MOOG::set_diversity(float newdiv)
{
  set_ith_objective(MOO_DIV_INDEX,newdiv);
}

int MOOG::get_rank()
{
  return (int)get_ith_objective(MOO_RANK_INDEX);
}

//we're trying to MINIMIZE RANK, so...
void MOOG::set_rank(int newrank)
{
  set_ith_objective(MOO_RANK_INDEX,(float)newrank);
}

void MOOG::init()
{
  //init
}

void MOOG::print_phenotype()
{
  printf("MOOG::print_phenotype()\n");
  _phenotype->print();
}

void MOOG::print()
{
  printf("MOOG::print()\n");
}

int MOOG::fprint_genome (char *outfilename)
{
  printf("MOOG::fprint %s\n",outfilename);
  return -1;
}

int MOOG::fscan_genome(char *infilename)
{

  printf("MOOG::fscan_genome %s\n",infilename);
  return -1;
}

void MOOG::mutate_with_prob(int prob)
{
  for (int i = 0; i< (int)_genome.size();i++)
    {
      if (binom(prob))
	{
	  _genome[i]->randomize();
	}
    }
} 

void MOOG::add_del_with_prob(int prob)
{ 
  
  int max = (int)_genome.size();
  
  vector<MOOL *>::iterator iter;
  
  iter = _genome.begin();

  for (int i = 0; i< max;i++)
    {
      iter = _genome.begin() + i;

      if (binom(prob))
	{
	  switch(int_rand_in_range(2))
	    {
	    case 0:
	       if ((int)_genome.size() > 1)
	       {
		 // iter = &(_genome[i]);
		 if (_genome[i] != NULL)
		   {
		     delete _genome[i];
		     _genome[i] = NULL;
		     _genome.erase(iter);
		     max -=1;
		   }
	       }
	       break;
	    case 1:
	      //      iter = &(_genome[i]);
	      _genome.insert(iter,random_locus());
	      // max += 1 ; ?!?!?
	      break;
	    }
	}
    }
}

MOOL* MOOG::random_locus()
{
  MOOL *newMOOL = new_MOOL();
  newMOOL->randomize();
  return newMOOL;
}

int MOOG::equal_to_genome(vector<MOOL *> othergenome)
{
  if (_genome.size() != othergenome.size())
    return 0;
  else
    {
      int maxit = (int)_genome.size();
      for (int i = 0; i<maxit;i++) 
	{
	  if ((_genome[i]->distance_to_locus(othergenome[i])) != 0.0)
	    {
	      return 0;
	    }
	}
    }
  return 1;
}


float MOOG::distance_to_genome(vector<MOOL *> othergenome)
{
  int maxit = (_genome.size() > othergenome.size()) ? othergenome.size(): _genome.size();
  
  float distance = 0;
  for (int i = 0; i < maxit;i++)
    distance += _genome[i]->distance_to_locus(othergenome[i]);
  
  distance = distance/maxit; //!

  //printf("DivLenMod is: %d\n",_Div_Len_Mod);

  int len_diff;
  float foo;
  
  if (_Div_Len_Mod)
    {
  len_diff = (_genome.size() -  othergenome.size());
  if (len_diff > 0)
    {
      if (len_diff < _Div_Len_Mod)
	foo = pow(_Div_Len_Mod,2);
	  else
	    foo = pow(_Div_Len_Mod,2) - pow((len_diff - _Div_Len_Mod),2);
	  
	  foo = foo/(pow(_Div_Len_Mod,2));

	  if (foo > 0)
	    distance += foo;
	}
    }
  
  return distance;
}

int MOOG::single_point_crossover(MOOG *p1, int p1x, MOOG*p2, int p2x)
{
  //don't bother to write it until you need to use it.
  printf("MOOG::single_point_crossover() has not been written\n");
  return 0;
}

int MOOG::two_point_crossover(MOOG *p1, int p1x1, int p1x2, MOOG *p2, int p2x1, int p2x2)
{
  
  vector <MOOL *>p1g = p1->get_genome(); //parent 1's genotype
  vector <MOOL *>p2g = p2->get_genome(); //parent 1's genotype


  p1x1 = (p1x1 < (int)p1g.size()) ? p1x1 : ((int)p1g.size() - 1);
  p1x2 = (p1x2 < (int)p1g.size()) ? p1x2 : ((int)p1g.size() - 1); 
  p2x1 = (p2x1 < (int)p2g.size()) ? p2x1 : ((int)p2g.size() - 1);
  p2x2 = (p2x2 < (int)p2g.size()) ? p2x2 : ((int)p2g.size() - 1);
  
  //  printf("MOOG::two_point_crossover: %d, %d, %d, %d, %d, %d\n",p1g.size(), p1x1, p1x2, p2g.size(),p2x1,p2x2);

  //_genome.clear();
  clear_genome();
  int i;

  //push back the first p1x1 members of p1
  for (i = 0; i< p1x1;i++)
    {
      //hurrah for copy constructors
      _genome.push_back(new_MOOL(p1g[i]));
    }
  //push back (p2x2 - p2x1) elements
  // starting at p2[p2x1]
  for(i=0;i<(p2x2-p2x1);i++)
    {
      _genome.push_back(new_MOOL(p2g[p2x1 + i]));
    }

  // push back (p1.size() - p1x2) elements
  // starting at p1[p1x2]
  for(i=0;i<((int)p1g.size() - p1x2);i++)
    {
      _genome.push_back(new_MOOL(p1g[p1x2 + i]));
    }

  if ((int)_genome.size() == 0)
    {
      printf("zero size!\n");
      exit(-1);
    }

  return 0;
}

vector<MOOL *> MOOG::get_flat_genome()
{
  vector<MOOL *> outvect;
  outvect.clear();
  for (int i = 0; i < (int)_genome.size(); i++)
    {
      vector <MOOL *> contentsOfLocus = _genome[i]->get_contents();
      for (int j = 0; j < (int)contentsOfLocus.size(); j++)
	{
	  outvect.push_back(contentsOfLocus[j]);
	}

    }
  
  return outvect;
}

void MOOG::set_genome(vector<MOOL *> newg)
{
  clear_genome();
  for (int i = 0; i < (int)newg.size(); i++)
    _genome.push_back(new_MOOL(newg[i]));
}

void MOOG::append_to_genome(vector<MOOL *> appendage)
{
  for (int i = 0; i < (int)appendage.size(); i++)
    _genome.push_back(appendage[i]);
}
