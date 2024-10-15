#include "MOOF.h"

MOOF::MOOF()
{

}

MOOF::MOOF(char *outfilename)
{
  _outfilename = outfilename;
  printf("MOOF::MOOF()\n");
  //  init();
}

MOOF::~MOOF()
{
  
}

MOOG * MOOF::get_ith_member(int i)
{

  if (i < (int)_pop.size())
    return _pop[i];
  else
    return NULL;
}


vector<MOOG *> MOOF::with_max_obj_val(vector<MOOG *>subpop, int objindex)
{
  if (subpop.size() <= 0)
    {
      return subpop;
    }
  if (objindex >= (int)(subpop[0]->get_objective_vals()).size())
    {
      printf("with_max_obj_val(vect,int) = bad obj index!\n");
      return subpop;
    }
  
  float bestval; 
  for (int i = 0; i < (int)subpop.size(); i++)
    {
      if (i == 0)
	bestval = subpop[i]->get_ith_objective(objindex);
      bestval = max(subpop[i]->get_ith_objective(objindex),bestval);
    }
 
  //printf("bestval for obj %d is %f\n",objindex,bestval);

  vector<MOOG *> newpop;
  newpop.clear();
  
  for (int i = 0; i < (int)subpop.size(); i++)
    {
      float curval  = subpop[i]->get_ith_objective(objindex);
      if (curval == bestval)
	newpop.push_back(subpop[i]);
    }

  return newpop;
}
  
vector<MOOG *> MOOF::with_max_obj_val(vector<MOOG *>subpop, vector<int> indexes)
{
  if (indexes.size() <= 1)
    return subpop;
  else
    {
      //printf("with max, subpop is %d, indexes are: ",(int)subpop.size());
      // for (int i = 0; i < (int)indexes.size(); i++)
      //	printf(" %d ",indexes[i]);
      // printf("\n");
      vector<MOOG *> newpop;
      int newval = indexes[indexes.size() -1];
      indexes.pop_back();
      newpop = with_max_obj_val(subpop,newval);
      // printf("..and newpop is: %d\n",(int)newpop.size());
      if ((int)newpop.size() > 0)
	return with_max_obj_val(newpop,indexes);
      else
	return subpop;
    }
}

//ranks in decreasing order of priority!
//so MOST IMPORTANT comes last!
MOOG* MOOF::best_by_priority(vector<int> ranks)
{
  // printf("best by priority - there are %d ranks\n",(int)ranks.size());
  vector<MOOG *> newpop =  with_max_obj_val(_pop,ranks);
  if (newpop.size() <= 0)
      printf("best_by_priority - no results!\n");
  else
    return newpop[0];
  return NULL;
}

int MOOF::fprint_pop_stats(char *outfilename, int gen)
{
  if (_do_logging)
    {
      FILE *outfile = fopen(outfilename,"a+");
      if (!outfile) 
	{
	  printf("MOOF.cc:fprint_pop_stats(fname,gen) -  cannot open file %s for output\n",outfilename);
	  return -1;
	}
      int popsize = (int)_pop.size();
      
      //fprintf(outfile,"\n**** GENERATION: %d, LAYER: %d ***\n",gen,layer);
      
      for (int i = 0; i < popsize; i++)
	{
	  if (_pop[i]->get_rank() >= (-1*_Layers))
	    {
	      fprintf(outfile,"%d ",gen);
	      _pop[i]->fprint_obj_vals(outfile,_MoMask);
	      fprintf(outfile,"%d\n",_num_P_Evals);
	      //_pop[i]->fprint_genome(outfile);
	    }
	}
      fflush(outfile);
      fclose(outfile);
    }
}

int MOOF::fprint_pop_stats(char *outfilename, int gen, int layer)
{
  if (_do_logging)
    {
      FILE *outfile = fopen(outfilename,"a+");
      if (!outfile) 
	{
	  printf("MOOF::fprint_pop_stats(fname,int,int) - cannot open file %s for output\n",outfilename);
	  return -1;
	}
      int popsize = (int)_pop.size();
      
      //fprintf(outfile,"\n**** GENERATION: %d, LAYER: %d ***\n",gen,layer);
      
      for (int i = 0; i < popsize; i++)
	{
	  if (_pop[i]->get_rank() >= (-1*layer))
	    {
	      fprintf(outfile,"%d ",gen);
	      _pop[i]->fprint_obj_vals(outfile);
	      fprintf(outfile,"%d\n",_num_P_Evals);
	      //_pop[i]->fprint_genome(outfile);
	    }
	}
      fflush(outfile);
      fclose(outfile);
    }
  return 0;
}

void MOOF::print_pop_stats()
{
  int popsize = (int)_pop.size();
  for (int i = 0; i<popsize;i++) 
    {
      //printf("Individual %i:
      _pop[i]->print_obj_vals();
      //  _pop[i]->print();
    }
}

unsigned int MOOF::get_MoMask()
{
  printf("MOOF::get_MoMask() :%d\n",(int)_MoMask);
  return _MoMask;
}

int MOOF::get_pop_size()
{
  return (int)_pop.size();
}

void MOOF::reset_ranks()
{
  for (int i = 0; i< (int)_pop.size(); i++)
    {
      _pop[i]->set_rank(0);
    }
}


// by default we're trying to MAXIMIZE all objectives
// so if there's an obj you're trying to minimize, multiply it by -1 !
int MOOF::pareto_dominates(vector<float> v1, vector<float> v2,unsigned int mask)
{
  int sz = (int)v1.size();
  int best_in_one = 0;
 
  
  for (int i = 0; i< sz; i++)
    {
      // if v2 > v1 in at least one dimension
      // then v1 !>p v2
      if (((unsigned int)(pow(2,i))) & mask) //all those we care about
	{
	  if (v1[i] < v2[i]) //no worse in any dimension
	    return 0;
	  if (v1[i] > v2[i]) //better in at least one
	    best_in_one = 1;
	}
    }
  return best_in_one;
} 

// naunced should be 0 by default
int MOOF::pareto_rank(int curr_rank,unsigned int mask, int nuanced)
{

  //printf("pareto_rank: %d, %d, %d\n",curr_rank,mask,nuanced);
  int newrank = curr_rank - 1;
 
  int popsize = (int)_pop.size();
  for (int i = 0; i< popsize; i++) 
    for (int j = 0; j< popsize; j++) 
      {
	if ((_pop[i] != NULL) && (_pop[j] != NULL)&&(i != j))
	  {  
	    if ((_pop[i]->get_rank() == curr_rank) && 
		(_pop[j]->get_rank() <= curr_rank )) //rank now neg
	      {                                      // so..
		if (nuanced != 0)
		  {
		    if (nuanced_dominates(_pop[j],_pop[i],mask))
		      {
			_pop[i]->set_rank(newrank);
			break;
		      }
		  }
		if(pareto_dominates(_pop[j]->get_objective_vals(),
				    _pop[i]->get_objective_vals(),
				    mask))
		  {
		    _pop[i]->set_rank(newrank);
		    break; //out of j loop
		  }
	
	      }
	  }
      }
  return 0;
}

//must presume pop is pareto front only
void MOOF::do_crowding(int num_per_bin)
{
  //  printf("crowding!\n");
  vector<int> indexes;
  for (int i = 0; i < (int)_pop.size(); i++)
    {
      
      indexes.clear();
      indexes.push_back(i);
      int numsofar = 1;
      vector<float> v1 = _pop[i]->get_objective_vals(_MoMask);
      for (int j = i+1; j < (int)_pop.size(); j++)
	{
	  vector<float> v2 = _pop[j]->get_objective_vals(_MoMask);
	  
	  if (v1 == v2)
	    {
	      numsofar++;
	      indexes.push_back(j);
	    }
	}
    
      
      while ((int)indexes.size() > num_per_bin)
	{
	  
	  printf("do crowding(%d) - there are %d in this spot. removing some\n",num_per_bin,numsofar); 
	  /*
	  for (int k = 0; k < (int)v1.size(); k++) 
	    { 
	      printf("%f ",v1[k]); 
	    }
	    printf("\n",v1[i]); 
	  */
	  
	  int to_delete = int_rand_in_range((int)indexes.size());
	  if (to_delete >= (int)indexes.size())
	    {
	      printf("do crowding() bad index of indexes!\n");
	      return;
	    }

	  int index_to_delete = indexes[to_delete]; 

	  if(index_to_delete >= (int)_pop.size())
	    {
	      printf("do crowding() bad index of pop!\n");
	      return;
	    }
	  vector<MOOG *>::iterator ind = _pop.begin()+index_to_delete;
	  delete _pop[index_to_delete];
	  _pop[index_to_delete] = NULL;
	  _pop.erase(ind);


	  vector<int>::iterator ind2 = indexes.begin()+to_delete;
	  indexes.erase(ind2);

	}
    }
}

int MOOF::pareto_rank_pop(int nuanced)
{
  return rank_to_layer(_Layers+1,_MoMask,nuanced);
}

int MOOF::rank_to_layer(int layers)
{
  //printf("MOOF::rank_to_layer: %d with Mask: %d\n",layers,(int)_MoMask);
  return rank_to_layer(layers,_MoMask,0);
}

int MOOF::rank_to_layer(int layers, unsigned int mask, int nuanced)
{
  // printf("MOOF::rank_to_layer(%d), mask:%d\n",layers,mask);
  for (int i = 0; i< layers; i++)
    {
      while(pareto_rank(-1*i,mask,nuanced))
	{}
    }
  return 0;
}

//<sigh> - rank is now negative.
int MOOF::cull_below_layer(int layer)
{

  vector<MOOG *>::iterator ind;
  
  //  for (int i = 0; i< (int)_pop.size(); i++)
  for (int i = ((int)_pop.size() - 1); i>= 0; i--)
    {
      if (_pop[i]->get_rank() < layer)
	{
	  //  ind = &(_pop[i]);
	  delete _pop[i];
	  _pop[i] = NULL;
	  //	  if (*ind != _pop[i])
	  // printf("not equal!\n");
	  ind = _pop.begin()+i;
	  _pop.erase(ind);
	  //return 1;
	}
    }
  return 0;
}

//rank is now negative
void MOOF::cull_all_below__Layer()
{
  return cull_all_below_layer(_Layers);
}
void MOOF::cull_all_below_layer(int layer)
{
  //printf("cull_all - layer is: %d\n",layer);

  // printf("Size before cull: %d, %d\n", (int)_pop.size(),(int)sizeof(_pop));
  while (cull_below_layer(-1*layer))
    {}
    
  // printf("Size after cull:%d, %d\n",(int)_pop.size(),(int)sizeof(_pop));
}


void MOOF::calc_pop_distances()
{
  if (_MoMask & MOO_DIV_MASK)
    {
      float running_total;
      int popsize = (int) _pop.size();
      for (int i = 0; i< popsize; i++) //must recalc each time thru
	{
	  running_total = 0.0;
	  for (int j = 0; j< popsize; j++) //must recalc each time thru
	    {
	      running_total += (_pop[i]->distance_to_genome(_pop[j]->get_genome()));
	    }
	  running_total = running_total/_pop.size();
	  _pop[i]->set_diversity(running_total);
	}
    }
}

void MOOF::evaluate_pop()
{
  int maxit = _pop.size();
  for (int i = 0; i< maxit;i++)
    {
      //if there are two masks, AND them.
      if (_MoMask2 > 0)
	_pop[i]->evaluate_objectives((_MoMask & _MoMask2));
      else
	_pop[i]->evaluate_objectives(_MoMask);
	
    _num_P_Evals++;
    }
}

void MOOF::run()
{ 
  FILE *tempout;
  unsigned long int numiters = 0;
  int numgens = 0;
 
  time_t randseed = time(NULL);

  if (_randseed == 0)
    {
      _randseed = (unsigned int)randseed;
    }
  printf("Random Seed is: %u\n",_randseed);
  srand(_randseed);
  //  getchar();
  
  tm *tod = localtime(&randseed);
  int num_of_day = 0;

  char rootfilename[20];
  char outfilename[30];// = "pop.out";
  char bestfilename[30];

  if (_do_logging)
    {
      while (1)
	{
	  sprintf(rootfilename,"data_tmp/%d-%d.%d.%d",
		  num_of_day,
		  tod->tm_hour,
		  tod->tm_mday,
		  tod->tm_mon);
	  
	  sprintf(outfilename,"%s.gen.xml",rootfilename);
	  sprintf(bestfilename,"%s.best",rootfilename);
	  
	  tempout = fopen(outfilename,"r");
	  // if file doesn't exist, good!
	  if (!tempout)
	    {
	      break;
	    }
	  else
	num_of_day++;
	}
      
      
      printf("genfile is: %s\n",outfilename);
      printf("bestfile is: %s\n",bestfilename);
      
      char sys_cmd[100];
      sprintf(sys_cmd,"ln -s -f %s gen.latest.$HOSTNAME",outfilename);
      if ( system(sys_cmd) != 0)
	printf("Error linking data file!\n");
      sprintf(sys_cmd,"ln -s -f %s best.latest.$HOSTNAME",bestfilename);
      if ( system(sys_cmd) != 0)
	printf("Error linking data file!\n");
      
      fprint_header(outfilename,_appname,_randseed);
      fprint_config(outfilename);
      fprint_obj_titles_as_xml(outfilename);
    } 
  
  unsigned long int Iter_Limit = (_MaxGens > 0) ? _MaxGens : -1000*_MaxGens;
  
  if (_MaxGens < 0)
    printf("Using %ld Evaluations as Limit, not Generations\n",Iter_Limit);
  
  while(numiters < Iter_Limit)
    {
      reset_ranks();
      rank_to_layer(_Layers+1,_MoMask,1);
      cull_all_below_layer(_Layers);
      
      if (_Do_Crowding)
	{
	  // do this to make sure we alwasy have at least
	  // one - otherwise we'd crowd down to zero in each bin
	  do_crowding(max(1,_Crowding_Bin_Size));
	}

      /*
      else printf("MOOF.cc: not doing crowding!\n");
      */


      if ((numgens%_SampleRate) == 0)
	{
	  printf("************ generation %d [%d] (%d) ********************\n",numgens,_num_P_Evals,(int)_pop.size());
	  // printf("Number of Loci: %d\n",get_pop_mass());
	  fprint_pop_stats(outfilename,numgens);
	  fprint_best(bestfilename); 
	  //print_pop();
	}
      add_new_members();
      numgens++;
      if (_MaxGens >0 )
	numiters = numgens;
      else
	numiters = _num_P_Evals;
    }
}

int MOOF::get_pop_mass()
{
  int total = 0;
  for (int i = 0; i< (int)_pop.size(); i++)
    {
      total += _pop[i]->get_len();
    }
  return total;
}

//void sort_pop_by_ith_objective(int i)
//{
//
//
