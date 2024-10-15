
#include "TPop.h"

TPop::TPop(char *infilename, char *appname, int do_log, unsigned int randseed, int render,int direct, int maxgraphsize, Tensegrity *seed) 
{
  _render = render;
  _configfilename = infilename;
  _appname = appname;
  _do_logging = do_log;
  _randseed = randseed;
  _indexes_to_ignore.clear();
  _directEncodingSize = direct;
  _maxGraphSize = maxgraphsize;
  if (direct)
    printf("TPop::TPop - using direct size %d\n",direct);
  init(seed);
}

TPop::~TPop()
{
  
}

void TPop::init(Tensegrity *seed){
    fscan_config();
    //  init_data_files();

    _world = new ODEWorld(_render,_configfilename);

    prev_best = 0;
    _pop.clear();
    int numadded = 0;

    printf("init pop size is: %d\n",_InitPopSize);
    while (numadded < _InitPopSize){
        //MultiAssembly *as = new MultiAssembly(i+1);
        Tensegrity *ts;
        if (seed == NULL){
            ts = new Tensegrity(_maxGraphSize);
        }
        else {
            ts = new Tensegrity(seed);
            ts->findRandomStringLabels();
        }
        if (is_in_pop(ts)){
            delete ts;
        }
        else{
            _pop.push_back(ts);
            numadded ++;
        }
    }
    _debug = 0;
    evaluate_pop();
}

int TPop::delete_ith_member(int i)
{
  //  printf("TPop::delete_ith_member() - null!\n"); 
  
  if (_pop[i] != NULL)
    {
      printf("deleting %d\n",i);
      // ind = &(_pop[i]);
      delete _pop[i];
      _pop[i] = NULL;
      //      _pop.erase(ind);
      return 0;
    }
  else
    {
      printf("Pop::delete_ith_member() - null!\n"); 
      return -1;
    }
}

void TPop::add_new_members(int curgen) 
{ 
  //printf("Pop::add_new_members\n");
  int orig_pop_size = _pop.size(); 
  int p1index = 0; 
  int p2index = 0; 
  int numadded = 0;
  int NewPopLimit = 0;
  Tensegrity *p1; 
  Tensegrity *p2; 
  Tensegrity *as1;
  Tensegrity *as2;
  
  int as1mutworked = 0;
  int as2mutworked = 0;

  vector<int> roulette_wheel = make_roulette(_Fitness_Index);


  while  ((int)_pop.size() < _InitPopSize)
    {
      if (_XoverRate == -1) //this is for purely random search
	{
	  as1 =  new Tensegrity();//, //hard coded for now
	  as2 =  new Tensegrity();
	}
      else // normal xover or mutation. 
	{
	  //2 should be missing index or most relevant objective
	  //spin the wheel...
	  int p1val = int_rand_in_range((int)roulette_wheel.size());
	  //find the index
	  p1index = roulette_wheel[p1val];
	  //select the parent.
	  p1 = _pop[p1index]; 

	  //and again for p2
	  int p2val = int_rand_in_range((int)roulette_wheel.size());  
	  p2index = roulette_wheel[p2val];
	  p2 = _pop[p2index]; 
	  
	  if (binom(_XoverRate) && (p1index != p2index))
	    { 
	      int p1x1 = 0;

	      p1x1 =  int_rand_in_range(p1->lengthOfGenotype());

	      as1 = new Tensegrity(p1);
	      as1->crossover(p1,p2,p1x1);
	      
	      as2 = new Tensegrity(p2);
	      as2->crossover(p2,p1,p1x1);

	    } 
	  else // we mutate
	    {
	      as1 = new Tensegrity(p1);
	      as1mutworked = as1->mutate();
	      as2 = new Tensegrity(p2);
	      as2mutworked = as2->mutate();
	    }
	  
	} 
      
      // if as1 doesn't belong get rid of it.
      if (!is_in_pop(as1))
	{
	  evaluate_member(as1); //don't grow!
	  _pop.push_back(as1);
	  numadded++;
	}
      if (!is_in_pop(as2))
	{
	  evaluate_member(as2); // don't grow!
	  _pop.push_back(as2);
	  numadded++;
	}      
    }
}

void TPop::print_pop()
{
  int maxit = _pop.size();
      for (int i = 0; i< maxit;i++)
	{
	  _pop[i]->print_obj_vals();
	  //	  _pop[i]->printGenome();
	}
}

void TPop::fprint_pop_stats(char *outfilename,int gennum)
{
  FILE *outfile = fopen(outfilename,"a+");
  for (uint i = 0; i< _pop.size();i++)
    {
      fprintf(outfile,"%d ",gennum);
      _pop[i]->fprint_obj_vals(outfile);
    }
  fclose(outfile);
  
}

void TPop::fprint_best(char *fname)
{
  //_pop[0]->fprint(fname);
  char dotfilename[40];
  sprintf(dotfilename,"%s.net",fname);
  _pop[0]->fprintGenome(dotfilename);
  _pop[0]->printGenome();
  char stringfilename[40];
  sprintf(stringfilename,"%s.str",fname);
  _pop[0]->fprintStringLabels(stringfilename);
  //if (_render == 2)
  // {
  printf("hello!\n");
  vector<float> result = _world->evaluate(_pop[0],0,0,0);
  print_vector_of_floats(result);
  printf("olleh!\n");
  // }
  
}

void TPop::evaluate_member(Tensegrity *as)
{
  

  vector<float> result = _world->evaluate(as,0,_render,0);
  // printf("pre-read\n");
  // print_vector_of_floats(result);
  as->set_objectives(result);
  // as->printGenome();
  //as->fprintGenome("curnetwork.net");
  //  as->fscanGenome("curnetwork.net");
   // printf("post-read\n");
   // as->printGenome();
  // as->printGenome();
   // as->fprintGenome("curnetwork2.net");
   // result.clear();
   // result = _world->evaluate(as,0,_render,0);
   // print_vector_of_floats(result);
  // printf("done\n");
  //getchar();
}

void TPop::evaluate_pop()
{
  //  printf("evaluating pop\n");
  int maxit = _pop.size();
  for (int i = 0; i< maxit;i++)
    {
      evaluate_member(_pop[i]);
    }
}


//makes a roulette wheel by creating N entries 
//for every member of the pop,
// where N == surviving_children()
// the values of the roulette are indeces to the pop
// i.e. if individual 5 has 3 surviving children,
// and     individual 6 has 2 
// and     indivitual 7 has 4 then the roulette will look like
// ....555667777...
//same, but for a given index, not just num children
vector<int> TPop::make_roulette(int index)
{

  //printf("making roulette with index %d\n",index);
  vector<int> roul;
  roul.clear();

  int use_funky_roulette = 0; //funky roulette is for objectives that you're trying to minimize
  
  float min_biomass = 0.;

  for (int i = 0; i < (int)_pop.size(); i++)
    {
      //calculate the sum of fitnesses across this index

      vector<float> objs = _pop[i]->get_objective_vals();
      float roulettechunk = (int)(objs[index]*100);

      //use this for maximizing
      //make a number of roulette slots proportional to your biomass
      for (int j = 0; j < roulettechunk; j++)
	{
	  roul.push_back(i);
	}
      
    }
  return roul;
}


int TPop::fscan_config()
{
 return fscan_config(_configfilename);
}

int TPop::fscan_config(char *infilename)
{
  FILE *infile = fopen(infilename,"r");

  // the defaults
 
  _MaxGens = 1000;
  _InitPopSize = 100;
  _XoverRate = 60;
  _LSMutRate =5;
  _MutRate = 2;
  _SampleRate = 1;
  _Fitness_Index = 3; //this is the index we care about

  printf("TPop::fscan_config() - filename is %s\n",infilename);
  if (!infile)
    {
      printf("cannot open file %s for input\n",infilename);
      return -1;
    }
 
  int val; 
  char inString[80];

  while (fscanf(infile,"%s = %d;\n",&inString,&val) != EOF)
    {
      printf("I read %s and %d\n",inString,val);
      if (strcmp(inString,"_MaxGens") == 0)
		  _MaxGens = val;
      else if (strcmp(inString,"_InitPopSize") == 0)
		  _InitPopSize= val;
      else if (strcmp(inString,"_XoverRate") == 0)
		  _XoverRate= val;
      else if (strcmp(inString,"_LSMutRate") == 0)
		  _LSMutRate= val;
      else if (strcmp(inString,"_MutRate") == 0)
		  _MutRate= val;
      else if (strcmp(inString,"_SampleRate") == 0)
		  _SampleRate= val;
      else if (strcmp(inString,"_Fitness_Index") == 0)
	  _Fitness_Index = val;
      else
	printf("unrecognized input value to parse_config: %s!\n",inString);
    }

  //  getchar();
  fclose(infile);
 
  
  return 0; 
}

void TPop::run()
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
	  
	  sprintf(outfilename,"%s.gen",rootfilename);
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
      
      // fprint_header(outfilename,_appname,_randseed);
      // fprint_config(outfilename);
      // fprint_obj_titles_as_xml(outfilename);
    } 
 
  unsigned long Iter_Limit = 1000;

  while(numgens < _MaxGens)
    {

     
      //      printf("sorting pop...\n");
      // printf("size : %d\n",(int)_pop.size());
      sort_pop_by_fitness();
      //print_pop();
      // getchar();
      maintainDiversity();
      // printf("culling...\n");
      cull_bottom_half();
      // printf("size now: %d\n",(int)_pop.size());
      //      print_pop();
      //      getchar();
      
      if ((numgens%_SampleRate) == 0)
	{
	  printf("************ generation %d ********************\n",numgens);
	  // printf("Number of Loci: %d\n",get_pop_mass());
	  fprint_pop_stats(outfilename,numgens);
	  fprint_best(bestfilename); 
	  print_pop();
	}

      
      add_new_members(numgens);
      numgens++;
      numiters = numgens;
    }
}

void TPop::maintainDiversity()
{
  //delete the oldest, to maintain diversity
  printf("hello\n");
  vector<int> to_delete;
  to_delete.clear();
  for (int i = _pop.size()-1 ; i >= 0; i--)
    {
      int j;
      vector<float> vals = _pop[i]->get_objective_vals();
      float curval = vals[_Fitness_Index];
      for (j = i-5; j >= 0; j--)
	{
	  printf("maintain: i:%d j%d\n",i,j);
	  vector<float> othervals = _pop[j]->get_objective_vals();
	  float otherval = othervals[_Fitness_Index];
	  if (curval == otherval)
	    {
	      printf("%f == %f, will delete %d\n",curval,otherval,j);
	      to_delete.push_back(j);
	    }
	  else
	    break;
	}
      i = j;
    }

  printf("erasing...\n");
  for (uint i = 0 ; i < to_delete.size(); i++)
    {
      int todel = to_delete[i];
      printf("deleting %d\n",todel);
      delete _pop[todel];
      _pop[todel] = NULL;
      printf("and erasing %d\n");
      _pop.erase(_pop.begin()+todel);
      //      delete_ith_member(to_delete[i]);
    }
  printf("done erasing\n");

}

int TPop::is_in_pop(Tensegrity *ts)
{
  for (uint i = 0; i < _pop.size(); i++)
    {
      if (ts->equals(_pop[i]))
	return 1;
    }
  return 0;
}

void TPop::sort_pop_by_fitness()
{
  for (uint i = 0; i < _pop.size(); i++)
    {
      Tensegrity *curtens = _pop[i];
      vector<float> curobjs = curtens->get_objective_vals();
      float maxval = curobjs[_Fitness_Index];
      int maxindex = i;

      for  (uint j = i; j < _pop.size(); j++)
	{
	  Tensegrity *jtens = _pop[j];
	  vector<float> jobjs = jtens->get_objective_vals();
	  float curval = jobjs[_Fitness_Index];
	  if (curval > maxval)
	    {
	      maxindex = j;
	      maxval = curval;
	    }
	}

      Tensegrity *tmp = _pop[i];
      
      _pop[i] = _pop[maxindex];
      _pop[maxindex] = tmp;
      
    }
  
}

void TPop::cull_bottom_half()
{
  uint fromval = (uint)_InitPopSize/2;
  printf("culling - fromval+1 is %d, _pop.size is: %d\n",fromval+1,_pop.size());
  for (uint i = fromval+1; i < _pop.size(); i++)
    {
      delete_ith_member(i);
    }
  printf("done culling\n");
  _pop.resize(fromval);
  printf("done resizing\n");
}
