
#include "Tensegrity.h"

Tensegrity::Tensegrity(int numEls)
{
  _ls = NULL;
  numElements = numEls;
  init();
}



Tensegrity::Tensegrity(Tensegrity *from)
{
  _ls = NULL;
  //make_tr3();
  vector<int> frompairs = from->pairs();
 
  
  int oldsizeOfFrom = (int)frompairs.size()/2;
  int sizeOfFrom = from->lengthOfGenotype()/WEIGHTS_PER_NET;

  //  printf("oldsizeOfFrom: %d\n",oldsizeOfFrom);
  // printf("sizeOfFrom: %d\n",sizeOfFrom);
  numElements = sizeOfFrom;

  init(); //after init has been called, the _graph should exist

  //  printf("i'm copying a tensegrity with %d bars\n",sizeOfFrom);
  vector<float> fromweights = from->genome();
  _genome.clear();
  for (unsigned int i = 0; i < fromweights.size(); i++)
    _genome.push_back(fromweights[i]);

  _stringLabels.clear();
  
  vector<int> labs = from->getStringLabels();
 
  for (unsigned int i = 0; i < labs.size(); i++)
    {
      _stringLabels.push_back(labs[i]);
    }

  applyLabelsToStrings();
}


Tensegrity::~Tensegrity()
{
  if(_ls != NULL) 
    { 
      // printf("deleting ls\n"); 
      delete(_ls); 
      // printf("done deleting ls\n");
 }
  if(_graph)
    delete(_graph);
}




int Tensegrity::shufflePair()
{
  
  //  printf("shufflepair\n");
  int firstIndex = int_rand_in_range((int)_pairs.size());
  int secondIndex = int_rand_in_range((int)_pairs.size());
 
  if ((firstIndex == secondIndex) || 
      (firstIndex == _pairs[secondIndex]) || 
      (secondIndex == _pairs[firstIndex])) 
      
    {
      
      // printf("can't swap pairs %d and %d\n",firstIndex,secondIndex);
      //print_vector_of_ints(_pairs);
      return 0; 
    }

  // print_vector_of_ints(_pairs);
  int firstIndexMatch = _pairs[firstIndex];  
  int secondIndexMatch = _pairs[secondIndex];  

  vector<Node *> mynodes = _graph->get_nodes();  

  Node *n1a = mynodes[firstIndex];//_graph->getNodeWithNumber(_nodes[firstIndex]->num);  
  Node *n1b = mynodes[firstIndexMatch];//_graph->getNodeWithNumber(_nodes[firstIndexMatch]->num); 
  Node *n2a = mynodes[secondIndex];//_graph->getNodeWithNumber(_nodes[secondIndex]->num);
  Node *n2b = mynodes[secondIndexMatch];//_graph->getNodeWithNumber(_nodes[secondIndexMatch]->num);
 
  if ((_graph->nodes_are_joined(n1a,n2b)) ||
      (_graph->nodes_are_joined(n2a,n1b)))
    {
      //      printf("can't swap %d and %d - they're joined!\n",firstIndex,secondIndex);
      return 0;
    }
  //    printf("swapping pairs %d and %d\n",firstIndex,secondIndex); 
  _pairs[firstIndex] = secondIndexMatch; 
  _pairs[secondIndex] = firstIndexMatch;
 _pairs[firstIndexMatch] = secondIndex;
  _pairs[secondIndexMatch] = firstIndex;
  
  //  print_vector_of_ints(_pairs);
   for (unsigned int i = 0; i < _pairs.size(); i++)
     {
       if (_pairs[i] == (int)i)
	 {
	   printf("a problem!\n");
	   return 0;
 	  //  getchar();
	 }
     }
  //printf("success\n");
 
  
  return 2;
}

//pick a string at random and shuffle it.
int Tensegrity::shuffleStrings()
{
  int numedges = _graph->num_edges();
  int stringOne = int_rand_in_range(numedges);
  int stringTwo = int_rand_in_range(numedges);
  int endOne = int_rand_in_range(2);
  int endTwo = int_rand_in_range(2);

  // should check here to make sure that nothing is paired
  
  _graph->set_pairs(_pairs);
  int result = _graph->swapEdges(stringOne,endOne,stringTwo,endTwo);

}

void Tensegrity::shufflePairs()
{
  //  printf("pairs originally:\n");
  //print_vector_of_ints(_pairs);
  
  // printf("there are %d elements to shuffle\n",(int)_elements_to_shuffle.size());
  // printf("and there are %d pairs \n",(int)_pairs.size());
  for (unsigned int i = 0; i < _elements_to_shuffle.size(); i++)
    {
      
      int firstIndex = i;
      int secondIndex = _elements_to_shuffle[i];
      //      printf("swapping pairs %d and %d\n",firstIndex,secondIndex);

      int firstIndexMatch = _pairs[firstIndex];
      int secondIndexMatch = _pairs[secondIndex];

      
      _pairs[firstIndex] = secondIndexMatch;
      _pairs[secondIndex] = firstIndexMatch;
      _pairs[firstIndexMatch] = secondIndex;
      _pairs[secondIndexMatch] = firstIndex;
    }
  //printf("pairs now:\n");
  //print_vector_of_ints(_pairs);
}

void Tensegrity::init()
{
 
  _ls = new LSystem(); 
  if (numElements == 3)
    make_tr3();
  else if (numElements == 4)
    make_tr4();
  else if (numElements == 15)
    make_tr15();
  else
    printf("dont know how to make a tensegrity with %d elements\n",numElements);
  //  printf("numelements: %d\n",numElements);
  for (int i = 0; i < numElements*WEIGHTS_PER_NET; i++)
    {
      _genome.push_back((dRandReal()*2.0)-1);
    }
}

void Tensegrity::print()
{
  _graph->print_graph();
  _ls->print();
}

void Tensegrity::print_graph()
{
  _graph->print_graph();
}

void Tensegrity::print_graphviz()
{
  _graph->set_pairs(_pairs);
  _graph->print_graphviz();

}
void Tensegrity::print_lsystem()
{
  _ls->print();
}

void Tensegrity::findPairs()
{
  _pairs.clear();
  _pairs = _graph->findMatchingPairsRecursively();

}

Graph * Tensegrity::graph()
{
  return _graph;
}

LSystem * Tensegrity::lsystem()
{
  return _ls;
}

void Tensegrity::make_tr15()
{
  _graph = new Graph();
  //  printf("make tr15!\n");
  fscan("tr15.tens");
  // getchar();
  //  _ls->print();
  grow(30,0);

  //hard-code element numbers
  int elnums[] = {1 , 2 , 3 , 1 , 4 , 3 , 5 , 2 , 6 , 4 , 7 , 5 , 8 , 6 , 9 , 7 , 10 , 8 , 11 , 9 , 12 , 10 , 13 , 11 , 14 , 12 , 15 , 13 , 15 , 14 };

  vector<Node *> gnomes = _graph->get_nodes();

  for (unsigned int g = 0; g < gnomes.size(); g++)
    {
      //      printf("setting %d %d\n",g,elnums[g]);
      gnomes[g]->setElementNum(elnums[g]);
    }
}
  /*
  Node 0, elem 1
Node 1, elem 2
Node 2, elem 3
Node 3, elem 1
Node 4, elem 4
Node 5, elem 3
Node 6, elem 5
Node 7, elem 2
Node 8, elem 6
Node 9, elem 4
Node 10, elem 7
Node 11, elem 5
Node 12, elem 8
Node 13, elem 6
Node 14, elem 9
Node 15, elem 7
Node 16, elem 10
Node 17, elem 8
Node 18, elem 11
Node 19, elem 9
Node 20, elem 12
Node 21, elem 10
Node 22, elem 13
Node 23, elem 11
Node 24, elem 14
Node 25, elem 12
Node 26, elem 15
Node 27, elem 13
Node 28, elem 15
Node 29, elem 14


  */

void Tensegrity::make_tr3()
{
  
  _graph = new Graph();
  _graph->make_tr3();
  _pairs.clear();
  _pairs.push_back(3);
  _pairs.push_back(4);
  _pairs.push_back(5);
  _pairs.push_back(0);
  _pairs.push_back(1);
  _pairs.push_back(2);
  
}

void Tensegrity::make_tr4()
{
   _graph = new Graph();
   _graph->make_tr4();
   _pairs.clear();
   _pairs.push_back(4);
   _pairs.push_back(5);
   _pairs.push_back(6);
   _pairs.push_back(7);
   _pairs.push_back(0);
   _pairs.push_back(1);
   _pairs.push_back(2);
   _pairs.push_back(3);

}
vector<int> Tensegrity::pairs()
{
  return _pairs;
}

int Tensegrity::newgrow(int size)
{
  int result = _graph->growWhileConnectingRods(size, _ls, 0);

  return result;
}

void Tensegrity::simplify()
{
  _graph->simplify_graph();
}

int Tensegrity::grow(int size, int print_intermed)
{
  //  printf("Tensegrity::grow to size:%d!\n",size);
  int result  = _graph->growNodeByNodeUntilSize(size,_ls);
  int tries = 1;
  int success = 0;
  
  if (result == -1)
    {
      //   printf("Tensegrity::grow - graph stopped growing too early\n");
      success = 0;
    }
  else
    {
      // if it is too small or an odd number of nodes
      //keep trying a few more times
      if ((_graph->num_nodes() < size) || (_graph->num_nodes()%2 != 0))
	{
	  do{
	    //	    printf("Tensegrity::grow - trying again with size now:%d\n",size+tries);
	    //	    getchar();
	    _graph->growNodeByNodeUntilSize(size+tries,_ls,print_intermed);
	    tries++;
	  }while (((_graph->num_nodes() < size) || (_graph->num_nodes()%2 != 0)) && (tries < 10));
	 
	  // it is good
	  //if (_graph->num_nodes() > size+2)
	  // {
	      //  printf("too big:(%d > %d)\n",_graph->num_nodes(),size);
	  //   success = 0;
	  //   }
	  if ((_graph->num_nodes() >= size) && (_graph->num_nodes()%2 == 0))
	    success = 1;
	}
      else // it is good
	success = 1;
    }

  if (success)
    findPairs();
  return success;
}

int Tensegrity::mutate()
{
 
  if (int_rand_in_range(2) == 0)
    {
      //      printf("mutating genome\n");
      int mutloc = int_rand_in_range((int)_genome.size());
      _genome[mutloc] = dRandReal()*2.0;
      //randomly change one of the genotype weights
      return 1;
    }
  else
    {
      //printf("mutating strings\n");
      mutateStringLabels();
    }
}

vector<float> Tensegrity::genome()
{
  return _genome;
}

int Tensegrity::lengthOfGenotype()
{
  return (int)_genome.size();
}

void Tensegrity::printGenome()
{
  print_vector_of_floats(_genome);
}

void Tensegrity::fprintGenome(char *fname)
{
  //  printf("fprintGenome: there are %d weights\n",(int)_genome.size());
  FILE *ofile = fopen(fname,"w+");
  fprint_vector_of_floats(ofile,_genome);
  fclose(ofile);
}

void Tensegrity::fprintStringLabels(char *fname)
{
  FILE *ofile = fopen(fname,"w+");
  fprint_vector_of_ints(ofile,_stringLabels);
  fclose(ofile);
}

void Tensegrity::fscanStringLabels(char *filename)
{
  printf("fscan labels: %s!\n",filename);
  FILE *infile = fopen(filename,"r"); 

  // perror(NULL);

  _stringLabels.clear();
  
  int val = 0;
  while (fscanf(infile,"%d  ",&val) != EOF)
    {
      //      printf("I read: %d\n",val);
      _stringLabels.push_back(val);
    }
  fclose(infile);

  applyLabelsToStrings();
}

void Tensegrity::fprint(char *filename)
{
  _ls->fprint(filename);
}

void Tensegrity::fscanGenome(char *filename)
{
  
  printf("fscan genome: %s!\n",filename);
  FILE *infile = fopen(filename,"r"); 

  // perror(NULL);

  _genome.clear();
  
  float val = 0;
  while (fscanf(infile,"%f  ",&val) != EOF)
    {
      //      printf("I read: %f\n",val);
      _genome.push_back(val);
    }
  fclose(infile);
  //  printf("done!\n");

}

void Tensegrity::fscan(char *filename)
{
  _ls->fscan(filename);
}


//don't cross string labels
void Tensegrity::crossover(Tensegrity *p1, Tensegrity *p2, int px)
{
  _genome.clear();
  vector<float> p1gen = p1->genome();
  vector<float> p2gen = p2->genome();
  for (int i = 0; i < px; i++)
    {
      _genome.push_back(p1gen[i]);
    }
  for (int i = px; i < (int)p2gen.size(); i++)
    {
      _genome.push_back(p2gen[i]);
    }
  
}

int Tensegrity::equals(Tensegrity *ints)
{
  
  vector<float> other = ints->genome();
  
  if (_genome.size() != other.size())
    return 0;
  
  for (unsigned int i = 0; i < _genome.size(); i++)
    {
      
      if (_genome[i] != other[i])
	return 0;

    }

  vector<int> otherlabs = ints->getStringLabels();

  if (_stringLabels.size() != otherlabs.size())
    return 0;

  for (unsigned int i = 0; i < _stringLabels.size(); i++)
    {
      if (_stringLabels[i] != otherlabs[i])
	return 0;
    }
  
  return 1;
}

void Tensegrity::emptyStringLabels()
{
  _graph->emptyStringLabels();
  getStringLabelsFromGraph();
}

void Tensegrity::getStringLabelsFromGraph()
{
  _stringLabels.clear();
  vector<int> labs = _graph->getStringLabels();
  for (unsigned int i = 0; i < labs.size(); i++)
    {
      _stringLabels.push_back(labs[i]);
    }
  
}

void Tensegrity::resetStringLabels()
{
  emptyStringLabels();
  findRandomStringLabels();
}

void Tensegrity::applyLabelsToStrings()
{
  _graph->setStringLabels(_stringLabels);
}
void Tensegrity::mutateStringLabels()
{
  _graph->mutateStringLabels();
  getStringLabelsFromGraph();
}

void Tensegrity::findRandomStringLabels()
{
  _graph->findRandomStringLabels();
  getStringLabelsFromGraph();
}

void Tensegrity::printGraphRodNumbers()
{ 
  vector<Node *> gnomes = _graph->get_nodes();
  for (unsigned int g = 0; g < gnomes.size(); g++)
    {
      printf("node %d is rod number %d\n",g,gnomes[g]->getElementNum());
    }

}
void Tensegrity::printStringLabels()
{
  printf("labels :");
  for (unsigned int i = 0; i < _stringLabels.size(); i++)
    printf(" %d", _stringLabels[i]);
  getStringLabelsFromGraph();
  printf("\nlabels2:");
  for (unsigned int i = 0; i < _stringLabels.size(); i++)
    printf(" %d", _stringLabels[i]);
  printf("\n");
}
