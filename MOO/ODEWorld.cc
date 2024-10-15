/*************************************************************************
 *                                                                       *
 * Open Dynamics Engine, Copyright (C) 2001,2002 Russell L. Smith.       *
 * All rights reserved.  Email: russ@q12.org   Web: www.q12.org          *
 *                                                                       *
 * This library is free software; you can redistribute it and/or         *
 * modify it under the terms of EITHER:                                  *
 *   (1) The GNU Lesser General Public License as published by the Free  *
 *       Software Foundation; either version 2.1 of the License, or (at  *
 *       your option) any later version. The text of the GNU Lesser      *
 *       General Public License is included with this library in the     *
 *       file LICENSE.TXT.                                               *
 *   (2) The BSD-style license that is included with this library in     *
 *       the file LICENSE-BSD.TXT.                                       *
 *                                                                       *
 * This library is distributed in the hope that it will be useful,       *
 * but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files    *
 * LICENSE.TXT and LICENSE-BSD.TXT for more details.                     *
 *                                                                       *
 *************************************************************************/

#include "ODEWorld.h"

#include "odeutils.h"

#include "exec-stream.h"
#include <iostream>
#include <sstream>
// NON CLASS FUNCTIONS
// to compute angles from rotation matrix
// from the ode mailing list: http://q12.org/pipermail/ode/2004-August/013702.html
void get_euler(const dReal * matrix,dReal &kx,dReal &ky,dReal &kz)
{       
  const dReal epsilon=0.0000001;
  
  if(matrix[8] < 1-epsilon && matrix[8] > -1+epsilon){
    ky=-asin(matrix[8]);
    dReal c=cos(ky);
    kx= atan2(matrix[9]/c,matrix[10]/c);
    kz= atan2(matrix[4]/c,matrix[0]/c);
  }       
  else    
    {       
      kz=0;   
      ky=-atan2(matrix[8],0);
      kx= atan2(-matrix[6],matrix[5]);
    }       
}       

//hope that both A and P are NxN matrices.
void Warshall (int N, int *A,int *P)
{
  int i,j,k;
  
  for (i = 0; i < N; i++)
    for (j = 0; j < N; j++)
      /* There's a path if there's an edge */
      P[i*N+j] = A[i*N+j];
  for (k = 0; k < N; k++)
    for (i = 0; i < N; i++)
      for (j = 0; j < N; j++)
	if (! P[i*N+j])
	  P[i*N+j] = P[i*N+k] && P[k*N+j];
} /* Warshall */

    

static void skipWhiteSpace (char *filename, FILE *f)
{
  int c,d;
  for(;;) {
    c = fgetc(f);
    if (c==EOF) printf("unexpected end of file in \"%s\"",filename);

    // skip comments
    if (c == '#') {
      do {
	d = fgetc(f);
	if (d==EOF) printf("unexpected end of file in \"%s\"",filename);
      } while (d != '\n');
      continue;
    }

    if (c > ' ') {
      ungetc (c,f);
      return;
    }
  }
}

static int readNumber (char *filename, FILE *f)
{
  int c,n=0;
  for(;;) {
    c = fgetc(f);
    if (c==EOF) printf("unexpected end of file in \"%s\"",filename);
    if (c >= '0' && c <= '9') n = n*10 + (c - '0');
    else {
      ungetc (c,f);
      return n;
    }
  }
}

static int getHighBitIndex (unsigned int x)
{
  int i = 0;
  while (x) {
    i++;
    x >>= 1;
  }
  return i-1;
}

// shift x left by i, where i can be positive or negative
#define SHIFTL(x,i) (((i) >= 0) ? ((x) << (i)) : ((x) >> (-i)))

// start simulation - set viewpoint
void ODEWorld::initGraphics()
{
  //these from the beginning of dsPlatformSimLoop
  createMainWindow(350,288);
  glXMakeCurrent (display,win,glx_context);
  //static float xyz[3] = {0.f,-2.0f,0.7600f};
  static float hpr[3] = {75.f,9.4000f,-0.0000f};
  static float xyz[3] = {-0.8f,-2.7f,0.54f};
  dsSetViewpoint (xyz,hpr);
  dsSetTextures(1);
  dsSetShadows(0);
  dsStartGraphics(0,0);
  writeframes = 0;
  frame = 1;
  /*
  printf ("To drop another object, press:\n");
  printf ("   b for box.\n");
  printf ("   s for sphere.\n");
  printf ("   c for cylinder.\n");
  printf ("   x for a composite object.\n");
  printf ("To select an object, press space.\n");
  printf ("To disable the selected object, press d.\n");
  printf ("To enable the selected object, press e.\n");
  printf ("To toggle showing the geom AABBs, press a.\n");
  printf ("To toggle showing the contact points, press t.\n");
  printf ("To toggle dropping from random position/orientation, press r.\n");
  */
}

//char locase (char c)
//{
//  if (c >= 'A' && c <= 'Z') return c - ('a'-'A');
//  else return c;
//}


void ODEWorld::put_sphere(float x, float y, float z)
{
  MyObject newobj; 
  memset (&newobj,0,sizeof(MyObject));
  newobj.body = dBodyCreate (world);
  newobj.geom.clear();

  size_t i;
  int j,k;
  dReal sides[3];
  dMass m;
  dMatrix3 R;
  
  newobj.rc = 0.9*dRandReal();
  newobj.gc = dRandReal();
  newobj.bc = dRandReal();
  
  if ((x == 0.) && (y == 0.) && (z== 0. ))
    {
      dBodySetPosition (newobj.body,
			//dRandReal()*2-1,dRandReal()*2-1,dRandReal()+2);
			dRandReal()*2-1,dRandReal()*2-1,_ELEM_UNIT_LEN/2+_ELEM_UNIT_RAD*2);
      
      dRFromAxisAndAngle (R,dRandReal()*2.0-1.0,dRandReal()*2.0-1.0,
			  dRandReal()*2.0-1.0,dRandReal()*10.0-5.0);
      
      dBodySetRotation (newobj.body,R);
    }
  else
    {
      
      //      printf("hello\n");
      dBodySetPosition (newobj.body,
			//dRandReal()*2-1,dRandReal()*2-1,dRandReal()+2);
			x,y,z);
      
      dRFromAxisAndAngle (R,x*10,y*10,z*10,z*10);
      
      dBodySetRotation (newobj.body,R);
    }

  for (k=0; k<3; k++) sides[k] = _ELEM_UNIT_LEN;
  sides[0] = _ELEM_UNIT_RAD;
  
  dMassSetSphere(&m,DENSITY,sides[0]);

  dGeomID newGeom = dCreateSphere(space,sides[0]);

  newobj.geom.push_back(newGeom);

  for (k=0; k < (int)newobj.geom.size(); k++) {
    if (newobj.geom[k]) 
      dGeomSetBody (newobj.geom[k],newobj.body);
    else
      break;
  } 

  dBodySetMass (newobj.body,&m);

  objvect.push_back(newobj);

}
void ODEWorld::put_capsule(float x, float y, float z)
{
  MyObject newobj; 
  memset (&newobj,0,sizeof(MyObject));
  newobj.body = dBodyCreate (world);
  newobj.geom.clear();

  size_t i;
  int j,k;
  dReal sides[3];
  dMass m;
  dMatrix3 R;

  newobj.rc = 0.9*dRandReal();
  newobj.gc = dRandReal();
  newobj.bc = dRandReal();

  if ((x == 0.) && (y == 0.) && (z== 0. ))
    {
      dBodySetPosition (newobj.body,
			//dRandReal()*2-1,dRandReal()*2-1,dRandReal()+2);
			dRandReal()*2-1,dRandReal()*2-1,_ELEM_UNIT_LEN/2+_ELEM_UNIT_RAD*2);
      
      dRFromAxisAndAngle (R,dRandReal()*2.0-1.0,dRandReal()*2.0-1.0,
			  dRandReal()*2.0-1.0,dRandReal()*10.0-5.0);
      
      dBodySetRotation (newobj.body,R);
    }
  else
    {
      
      //      printf("hello\n");
      dBodySetPosition (newobj.body,
			//dRandReal()*2-1,dRandReal()*2-1,dRandReal()+2);
			x,y,z);
      
      dRFromAxisAndAngle (R,x*10,y*10,z*10,z*10);
      
      dBodySetRotation (newobj.body,R);
    }

  for (k=0; k<3; k++) sides[k] = _ELEM_UNIT_LEN;
  sides[0] = _ELEM_UNIT_RAD;

  dMassSetCapsule (&m,DENSITY,3,sides[0],sides[1]);

  dGeomID newGeom = dCreateCapsule (space,sides[0],sides[1]);

  /*
  //If you want boxes instead of caps
  sides[0] = _ELEM_UNIT_RAD;
  sides[1] = _ELEM_UNIT_RAD;
  sides[2] = _ELEM_UNIT_LEN;

  dMassSetBox(&m,DENSITY,sides[0],sides[1],sides[2]);

  dGeomID newGeom = dCreateBox(space,sides[0],sides[1],sides[2]);
  */

  newobj.geom.push_back(newGeom);

  for (k=0; k < (int)newobj.geom.size(); k++) {
    if (newobj.geom[k]) 
      dGeomSetBody (newobj.geom[k],newobj.body);
    else
      break;
  } 

  dBodySetMass (newobj.body,&m);

  objvect.push_back(newobj);
}


float ODEWorld::get_tensegrity_volume()
{

  float myvol = -1;
 try {
    //exec_stream_t es( "/usr/local/bin/qconvex FS", "" ); // run perl without any arguments 
    exec_stream_t es( "qconvex", "FS" ); // run perl without any arguments 
    es.in() << "3\n"; 
    es.in() << (int)_elements.size()*2 << "\n";
    for (uint i = 0; i < _elements.size(); i++)
      {
	Element *curel = _elements[i];
	dVector3 top;
	dVector3 bot;
	
	curel->getBotPos(bot);
	curel->getTopPos(top);
	
	es.in() << bot[0] << " " << bot[1] << " " << bot[2] << "\n";
	es.in() << top[0] <<" " << top[1] <<" " << top[2] << "\n";
      }
    es.close_in();                        // after the input was closed 
    std::string s1;
    std::string s2;

    std::getline( es.out(), s1);  // get the first line
    //std::getline( es.out(), s2);
    
    std::string something,area,vol,more; 
    es.out() >> something; // read the first word of output 
    es.out() >> area; // read the next word of output 
    es.out() >> vol; // read the next word of output 
    
    //  std::cerr << "hello!\n";
    //    std::cerr << area << "\n";
    // std::cerr << vol << "\n";

    std::istringstream i(vol);
    
    i >> myvol;
    
    //    printf("float vol is: %f\n",myvol);
 }catch( std::exception const & e ) {
   std::cerr << "error: "  <<  e.what()  <<  "\n";
 } 

 return myvol;
 
}
void ODEWorld::fprint_string_lengths(char *filename)
{
  FILE *ofile = fopen(filename,"w+");
  
  for (uint i = 0; i < _strings.size(); i++)
    {
      String *curs = _strings[i];
      Element *from = curs->fromEl();
      Element *to = curs->toEl();
      
      int fromindex = -1;
      int toindex = -1;
      for (uint e = 0; e < _elements.size(); e++)
	{
	  Element *curel = _elements[e];
	  if (curel == from)
	    fromindex = e;
	  if (curel == to)
	    toindex = e;
	}
      fprintf(ofile,"String %d from %d",i,fromindex);
     
      fprintf(ofile," %d",curs->fromSide);
      
      fprintf(ofile," to %d",toindex);
      
      fprintf(ofile," %d",curs->toSide);

      fprintf(ofile," length: %f rest: %f;\n",curs->length(),curs->getRestLength());
      
    }

  fclose(ofile);

}

 
void ODEWorld::print_string_lengths()
{
  for (uint i = 0; i < _strings.size(); i++)
    {
      String *curs = _strings[i];
      Element *from = curs->fromEl();
      Element *to = curs->toEl();
      
      int fromindex = -1;
      int toindex = -1;
      for (uint e = 0; e < _elements.size(); e++)
	{
	  Element *curel = _elements[e];
	  if (curel == from)
	    fromindex = e;
	  if (curel == to)
	    toindex = e;
	}
      printf("String %d from %d",i,fromindex);
     
      if (curs->fromSide)
	printf(" top");
      else
	printf(" bot");
      
      printf("to %d",toindex);
      if (curs->toSide)
	printf(" top");
      else
	printf(" bot");

      printf("length: %f rest: %f\n",curs->length(),curs->getRestLength());
      
    } 

  printf("float stringLenArray[] = {");
  for (uint i = 0; i < _strings.size(); i++)
    {
      String *curs = _strings[i];
      Element *from = curs->fromEl();
      Element *to = curs->toEl();
      
      int fromindex = -1;
      int toindex = -1;
      for (uint e = 0; e < _elements.size(); e++)
	{
	  Element *curel = _elements[e];
	  if (curel == from)
	    fromindex = e;
	  if (curel == to)
	    toindex = e;
	}
      printf(" %f,",curs->getRestLength()); 
      
    }
  printf("};\n"); 
}

void ODEWorld::print_capsule_ends()
{
  printVelocities();
  float vol = get_tensegrity_volume();
  printf("float vol is: %f\n",vol);

  printf("//number of points\n");
  printf("%d\n",(int)_elements.size()*2);
  printf("coordinates\n");
  for (uint i = 0; i < _elements.size(); i++)
    {
      Element *curel = _elements[i];
      dVector3 top;
      dVector3 bot;
      
      curel->getBotPos(bot);
      curel->getTopPos(top);
      
      printf("%f %f %f\n",bot[0],bot[1],bot[2]);
      printf("%f %f %f\n",top[0],top[1],top[2]);
    }

  printf("\n//number of struts\n");
  printf("%d\n",(int)_elements.size());
  printf("//indexes\n");
  for (uint i = 0; i < _elements.size(); i++)
    {
      printf("%d %d\n",2*i,2*i+1 );
    }

  printf("\n//number of cables\n");
  printf("%d\n",(int)_strings.size());
  printf("//indexes:\n");
  for (uint i = 0; i < _strings.size(); i++)
    {
      String *curstring = _strings[i];
      Element *fromEl = curstring->fromEl(); 
      int fromSide = curstring->fromSide;
      Element *toEl = curstring->toEl(); 
      int toSide = curstring->toSide;

      int indexOfFrom = 0;
      int indexOfTo = 0;
      for (uint e = 0; e < _elements.size(); e++)
	{
	  if (_elements[e] == fromEl)
	    indexOfFrom = e;
	  if (_elements[e] == toEl)
	    indexOfTo = e;
	}

      printf("%d %d\n",2*indexOfFrom+fromSide,2*indexOfTo+toSide);
      
    }

  printf("max force is: %f\n",g_maxforce);
  printf("max vel is: %f\n",g_maxvel);
  /*
  char ptsfilename[60];
  sprintf(ptsfilename,"points.txt");
  FILE *outfile = fopen(ptsfilename,"w");
  fprintf(outfile,"3\n");
  fprintf(outfile,"%d\n",(int)_elements.size()*2);
  for (uint i = 0; i < _elements.size(); i++)
    {
      Element *curel = _elements[i];
      dVector3 top;
      dVector3 bot;
      
      curel->getBotPos(bot);
      curel->getTopPos(top);
      
      fprintf(outfile,"%f %f %f\n",bot[0],bot[1],bot[2]);
      fprintf(outfile,"%f %f %f\n",top[0],top[1],top[2]);
    }
  fclose(outfile);
  */
  // char syscmd[120];
  //  sprintf(syscmd,"cat points.txt | qconvex FS > volume.txt");

  
  try {
    //exec_stream_t es( "/usr/local/bin/qconvex FS", "" ); // run perl without any arguments 
    exec_stream_t es( "qconvex", "FS" ); // run perl without any arguments 
    es.in() << "3\n";
    es.in() << (int)_elements.size()*2 << "\n";
    for (uint i = 0; i < _elements.size(); i++)
      {
	Element *curel = _elements[i];
	dVector3 top;
	dVector3 bot;
	
	curel->getBotPos(bot);
	curel->getTopPos(top);
	
	es.in() << bot[0] << " " << bot[1] << " " << bot[2] << "\n";
	es.in() << top[0] <<" " << top[1] <<" " << top[2] << "\n";
      }
    es.close_in();                        // after the input was closed 
    std::string s1;
    std::string s2;
    std::getline( es.out(), s1);
    //std::getline( es.out(), s2);
    
    std::string something,area,vol,more; 
    es.out() >> something; // read the first word of output 
    es.out() >> area; // read the first word of output 
    es.out() >> vol; // read the first word of output 
    
    //    std::cerr << "hello!\n";
    std::cerr << area << "\n";
    std::cerr << vol << "\n";

    std::istringstream i(vol);
    float myvol;
    
    i >> myvol;
    
    printf("float vol is: %f\n",myvol);
  }catch( std::exception const & e ) {
    std::cerr << "error: "  <<  e.what()  <<  "\n";
  }
 

  // if ( system(syscmd) != 0)
  //  printf("Error linking data file!\n");
  
}
// called when a key pressed
void ODEWorld::command (int cmd)
{
  char cmdstr[5];
  sprintf(cmdstr,"%c",cmd);
  switch(cmd)
    {
    case 'q':
      _run_interactive = 0;
      break;
    case '<':
      rotate_turtle(ROT_MINUS_90);
      break;
    case '>':
      rotate_turtle(ROT_PLUS_90);
      break;
    case 'r':
      resetTens();
      break;
    case 'a':
      activate_string(1);
      break;
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
      printf("cur step: %d\n",cur_step);
      activate_string(atoi(cmdstr));
      break;

      
    case 'n':
      _use_networks = !_use_networks;
      if (!_use_networks)
	{
	  for (uint i = 0; i < _strings.size(); i++)
	    _strings[i]->deactivate();
	}
      break;
    case 'b':
      _collisions_enabled = 1;
      dBodyEnable(_elements[0]->_body);
      dWorldSetGravity(world,_Horizontal_Gravity_Vector,0,-0.1); 
      break;
    case 'p':
      //      fill_grid();
      // print_grid();
      // fprint_grid("outworld.world");
      //print_capsule_ends();
      print_string_lengths();
      fprint_string_lengths("lens.string");
      //print_current_tens();
      break;
    case 's':
      //      try_to_put_brick(4);
      _stopFlag = !_stopFlag;
      printf("stopflag now: %d\n",_stopFlag);
      break;
    case 'l':
      printf("printlens!\n");
      _printlens = !_printlens;
      break;
    case 'c':
      //fill_grid();
      //compare_to_goal();
      //calc_grid_empty_fill();
      centerOfMass(_currCOM);
      printf("COM: %f,%f,%f\n",_currCOM[0],_currCOM[1],_currCOM[2]);
      _drawCOM = !_drawCOM;
      break;
    case 'o':
      _enable_ODE = !_enable_ODE;
      break;
    case 'w':
      describeWorld();
      break;
    case 'd':
      float dt = distanceTraveled(_currCOM);
      printf("distance:%f\n",dt);
      break;
      /*
	case 'z':
	centerOfMass(_currCOM);
	printf("COM: %f,%f,%f\n",_currCOM[0],_currCOM[1],_currCOM[2]);
	break;
      */
    }
}


// draw a geom

void ODEWorld::drawGeom (dGeomID g, const dReal *pos, const dReal *R, int show_aabb)
{
  int i;
  
  if (!g) return;
  if (!pos) pos = dGeomGetPosition (g);
  if (!R) R = dGeomGetRotation (g);
  
  int type = dGeomGetClass (g);
  if (type == dBoxClass) {
    dVector3 sides;
    dGeomBoxGetLengths (g,sides);
    dsDrawBox (pos,R,sides);
  }
  else if (type == dSphereClass) {
    dsDrawSphere (pos,R,dGeomSphereGetRadius (g));
  }
  else if (type == dCapsuleClass) {
    dReal radius,length;
    dGeomCapsuleGetParams (g,&radius,&length);
    dsDrawCapsule (pos,R,length,radius);
  }
  /*
  else if (type == dCCylinderClass) {
    dReal radius,length;
    dGeomCCylinderGetParams (g,&radius,&length);
    dsDrawCappedCylinder (pos,R,length,radius);
  }
  // cylinder option not yet implemented
  else if (type == dCylinderClass) {
    dReal radius,length;
    dGeomCylinderGetParams (g,&radius,&length);
    dsDrawCylinder (pos,R,length,radius);
  }
*/
  else if (type == dGeomTransformClass) {
    dGeomID g2 = dGeomTransformGetGeom (g);
    const dReal *pos2 = dGeomGetPosition (g2);
    const dReal *R2 = dGeomGetRotation (g2);
    dVector3 actual_pos;
    dMatrix3 actual_R;
    dMULTIPLY0_331 (actual_pos,R,pos2);
    actual_pos[0] += pos[0];
    actual_pos[1] += pos[1];
    actual_pos[2] += pos[2];
    dMULTIPLY0_333 (actual_R,R,R2);
    drawGeom (g2,actual_pos,actual_R,0);
  }

  if (show_aabb) {
    // draw the bounding box for this geom
    dReal aabb[6];
    dGeomGetAABB (g,aabb);
    dVector3 bbpos;
    for (i=0; i<3; i++) bbpos[i] = 0.5*(aabb[i*2] + aabb[i*2+1]);
    dVector3 bbsides;
    for (i=0; i<3; i++) bbsides[i] = aabb[i*2+1] - aabb[i*2];
    dMatrix3 RI;
    dRSetIdentity (RI);
    dsSetColorAlpha (1,0,0,0.5);
    dsDrawBox (bbpos,RI,bbsides);
  }
}

// BEGIN ODEWORLD CLASS DEFINITION

ODEWorld::ODEWorld(int do_render,char *configfilename)
{
  init(do_render,configfilename);
}

ODEWorld::ODEWorld(int do_render)
{
  init(do_render,"block.cfg");
}

ODEWorld::ODEWorld()
{
  init(1,"block.cfg");
}

ODEWorld::~ODEWorld()
{
  remove_all_objects();
  dJointGroupDestroy(contactgroup);
  dSpaceDestroy(space);
  dWorldDestroy(world);
}

void ODEWorld::disableObject(MyObject inobj)
{
  dBodyDisable(inobj.body);
  for (int i = 0; i < (int)inobj.geom.size(); i++)
    {
      if (inobj.geom[i] != NULL)
	{
	  disableGeom(inobj.geom[i]);
	}
    }
}

void ODEWorld::disableGeom(dGeomID ingeom)
{
  if (dGeomGetClass(ingeom) == dGeomTransformClass)
    {
      disableGeom(dGeomTransformGetGeom(ingeom));
      dGeomDisable(ingeom);
       
    }
  else if (dGeomGetClass(ingeom) == dBoxClass)
    {
      dGeomDisable(ingeom);
    }
}

void ODEWorld::remove_all_objects()
{
  dJointGroupEmpty (contactgroup);
  // printf("Removing all objects!\n");
  for (int k = 0; k < (int)objvect.size();k++)
    {
      int is_module = 0; 
      //don't wanna destroy things if they're mondules
      dBodyID curbody = objvect[k].body;

      dBodyDestroy (objvect[k].body);
      int l;
      for (l=0; l < (int)objvect[k].geom.size(); l++) 
	{
	  if (objvect[k].geom[l]) 
	    {
	      dSpaceRemove(space,objvect[k].geom[l]);
	      dGeomDestroy (objvect[k].geom[l]);
	    }
	  else
	    break;
	} 
    } 
  objvect.clear();
  
}

//void ODEWorld::init(int argc, char **argv)
void ODEWorld::init(int do_render, char *configfilename)
{
 
  //  _Fitness_Mode = 0;

  cur_tens = NULL;
  _printlens = 0;
  _drawCOM = 0;
  _use_networks = 0;
  fscan_config(configfilename);
  render = do_render;
  if (render)
    initGraphics();
  globalself = this;

  test_foo = 1234;
  //render = 0;
  objvect.clear();
  modvect.clear();
  allModules.clear();
  xpos = 0;
  ypos = 0;
  zpos = BRICK_WIDTH/2;
  epsilon = .0;
  offset = 0;
  orientation = 0;
  cur_step = 0;
  cur_cmd_index = 0;
  done_evaluating = 1;
  create_world();
  //  create_turtle();
  create_markers();


  _collisions_enabled = 0;
  // _Margin = 1;

}

//return 1 of any objects are moving with linear or angular velocity above 
// the MIN_VEL threshold.
dReal ODEWorld::objects_are_moving()
{
  dReal maxvel = 0.;
  
  for (int i = 0; i < (int)objvect.size(); i++)
    {
      const dReal *vel = dBodyGetLinearVel(objvect[i].body);
      const dReal *avel = dBodyGetAngularVel(objvect[i].body);
      for (int j = 0; j <3; j++)
	{
	 
	  //if (render) printf("vel: %f, avel: %f\n",vel[j],avel[j]);
	  maxvel = (fabs(vel[j]) > maxvel) ? fabs(vel[j]) : maxvel;
	  maxvel = (fabs(avel[j]) > maxvel) ? fabs(avel[j]) : maxvel;
	  //maxvel = max(fabs(vel[j]),maxvel);
	  //maxvel = max(fabs(avel[j]),maxvel);

	  //if ((fabs(vel[j]) > MIN_VEL) || (fabs(avel[j]) > MIN_VEL))
	  // if (maxvel > MIN_VEL)
	  //  return maxvel;
	}
    }
  //  if (render) printf("maxvel: %f\n",maxvel);
  return maxvel ;
}


void ODEWorld::gather_data()
{
  dReal max_x = -1000.;
  dReal min_x = 1000.;
  dReal max_z = -1.;
  for (int i = 0; i < (int)objvect.size(); i++)
    {
      const dReal *pos = dBodyGetPosition (objvect[i].body);
      max_x = max(pos[0],max_x);
      min_x = min(pos[0],min_x);
      max_z = max(pos[2],max_z);
    }
  printf("max_x: %f, min_x: %f, max_z: %f\n",max_x,min_x,max_z);
}


dGeomID ODEWorld::copyGeom(dGeomID ingeom)
{
  dGeomID copiedGeom = NULL;

  if (dGeomGetClass(ingeom) == dGeomTransformClass)
    {
      copiedGeom = dCreateGeomTransform(space);
      dGeomTransformSetCleanup (copiedGeom,1);

      dGeomID subGeom = copyGeom(dGeomTransformGetGeom(ingeom));
      dGeomTransformSetGeom (copiedGeom, subGeom);
      // the manual says that if T is a transform encapsulating geom E
      // then E should NOT be inserted into the space.
      // we fix this by removing it after copying
      // indeed - this resolves most weird problems i had
      dSpaceRemove(space,subGeom);
      
       
    }
  else if (dGeomGetClass(ingeom) == dBoxClass)
    {
      // printf("ODEWorld::copyGeom -- copying box\n");
      dVector3 sides;
      dGeomBoxGetLengths(ingeom, sides);
      copiedGeom = dCreateBox(space,sides[0],sides[1],sides[2]);

      const dReal *r2 = dGeomGetRotation(ingeom);
      dGeomSetRotation(copiedGeom,r2);
      const dReal *inpos = dGeomGetPosition (ingeom);
      dGeomSetPosition(copiedGeom,inpos[0],inpos[1],inpos[2]);
    }
  else
    {
      printf("ODEWorld::copyGeom - I don't know how to copy geom class %d\n",dGeomGetClass(ingeom));
    }
 

  
  return copiedGeom;
  
}

void ODEWorld::describeWorld()
{
  printf("describing world objects:\n");
  //for (int i = 0; i < (int)objvect.size(); i++)
  //   describeObject(objvect[i]);
  printf("float posArray[] = {");
  for (int i = 0; i < (int)objvect.size(); i++)
      describeObjectPos(objvect[i]);
  printf("}\n");
  printf("float rotArray[] = {");
  for (int i = 0; i < (int)objvect.size(); i++)
      describeObjectRot(objvect[i]);
  printf("}\n");
  /*
  printf("describing world modules:\n");
  for (int i = 0; i < (int)modvect.size(); i++)
      describeObject(modvect[i]);
  */
  

}

void ODEWorld::describeObjectPos(MyObject inobj)
{
  
  const dReal *pos1 = dBodyGetPosition (inobj.body);
  printf("%f,%f,%f,\n",pos1[0],pos1[1],pos1[2]);

}
void ODEWorld::describeObjectRot(MyObject inobj)
{
  
  const dReal *r2 = dBodyGetQuaternion(inobj.body);
  printf("%f,%f,%f,%f,\n", r2[0], r2[1], r2[2], r2[3]);

}

void ODEWorld::describeObject(MyObject inobj)
{
  
  const dReal *pos1 = dBodyGetPosition (inobj.body);
  printf("pos of body %d: (%f,%f,%f)\n",inobj.body,pos1[0],pos1[1],pos1[2]);

  const dReal *r2 = dBodyGetQuaternion(inobj.body);
  printf("rot of body: %d\n (%f %f %f %f)\n", inobj.body,
	 r2[0],
	 r2[1],
	 r2[2],
	 r2[3]);

  dMass m;
  dBodyGetMass(inobj.body,&m);
  //  printf("pos of mass (%f) of body %d: (%f,%f,%f)\n",m.mass,inobj.body,m.c[0],m.c[1],m.c[2]);
  //printf("tensor of mass: (%f,%f,%f,%f,%f,%f,%f,%f,%f)\n",m.I[0],m.I[1],m.I[2],m.I[3],m.I[4],m.I[5],m.I[6],m.I[7],m.I[8]);

  for (int i = 0; i < (int)inobj.geom.size(); i++)
    {
      if (inobj.geom[i] != NULL)
	{
	  printf("geom %d of body %d:",i,inobj.body);
	  describeGeom(inobj.geom[i],inobj);
	}
    }
}

void ODEWorld::describeGeom(dGeomID ingeom, MyObject inobj)
{
   if (dGeomGetClass(ingeom) == dGeomTransformClass)
    {
      printf("is transform geom %d ",ingeom);
      const dReal *inpos = dGeomGetPosition (ingeom);
      //printf("with rel pos: (%f,%f,%f)\n",inpos[0],inpos[1],inpos[2]);
      // printf("whose encapsed geom :");
      //  describeGeom(dGeomTransformGetGeom(ingeom),inobj);
      dVector3 actualPos;
      inpos = dGeomGetPosition(dGeomTransformGetGeom(ingeom));
      dBodyGetRelPointPos(inobj.body,inpos[0],inpos[1],inpos[2],actualPos);
      printf("with actual pos: (%f,%f,%f)\n",actualPos[0],actualPos[1],actualPos[2]);
      
    }
  else if (dGeomGetClass(ingeom) == dBoxClass)
    {
      printf("is box geom %d\n ",ingeom);
      const dReal *inpos = dGeomGetPosition (ingeom);
      printf("with pos: (%f,%f,%f)\n",inpos[0],inpos[1],inpos[2]);
      
    }
}


void ODEWorld::resetTens()
{
  if (_elements.size() == 15)
    resetT15();
  else if (_elements.size() == 3)
    resetT3();
  else
    resetT4();

}
void ODEWorld::resetT15()
{
float posArray[] = {0.370806,0.734671,1.239010,
1.046146,0.168744,1.454565,
0.600465,0.531592,1.052136,
1.415586,0.481310,0.779344,
1.835244,0.504894,1.249013,
2.228727,0.073308,0.919149,
2.518035,0.666757,0.774735,
2.969034,0.160290,0.980368,
3.307302,0.481153,0.560717,
3.734122,0.643715,0.934848,
4.137555,0.193420,0.649916,
4.686946,0.693471,0.500907,
5.137127,0.635191,0.740039,
5.450599,0.600656,0.451898,
		    5.832082,0.928574,0.547618 };

float rotArray[] = {0.715447,0.180996,-0.021786,0.674464,
0.451862,0.572758,0.057376,0.681526,
0.939954,-0.111070,0.300814,-0.116881,
-0.005565,0.754780,0.462333,0.465322,
0.797650,-0.085240,0.543381,-0.247438,
0.373271,0.627362,0.669450,0.137558,
-0.027621,0.622825,0.364329,0.691802,
-0.509451,-0.712184,-0.154562,-0.457564,
-0.518094,0.646871,-0.523425,0.197897,
0.123592,-0.514219,0.063518,-0.846327,
-0.128279,-0.851964,0.061029,-0.503961,
0.462209,-0.643381,0.175478,-0.584492,
0.416483,0.656930,0.303496,0.550340,
0.581378,-0.254170,0.763188,0.122234,
		    -0.784703,0.245436,-0.518598,0.234647 };


  //  printf("reset T15!\n");
//float posArray[] = {0.500000,0.500000,1.400000, 1.192617,0.203555,1.347559, 0.551026,0.344277,1.245834, 1.454830,0.530861,0.725443, 1.913313,0.690340,1.153161, 2.387182,0.292478,0.648627, 2.695339,0.856348,0.760033, 3.132980,0.356499,1.102586, 3.583817,0.653464,0.613952, 3.841089,0.734489,1.258856, 4.334470,0.223868,0.942998, 4.854397,0.703560,1.167942, 5.196823,0.665426,1.512765, 5.534090,0.569406,1.374975, 5.928133,0.818168,1.594357 };

//float rotArray[] = {0.753902,0.209440,0.209440,0.586431, 0.490054,-0.354390,0.625013,-0.493571, -0.327841,0.511388,-0.004966,0.794341, -0.380617,0.705242,-0.596436,0.045052, 0.109415,0.567362,0.147813,0.802670, 0.077347,0.931602,-0.094279,0.342413, -0.481891,-0.187832,-0.635650,-0.573106, -0.762996,-0.445509,-0.452514,-0.120784, 0.473849,0.123720,0.800380,0.345764, -0.051974,0.368154,-0.048153,0.927061, -0.511861,0.040959,-0.815054,0.268343, -0.846057,0.108926,-0.503878,-0.135759, 0.291876,-0.443219,0.368999,-0.763024, 0.670494,-0.488617,0.506614,-0.234592, -0.116229,-0.404317,-0.132711,-0.897445};

//float stringLenArray[] = { 1.200000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 0.900000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.100000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.100000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.100000, 1.000000, 1.000000, 0.900000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.100000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.100000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.100000, 1.000000, 1.100000, 1.100000, 0.900000, 1.000000, 1.000000, 1.000000};

float stringLenArray[] = { 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.100000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000, 1.000000};


  /*
float posArray[] = {2.115788,0.335339,0.568919,
1.583725,-0.104987,0.288688,
1.936641,0.420264,0.456604,
0.994386,-0.196500,0.490777,
0.691019,-0.691197,0.722328,
0.260660,-0.967368,0.564361,
-0.285295,-1.180552,0.937776,
-0.648877,-1.691613,0.962180,
-1.225385,-1.717138,0.903513,
-1.837510,-2.119426,1.207884,
-2.211310,-2.474411,0.924140,
-2.564089,-3.068701,0.576973,
-2.896726,-3.417859,0.997763,
-3.304702,-3.608099,0.626107,
-3.318582,-4.168811,0.415418
};

float rotArray[] = {0.520303,0.811230,0.191461,0.185831,
0.688332,0.543375,-0.428090,-0.218362,
0.668863,0.649936,-0.240028,0.269430,
0.320489,-0.353333,-0.460268,0.748729,
0.522649,0.037252,-0.669101,0.527023,
0.331551,-0.421265,-0.536551,0.651707,
-0.469522,0.180587,0.523810,-0.687430,
-0.446227,-0.703293,0.035220,0.552287,
-0.530877,0.274184,0.595770,-0.536704,
-0.116437,0.473134,0.453764,-0.746113,
-0.475122,0.188938,0.744712,-0.428912,
0.497064,0.766804,0.121087,-0.387656,
0.277797,0.586455,0.445411,-0.616853,
-0.201329,0.156457,0.833773,-0.489704,
-0.177594,-0.157021,0.586076,-0.774803
};
*/
/*
  float posArray[] = { 1.136333,0.108376,1.277657,
		       0.840117,-0.870385,1.152206,
		       0.900578,-0.189285,1.126509,
		       0.125849,-0.718208,0.926789,
		       0.010181,-1.170146,1.535495,
		       -0.261659,-1.485432,0.886221,
		       -0.889339,-1.364833,1.285771,
		       -0.937866,-2.023055,1.115332,
		       -1.555283,-1.888600,0.871532,
		       -1.813877,-2.316199,1.316459,
		       -2.137361,-2.432540,0.783254,
		       -2.369857,-3.100009,0.739655,
		       -2.829873,-3.252366,0.976473,
		       -2.861639,-3.603222,0.561968,
		       -2.955992,-4.115821,0.667014 };
  float rotArray[] = { 0.660155,0.564329,0.164146,0.467744,
		       0.722004,0.626255,-0.293637,0.017096,
		       0.919916,0.265233,0.043710,0.285473,
		       -0.421574,-0.554245,0.453858,0.555968,
		       0.717449,0.318157,-0.428239,0.447945,
		       -0.063803,-0.875627,-0.246744,0.410274,
		       -0.565456,0.293856,0.440938,-0.632046,
		       -0.413513,-0.802395,-0.212975,0.373913,
		       -0.030620,0.742416,0.054421,-0.667023,
		       -0.280520,0.113474,0.575930,-0.759432,
		       -0.392232,0.278517,0.840903,-0.247923,
		       0.756499,0.592908,-0.264507,-0.078769,
		       0.280895,0.720603,0.251686,-0.581793,
		       0.136948,0.446963,0.701933,-0.537363,
		       0.507832,0.615650,0.153967,-0.582560};

*/


// printf("USING HEIGHT HACK FIX!\n");
  for (uint i = 0; i < _elements.size(); i++)
    {
      int pos = i*3;
      int rot = i*4;
      
      const dQuaternion RR ={rotArray[rot],rotArray[rot+1],rotArray[rot+2],rotArray[rot+3]};
      
      dBodySetPosition(_elements[i]->body(),posArray[pos],posArray[pos+1],posArray[pos+2]+0.1);
      dBodySetQuaternion(_elements[i]->body(),RR);
    }
 
  //float stringLenArray[] = { 1.500000, 1.000000, 1.100000, 1.300000, 1.200000, 0.700000, 1.100000, 0.800000, 0.800000, 1.000000, 0.700000, 0.700000, 1.200000, 1.100000, 1.400000, 1.400000, 0.800000, 1.100000, 1.500000, 1.100000, 1.300000, 1.000000, 1.100000, 1.300000, 1.200000, 1.000000, 0.900000, 1.100000, 1.000000, 1.100000, 0.800000, 0.800000, 1.200000, 1.100000, 1.100000, 1.100000, 1.000000, 1.100000, 1.100000, 1.200000, 1.100000, 0.900000, 1.300000, 1.000000, 1.100000, 1.000000, 1.300000, 1.100000, 1.200000, 1.400000, 1.100000, 0.900000, 0.900000, 1.000000, 1.200000, 1.000000, 0.900000, 0.900000, 1.300000, 1.000000, 1.100000, 0.800000, 1.100000, 1.300000, 1.100000, 1.100000, 1.000000, 1.000000, 0.800000, 1.300000, 1.100000, 1.200000, 1.000000, 1.000000, 0.900000, 1.000000, 0.900000, 1.200000};
/*
  float stringLenArray[] = { 1.500, 1.000, 1.100, 1.300, 1.200, 0.700, 1.100, 0.8, 0.8, 1.0, 0.7, 0.7, 1.2, 1.1, 1.4, 1.4, 0.8, 1.1, 1.5, 1.1, 1.2, 1.0, 1.1, 1.3, 1.2, 1.0, 0.9, 1.1, 1.0, 1.1, 0.8, 0.8, 1.2, 1.1, 1.1, 1.1, 1.0, 1.1, 1.1, 1.2, 1.1, 0.9, 1.3, 1.0, 1.1, 1.0, 1.3, 1.1, 1.2, 1.4, 1.1, 0.9, 0.9, 1.0, 1.2, 1.0, 0.9, 0.9, 1.3, 0.9, 1.1, 0.9, 1.1, 1.3, 1.1, 1.1, 1.0, 1.0, 0.8, 1.3, 1.1, 1.2, 1.0, 1.0, 0.9, 1.0, 1.0, 1.200000 };
*/

  for (uint i = 0; i < _strings.size() ; i++)
    {
      _strings[i]->setRestLength(stringLenArray[i]);
    }
  //describeWorld();


/* 
  float stringLenArray = {


String 0 from 0 botto 1 botlength: 1.246888 rest: 0.900000
String 1 from 1 botto 2 botlength: 1.229885 rest: 1.000000
String 2 from 1 botto 0 toplength: 1.219049 rest: 1.000000
String 3 from 2 botto 3 botlength: 1.582963 rest: 1.000000
String 4 from 3 botto 0 toplength: 0.999926 rest: 1.000000
String 5 from 0 topto 2 toplength: 0.999383 rest: 1.000000
String 6 from 2 topto 4 botlength: 1.492172 rest: 1.100000
String 7 from 4 botto 0 botlength: 0.999937 rest: 1.000000
String 8 from 3 botto 1 toplength: 1.564390 rest: 1.000000
String 9 from 1 botto 3 botlength: 1.000018 rest: 1.000000
String 10 from 1 topto 5 botlength: 1.000043 rest: 1.000000
String 11 from 5 botto 2 toplength: 0.863303 rest: 1.000000
String 12 from 4 botto 3 toplength: 1.569203 rest: 1.000000
String 13 from 1 botto 4 botlength: 0.999806 rest: 1.000000
String 14 from 4 botto 2 botlength: 1.080705 rest: 1.000000
String 15 from 3 botto 4 botlength: 1.099236 rest: 1.100000
String 16 from 0 botto 2 botlength: 0.999824 rest: 1.000000
String 17 from 3 topto 6 botlength: 1.305355 rest: 1.000000
String 18 from 6 botto 1 toplength: 1.000226 rest: 1.000000
String 19 from 5 botto 4 toplength: 1.478472 rest: 1.000000
String 20 from 3 botto 2 toplength: 1.203610 rest: 1.000000
String 21 from 1 botto 2 toplength: 1.439517 rest: 1.000000
String 22 from 5 botto 4 botlength: 1.160892 rest: 1.000000
String 23 from 5 botto 3 botlength: 1.100117 rest: 1.100000
String 24 from 4 topto 7 botlength: 1.241912 rest: 1.000000
String 25 from 7 botto 3 toplength: 0.999909 rest: 1.000000
String 26 from 6 botto 5 toplength: 1.602052 rest: 1.000000
String 27 from 3 botto 6 botlength: 1.232102 rest: 1.000000
String 28 from 6 botto 5 botlength: 1.000032 rest: 1.000000
String 29 from 6 botto 4 botlength: 0.999839 rest: 1.000000
String 30 from 5 topto 8 botlength: 1.154900 rest: 1.000000
String 31 from 8 botto 4 toplength: 1.100071 rest: 1.100000
String 32 from 7 botto 6 toplength: 1.638550 rest: 1.000000
String 33 from 4 botto 7 botlength: 1.327680 rest: 1.000000
String 34 from 7 botto 6 botlength: 0.999374 rest: 1.000000
String 35 from 7 botto 5 botlength: 0.999518 rest: 1.000000
String 36 from 6 topto 9 botlength: 1.163089 rest: 1.000000
String 37 from 9 botto 5 toplength: 0.999905 rest: 1.000000
String 38 from 8 botto 7 toplength: 1.618768 rest: 1.000000
String 39 from 5 botto 8 botlength: 1.365868 rest: 1.000000
String 40 from 8 botto 7 botlength: 0.999916 rest: 1.000000
String 41 from 8 botto 6 botlength: 0.952354 rest: 1.000000
String 42 from 7 topto 10 botlength: 1.175370 rest: 1.000000
String 43 from 10 botto 6 toplength: 1.000018 rest: 1.000000
String 44 from 9 botto 8 toplength: 1.647254 rest: 1.000000
String 45 from 6 botto 9 botlength: 1.406969 rest: 1.000000
String 46 from 9 botto 8 botlength: 1.000025 rest: 1.000000
String 47 from 9 botto 7 botlength: 1.054551 rest: 1.100000
String 48 from 8 topto 11 botlength: 1.001090 rest: 1.000000
String 49 from 11 botto 7 toplength: 1.000266 rest: 1.000000
String 50 from 10 botto 9 toplength: 1.660446 rest: 1.000000
String 51 from 7 botto 10 botlength: 1.386316 rest: 1.000000
String 52 from 10 botto 9 botlength: 1.000030 rest: 1.000000
String 53 from 10 botto 8 botlength: 0.999826 rest: 1.000000
String 54 from 9 topto 12 botlength: 1.099807 rest: 1.000000
String 55 from 12 botto 8 toplength: 0.999925 rest: 1.000000
String 56 from 11 botto 10 toplength: 1.516793 rest: 1.000000
String 57 from 8 botto 11 botlength: 1.499184 rest: 1.000000
String 58 from 11 botto 10 botlength: 1.099553 rest: 1.100000
String 59 from 11 botto 9 botlength: 0.999701 rest: 1.000000
String 60 from 10 topto 13 botlength: 1.097453 rest: 1.000000
String 61 from 13 botto 9 toplength: 1.000069 rest: 1.000000
String 62 from 12 botto 11 toplength: 1.982019 rest: 1.100000
String 63 from 9 botto 12 botlength: 1.339522 rest: 1.000000
String 64 from 12 botto 11 botlength: 0.739968 rest: 1.000000
String 65 from 12 botto 10 botlength: 0.890816 rest: 1.000000
String 66 from 11 topto 14 botlength: 1.286991 rest: 1.000000
String 67 from 14 botto 10 toplength: 1.000027 rest: 1.000000
String 68 from 13 botto 12 toplength: 1.936885 rest: 1.000000
String 69 from 10 botto 13 botlength: 1.450904 rest: 1.000000
String 70 from 13 botto 12 botlength: 0.560997 rest: 1.000000
String 71 from 13 botto 11 botlength: 0.999294 rest: 1.000000
String 72 from 12 topto 14 toplength: 0.999975 rest: 1.000000
String 73 from 14 topto 11 toplength: 1.164722 rest: 1.000000
String 74 from 14 botto 13 toplength: 1.797637 rest: 1.000000
String 75 from 13 topto 12 toplength: 0.998612 rest: 1.000000
String 76 from 11 botto 14 botlength: 1.195879 rest: 1.000000
String 77 from 14 topto 13 toplength: 0.999902 rest: 1.000000 
  */

}

void ODEWorld::resetT4()
{
  //  printf("resetting T4!\n");
  dBodyID body1 = _elements[0]->body();
  //dBodySetPosition(body1,0.939906,0.321858,1.029813);
  dBodySetPosition(body1,0.859966,-0.077732,0.518160);
  const dQuaternion r2 ={0.525200, 0.510302, 0.283533, 0.619166};
  dBodySetQuaternion(body1,r2);

  body1 = _elements[1]->body();
  dBodySetPosition(body1,0.768794,0.241168,0.504734);
  const dQuaternion r3 ={0.808364,0.292772,-0.495664,0.123086};
  dBodySetQuaternion(body1,r3);

  body1 = _elements[2]->body();
  //dBodySetPosition(body1,0.896067,0.069335,1.098616);
  dBodySetPosition(body1,1.250375,0.379300,0.517653);
  const dQuaternion r4 ={-0.637062,-0.269559,0.503391,0.517772};
  dBodySetQuaternion(body1,r4);

  body1 = _elements[3]->body();
  dBodySetPosition(body1,1.341462,0.060765,0.494933);
  const dQuaternion r5 ={0.340579,0.260767,0.517145,0.740654};
  dBodySetQuaternion(body1,r5);

  /*
pos of body 6394848: (0.859966,-0.077732,0.518160)
rot of body: 6394848
 (0.525200 0.510302 0.283533 0.619166)
 (-0.000161 -0.000217 -0.001085 0.000000)
 (-0.002598 -0.001174 0.004332 0.000000)
 (0.000000 0.000000 0.000000 0.000000)
geom 0 of body 6394848:
pos of body 6395616: (0.768794,0.241168,0.504734)
rot of body: 6395616
 (0.808364 0.292772 -0.495664 0.123086)
 (-0.001977 -0.001027 -0.001683 0.000000)
 (0.016089 0.012842 0.007104 0.000000)
 (0.000000 0.000000 0.000000 0.000000)
geom 0 of body 6395616:
pos of body 6396432: (1.250375,0.379300,0.517653)
rot of body: 6396432
 (-0.637062 -0.269559 0.503391 0.517772)
 (-0.001844 0.002223 -0.001137 0.000000)
 (-0.014106 0.009534 0.004718 0.000000)
 (0.000000 0.000000 0.000000 0.000000)
geom 0 of body 6396432:
pos of body 6397168: (1.341462,0.060765,0.494933)
rot of body: 6397168
 (0.340579 0.260767 0.517145 0.740654)
 (-0.000651 0.001089 0.000176 0.000000)
 (0.000822 0.001093 0.001687 0.000000)
 (0.000000 0.000000 0.000000 0.000000)
  */
}
void ODEWorld::resetT3()
{
  /*
  printf("there are %d objects in vect\n",objvect.size());
  printf("red body is %d\n",objvect[0].body);
  printf("g body is %d\n",objvect[1].body);
  printf("b body is %d\n",objvect[2].body);
  */
  objvect[0].rc = 0.2;
  objvect[0].gc =  0.2;
  objvect[0].bc = 0.2;
  objvect[1].rc =  0.2;
  objvect[1].gc =  0.2;
  objvect[1].bc = 0.2;
  objvect[2].rc = 0.2;
  objvect[2].gc = 0.2;
  objvect[2].bc = 0.2;
  
  /*
pos of body 22124688: (0.939906,0.321858,0.529813)
rot of body: 22124688
 (0.689774 0.415068 0.385473 0.450934)
 (0.004134 0.002377 -0.003443 0.000000)
 (-0.047379 0.018451 -0.020908 0.000000)
 (0.000000 0.000000 0.000000 0.000000)
geom 0 of body 22124688:pos of body 22125440: (0.886324,0.141222,0.344122)
rot of body: 22125440
 (0.626866 -0.191145 0.751742 0.073397)
 (-0.009823 0.004678 0.002710 0.000000)
 (0.004050 0.001349 0.017745 0.000000)
 (0.000000 0.000000 0.000000 0.000000)
geom 0 of body 22125440:pos of body 22126256: (0.896067,0.069335,0.598616)
rot of body: 22126256
 (-0.090374 0.671883 -0.506175 0.533097)
 (0.001321 -0.001149 0.006585 0.000000)
 (0.002293 -0.005134 0.001015 0.000000)
 (0.000000 0.000000 0.000000 0.000000)

  */

  dBodyID body1 = _elements[0]->body();
  //dBodySetPosition(body1,0.939906,0.321858,1.029813);
  dBodySetPosition(body1,0.939906,0.321858,0.529813);

  const dQuaternion r2 ={ 0.689774, 0.415068, 0.385473 ,0.450934};

  dBodySetQuaternion(body1,r2);

  body1 = _elements[1]->body();
  //dBodySetPosition(body1,0.886324,0.141222,0.844122);
  dBodySetPosition(body1,0.886324,0.141222,0.344122);
  const dQuaternion r3 ={ 0.626866,-0.191145,0.751742,0.073397};
  dBodySetQuaternion(body1,r3);

  body1 = _elements[2]->body();
  //dBodySetPosition(body1,0.896067,0.069335,1.098616);
  dBodySetPosition(body1,0.896067,0.069335,0.598616);
  const dQuaternion r4 ={-0.090374,0.671883,-0.506175,0.533097};

  dBodySetQuaternion(body1,r4);
}
//copy an object so that it can be reused as a module
MyObject ODEWorld::copyObject(MyObject inobj)
{
  MyObject newobj;
  memset (&newobj,0,sizeof(MyObject));
  newobj.rc = inobj.rc;
  newobj.gc = inobj.gc;
  newobj.bc = inobj.bc;
  newobj.modID = inobj.modID;
  newobj.refcount = inobj.refcount;

  //create it
  newobj.body = dBodyCreate(world);


  //copy the mass accordingly
  dMass fromMass;
  dBodyGetMass(inobj.body,&fromMass);
  dMass newmass;
  dMassSetZero(&newmass);
  dMassAdd(&newmass,&fromMass);
  dBodySetMass(newobj.body,&newmass);

  // set the position and rotation accordingly
  const dReal *inpos = dBodyGetPosition (inobj.body);
  const dReal *r2 = dBodyGetRotation(inobj.body);
  dBodySetPosition(newobj.body,inpos[0],inpos[1],inpos[2]);
  dBodySetRotation(newobj.body,r2);


  for (int i = 0; i < (int)inobj.geom.size(); i++)
    {
      
      if (inobj.geom[i] != NULL)
	{
	  //copy each associated geom
	  dGeomID copied = copyGeom(inobj.geom[i]);
	  dGeomSetBody(copied,newobj.body);
	  newobj.geom.push_back(copied); 
	  //set its body
	  /*
	  if (dGeomGetClass(newobj.geom[i]) == dGeomTransformClass)
	    {
	       dGeomSetBody(dGeomTransformGetGeom(newobj.geom[i]),newobj.body);
	     }
	  */
	}
      /*
      else
	newobj.geom[i] = NULL;
      */
    } 

  return newobj;
  
}



void ODEWorld::destroyObject(MyObject inobj)
{
  dBodyDestroy (inobj.body);
  int l;
  for (l=0; l < (int)inobj.geom.size(); l++) {
    if (inobj.geom[l])
      dGeomDestroy (inobj.geom[l]);
    else
      break;
  }
}

MyObject ODEWorld::objectOwningGeom(dGeomID ingeom)
{

  if (geom_is_in_object(ingeom,turtle))
    return turtle;

  for (int i = 0; i < (int)objvect.size(); i++)
    {
      if (geom_is_in_object(ingeom,objvect[i]))
	return objvect[i];
    }
  
  MyObject defaultObj;
  defaultObj.body = (dBodyID)BAD_BODY;
  printf("couldn't find a body which owns geom %d\n",ingeom);
  return defaultObj;
}

//return the object where the turtle is -- otherwise return the turtle
MyObject ODEWorld::objectAtTurtle()
{
  MyObject returnedObj = turtle;
  // enable the turtle
  // see if it collides
  // remove any object it collides with
  // disable the turtle

  dBodyEnable(turtle.body);
  if (!can_place_object(turtle))
    {
      moduleContacts.clear();
      dSpaceCollide (space,0,&staticNearCallbackCustom);
      //printf("there are %d contacts saved\n",(int)moduleContacts.size());
      for (int i = 0; i < (int)moduleContacts.size(); i++)
	{
	  dContact curcontact = moduleContacts[i];
	  //  printf("geom ids are: %d, %d and body ids are: %d,%d -- turtle body is: %d\n", curcontact.geom.g1, curcontact.geom.g2, dGeomGetBody(curcontact.geom.g1), dGeomGetBody(curcontact.geom.g2), turtle.body);
	  
	  if ((geom_is_in_object(curcontact.geom.g1,turtle)) ||
	      (geom_is_in_object(curcontact.geom.g2,turtle)))
	    {
	      
	      //   printf("curcontact depth is: %f\n",curcontact.geom.depth);
	      
	      if (curcontact.geom.depth > MAX_PENETRATION_DEPTH)
		{
		  
		  
		  MyObject o1 = objectOwningGeom(curcontact.geom.g1);
		  MyObject o2 = objectOwningGeom(curcontact.geom.g2);
		  
		  returnedObj = (o1.body == turtle.body) ? o2 : o1;
		  
		  break;
		}	
	    } 
	  
	  
	}
    } 
  
  dBodyDisable(turtle.body);
  
  return returnedObj;
}


int ODEWorld::rotate_turtle(int val)
  {
    float rot_angle = 0;
   
    dQuaternion localQ;
    
    switch(val)
      {
      case ROT_PLUS_90:
	rot_angle = 90;
	break;
      case ROT_MINUS_90:
	rot_angle = -90;
	break;
      case ROT_180:
	rot_angle = 180;
	break;
      }
    
    const dReal *rv;
    

    //local rotation quat
    //yawing rotates around the local x axis
  //    z
  //    | y
  //    |/
  //    ----  x
    dQFromAxisAndAngle(localQ,0,1,0,rot_angle*M_PI/180);
    const dReal *globalQ = dBodyGetQuaternion(turtle.body);

    dQuaternion newQ;
    dQMultiply0(newQ,globalQ,localQ);
    dBodySetQuaternion(turtle.body,newQ);
    
    const dReal *RM = dBodyGetRotation(turtle.body);

    dVector3 frontvector;
    dBodyGetRelPointPos(turtle.body,0,0,BRICK_WIDTH,frontvector);
    
  return 0;
  
}

void ODEWorld::delete_all_elements()
{
  for (uint i = 0; i < _elements.size(); i++)
    {
      delete(_elements[i]);
      _elements[i] = NULL;
    }

  _elements.clear();
}
int ODEWorld::reset()
{

  comVectorX.clear();
  comVectorY.clear();
  comVectorZ.clear();
  //  printf("reset!\n");
  dJointGroupEmpty (contactgroup);
  delete_all_elements();
  remove_all_objects();
 
  _stopFlag = 0;
  _collisions_enabled = 0;
  dWorldSetGravity (world,0,0,0); 

  //for (uint i = 0; i < _elements.size(); i++) _elements[i]->makeNewNetwork();
  
  //  dBodySetPosition (turtle.body,xpos,ypos,zpos);
  
  return 0;
} 

int ODEWorld::get_test_foo()
{
  return test_foo;
}
// simulation loop
//called every step


void ODEWorld::drawObjects()
{
  //printf("drawObjects!\n");
  dsSetColor (1,1,0);
  for (int j=0; j < (int)turtle.geom.size(); j++) {
    dsSetColor(1,0,0);
    drawGeom(turtle.geom[j],0,0,show_aabb);
    dsSetColor(0,0,0);
    drawGeom(upperLeftMarker.geom[j],0,0,show_aabb);
    drawGeom(upperRightMarker.geom[j],0,0,show_aabb);
  }
  for (int i=0; i<(int)objvect.size(); i++) {
    for (int j=0; j < (int)objvect[i].geom.size(); j++) {
      if (! dBodyIsEnabled (objvect[i].body)) {
	dsSetColor (1,0,0);
      }
      else {
	dsSetColor(objvect[i].rc,objvect[i].gc,objvect[i].bc);
      }
      if (objvect[i].geom[j])
	drawGeom (objvect[i].geom[j],0,0,show_aabb);
      else
	break;
    }
  }
  

  dsSetColor (1,0,0);
  //drawString(_strings[0]);

  for (uint i = 0; i < _strings.size(); i++)
    {
      dsSetColor(1,0,0);
      if (_strings[i]->is_active != 0)
	{
	  dsSetColor(0.5,1,0.5);
	}
      
      drawString(_strings[i]);
      // drawStringForce(_strings[i]);
    }

  if (_drawCOM)
    drawCOM();
  //dsSetColor (0,0,0);
  // drawForce(_elements[i]);
  for (uint i = 0; i < _elements.size(); i++)
    {
      //drawForce(_elements[i]);
    }

  glFlush();
  glXSwapBuffers (display,win);
  XSync (display,0);		
  if (writeframes)
    {
      captureFrame(frame);
      frame++;
    }
}	

int ODEWorld::growAndShowTensegrity(Tensegrity *ints)
{
 
  int stillgrowing = 1;
  do{
    
    vector<String *> oldstrings;
    vector<Element *> oldelems;
   
    //ok, and somehow these will be useful...
    for (uint i = 0; i < _strings.size(); i++)
      {
	oldstrings.push_back(_strings[i]);
      }
    for (uint i = 0; i < _elements.size(); i++)
      {
	oldelems.push_back(_elements[i]);
      }
    
    
    reset();
    doGraphicsStuff();
    stillgrowing = ints->newgrow(10);
    makeFromNewTens(ints,0);
   
    //run_tensegrity(0);
    drawObjects();
    getchar();
    
  }while (stillgrowing);

  printf("done growing - simplifying\n");
  reset();
  doGraphicsStuff();
  
  ints->simplify();
  makeFromNewTens(ints,0);
  drawObjects();
  getchar();
}

int ODEWorld::makeFromTensegrity(Tensegrity *ints, int is_static)
{
  return makeFromGraph(ints->graph(),ints->pairs(),is_static);
}

void ODEWorld::print_current_tens()
{
  cur_tens->print_graphviz();
}
int ODEWorld::makeFromNewTens(Tensegrity *ints, int is_static)
{

  printf("ODEWorld::makeFromNewTens!\n");
  Graph *ingraph = ints->graph();
  _elements.clear();
  _strings.clear();
  vector<Node *> alreadyMade;
  alreadyMade.clear();
  
  vector<Node *> graphNodes = ingraph->get_nodes();
  vector<Edge *> graphEdges = ingraph->get_edges();

  vector<Node *> nodeElementMatches;
  // this will be a vector of Nodes
  // corresponding to the endpoints of elements
  // 0 = element0.bottom
  // 1 = element0.top
  // 2 = element1.bottom
  // etc
  nodeElementMatches.clear();
  

  //for (uint i = 0; i < graphNodes.size(); i++) { if (graphNodes[i]->other == NULL) { printf("ODEWorld::make from newTens: i got a bad graph!\n"); printf("ODEWorld::make from newTens: node %d has no match!\n",i); return 0; } }

  // first make each element
  for (uint i = 0; i < graphNodes.size(); i++)
    {
      
      Node *curnode = graphNodes[i];
      Node *othernode = curnode->other;
     
      // this keeps track of nodes that have been used already
      //      vector<Node *>::iterator found = find(alreadyMade.begin(), alreadyMade.end(),curnode);
      int found = 0;
      for (uint f = 0; f < alreadyMade.size(); f++)
	{
	  if (alreadyMade[f] == curnode)
	    {
	      found = 1;
	      break;
	    }
	}
      
      if (!found) //hasn't been used
	{
	  
	      //this is used to figure out what to attach strings to
	      nodeElementMatches.push_back(curnode);
	      nodeElementMatches.push_back(othernode);
	  
	      MyObject curobj;
	      alreadyMade.push_back(curnode);//push back that node
	      if (othernode)
		alreadyMade.push_back(othernode); //and its other end if it exists

	      //	  printf("making element between %d and %d\n",i,rods[i]);
	      if (i == 0)
		{
		  if (othernode) //if you have an other you're a capsule
		    put_capsule(0.5,0.5,_ELEM_UNIT_LEN/2 + _ELEM_UNIT_RAD*2);
		  else //otherwise you're a sphere
		      put_sphere(0.5,0.5,_ELEM_UNIT_LEN/2 + _ELEM_UNIT_RAD*2);
		}
	      else if (!is_static) 
		{
		  if (othernode)
		    put_capsule();
		  else
		    put_sphere();
		}
	      else
		{
		  printf("making in static positions\n");
		  if (othernode)
		    put_capsule(0.5*i+0.5,0.5*i+0.5,0.5*i+0.5);
		  else
		    put_sphere(0.5*i+0.5,0.5*i+0.5,0.5*i+0.5);
		}
	      curobj = objvect[objvect.size()-1];  
	      Element *elem1 = new Element(curobj.body); 
	      if (!othernode)
		elem1->_is_rod = 0;

	      _elements.push_back(elem1);
	}
    }

  //we wanna always disable the 0th.
  dBodyDisable(_elements[0]->_body);
  
  for (uint e = 0; e < graphEdges.size(); e++) //for every edge
    {
      Node *from = graphEdges[e]->from;
      Node *to = graphEdges[e]->to;

      int indexOfFrom = -1;
      int indexOfTo = -1;

      for (int n = 0; n < (int)nodeElementMatches.size(); n++) //find the from and to
	{
	  if (nodeElementMatches[n] == from)
	    {
	      indexOfFrom = n;
	      //      printf("found index of from: %d\n",n);
	    }
	  if (nodeElementMatches[n] == to)
	    {
	      indexOfTo = n;
	      //  printf("found index of to: %d\n",n);
	    }
	  
	  if ((indexOfFrom != -1) && (indexOfTo != -1))
	    break;
	}

      if ((indexOfFrom == -1) || (indexOfTo == -1))
	{
	  printf("couldn't find elements!\n");
	  return 0;
	}
      int fromLoc = indexOfFrom%2; //each element is two endpoints 
      int toLoc = indexOfTo%2; // 
      int fromElIndex = (int)floor(indexOfFrom/2);
      int toElIndex = (int)floor(indexOfTo/2);

      // printf("edge is:\n");
      //      graphEdges[e]->print();
      //      printf("making string between element %d (%d) and element %d (%d)\n",fromElIndex,fromLoc,toElIndex,toLoc);
      String *newstring = new String(_elements[fromElIndex],fromLoc,_elements[toElIndex],toLoc);
      _strings.push_back(newstring);
    }

}

int ODEWorld::makeFromGraph(Graph *ingraph, vector<int> rods, int is_static)
{

  //  printf("makefromGraph!\n");
  // getchar();
  _elements.clear();
  _strings.clear();
  vector<int> alreadyMade;
  alreadyMade.clear();
  
  vector<Node *> graphNodes = ingraph->get_nodes();
  vector<Edge *> graphEdges = ingraph->get_edges();

  vector<Node *> nodeElementMatches;
  // this will be a vector of Nodes
  // corresponding to the endpoints of elements
  // 0 = element0.bottom
  // 1 = element0.top
  // 2 = element1.bottom
  // etc
  nodeElementMatches.clear();
  

  if (rods.size() != graphNodes.size())
    {
      //      printf("ODEWorld::make from graph: i got a bad graph!\n");
      return 0;
    }
  // first make each element
  for (uint i = 0; i < rods.size(); i++)
    {
      vector<int>::iterator found = find(alreadyMade.begin(), alreadyMade.end(),rods[i]);
      if (found == alreadyMade.end()){
        int bottom = i;
        int top = rods[i];
        
        nodeElementMatches.push_back(graphNodes[bottom]);
        nodeElementMatches.push_back(graphNodes[top]);
        
        MyObject curobj;
        alreadyMade.push_back(i);//push back that node
        alreadyMade.push_back(rods[i]); //and its other end
        //	  printf("making element between %d and %d\n",i,rods[i]);
        if (i == 0){
          put_capsule(0.5,0.5,_ELEM_UNIT_LEN/2 + _ELEM_UNIT_RAD*2);
        }
        else if (!is_static){ 
          put_capsule();
        }
        else{
          printf("making in static positions\n");
          put_capsule(0.5*i+0.5,0.5*i+0.5,0.5*i+0.5);
        }
        curobj = objvect[objvect.size()-1];  
        Element *elem1 = new Element(curobj.body); 
        _elements.push_back(elem1);

        graphNodes[top]->setElementNum((int)_elements.size());
        graphNodes[bottom]->setElementNum((int)_elements.size());
	}
    }

  /*
  for (uint i = 0; i < graphNodes.size(); i++)
    {
      printf("Node %d, elem %d\n",i,graphNodes[i]->getElementNum());
    }
  */
  //we wanna always disable the 0th.
  
  dBodyDisable(_elements[0]->_body);
  
  for (uint e = 0; e < graphEdges.size(); e++)
    {
      
      //      char nodestub = graphEdges[e]->stublabel();
      char nodestub = graphEdges[e]->stublabel();
      //      printf("stublabel is: %c\n",nodestub);
      Node *from = graphEdges[e]->from;
      Node *to = graphEdges[e]->to;

      int indexOfFrom = -1;
      int indexOfTo = -1;

      for (int n = 0; n < (int)nodeElementMatches.size(); n++)
	{
	  if (nodeElementMatches[n] == from)
	    {
	      indexOfFrom = n;
	      //      printf("found index of from: %d\n",n);
	    }
	  if (nodeElementMatches[n] == to)
	    {
	      indexOfTo = n;
	      //  printf("found index of to: %d\n",n);
	    }
	  
	  if ((indexOfFrom != -1) && (indexOfTo != -1))
	    break;
	}

      if ((indexOfFrom == -1) || (indexOfTo == -1))
	{
	  printf("couldn't find elements!\n");
	  return 0;
	}
      int fromLoc = indexOfFrom%2; // 
      int toLoc = indexOfTo%2; // 
      int fromElIndex = (int)floor(indexOfFrom/2);
      int toElIndex = (int)floor(indexOfTo/2);

      // printf("edge is:\n");
      //      graphEdges[e]->print();
      //      printf("making string between element %d (%d) and element %d (%d)\n",fromElIndex,fromLoc,toElIndex,toLoc);
      String *newstring = new String(_elements[fromElIndex],fromLoc,_elements[toElIndex],toLoc,nodestub);
      //      getchar();
      // printf("setting string rod number to %d\n",atoi((graphEdges[e]->getlabel())));

      newstring->setRodNumber(atoi(graphEdges[e]->getlabel()));

      (fromLoc == 0) ? 
	_elements[fromElIndex]->attachStringToBottom(newstring) :
	_elements[fromElIndex]->attachStringToTop(newstring);
	
      (toLoc == 0) ? 
	_elements[toElIndex]->attachStringToBottom(newstring) :
	_elements[toElIndex]->attachStringToTop(newstring);

      _strings.push_back(newstring);
    }

  return 1;
}


float ODEWorld::getMaxForce()
{
  dReal maxforce = -1;
  
  for (uint i = 0; i < _elements.size(); i++)
    {
      const dReal *force = dBodyGetForce(_elements[i]->body());
      const dReal *torq = dBodyGetTorque(_elements[i]->body());
      float v1 = fabs(sqrt((force[0]*force[0])+(force[1]*force[1])+(force[2]*force[2])));
      float v2 = fabs(sqrt((torq[0]*torq[0])+(torq[1]*torq[1])+(torq[2]*torq[2])));
      maxforce = (maxforce < v1) ? v1 : maxforce;
      maxforce = (maxforce < v2) ? v2 : maxforce;
      
      //printf("velocity of %d: (%f %f %f),(%f %f %f)\n",i, vel[0],vel[1],vel[2], avel[0],avel[1],avel[2]);
      
    }
  return maxforce;

}

float ODEWorld::getMaxVelocity()
{

  float maxvel = -1;
  
  for (uint i = 0; i < _elements.size(); i++)
    {
      const dReal *vel = dBodyGetLinearVel(_elements[i]->_body);
      const dReal *avel = dBodyGetAngularVel(_elements[i]->_body);
      float v1 = fabs(sqrt((vel[0]*vel[0])+(vel[1]*vel[1])+(vel[2]*vel[2])));
      float v2 = fabs(sqrt((avel[0]*avel[0])+(avel[1]*avel[1])+(avel[2]*avel[2])));

      maxvel = (maxvel < v1) ? v1 : maxvel;
      maxvel = (maxvel < v2) ? v2 : maxvel;
      
      //      printf("maxvel is: %f\n",maxvel);
      //printf("velocity of %d: (%f %f %f),(%f %f %f)\n",i, vel[0],vel[1],vel[2], avel[0],avel[1],avel[2]);
      
    }
  return maxvel;
}

void ODEWorld::make_capsules()
{

  _strings.clear();
  _elements.clear();

  MyObject curobj;

  // for tr-9
  put_capsule(); 
  curobj = objvect[objvect.size()-1];  
  Element *elem1 = new Element(curobj.body); 

  put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem2 = new Element(curobj.body);

    put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem3 = new Element(curobj.body);

    put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem4 = new Element(curobj.body);

  put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem5 = new Element(curobj.body);

  put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem6 = new Element(curobj.body);

  put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem7 = new Element(curobj.body);

  put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem8 = new Element(curobj.body);

  put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem9 = new Element(curobj.body);


  _elements.push_back(elem1); 
  _elements.push_back(elem2);
  _elements.push_back(elem3);
  _elements.push_back(elem4);
  _elements.push_back(elem5);
  _elements.push_back(elem6);
  _elements.push_back(elem7);
  _elements.push_back(elem8);
  _elements.push_back(elem9);

  String *string1 = new String(elem5,1,elem2,1   );
  String *string2 = new String(elem6,1,elem2,1 );
  String *string3 = new String( elem3,0 ,elem4,1 );
  String *string4 = new String(elem5,1,elem6,1 );
  String *string5 = new String(elem6,1, elem3,0  );
  String *string6 = new String(elem5,1, elem3,0 );
  String *string7 = new String(elem3,1, elem7,1 );
  String *string8 = new String( elem7,1 ,elem1,1);
  String *string9 = new String(elem1,1, elem8,1 );
  String *string10= new String( elem8,1 ,elem1,0);
  String *string11 = new String(elem1,0, elem9,1 );
  String *string12 = new String(elem4,1, elem6,0 );
  String *string13 = new String(elem3,1, elem9,0 );
  String *string14 = new String( elem9,0 ,elem1,0);
  String *string15 = new String(elem3,1, elem2,0 );
  String *string16 = new String(  elem2,0 ,elem4,1);
  String *string17 = new String(elem1,0, elem8,0 );
String *string18 = new String( elem8,0 ,elem4,1);
String *string19 = new String( elem7,0 ,elem5,1);
String *string20= new String(elem1,1, elem5,0 );
String *string21= new String( elem5,0 ,elem6,1);
String *string22= new String( elem7,1 , elem8,1 );
String *string23= new String( elem6,0 , elem4,0 );
String *string24= new String( elem6,0 , elem9,0 );
String *string25= new String( elem5,0 , elem8,1 );
String *string26= new String( elem2,0 , elem7,1 );
String *string27= new String( elem8,0 , elem9,1  );
String *string28= new String( elem8,1 , elem9,0  );
String *string29= new String( elem4,0 , elem2,0 );
String *string30= new String( elem9,0 , elem8,0  );
String *string31= new String( elem4,0 , elem9,0  );
String *string32= new String( elem5,0 , elem7,0  );
String *string33= new String( elem2,0 , elem5,0  );
String *string34= new String( elem8,0 , elem7,0  );
String *string35= new String( elem9,0 , elem7,1  );
String *string36= new String( elem2,0 , elem6,0  );
String *string37= new String( elem8,0 , elem6,0  );
String *string38= new String( elem7,0 , elem9,1  );
String *string39= new String( elem5,0 , elem7,1  );
String *string40= new String( elem5,0 , elem9,1  );
String *string41= new String( elem9,1 , elem8,1  );
String *string42= new String(elem3,1, elem4,0  );
String *string43= new String(elem2,1, elem3,0  );

 _strings.push_back(string1);
 _strings.push_back(string2); 
  _strings.push_back(string3);
  _strings.push_back(string4);  

  _strings.push_back(string5);
  _strings.push_back(string6); 
  _strings.push_back(string7);
  _strings.push_back(string8);  

  _strings.push_back(string9);
  _strings.push_back(string10); 
  _strings.push_back(string11);
  _strings.push_back(string12);  

  _strings.push_back(string13);
  _strings.push_back(string14); 
  _strings.push_back(string15);
  _strings.push_back(string16);
 
  _strings.push_back(string17);
  _strings.push_back(string18);
  _strings.push_back(string19);

			      
  _strings.push_back(string20);
  _strings.push_back(string21);
  _strings.push_back(string22);
  _strings.push_back(string23);
  _strings.push_back(string24);
  _strings.push_back(string25);
  _strings.push_back(string26);
  _strings.push_back(string27);
  _strings.push_back(string28);
  _strings.push_back(string29);


  _strings.push_back(string30);
  _strings.push_back(string31);
  _strings.push_back(string32);
  _strings.push_back(string33);
  _strings.push_back(string34);
  _strings.push_back(string35);
  _strings.push_back(string36);
  _strings.push_back(string37);
  _strings.push_back(string38);
  _strings.push_back(string39);

  _strings.push_back(string40);
  _strings.push_back(string41);
  _strings.push_back(string42);
  _strings.push_back(string43);

  /*
  // for evo tr6
 put_capsule(); 
  curobj = objvect[objvect.size()-1];  

  Element *elem1 = new Element(curobj.body); 

  put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem2 = new Element(curobj.body);

    put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem3 = new Element(curobj.body);

    put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem4 = new Element(curobj.body);

  put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem5 = new Element(curobj.body);

  put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem6 = new Element(curobj.body);

  _elements.push_back(elem1); 
  _elements.push_back(elem2);
  _elements.push_back(elem3);
  _elements.push_back(elem4);
  _elements.push_back(elem5);
  _elements.push_back(elem6);

String *string1 = new String(elem4,1,elem2,0);
String *string2 = new String(elem1,0 , elem5,1 );
String *string3 = new String(elem2,0, elem3,0 );
String *string4 = new String(elem6,0 , elem1,0 );
String *string5 = new String(elem1,0 , elem4,0);
String *string6 = new String(elem4,0 , elem1,1);
String *string7 = new String(elem1,1 , elem6,1);
String *string8 = new String(elem6,1 , elem5,1);
String *string9 = new String(elem1,1 , elem2,1);
String *string10 = new String(elem2,1 , elem3,1);
String *string11 = new String(elem3,1 , elem5,0);
String *string12 = new String(elem5,0 ,elem2,0);
String *string13 = new String(elem6,0 , elem5,0 );
String *string14 = new String(elem4,0 , elem6,1 );
String *string15 = new String(elem5,0 , elem3,0 );
String *string16 = new String(elem4,0 , elem2,1 );
String *string17 = new String(elem3,0 , elem6,0 );
String *string18 = new String(elem4,1, elem3,1);
String *string19 = new String(elem4,1, elem5,1);
 _strings.push_back(string1);
  _strings.push_back(string2); 
  _strings.push_back(string3);
  _strings.push_back(string4);  

  _strings.push_back(string5);
  _strings.push_back(string6); 
  _strings.push_back(string7);
  _strings.push_back(string8);  

  _strings.push_back(string9);
  _strings.push_back(string10); 
  _strings.push_back(string11);
  _strings.push_back(string12);  

  _strings.push_back(string13);
  _strings.push_back(string14); 
  _strings.push_back(string15);
  _strings.push_back(string16);
 
  _strings.push_back(string17);
  _strings.push_back(string18);
  _strings.push_back(string19);
  */
  /*
  //FOR evo-tr5
  put_capsule(); 
  curobj = objvect[objvect.size()-1];  

  Element *elem1 = new Element(curobj.body); 

  put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem2 = new Element(curobj.body);

    put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem3 = new Element(curobj.body);

    put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem4 = new Element(curobj.body);

    put_capsule();
  curobj = objvect[objvect.size()-1]; 
  Element *elem5 = new Element(curobj.body);

  
  _elements.push_back(elem1); 
  _elements.push_back(elem2);
  _elements.push_back(elem3);
  _elements.push_back(elem4);
  _elements.push_back(elem5);
  
  String *string1 = new String(elem3,1,elem1,1 );
  String *string2 = new String(elem1,1,elem2,0 );
  String *string3 = new String(elem4,0,elem5,0);
  String *string4 = new String(elem5,1,elem2,1);
  
  String *string5 = new String(elem3,1,elem4,1 );
  String *string6 = new String(elem4,1,elem2,1 );
  String *string7 = new String(elem2,1,elem1,0);
  String *string8 = new String(elem1,0,elem5,0 );
  
  String *string9 = new String(elem2,0,elem4,0 );
  String *string10 = new String(elem4,1,elem1,0);
  String *string11 = new String(elem3,0,elem4,0 );
  String *string12 = new String(elem5,1,elem3,0 );
  
  String *string13 = new String(elem3,0,elem2,0 );
  String *string14 = new String(elem2,1,elem3,0 );
  String *string15 = new String(elem3,1,elem5,0 );
  String *string16 = new String(elem1,1,elem5,1 );
  

  _strings.push_back(string1);
  _strings.push_back(string2); 
  _strings.push_back(string3);
  _strings.push_back(string4);  

  _strings.push_back(string5);
  _strings.push_back(string6); 
  _strings.push_back(string7);
  _strings.push_back(string8);  

  _strings.push_back(string9);
  _strings.push_back(string10); 
  _strings.push_back(string11);
  _strings.push_back(string12);  

  _strings.push_back(string13);
  _strings.push_back(string14); 
  _strings.push_back(string15);
  _strings.push_back(string16);  
  */
  // there are three cylinders
  // and 9 "strings"
  // corresponding to 18 forces
  // three for each end of a cylinder.
  /*  FOR tr3
  put_capsule(); 
  curobj = objvect[objvect.size()-1]; 
  // dBodySetPosition(curobj.body,1,1,1);

  dReal bar1bot[3]; 
  dReal bar1top[3]; 

  Element *elem1 = new Element(curobj.body);

  
  elem1->getSidePos(0,bar1bot); 
  elem1->getSidePos(1,bar1top); 

  //  dBodyGetRelPointPos(curobj.body,_ELEM_UNIT_LEN/2,0.0,0.0,bar1top);
  
  printf("bar1top: %f %f %f\n",bar1top[0],bar1top[1],bar1top[2]); 
  printf("bar1bot: %f %f %f\n",bar1bot[0],bar1bot[1],bar1bot[2]); 

  dVector3 outvec; 

  vectorAdd(bar1top,bar1bot,outvec); 
  printf("outvec is: %f %f %f\n",outvec[0],outvec[1],outvec[2]); 

  printf("distance is: %f\n",distance(bar1top, bar1bot)); 
  //  dBodyAddForceAtRelPos(curobj.body,1.0,0.0,0.0,_ELEM_UNIT_LEN/2,0.0,0.0);
  //dBodyAddForceAtRelPos(curobj.body,-1.0,0.0,0.0,-1*_ELEM_UNIT_LEN/2,0.0,0.0);

  put_capsule(); 
  curobj = objvect[objvect.size()-1]; 
  // dBodySetPosition(curobj.body,2,2,2);
  Element *elem2 = new Element(curobj.body); 

  dReal bar2top[3]; 
  dReal bar2bot[3]; 

  */
  /*
  _elements.push_back(elem1);
  _elements.push_back(elem2);
  
  String *string1 = new String(elem1,1, elem2, 1);
  String *string2 = new String(elem1,0, elem2, 0);
  String *string3 = new String(elem1,1, elem2, 0);
  String *string4 = new String(elem1,0, elem2, 1);

  _strings.push_back(string1);
  _strings.push_back(string2);
  _strings.push_back(string3);
  _strings.push_back(string4); 

  */

  /* FOR TR3
  put_capsule();  
  curobj = objvect[objvect.size()-1]; 
  objvect[objvect.size()-1].rc = 1; 
  objvect[objvect.size()-1].gc = 0; 
  objvect[objvect.size()-1].bc = 0; 
  // dBodySetPosition(curobj.body,1,1,1);
  Element *elem3 = new Element(curobj.body); 

  dBodyDisable(curobj.body); 
  

 


  _elements.push_back(elem1); 
  _elements.push_back(elem2); 
  _elements.push_back(elem3); 


  String *string1 = new String(elem1,1, elem2, 1);  
  String *string2 = new String(elem2,1, elem3, 1);  
  String *string3 = new String(elem3,1, elem1, 1);  

  String *string4 = new String(elem1,0, elem2, 0); 
  String *string5 = new String(elem2,0, elem3, 0); 
  String *string6 = new String(elem3,0, elem1, 0); 

  String *string7 = new String(elem1,1, elem2, 0); 
  String *string8 = new String(elem2,1, elem3, 0); 
  String *string9 = new String(elem3,1, elem1, 0);  

  */
 
  
  //  String *string10 = new String(elem1,1, elem3, 0); 

  /*
 put_capsule(); 
  curobj = objvect[objvect.size()-1];
  objvect[objvect.size()-1].rc = 0;
  objvect[objvect.size()-1].gc = 1;
  objvect[objvect.size()-1].bc = 0;
  // dBodySetPosition(curobj.body,1,1,1);
  Element *elem4 = new Element(curobj.body);

  //  _elements.push_back(elem4);
  String *string1 = new String(elem1,1, elem2, 1); 
  String *string2 = new String(elem2,1, elem3, 1); 
  String *string3 = new String(elem3,1, elem4, 1); 
  String *string4 = new String(elem4,1, elem1, 1);

  String *string5 = new String(elem1,0, elem2, 0); 
  String *string6 = new String(elem2,0, elem3, 0); 
  String *string7 = new String(elem3,0, elem4, 0); 
  String *string8 = new String(elem4,0, elem1, 0);

  
  String *string9 = new String(elem1,1, elem2, 0);
  String *string10 = new String(elem1,1, elem3, 0);

  String *string11 = new String(elem2,1, elem3, 0);
  String *string12 = new String(elem2,1, elem4, 0);

  String *string13 = new String(elem3,1, elem4, 0);
  String *string14 = new String(elem3,1, elem1, 0);

  String *string15 = new String(elem4,1, elem1, 0);
  String *string16 = new String(elem4,1, elem2, 0);
  */

  /* for TR3
  _strings.push_back(string1);
  _strings.push_back(string2);
  _strings.push_back(string3);
  _strings.push_back(string4);
  _strings.push_back(string5);
  _strings.push_back(string6);
  _strings.push_back(string7);
  _strings.push_back(string8);
  _strings.push_back(string9); 
  _strings.push_back(string10);
  */
  

  /*
  _strings.push_back(string10);
  _strings.push_back(string11);
  _strings.push_back(string12);
  _strings.push_back(string13);
  _strings.push_back(string14);
  _strings.push_back(string15);
  _strings.push_back(string16);
  */

}

void ODEWorld::activate_string(int i)
{
  _strings[i]->toggle_active();
}

void ODEWorld::pull_strings()
{
  for (uint i = 0; i < _strings.size(); i++)
    {
      _strings[i]->pull();
    }

}

void ODEWorld::calc_nets()
{
  

}

void ODEWorld::copyWeightsToElements(Tensegrity *ts)
{
 
  vector<float> tvals = ts->genome();
  
  // vector<int> frompairs = ts->pairs();
  //  int sizeOfTS = (int)frompairs.size()/2;
  
  //vector<float> el1vals; vector<float> el2vals; vector<float> el3vals; vector<float> el4vals; el1vals.clear(); el2vals.clear(); el3vals.clear(); el4vals.clear();

  vector<float> elvals;
  elvals.clear();

  int sizeOfTS = tvals.size()/WEIGHTS_PER_NET;

  if (sizeOfTS != (int)_elements.size())
    printf("copyWeights has a size problem!\n");

  for (uint e = 0; e < _elements.size(); e++)
    {
      elvals.clear();
      Element *curel = _elements[e];
      for (uint w = 0; w < WEIGHTS_PER_NET; w++)
	{
	  elvals.push_back(tvals[e*WEIGHTS_PER_NET + w]);
	}
      curel->setWeights(elvals);
    }
 
  /*
  for (int i = 0; i < 12; i++)
    {
      el1vals.push_back(tvals[i]);
      el2vals.push_back(tvals[12+i]);
      el3vals.push_back(tvals[24+i]);
      if (sizeOfTS == 4)
	{
	  //	  printf("pushing back %f\n",tvals[36+i]);
	  el4vals.push_back(tvals[36+i]);
	}
    }

  _elements[0]->setWeights(el1vals);
  _elements[1]->setWeights(el2vals);
  _elements[2]->setWeights(el3vals);
  if (sizeOfTS == 4)
    {
      //      printf("setting\n");
      _elements[3]->setWeights(el4vals);
    }
  */
  /*
  print_vector_of_floats(el1vals);
  print_vector_of_floats(el2vals);
  print_vector_of_floats(el3vals);
  print_vector_of_floats(el4vals);
  */
}

void ODEWorld::updateStringLabels(Tensegrity *tens)
{

  //  printf("updating\n");
  Graph *tgraph = tens->graph();
  vector<Edge *> graphEdges = tgraph->get_edges();

  for (uint s = 0; s < _strings.size(); s++)
    {
      _strings[s]->setRodNumber(atoi(graphEdges[s]->getlabel()));
      //   printf("rod number %d (%d)\n",_strings[s]->rodNumber,atoi(graphEdges[s]->getlabel()));
}

}

vector<float> ODEWorld::evaluateJiggle(Tensegrity *ts, int grow, int do_render, int stopOnFlag, char *prefix)
{
  vector<float> results;
  results.clear();

  for (int i = 0; i < 20; i++)
    {
      cur_tens = ts;
      render = do_render;
      
      reset();
      int result;
      int is_static = 0;
      result = 1;
      
      
      result = makeFromTensegrity(ts,is_static);
      
      resetTens();
      updateStringLabels(ts);
      copyWeightsToElements(ts);
  
      
      //float rate; if (i < 0) //we're stepping world faster rate = fabs(0.01*i); else //we're updating networks slower rate = i+1;

      run_tensegrity_jiggle(stopOnFlag,i*5);
	  
      float dt = distanceTraveled(_currCOM);
      printf("distance:%f\n",dt);
      results.push_back(dt);
    } 
  return results;

}

vector<float> ODEWorld::evaluate(Tensegrity *ts, int readFromFile, int do_render, int stopOnFlag)
{

  //  printf("evaluating...\n");
  ts->fprintGenome("curnetwork.net");
  //  ts->printStringLabels();
  cur_tens = ts;
  _use_networks = 0;
  render = do_render;

  reset();
  int result;
  int is_static = 0;
  result = 1;
  
  vector<float> results;
  results.clear();
  
  result = makeFromTensegrity(ts,is_static);

  //  ts->findRandomStringLabels();
  //ts->mutateStringLabels();
  //ts->mutateStringLabels();
  //ts->mutateStringLabels();
 
  //  resetTens();
  resetTens();
  updateStringLabels(ts);
  //resetT3();
  copyWeightsToElements(ts);
  
  //  ts->printStringLabels();
  
  //  ts->print();
  //ts->print_graphviz();
  

  if (result != 0)
    {
      run_tensegrity(stopOnFlag);

      float dt = distanceTraveled(_currCOM);
      printf("distance:%f\n",dt);

      float vol = get_tensegrity_volume();
      results.push_back(0.); // rank
      results.push_back(dt);
    }
  else
    {
      //      printf("bad tensegrity\n");
      results.push_back(0.); // rank
      results.push_back(0);
    }
  
  return results;
}

void ODEWorld::run_element_networks()
{
  for (uint i = 0; i < _elements.size(); i++)
    {
      //      _elements[i]->printNetwork();
      _elements[i]->activateStrings(i);
      if (_printlens){
        FILE * f = fopen("stringlens.txt","a+");
        for (uint s = 0; s < _strings.size(); s++){
          //fprintf(f,"%f ",_strings[s]->curLength());
          if (_strings[s]->rodNumber > 0){
            fprintf(f,"%d ",_strings[s]->is_active);
          }
        }
        fprintf(f,"\n");
        fclose(f);
	    }
    }
  //  printf("\n");
  //  getchar();
}

/*
  every string is labeled with either -1 of the number of the element
  but element number starts at 1, not 0, so must -- the number to find index
*/
void ODEWorld::attachStringsToNets()
{
  for (uint i = 0; i < _strings.size(); i++)
    {
      //      printf("String %d\n",i);
      //      _strings[i]->attachToNet();
      String *st = _strings[i];
      char label = _strings[i]->_label;
      //      int val = ((int)label) - 97;
      int val = st->rodNumber;
      
      
      //      int ind = val/2;
      //int side = val%2;
	//      if ((ind >= 0) && (ind < (int)_elements.size()))
      if ((val != -1) && (val >= 0) && (val <= (int)_elements.size()))
	{
	  //	  printf("attaching string %d to rod number %d ",i,val);
	  // must offset by one to match the index
	  Element *curel = _elements[val - 1];
	  if (curel == st->fromEl())
	    {
	   	  //   printf("(%d)\n",st->fromSide);
	      curel->setPullString(st->fromSide,st);
	    }
	  else if (curel == st->toEl())
	    {
	   	  //   printf("(%d)\n",st->toSide);
	      curel->setPullString(st->toSide,st);
	    }
	  else
	    {
	      printf("uh oh...\n");
	    }
	}
    }

}


void ODEWorld::drawCOM()
{
  dsSetColor(1,0,0);
  dVector3 com;
  centerOfMass(com);
  comVectorX.push_back(com[0]);
  comVectorY.push_back(com[1]);
  comVectorZ.push_back(com[2]+1.);

  const dReal *R = dBodyGetRotation(objvect[0].body);
  float radius = .1;
 
  //  printf("drawCOM %f %f %f\n",com[0],com[1],com[2]);
  for (uint i = 0; i < comVectorX.size(); i+=100)
    {
      dVector3 pos;
      pos[0] = comVectorX[i];
      pos[1] = comVectorY[i];
      pos[2] = comVectorZ[i];
      //      dsDrawSphere(pos,myR,0.5);
      setupDrawingMode();
      glEnable (GL_NORMALIZE);
      glShadeModel (GL_SMOOTH);
      setTransform (pos,R);
      glScaled (radius,radius,radius);
      drawSphere();
      glPopMatrix();
      glDisable (GL_NORMALIZE);
      
      // draw shadows
      if (use_shadows) {
	glDisable (GL_LIGHTING);
	if (use_textures) {
	  ground_texture->bind (1);
	  glEnable (GL_TEXTURE_2D);
	  glDisable (GL_TEXTURE_GEN_S);
	  glDisable (GL_TEXTURE_GEN_T);
	  glColor3f (SHADOW_INTENSITY,SHADOW_INTENSITY,SHADOW_INTENSITY);
	}
	else {
	  glDisable (GL_TEXTURE_2D);
	  glColor3f (GROUND_R*SHADOW_INTENSITY,GROUND_G*SHADOW_INTENSITY,
		     GROUND_B*SHADOW_INTENSITY);
	}
	glShadeModel (GL_FLAT);
	glDepthRange (0,0.9999);
	drawSphereShadow (pos[0],pos[1],pos[2],radius);
	glDepthRange (0,1);
      }
    }
}

//center of mass of all of the elements
void ODEWorld::centerOfMass(dVector3 result)
{
 
  float xcenter = 0;
  float ycenter = 0;
  float zcenter = 0;

  for (uint i = 0; i < _elements.size(); i++)
    {
      dBodyID curbody = _elements[i]->body();
      const dReal *bodypos = dBodyGetPosition(curbody);
      xcenter += bodypos[0];
      ycenter += bodypos[1];
      zcenter += bodypos[2];
    }

  result[0] = xcenter/_elements.size();
  result[1] = ycenter/_elements.size();
  result[2] = zcenter/_elements.size();

}

//how far did the struct travel?
float ODEWorld::distanceTraveled(dVector3 initpos)
{
  
  dVector3 curpos;
  centerOfMass(curpos);

  //  printf("init: %f,%f,%f\n",initpos[0],initpos[1],initpos[2]);
  dVector3 outvec;
  
  outvec[0] = curpos[0] - initpos[0];
  outvec[1] = curpos[1] - initpos[1];
  outvec[2] = curpos[2] - initpos[2];
 
  //  printf("final: %f,%f,%f\n",curpos[0],curpos[1],curpos[2]);

  //printf("mag: %f\n",magnitude(outvec));
  float distance = fabs(magnitude(outvec));
  return distance;
}

void ODEWorld::run_tensegrity_jiggle(int stopOnFlag, int timedelay)
{

  _collisions_enabled = 1;
  _use_networks = 1;
  dBodyEnable(_elements[0]->_body);
  dWorldSetGravity(world,_Horizontal_Gravity_Vector,0,-0.1); 
  
  resetTens();
  attachStringsToNets();

  for (uint i = 0; i < _elements.size(); i++)
    {
      _elements[i]->setTimeDelay(timedelay);
    }
  cur_step = 0;
  _enable_ODE = 1;
  _stopFlag = 0;


  float maxforce = -1;

  time_t starttime = time(NULL);
 
  float avgvel = 0;
  
  vector<float> velmem;
  velmem.clear();

  int run_network_every = 1;
  int MAX_STEPS = 10000;
  float world_step_rate = 0.007;
  
  printf("run network every: %d\n",run_network_every);

  printf("render is: %d\n",render);
  printf("delay: %d, world_step_rate: %f, max steps: %d\n",timedelay,world_step_rate,MAX_STEPS);
  
  centerOfMass(_currCOM);

  
  do
    {
      //      simLoop(0);
      if(render != 0)
	{
	  doGraphicsStuff();
	  dsSetColor (0,0,1);
	}
      
      if (_enable_ODE)
	{
	  resetForces();
	
	  if (_use_networks)
	    {
	      if ((cur_step % run_network_every) == 0)
		run_element_networks();
	    }

	  pull_strings();
	  addDrags();
	 
	  maxforce = getMaxForce();
	  g_maxforce = maxforce;
	  g_maxvel = getMaxVelocity();

	  velmem.push_back(g_maxvel);
	  if (velmem.size() > 100)
	      velmem.erase(velmem.begin());
	  
	  if (render == 1) drawObjects();
	  if (_collisions_enabled)
	    {
	      dJointGroupEmpty (contactgroup);
	      dSpaceCollide (space,0,&staticNearCallback);
	    }
	  //dSpaceCollide (space,0,&nearCallback);
	  //dWorldStep(world,0.01);
	  dWorldStep(world,world_step_rate);
	  
	  if (velmem.size() > 50)
	    {
	      avgvel = 0;
	      for (uint i  = 0; i < velmem.size(); i++)
		{
		  avgvel+= velmem[i];
		}
	      avgvel = avgvel/velmem.size();
	    }
	}

      
      cur_step++;
      if (_stopFlag)
	break;
      // dsSetTexture (DS_WOOD);
      //draw the turtle

      //      getchar();
    } while (cur_step < 20000);

  float dt = distanceTraveled(_currCOM);

}
void ODEWorld::run_tensegrity(int stopOnFlag)
{

  //printf("run tens!\n");
  //  getchar();
  //  printf("COLLS DISABLED\n");
  _collisions_enabled = 1;
  //  printf("NETWORKS DISABLED\n");
  _use_networks = 1;
  // printf("ELEMENT 0 DISABLED!\n");
  dBodyEnable(_elements[0]->_body);
  //  dBodyDisable(_elements[0]->_body);
  // find network stuff here
  //  printf("stopOnFlag is %d\n",stopOnFlag);
  //dBodyEnable(_elements[0]->_body);
  dWorldSetGravity(world,_Horizontal_Gravity_Vector,0,-0.1); 
  //printf("GRAVITY IS 0\n");
  //dWorldSetGravity (world,0,0,0); 
  //render = 1;
  
  resetTens();
  attachStringsToNets();
  //dWorldSetGravity(world,_Horizontal_Gravity_Vector,0,-0.1); 

  cur_step = 0;
  _enable_ODE = 1;
  _stopFlag = 0;

  //  make_capsules();
  //printf("evaluate assembly:\n");
  //in_as->print();

  float maxforce = -1;

  time_t starttime = time(NULL);
 
  float avgvel = 0;
  
  vector<float> velmem;
  velmem.clear();

  centerOfMass(_currCOM);
  int MAX_STEPS = 10000;
  do
    {
      //      simLoop(0);
      if(render != 0)
	{
	  doGraphicsStuff();
	  dsSetColor (0,0,1);
	}
      
      if (cur_step == 5000)
	{
	  //  dBodyEnable(_elements[0]->_body);
	  //dWorldSetGravity(world,_Horizontal_Gravity_Vector,0,-0.1); 
	}
      if (_enable_ODE)
	{
	  resetForces();
	
	  if (_use_networks)
	    {
	      //  printf("running networks - curstep: %d\n",cur_step);
	      run_element_networks();
	    }

	  pull_strings();
	  addDrags();
	 
	  maxforce = getMaxForce();
	  g_maxforce = maxforce;
	  g_maxvel = getMaxVelocity();

	  velmem.push_back(g_maxvel);
	  if (velmem.size() > 100)
	      velmem.erase(velmem.begin());
	  
	  if (render == 1) drawObjects();
	  if (_collisions_enabled)
	    {
	      dJointGroupEmpty (contactgroup);
	      dSpaceCollide (space,0,&staticNearCallback);
	    }
	  //dSpaceCollide (space,0,&nearCallback);
	  //dWorldStep(world,0.01);
	  dWorldStep(world,0.007);
	  
	  if (velmem.size() > 50)
	    {
	      avgvel = 0;
	      for (uint i  = 0; i < velmem.size(); i++)
		{
		  avgvel+= velmem[i];
		}
	      avgvel = avgvel/velmem.size();
	    }
	}

      
      cur_step++;
      if (_stopFlag)
	break;
      // dsSetTexture (DS_WOOD);
      //draw the turtle

      //      getchar();
    } while (cur_step < 20000);

  float dt = distanceTraveled(_currCOM);
  //  printf("distance:%f\n",dt);
  //  printf("avgvel is: %f\n",avgvel); 
  //  printf("time elapsed: %d\n",endtime-starttime);

  /*
  if (render == 2)
    {
      doGraphicsStuff();
      drawObjects();
    }
  */
  //  printf("maxforce is: %f\n",maxforce);
}

void ODEWorld::addDrags()
{
  for (uint i = 0; i < _elements.size(); i++)
    {
      const dReal *vel = dBodyGetLinearVel(_elements[i]->_body);
      const dReal *avel = dBodyGetAngularVel(_elements[i]->_body);
      dVector3 drag;
      drag[0] = -1*vel[0]*_DAMP_CONSTANT;
      drag[1] = -1*vel[1]*_DAMP_CONSTANT;
      drag[2] = -1*vel[2]*_DAMP_CONSTANT;

      dVector3 adrag;
      adrag[0] = -1*avel[0]*_ANG_DAMP_CONSTANT;
      adrag[1] = -1*avel[1]*_ANG_DAMP_CONSTANT;
      adrag[2] = -1*avel[2]*_ANG_DAMP_CONSTANT;

      dBodyAddForce(_elements[i]->_body,drag[0],drag[1],drag[2]);
      dBodyAddTorque(_elements[i]->_body,adrag[0],adrag[1],adrag[2]);
      //    printf("adding drag (%f, %f, %f) to body %d\n",drag[0],drag[1],drag[2],i);
    }

}
void ODEWorld::printVelocities()
{
  for (uint i = 0; i < _elements.size(); i++)
    {
      const dReal *vel = dBodyGetLinearVel(_elements[i]->_body);
      const dReal *avel = dBodyGetAngularVel(_elements[i]->_body);
      
      float v1 = sqrt((vel[0]*vel[0])+(vel[1]*vel[1])+(vel[2]*vel[2]));
      float v2 = sqrt((avel[0]*avel[0])+(avel[1]*avel[1])+(avel[2]*avel[2]));

      printf("velocity of %d: (%f),(%f)\n",i, vel[0],vel[1],vel[2], avel[0],avel[1],avel[2]);

    }

}

void ODEWorld::resetForces()
{
  for (uint i = 0; i < _elements.size(); i++)
    {
      dBodySetForce(_elements[i]->_body,0,0,0);
      dBodySetTorque(_elements[i]->_body,0,0,0);
    }
}
void ODEWorld::print_forces()
{
  for (uint i = 0; i < _elements.size(); i++)
    {
      printf("force on element %d:",i);
      _elements[i]->print_force(); 
    }
  printf("\n");
  
}
void ODEWorld::run_interactive()
{
  render = 1;
  //printf("evaluate assembly:\n");
  //in_as->print();
  cur_cmd_index = 0;
  done_evaluating = 0;
  assembly_completed = 0;
  done_building= 0;
  is_glued = 0;
  have_melted_scaffolding = 0;
  cur_step = 0;
  reject = 0;
  _run_interactive = 1;
  _enable_ODE = 1;
  while(_run_interactive)
    {
      //      simLoop(0);
      doGraphicsStuff();
      if(render != 0)
	dsSetColor (0,0,2);
      
      if (_enable_ODE)
	{
	  dJointGroupEmpty (contactgroup);
	  dSpaceCollide (space,0,&staticNearCallback);
	  //dSpaceCollide (space,0,&nearCallback);
	  dWorldStep(world,0.05);
	}
      cur_step++;
      // dsSetTexture (DS_WOOD);
      //draw the turtle
      if (render!=0) drawObjects();
      
    }
}



void ODEWorld::set_new_margin()
{
  set_new_margin(_Margin*1.25);
}

void ODEWorld::resetView()
{

  float hpr[3] = {90.f,-0.6000f,-0.0000f};
  float xyz[3] = {0.6f,-2.5f,0.2600f};
  dsSetViewpoint (xyz,hpr);

}

void ODEWorld::zoomout()
{
  float newy = view_xyz[1]*2;
  
  float newview[3];
  
  newview[0] = view_xyz[0];
  newview[1] = newy;
  newview[2] = view_xyz[2];
  
  float xyz[3],hpr[3];
  dsGetViewpoint (xyz,hpr);
  printf("old ");
  printf ("Viewpoint = (%.4f,%.4f,%.4f,%.4f,%.4f,%.4f)\n",
	  xyz[0],xyz[1],xyz[2],hpr[0],hpr[1],hpr[2]);

  
  dsSetViewpoint(newview,view_hpr);
  dsGetViewpoint (xyz,hpr);
  printf("new ");
  printf ("Viewpoint = (%.4f,%.4f,%.4f,%.4f,%.4f,%.4f)\n",
	  xyz[0],xyz[1],xyz[2],hpr[0],hpr[1],hpr[2]);

}

void ODEWorld::set_new_margin(float val)
{

 
  float oldmargin = _Margin;
  printf("old margin: %f,",_Margin);

  _Margin = val;

  printf("new margin: %f\n,",_Margin);
  
  float newy = view_xyz[1]*(_Margin/oldmargin);
  
  float newview[3];
  
  newview[0] = view_xyz[0];
  newview[1] = newy;
  newview[2] = view_xyz[2];
  
  float xyz[3],hpr[3];
  dsGetViewpoint (xyz,hpr);
  printf("old ");
  printf ("Viewpoint = (%.4f,%.4f,%.4f,%.4f,%.4f,%.4f)\n",
	  xyz[0],xyz[1],xyz[2],hpr[0],hpr[1],hpr[2]);

  
  dsSetViewpoint(newview,view_hpr);
  dsGetViewpoint (xyz,hpr);
  printf("new ");
  printf ("Viewpoint = (%.4f,%.4f,%.4f,%.4f,%.4f,%.4f)\n",
	  xyz[0],xyz[1],xyz[2],hpr[0],hpr[1],hpr[2]);

  dBodySetPosition (upperLeftMarker.body,-MAX_X*_Margin,0,MAX_Z*_Margin);
  dBodySetPosition (upperRightMarker.body,+MAX_X*_Margin,0,MAX_Z*_Margin);

}


float ODEWorld::mass_of_objects()
{
  float mass = 0.;
  for (int i = 0; i < (int)objvect.size(); i++)
    {
      dMass curmass;
      dBodyGetMass(objvect[i].body,&curmass);
      mass += curmass.mass;
    }
  //about what a brick weighs
  return (float)rintf(mass/0.009851);
}


float ODEWorld::min_distance_between_objects(MyObject b1, MyObject b2)
{
  float dist = 1000.;

  for (int b1g = 0; b1g < (int)b1.geom.size(); b1g++)
    {
      dGeomID b1geom = b1.geom[b1g];
      if (b1geom != NULL)
	{
	  for (int b2g = 0; b2g < (int)b2.geom.size(); b2g++)
	    {
	      dGeomID b2geom = b2.geom[b2g];
	      if (b2geom != NULL)
		{
		  dist = min(min_distance_between_geoms(b1geom,b2geom),dist);
		}
	      else break;
	    }
	}
      else
	break;
    }

  return dist;
}

//min distance between two geoms is the minumum distance 
//between all corners of both of their bounding boxes.
float ODEWorld::min_distance_between_geoms(dGeomID o1, dGeomID o2)
{
  dReal dv1[6];
  dReal dv2[6];
  dGeomGetAABB(o1,dv1);
  dGeomGetAABB(o2,dv2);

  //all the corners of the bounding box for obj 1
  dReal o1Corners[8][3] = {{dv1[0],dv1[2],dv1[4]},
                           {dv1[1],dv1[2],dv1[4]},
			   {dv1[0],dv1[3],dv1[4]},
			   {dv1[1],dv1[3],dv1[4]},
			   {dv1[0],dv1[2],dv1[5]},
			   {dv1[1],dv1[2],dv1[5]},
			   {dv1[0],dv1[3],dv1[5]}, 
			   {dv1[1],dv1[3],dv1[5]}};


  //all the corners of the bounding box for obj 2
  dReal o2Corners[8][3] = {{dv2[0],dv2[2],dv2[4]},
                           {dv2[1],dv2[2],dv2[4]},
			   {dv2[0],dv2[3],dv2[4]},
			   {dv2[1],dv2[3],dv2[4]},
			   {dv2[0],dv2[2],dv2[5]},
			   {dv2[1],dv2[2],dv2[5]},
			   {dv2[0],dv2[3],dv2[5]},
			   {dv2[1],dv2[3],dv2[5]}};

  float distances[8][8];
  //look at all of the distances
  for (int o1ci = 0; o1ci < 8; o1ci++)
    {
      for (int o2ci = 0; o2ci < 8; o2ci++)
	{
	  distances[o1ci][o2ci] = (pow(o1Corners[o1ci][0] - o2Corners[o2ci][0],2) + //distance b/w xs
				   pow(o1Corners[o1ci][1] - o2Corners[o2ci][1],2) + //distance b/w ys
				   pow(o1Corners[o1ci][2] - o2Corners[o2ci][2],2)); //distance b/w zs
	}

    }

  float good_mindist;

  //if I wanted to be super-kewl I could calculate this in the loop above
  //but this is more clear.
  for (int i = 0; i < 8; i++)
    for (int j = 0; j < 8; j++)
      {
	if ((i == 0)&& (j == 0))
	  good_mindist = distances[i][j];
	
	good_mindist = min(good_mindist,distances[i][j]);
	
      }

  return good_mindist;
}

MyObject  ODEWorld::object_at(dReal xv, dReal yv, dReal zv)
{

  MyObject theobject;
  
  theobject.geom[0] = NULL;

  for (int k=0; k<(int)objvect.size(); k++) {
    if (dBodyIsEnabled(objvect[k].body))
      {
	for (int g = 0; g < (int)objvect[k].geom.size(); g++)
	  {
	    if (objvect[k].geom[g] != NULL)
	      {
		dReal depth;
		const dReal *rv;
		
		// if we don't check then the depth will be wrong
		if (dGeomGetClass(objvect[k].geom[g]) != dGeomTransformClass)
		  {
		    depth = dGeomBoxPointDepth(objvect[k].geom[g],xv,yv,zv);
		    rv = dBodyGetRotation(objvect[k].body);
		  }
		else
		  {
		    const dReal *bodypos = dBodyGetPosition(objvect[k].body);
		    dReal relx = xv - bodypos[0];
		    dReal rely = yv - bodypos[1];
		    dReal relz = zv - bodypos[2];
		    //depth = dGeomBoxPointDepth(dGeomTransformGetGeom(objvect[k].geom[g]),xv,yv,zv);
		    depth = dGeomBoxPointDepth(dGeomTransformGetGeom(objvect[k].geom[g]),relx,rely,relz);
		    //depth = dGeomBoxPointDepth(objvect[k].geom[g],relx,rely,relz);
		    rv = dGeomGetRotation(dGeomTransformGetGeom(objvect[k].geom[g]));
		  }
		if (depth > 0)
		  {
		    return objvect[k];
		  }
	      }
	    else break;
	  }
      }
  }
  
  return theobject;
}


void ODEWorld::create_markers()
{
 
  dReal sides[3];
  dMass m;
  upperLeftMarker.body = dBodyCreate(world);
  upperLeftMarker.rc = 0;
  upperLeftMarker.gc = 0;
  upperLeftMarker.bc = 1;
  dBodySetPosition (upperLeftMarker.body,-MAX_X*_Margin,0,MAX_Z*_Margin);
  sides[0] = 0.05;
  dBodySetData (upperLeftMarker.body,(void*) 0);
  //  dMassSetZero(&m);
  dMassSetZero(&m);
  upperLeftMarker.geom.clear();
  dGeomID sphere = dCreateSphere (space,sides[0]);
  upperLeftMarker.geom.push_back(sphere);
  /*
  for (int g = 1; g < GPB; g++)
    {
      upperLeftMarker.geom[g] = NULL;
    }
  */ 
  int k;
  for (k=0; k < (int)upperLeftMarker.geom.size(); k++) {
    if (upperLeftMarker.geom[k]) 
      dGeomSetBody(upperLeftMarker.geom[k],upperLeftMarker.body);
  }
  dBodySetMass(upperLeftMarker.body,&m);
  dBodyDisable (upperLeftMarker.body);
  
  
  upperRightMarker.body = dBodyCreate(world);
  upperRightMarker.rc = 0;
  upperRightMarker.gc = 0;
  upperRightMarker.bc = 1;
  dBodySetPosition (upperRightMarker.body,+MAX_X*_Margin,0,MAX_Z*_Margin);
  sides[0] = 0.05;
  dBodySetData (upperRightMarker.body,(void*) 0);
  //  dMassSetZero(&m);
  dMassSetZero(&m);
  upperRightMarker.geom.clear();
  dGeomID ursphere = dCreateSphere (space,sides[0]);
  upperRightMarker.geom.push_back(ursphere);  
  dGeomSetBody(ursphere,upperRightMarker.body);
  /*
  for (int g = 1; g < GPB; g++)
    {
      upperRightMarker.geom[g] = NULL;
    }
  
  for (k=0; k < GPB; k++) {
    if (upperRightMarker.geom[k]) 
      dGeomSetBody(upperRightMarker.geom[k],upperRightMarker.body);
  }
  */
  dBodySetMass(upperRightMarker.body,&m);
  dBodyDisable (upperRightMarker.body);
}

void ODEWorld::create_turtle()
{
 
  printf("creating turtle world is: %d\n",world);
  dReal sides[3];
  dMass m;
  printf("creating body\n");
  turtle.body = dBodyCreate(world);
  turtle.rc = 1;
  turtle.gc = 0;
  turtle.bc = 0;
  printf("setting position\n");
  dBodySetPosition (turtle.body,xpos,ypos,zpos);
  //sides[0] = 0.05;
  sides[0] = BRICK_WIDTH/2;
  sides[1] = BRICK_WIDTH/2;
  sides[2] = BRICK_WIDTH;
  
  printf("setting data\n");
  //dBodySetData (turtle.body,(void*) 0);
  //  dMassSetZero(&m);
  printf("setting mass\n");
  dMassSetZero(&m);
  printf("creating sphere\n");
  // turtle.geom[0] = dCreateSphere (space,sides[0]);
  turtle.geom.clear();
  dGeomID tg = dCreateBox(space,sides[0],sides[1],sides[2]);
  turtle.geom.push_back(tg); 
  dGeomSetBody(tg,turtle.body);
  /*
  for (int g = 1; g < ; g++)
    {
      turtle.geom[g] = NULL;
    }
  
  
  int k;
  for (k=0; k < GPB; k++) {
    //printf("setting geom body %d for turtle\n",k);
    if (turtle.geom[k]) 
      dGeomSetBody(turtle.geom[k],turtle.body);
  }
  */
  printf("setting body mass\n");
  dBodySetMass(turtle.body,&m);
  printf("disabling\n");
  dBodyDisable (turtle.body);
  printf("done creating turtle...\n");

}

void ODEWorld::create_world()
{
  world = dWorldCreate();
  space = dHashSpaceCreate (0);
  contactgroup = dJointGroupCreate (0);
  dWorldSetGravity(world,_Horizontal_Gravity_Vector,0,-0.5); 
  // this is better than tilting the plane -- which GL doesn't enjoy doing
  //dWorldSetGravity (world,-0.05,0,-0.5); 
  //smaller numbers tend to make it more stable...?
  dWorldSetCFM (world,1e-5);
  //dWorldSetERP (world,0.00002);
  printf(" ERP is: %f\n",dWorldGetERP(world)); //default is 0.2
  //dWorldSetCFM (world,0);
  /*
    Set and get the maximum correcting velocity that contacts are allowed to 
    generate. The default value is infinity (i.e. no limit). Reducing this 
    value can help prevent "popping" of deeply embedded objects. 
  */
  dWorldSetContactMaxCorrectingVel (world,0.1);
  /*
    Set and get the depth of the surface layer around all geometry objects. 
    Contacts are allowed to sink into the surface layer up to the given depth 
    before coming to rest. The default value is zero. Increasing this to some 
    small value (e.g. 0.001) can help prevent jittering problems due to 
    contacts being repeatedly made and broken. 
  */
  dWorldSetContactSurfaceLayer (world,0.001);
  dCreatePlane (space,0,0,1,0);
  printf("World id is:%d\n",world);
}


//used just to detect collisions and stuff
void ODEWorld::staticNearCallbackCustom(void *data, dGeomID o1, dGeomID o2)
{
   globalself->nearCallbackCustom(o1, o2);
}

//this version "remembers" the contacts
// and so can be used to figure out whether
// an object will collide before it is placed
void ODEWorld::nearCallbackCustom(dGeomID o1, dGeomID o2)
{

  // getchar();

  int i;
  if ((!o1) || (!o2))
    {
      printf("one of the geoms doesn't exist!\n");
      return;
    }

  dBodyID b1 = dGeomGetBody(o1);
  dBodyID b2 = dGeomGetBody(o2);
 
  int foundB1 = 0;
  int foundB2 = 0;
  for (uint e = 0; e < _elements.size(); e++)
    {
      Element *curel = _elements[e];
      dBodyID elbod = curel->_body;
      if (elbod == b1)
	{
	  foundB1 = 1;
	  //	  printf("b1 belongs to element %d\n",e);
	}
      else if (elbod == b2)
	{
	foundB2 = 1;
	//	printf("b2 belongs to element %d\n",e);
	}
    }


  if (b1 == b2) // don't collide if connected to the same body
    {
      return;
    }
  //if (b1 && b2 && !((dBodyIsEnabled(b1))&&(dBodyIsEnabled(b2)))) { //  printf("I won't collide a disabled object!\n"); return; }
  if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact)) return;

  if (!foundB1 || !foundB2) return;


  dContact contact[MAX_CONTACTS];   // up to MAX_CONTACTS contacts per box-box
  for (i=0; i<MAX_CONTACTS; i++) {
    /*
    contact[i].surface.mode = dContactBounce | dContactSoftCFM;
    contact[i].surface.mu = dInfinity;
    contact[i].surface.mu2 = 0;
    contact[i].surface.bounce = 0;
    contact[i].surface.bounce_vel = dInfinity;
    contact[i].surface.soft_cfm = 0.01;
    */
    contact[i].surface.mode = dContactBounce | dContactSoftCFM;
    // mu is friction
    contact[i].surface.mu = dInfinity;
    contact[i].surface.mu2 = 0;
    contact[i].surface.bounce = 0.1;
    contact[i].surface.bounce_vel = 0.1;
    contact[i].surface.soft_cfm = 0.01;
  } 
  if (int numc = dCollide (o1,o2,MAX_CONTACTS,&contact[0].geom,
			   sizeof(dContact))) {
    dMatrix3 RI;
    dRSetIdentity (RI);
    const dReal ss[3] = {0.02,0.02,0.02};
    for (i=0; i<numc; i++) {
      //printf("depth is %f\n",contact[i].geom.depth);
      moduleContacts.push_back(contact[i]);
      if (_collisions_enabled)
	{
	  dJointID c = dJointCreateContact (world,contactgroup,contact+i);
	  dJointAttach (c,b1,b2);
	}
      //if (show_contacts) dsDrawBox (contact[i].geom.pos,RI,ss);
    }
  }

  //  getchar();
}

// this is called by dSpaceCollide when two objects in space are
// potentially colliding.

//this one is staticit has no "this" pointer,
// nor any of the other members of the World class.
// (from wiki)
void ODEWorld::staticNearCallback (void *data, dGeomID o1, dGeomID o2)
{
  // pData is the "this" that was passed into dSpaceCollide
  //ODEWorld* pThis = (ODEWorld*)data;
  //if (pThis)
  globalself->nearCallback(o1, o2);
    //   pThis->nearCallback(o1, o2);
}
void ODEWorld::nearCallback(dGeomID o1, dGeomID o2)
{
  
  int i;
  if ((!o1) || (!o2))
    {
      printf("one of the geoms doesn't exist!\n");
      return;
    }

  // if (o1->body && o2->body) return;

  // exit without doing anything if the two bodies are connected by a joint
  dBodyID b1 = dGeomGetBody(o1);
  dBodyID b2 = dGeomGetBody(o2);


  //if (!b1 || !b2) //if not attached to a body, ignore { return; }

  if (b1 == b2) // don't collide if connected to the same body
    {
      return;
    }
  if (b1 && b2 && !((dBodyIsEnabled(b1))&&(dBodyIsEnabled(b2))))
    {
      //  printf("I won't collide a disabled object!\n");
    return;
    }
  if (b1 && b2 && dAreConnectedExcluding (b1,b2,dJointTypeContact)) return;


  /* 
  printf("ODEWorld::nearCallback - colliding these geoms\n"); 
  describeGeom(o1); 
  describeGeom(o2); 
  */
  

  dContact contact[MAX_CONTACTS];   // up to MAX_CONTACTS contacts per box-box
  for (i=0; i<MAX_CONTACTS; i++) {
    /*

    //THESE ARE IMPORTANT PARAMETERS OF CONTACTS! -JR
    contact[i].surface.mode = dContactBounce | dContactSoftCFM;
    //columb friction. try setting to zero
    //nope! that makes things slide away as if they were ice.
    //infinity is better
    contact[i].surface.mu = dInfinity;
    contact[i].surface.mu2 = 0;
    //bounciness - try setting to zero
    //contact[i].surface.bounce = 0.1;
    contact[i].surface.bounce = 0;
    // min velocity needed to bounce
    // contact[i].surface.bounce_vel = 0.1;
    contact[i].surface.bounce_vel = dInfinity;
    contact[i].surface.soft_cfm = 0.01;
    */

    contact[i].surface.mode = dContactBounce | dContactSoftCFM;
    // mu is friction
    contact[i].surface.mu = dInfinity;
    contact[i].surface.mu2 = 0;
    contact[i].surface.bounce = 0.1;
    contact[i].surface.bounce_vel = 0.1;
    contact[i].surface.soft_cfm = 0.01;
  }
  if (int numc = dCollide (o1,o2,MAX_CONTACTS,&contact[0].geom,
			   sizeof(dContact))) {
    dMatrix3 RI;
    dRSetIdentity (RI);
    const dReal ss[3] = {0.02,0.02,0.02};
    for (i=0; i<numc; i++) {
      dJointID c = dJointCreateContact (world,contactgroup,contact+i);
      dJointAttach (c,b1,b2);
      //  if (show_contacts) dsDrawBox (contact[i].geom.pos,RI,ss);
    }
  }
}


void ODEWorld::doGraphicsStuff()
{
  XEvent event;
    while (XPending (display)) {
      XNextEvent (display,&event);
      handleEvent(event);
    } 
    
    dsDrawFrame (width,height);
    
    //  glFlush();
    //glXSwapBuffers (display,win);
    //XSync (display,0);					 

}


void ODEWorld::dsDrawFrame (int width, int height)
{
  //if (current_state < 1) dsDebug ("internal error");
  //current_state = 2;

  // setup stuff
  glEnable (GL_LIGHTING);
  glEnable (GL_LIGHT0);
  glDisable (GL_TEXTURE_2D);
  glDisable (GL_TEXTURE_GEN_S);
  glDisable (GL_TEXTURE_GEN_T);
  glShadeModel (GL_FLAT);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LESS);
  glEnable (GL_CULL_FACE);
  glCullFace (GL_BACK);
  glFrontFace (GL_CCW);

  // setup viewport
  glViewport (0,0,width,height);
  glMatrixMode (GL_PROJECTION);
  glLoadIdentity();
  const float vnear = 0.1f;
  const float vfar = 100.0f;
  const float k = 0.8f;     // view scale, 1 = +/- 45 degrees
  if (width >= height) {
    float k2 = float(height)/float(width);
    glFrustum (-vnear*k,vnear*k,-vnear*k*k2,vnear*k*k2,vnear,vfar);
  }
  else {
    float k2 = float(width)/float(height);
    glFrustum (-vnear*k*k2,vnear*k*k2,-vnear*k,vnear*k,vnear,vfar);
  }

  // setup lights. it makes a difference whether this is done in the
  // GL_PROJECTION matrix mode (lights are scene relative) or the
  // GL_MODELVIEW matrix mode (lights are camera relative, bad!).
  static GLfloat light_ambient[] = { 0.5, 0.5, 0.5, 1.0 };
  static GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
  static GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
  glLightfv (GL_LIGHT0, GL_AMBIENT, light_ambient);
  glLightfv (GL_LIGHT0, GL_DIFFUSE, light_diffuse);
  glLightfv (GL_LIGHT0, GL_SPECULAR, light_specular);
  glColor3f (1.0, 1.0, 1.0);

  // clear the window
  glClearColor (0.5,0.5,0.5,0);
  glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  // snapshot camera position (in MS Windows it is changed by the GUI thread)
  float view2_xyz[3];
  float view2_hpr[3];
  memcpy (view2_xyz,view_xyz,sizeof(float)*3);
  memcpy (view2_hpr,view_hpr,sizeof(float)*3);

  // go to GL_MODELVIEW matrix mode and set the camera
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
  setCamera (view2_xyz[0],view2_xyz[1],view2_xyz[2],
	     view2_hpr[0],view2_hpr[1],view2_hpr[2]);

  // set the light position (for some reason we have to do this in model view.
  static GLfloat light_position[] = { LIGHTX, LIGHTY, 1.0, 0.0 };
  glLightfv (GL_LIGHT0, GL_POSITION, light_position);

  // draw the background (ground, sky etc)
  drawSky (view2_xyz);
  drawGround();

  // draw the little markers on the ground
  drawPyramidGrid();

  // leave openGL in a known state - flat shaded white, no textures
  glEnable (GL_LIGHTING);
  glDisable (GL_TEXTURE_2D);
  glShadeModel (GL_FLAT);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LESS);
  glColor3f (1,1,1);
  setColor (1,1,1,1);

  // draw the rest of the objects. set drawing state first.
  color[0] = 1;
  color[1] = 1;
  color[2] = 1;
  color[3] = 1;
  //  tnum = 0;
  // if (fn->step) fn->step (pause);
}

void ODEWorld::setCamera (float x, float y, float z, float h, float p, float r)
{
  glMatrixMode (GL_MODELVIEW);
  glLoadIdentity();
  glRotatef (90, 0,0,1);
  glRotatef (90, 0,1,0);
  glRotatef (r, 1,0,0);
  glRotatef (p, 0,1,0);
  glRotatef (-h, 0,0,1);
  glTranslatef (-x,-y,-z);
}
void ODEWorld::drawGround()
{
  //printf("draw ground!\n");
  glDisable (GL_LIGHTING);
  glShadeModel (GL_FLAT);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LESS);
  // glDepthRange (1,1);

  if (use_textures) {
    // printf("drawing round texture!\n");
    glEnable (GL_TEXTURE_2D);
    ground_texture->bind (0);
  }
  else {
    glDisable (GL_TEXTURE_2D);
    glColor3f (GROUND_R,GROUND_G,GROUND_B);
    //glColor3f (1.,1.,1.); //use this for pretty publication pix
  }

  // ground fog seems to cause problems with TNT2 under windows
  /*
  GLfloat fogColor[4] = {0.5, 0.5, 0.5, 1};
  glEnable (GL_FOG);
  glFogi (GL_FOG_MODE, GL_EXP2);
  glFogfv (GL_FOG_COLOR, fogColor);
  glFogf (GL_FOG_DENSITY, 0.05f);
  glHint (GL_FOG_HINT, GL_NICEST); // GL_DONT_CARE);
  glFogf (GL_FOG_START, 1.0);
  glFogf (GL_FOG_END, 5.0);
  */

  const float gsize = 100.0f;
  const float offset = 0; // -0.001f; ... polygon offsetting doesn't work well

  glBegin (GL_QUADS);
  glNormal3f (0,0,1);
  glTexCoord2f (-gsize*ground_scale + ground_ofsx,
		-gsize*ground_scale + ground_ofsy);
  glVertex3f (-gsize,-gsize,offset);
  glTexCoord2f (gsize*ground_scale + ground_ofsx,
		-gsize*ground_scale + ground_ofsy);
  glVertex3f (gsize,-gsize,offset);
  glTexCoord2f (gsize*ground_scale + ground_ofsx,
		gsize*ground_scale + ground_ofsy);
  glVertex3f (gsize,gsize,offset);
  glTexCoord2f (-gsize*ground_scale + ground_ofsx,
		gsize*ground_scale + ground_ofsy);
  glVertex3f (-gsize,gsize,offset);
  glEnd();

  glDisable (GL_FOG);
}

void ODEWorld::setColor (float r, float g, float b, float alpha)
{
  GLfloat light_ambient[4],light_diffuse[4],light_specular[4];
  light_ambient[0] = r*0.3f;
  light_ambient[1] = g*0.3f;
  light_ambient[2] = b*0.3f;
  light_ambient[3] = alpha;
  light_diffuse[0] = r*0.7f;
  light_diffuse[1] = g*0.7f;
  light_diffuse[2] = b*0.7f;
  light_diffuse[3] = alpha;
  light_specular[0] = r*0.2f;
  light_specular[1] = g*0.2f;
  light_specular[2] = b*0.2f;
  light_specular[3] = alpha;
  glMaterialfv (GL_FRONT_AND_BACK, GL_AMBIENT, light_ambient);
  glMaterialfv (GL_FRONT_AND_BACK, GL_DIFFUSE, light_diffuse);
  glMaterialfv (GL_FRONT_AND_BACK, GL_SPECULAR, light_specular);
  glMaterialf (GL_FRONT_AND_BACK, GL_SHININESS, 5.0f);
}
void ODEWorld::drawPyramidGrid()
{
  // setup stuff
  glEnable (GL_LIGHTING);
  glDisable (GL_TEXTURE_2D);
  glShadeModel (GL_FLAT);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LESS);

  // draw the pyramid grid
  for (int i=-1; i<=1; i++) {
    for (int j=-1; j<=1; j++) {
      glPushMatrix();
      glTranslatef ((float)i,(float)j,(float)0);
      if (i==1 && j==0) setColor (1,0,0,1);
      else if (i==0 && j==1) setColor (0,0,1,1);
      else setColor (1,1,0,1);
      const float k = 0.03f;
      glBegin (GL_TRIANGLE_FAN);
      glNormal3f (0,-1,1);
      glVertex3f (0,0,k);
      glVertex3f (-k,-k,0);
      glVertex3f ( k,-k,0);
      glNormal3f (1,0,1);
      glVertex3f ( k, k,0);
      glNormal3f (0,1,1);
      glVertex3f (-k, k,0);
      glNormal3f (-1,0,1);
      glVertex3f (-k,-k,0);
      glEnd();
      glPopMatrix();
    }
  }
}

void ODEWorld::drawSky (float view_xyz[3])
{
  glDisable (GL_LIGHTING);
  /*
  if (use_textures) {
    glEnable (GL_TEXTURE_2D); 
    sky_texture->bind (0); 
    }
  else {
  */
  glDisable (GL_TEXTURE_2D);
  //glColor3f (0,0.5,1.0);
  glColor3f (SKY_RED,SKY_GREEN,SKY_BLUE);
     // glColor3f (1.,1.,1.0); //use for pretty publication pix
 // }

  // make sure sky depth is as far back as possible
  glShadeModel (GL_FLAT);
  glEnable (GL_DEPTH_TEST);
  glDepthFunc (GL_LEQUAL);
  glDepthRange (1,1);

  const float ssize = 1000.0f;
  static float offset = 0.0f;

  float x = ssize*sky_scale;
  float z = view_xyz[2] + sky_height;

  glBegin (GL_QUADS);
  glNormal3f (0,0,-1);
  glTexCoord2f (-x+offset,-x+offset);
  glVertex3f (-ssize+view_xyz[0],-ssize+view_xyz[1],z);
  glTexCoord2f (-x+offset,x+offset);
  glVertex3f (-ssize+view_xyz[0],ssize+view_xyz[1],z);
  glTexCoord2f (x+offset,x+offset);
  glVertex3f (ssize+view_xyz[0],ssize+view_xyz[1],z);
  glTexCoord2f (x+offset,-x+offset);
  glVertex3f (ssize+view_xyz[0],-ssize+view_xyz[1],z);
  glEnd();

  offset = offset + 0.002f;
  if (offset > 1) offset -= 1;

  glDepthFunc (GL_LESS);
  glDepthRange (0,1);
} 
  
void ODEWorld::handleEvent (XEvent &event)
{
  static int mx=0,my=0; 	// mouse position
  static int mode = 0;		// mouse button bits

  switch (event.type) {

  case ButtonPress: {
    if (event.xbutton.button == Button1) mode |= 1;
    if (event.xbutton.button == Button2) mode |= 2;
    if (event.xbutton.button == Button3) mode |= 4;
    mx = event.xbutton.x;
    my = event.xbutton.y;
  }
  return;

  case ButtonRelease: {
    if (event.xbutton.button == Button1) mode &= (~1);
    if (event.xbutton.button == Button2) mode &= (~2);
    if (event.xbutton.button == Button3) mode &= (~4);
    mx = event.xbutton.x;
    my = event.xbutton.x;
  }
  return;

  case MotionNotify: {
    if (event.xmotion.is_hint) {
      Window root,child;
      unsigned int mask;
      XQueryPointer (display,win,&root,&child,&event.xbutton.x_root,
		     &event.xbutton.y_root,&event.xbutton.x,&event.xbutton.y,
		     &mask);
    }
    dsMotion (mode, event.xmotion.x - mx, event.xmotion.y - my);
    mx = event.xmotion.x;
    my = event.xmotion.y;
  }
  return;

  case KeyPress: {
    KeySym key;
    XLookupString (&event.xkey,NULL,0,&key,0);
    if ((event.xkey.state & ControlMask) == 0) {
      //if (key >= ' ' && key <= 126 && fn->command) fn->command (key);
      if (key >= ' ' && key <= 126) command(key);
    }
    else if (event.xkey.state & ControlMask) {
      switch (key) {
      case 'r':
	resetView();
	break;
      case 'b':
	zoomout();
	break;
      case 't': case 'T':
	dsSetTextures (dsGetTextures() ^ 1);
	//use_textures = !use_textures;
	printf("use texture now %d!\n",use_textures);
	break;
      case 's': case 'S':
	dsSetShadows (dsGetShadows() ^ 1);
	break;
      case 'x': case 'X':
	run = 0;
	break;
      case 'p': case 'P':
	pause ^= 1;
	singlestep = 0;
	break;
      case 'o': case 'O':
	if (pause) singlestep = 1;
	break;
      case 'v': case 'V': {
	float xyz[3],hpr[3];
	dsGetViewpoint (xyz,hpr);
	printf ("Viewpoint = (%.4f,%.4f,%.4f,%.4f,%.4f,%.4f)\n",
		xyz[0],xyz[1],xyz[2],hpr[0],hpr[1],hpr[2]);
	break;
      }
      case 'w': case 'W':
	writeframes ^= 1;
	if (writeframes) printf ("Now writing frames to PPM files\n");
	break;
      }
    }
    last_key_pressed = key;		// a kludgy place to put this...
  }
  return;

  case KeyRelease: {
    // hmmmm...
  }
  return;

  case ClientMessage:
    if (event.xclient.message_type == wm_protocols_atom &&
	event.xclient.format == 32 &&
	Atom(event.xclient.data.l[0]) == wm_delete_window_atom) {
      run = 0;
      return;
    }
    return;

  case ConfigureNotify:
    width = event.xconfigure.width;
    height = event.xconfigure.height;
    return;
  }
}

// this is because encapsulated geoms aren't associated with a body, so can't call 
// dGeomGetBody to figure out which body it's attached to
int ODEWorld::geom_is_in_object(dGeomID ingeom, MyObject inobj)
{
  //printf("is %d a geom in %d?\n",ingeom,inobj);
  for (int i = 0; i < (int)inobj.geom.size(); i++)
    {
     
      dGeomID curgeom = inobj.geom[i];
      
      if (curgeom != NULL)
	{
	  if (curgeom == ingeom)
	    return 1;
	  
	  if (dGeomGetClass(curgeom) == dGeomTransformClass)
	    {
	      // my instinct is to do a recursive call
	      //with dGeomTransformGetGeom and inobj
	      // but that will only cause an infinite loop
	      // becaause we'll keep hitting the same geomtransform in the inobj
	      // anc keep recursing down it, which isn't what we want.
	      // so let's assume that GeomTransforms never encapsulate
	      // other geomTransforms
	      if (dGeomTransformGetGeom(curgeom) == ingeom)
		return 1;
	    }
	}
    }
  return 0;
}

int ODEWorld::countCollisions()
{
   moduleContacts.clear();
   dSpaceCollide (space,0,&staticNearCallbackCustom);
   int col = (int)moduleContacts.size();
   //   printf("there are %d collissions!\n",col);
   return col;
}

float ODEWorld::contactDepths()
{
  float depth = 0;
   moduleContacts.clear();
   dSpaceCollide (space,0,&staticNearCallbackCustom);
   //   int col = (int)moduleContacts.size();
   //printf("there are %d collissions!\n",col);
   for (uint i = 0; i < moduleContacts.size(); i++)
     {
       depth+= moduleContacts[i].geom.depth;
     }
   return depth ;

}

int ODEWorld::can_place_object(MyObject inobj)
{
  //printf("can place obj!\n");
  int collides = 0;

  moduleContacts.clear();
  dSpaceCollide (space,0,&staticNearCallbackCustom);
  
  //printf("there are %d contacts saved\n",(int)moduleContacts.size());
  for (int i = 0; i < (int)moduleContacts.size(); i++)
    {
      dContact curcontact = moduleContacts[i];
      //      printf("geom ids are: %d, %d and body ids are: %d,%d -- newobj body is: %d\n", curcontact.geom.g1, curcontact.geom.g2, dGeomGetBody(curcontact.geom.g1), dGeomGetBody(curcontact.geom.g2), inobj.body);
      
      if ((geom_is_in_object(curcontact.geom.g1,inobj)) ||
	  (geom_is_in_object(curcontact.geom.g2,inobj)))
	{
      
	  //printf("curcontact depth is: %f\n",curcontact.geom.depth);
	  
	  if (curcontact.geom.depth > MAX_PENETRATION_DEPTH)
	    {
	      //   printf("too much depth! %f\n",curcontact.geom.depth);
	      collides = 1;
	      break;
	    }
	}
    }	
  
  return !collides;
}


void ODEWorld::createMainWindow (int _width, int _height)
{
  // create X11 display connection
  display = XOpenDisplay (NULL);
  //if (!display) dsError ("can not open X11 display");
  screen = DefaultScreen(display);

  // get GL visual
  static int attribList[] = {GLX_RGBA, GLX_DOUBLEBUFFER, GLX_DEPTH_SIZE,16,
			     GLX_RED_SIZE,4, GLX_GREEN_SIZE,4,
			     GLX_BLUE_SIZE,4, None};
  visual = glXChooseVisual (display,screen,attribList);
  //if (!visual) dsError ("no good X11 visual found for OpenGL");

  // create colormap
  colormap = XCreateColormap (display,RootWindow(display,screen),
			      visual->visual,AllocNone);

  // initialize variables
  win = 0;
  width = _width;
  height = _height;
  glx_context = 0;
  last_key_pressed = 0;

  //if (width < 1 || height < 1) dsDebug (0,"bad window width or height");

  // create the window
  XSetWindowAttributes attributes;
  attributes.background_pixel = BlackPixel(display,screen);
  attributes.colormap = colormap;
  attributes.event_mask = ButtonPressMask | ButtonReleaseMask |
    KeyPressMask | KeyReleaseMask | ButtonMotionMask | PointerMotionHintMask |
    StructureNotifyMask;
  win = XCreateWindow (display,RootWindow(display,screen),50,50,width,height,
		       0,visual->depth, InputOutput,visual->visual,
		       CWBackPixel | CWColormap | CWEventMask,&attributes);

  // associate a GLX context with the window
  glx_context = glXCreateContext (display,visual,0,GL_TRUE);
  //if (!glx_context) dsError ("can't make an OpenGL context");

  // set the window title
  XTextProperty window_name;
  window_name.value = (unsigned char *) "Simulation";
  window_name.encoding = XA_STRING;
  window_name.format = 8;
  window_name.nitems = strlen((char *) window_name.value);
  XSetWMName (display,win,&window_name);

  // participate in the window manager 'delete yourself' protocol
  wm_protocols_atom = XInternAtom (display,"WM_PROTOCOLS",False);
  wm_delete_window_atom = XInternAtom (display,"WM_DELETE_WINDOW",False);
  //if (XSetWMProtocols (display,win,&wm_delete_window_atom,1)==0) dsError ("XSetWMProtocols() call failed");

  // pop up the window
  XMapWindow (display,win);
  XSync (display,win);
}

void ODEWorld::dsSetViewpoint (float xyz[3], float hpr[3])
{
 if (xyz) {
    view_xyz[0] = xyz[0];
    view_xyz[1] = xyz[1];
    view_xyz[2] = xyz[2];
  }
  if (hpr) {
    view_hpr[0] = hpr[0];
    view_hpr[1] = hpr[1];
    view_hpr[2] = hpr[2];
    wrapCameraAngles();
  }
}
void ODEWorld::dsSetTextures(int a)
{
  use_textures = (a != 0);
  tnum = use_textures;
}
void ODEWorld::drawCapsule (float l, float r)
{
  int i,j;
  float tmp,nx,ny,nz,start_nx,start_ny,a,ca,sa;
  // number of sides to the cylinder (divisible by 4):
  const int n = capped_cylinder_quality*4;

  l *= 0.5;
  a = float(M_PI*2.0)/float(n);
  sa = (float) sin(a);
  ca = (float) cos(a);

  // draw cylinder body
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_STRIP);
  for (i=0; i<=n; i++) {
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,l);
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,-l);
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw first cylinder cap
  start_nx = 0;
  start_ny = 1;
  for (j=0; j<(n/4); j++) {
    // get start_n2 = rotated start_n
    float start_nx2 =  ca*start_nx + sa*start_ny;
    float start_ny2 = -sa*start_nx + ca*start_ny;
    // get n=start_n and n2=start_n2
    nx = start_nx; ny = start_ny; nz = 0;
    float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
    glBegin (GL_TRIANGLE_STRIP); 
    glClear (GL_COLOR_BUFFER_BIT);
    //glColor3f(1.0f,0.0f,0.0f);
    for (i=0; i<=n; i++) {
      glNormal3d (ny2,nz2,nx2);
      glVertex3d (ny2*r,nz2*r,l+nx2*r);
      glNormal3d (ny,nz,nx);
      glVertex3d (ny*r,nz*r,l+nx*r);
      // rotate n,n2
      tmp = ca*ny - sa*nz;
      nz = sa*ny + ca*nz;
      ny = tmp;
      tmp = ca*ny2- sa*nz2;
      nz2 = sa*ny2 + ca*nz2;
      ny2 = tmp;
    }
    glEnd();
    start_nx = start_nx2;
    start_ny = start_ny2;
  }

  // draw second cylinder cap
  start_nx = 0;
  start_ny = 1;
  for (j=0; j<(n/4); j++) {
    // get start_n2 = rotated start_n
    float start_nx2 = ca*start_nx - sa*start_ny;
    float start_ny2 = sa*start_nx + ca*start_ny;
    // get n=start_n and n2=start_n2
    nx = start_nx; ny = start_ny; nz = 0;
    float nx2 = start_nx2, ny2 = start_ny2, nz2 = 0;
    glBegin (GL_TRIANGLE_STRIP);
    glClear (GL_COLOR_BUFFER_BIT);
    //glColor3f(0.0f,0.0f,1.0f);
    for (i=0; i<=n; i++) {
      glNormal3d (ny,nz,nx);
      glVertex3d (ny*r,nz*r,-l+nx*r);
      glNormal3d (ny2,nz2,nx2);
      glVertex3d (ny2*r,nz2*r,-l+nx2*r);
      // rotate n,n2
      tmp = ca*ny - sa*nz;
      nz = sa*ny + ca*nz;
      ny = tmp;
      tmp = ca*ny2- sa*nz2;
      nz2 = sa*ny2 + ca*nz2;
      ny2 = tmp;
    }
    glEnd();
    start_nx = start_nx2;
    start_ny = start_ny2;
  }

  glPopMatrix();
}

void ODEWorld::dsDrawCapsule (const float pos[3], const float R[12],
				      float length, float radius)
{
  //  if (current_state != 2) dsError ("drawing function called outside simulation loop");
  setupDrawingMode();
  glShadeModel (GL_SMOOTH);
  setTransform (pos,R);
  drawCapsule (length,radius);
  glPopMatrix();

  if (use_shadows) {
    setShadowDrawingMode();
    setShadowTransform();
    setTransform (pos,R);
    drawCapsule (length,radius);
    glPopMatrix();
    glPopMatrix();
    glDepthRange (0,1);
  }
}
/*
void ODEWorld::drawCylinder (float l, float r, float zoffset)
{
  int i;
  float tmp,ny,nz,a,ca,sa;
  const int n = 24;	// number of sides to the cylinder (divisible by 4)

  l *= 0.5;
  a = float(M_PI*2.0)/float(n);
  sa = (float) sin(a);
  ca = (float) cos(a);

  // draw cylinder body
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_STRIP);
  for (i=0; i<=n; i++) {
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,l+zoffset);
    glNormal3d (ny,nz,0);
    glVertex3d (ny*r,nz*r,-l+zoffset);
    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw top cap
  glShadeModel (GL_FLAT);
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);
  glNormal3d (0,0,1);
  glVertex3d (0,0,l+zoffset);
  for (i=0; i<=n; i++) {
    if (i==1 || i==n/2+1)
      setColor (color[0]*0.75f,color[1]*0.75f,color[2]*0.75f,color[3]);
    glNormal3d (0,0,1);
    glVertex3d (ny*r,nz*r,l+zoffset);
    if (i==1 || i==n/2+1)
      setColor (color[0],color[1],color[2],color[3]);

    // rotate ny,nz
    tmp = ca*ny - sa*nz;
    nz = sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();

  // draw bottom cap
  ny=1; nz=0;		  // normal vector = (0,ny,nz)
  glBegin (GL_TRIANGLE_FAN);
  glNormal3d (0,0,-1);
  glVertex3d (0,0,-l+zoffset);
  for (i=0; i<=n; i++) {
    if (i==1 || i==n/2+1)
      setColor (color[0]*0.75f,color[1]*0.75f,color[2]*0.75f,color[3]);
    glNormal3d (0,0,-1);
    glVertex3d (ny*r,nz*r,-l+zoffset);
    if (i==1 || i==n/2+1)
      setColor (color[0],color[1],color[2],color[3]);

    // rotate ny,nz
    tmp = ca*ny + sa*nz;
    nz = -sa*ny + ca*nz;
    ny = tmp;
  }
  glEnd();
}
*/
void ODEWorld::dsDrawBoxD (const double pos[3], const double R[12], const double sides[3])
{
 int i;
  float pos2[3],R2[12],fsides[3];
  for (i=0; i<3; i++) pos2[i]=(float)pos[i];
  for (i=0; i<12; i++) R2[i]=(float)R[i];
  for (i=0; i<3; i++) fsides[i]=(float)sides[i];
  dsDrawBox (pos2,R2,fsides);
}

void ODEWorld::drawForce(Element *inel)
{
  setupDrawingMode();
  glShadeModel (GL_FLAT);
  glLineWidth(5.0f);

  const dReal *pos = dBodyGetPosition(inel->body());
  const dReal *force = dBodyGetForce(inel->body());
  //  printf("%f %f %f, %f %f %f\n",pos[0],pos[1],pos[2],force[0],force[1],force[2]);
  //  printf("drawing vectors:\n");
  //printVector(fromvec);
  //printVector(tovec);

  float fx = pos[0];
  float fy = pos[1];
  float fz = pos[2];
  
  float tx =pos[0]+(force[0]*6.0);
  float ty =pos[1]+(force[1]*6.0);
  float tz =pos[2]+(force[2]*6.0);

  // printf("%f %f %f, %f %f %f\n",fx,fy,fz,tx,ty,tz);
  glPushMatrix();
  //  glLoadIdentity();

  glBegin(GL_LINES);
  glVertex3f(fx, fy, fz);
  glVertex3f(tx, ty, tz);
  glEnd();


  glPopMatrix();
  
}


void ODEWorld::drawStringForce(String *instring)
{
  setupDrawingMode();
  glShadeModel (GL_FLAT);
  glLineWidth(5.0f);

  dVector3 fromvec;
  instring->fromLoc(fromvec);
  dVector3 tovec;
  instring->toLoc(tovec);

  //  printf("drawing vectors:\n");
  //printVector(fromvec);
  //printVector(tovec);

  float fx = fromvec[0];
  float fy = fromvec[1];
  float fz = fromvec[2];

  dVector3 forcevec;
  instring->forceOn(instring->fromEl(), forcevec);

  float tx = fromvec[0] + (forcevec[0]*6);
  float ty = fromvec[1] + (forcevec[1]*6);
  float tz = fromvec[2] + (forcevec[2]*6);

  //  printf("force on (%f %f %f) is (%f %f %f) -> (%f %f %f)\n",fx,fy,fz,forcevec[0],forcevec[1],forcevec[1],tx,ty,tz);


  //  tx *= 2;
   // ty *= 2;
  //  tz *= 2;

  glPushMatrix();
  //  glLoadIdentity();

  glBegin(GL_LINES);
  glVertex3f(fx, fy, fz);
  glVertex3f(tx, ty, tz);
  glEnd();



  fx = tovec[0];
  fy = tovec[1];
  fz = tovec[2];

  instring->forceOn(instring->toEl(), forcevec);
  tx =tovec[0] + (forcevec[0]*6);
  ty =tovec[1] + (forcevec[1]*6);
  tz =tovec[2] + (forcevec[2]*6);

  //  tx *= 2;
  //  ty *= 2;
  //  tz *= 2;
  
  //  printf("force on (%f %f %f) is (%f %f %f) -> (%f %f %f)\n",fx,fy,fz,forcevec[0],forcevec[1],forcevec[1],tx,ty,tz);

  //  glLoadIdentity();

  
  glBegin(GL_LINES);
  glVertex3f(fx, fy, fz);
  glVertex3f(tx, ty, tz);
  glEnd();

  glPopMatrix();

}

void ODEWorld::drawString(String *instring)
{
  setupDrawingMode();
  glShadeModel (GL_FLAT);

  if (instring->is_active)
    glLineWidth(4.0f);
  else
    glLineWidth(2.0f);

  dVector3 fromvec;
  instring->fromLoc(fromvec);
  dVector3 tovec;
  instring->toLoc(tovec);

  //  printf("drawing vectors:\n");
  //printVector(fromvec);
  //printVector(tovec);

  float fx = fromvec[0];
  float fy = fromvec[1];
  float fz = fromvec[2];
  
  float tx = tovec[0];
  float ty = tovec[1];
  float tz = tovec[2];

  //  printf("string between (%f %f %f) and (%f %f %f)\n",fx,fy,fz,tx,ty,tz);
  glPushMatrix();
  //  glLoadIdentity();

  glBegin(GL_LINES);
  glVertex3f(fx, fy, fz);
  glVertex3f(tx, ty, tz);
  glEnd();


  glPopMatrix();
  
}
void ODEWorld::dsDrawBox (const float pos[3], const float R[12], const float sides[3])
{
  //  if (current_state != 2) dsError ("drawing function called outside simulation loop");
  setupDrawingMode();
  glShadeModel (GL_FLAT);
  setTransform (pos,R);
  drawBox (sides);
  glPopMatrix();

  if (use_shadows) {
    setShadowDrawingMode();
    setShadowTransform();
    setTransform (pos,R);
    drawBox (sides);
    glPopMatrix();
    glPopMatrix();
    glDepthRange (0,1);
  }
}

void ODEWorld::setShadowTransform()
{
  GLfloat matrix[16];
  for (int i=0; i<16; i++) matrix[i] = 0;
  matrix[0]=1;
  matrix[5]=1;
  matrix[8]=-LIGHTX;
  matrix[9]=-LIGHTY;
  matrix[15]=1;
  glPushMatrix();
  glMultMatrixf (matrix);
}

void ODEWorld::drawSphereShadow (float px, float py, float pz, float radius)
{
  // calculate shadow constants based on light vector
  static int init=0;
  static float len2,len1,scale;
  if (!init) {
    len2 = LIGHTX*LIGHTX + LIGHTY*LIGHTY;
    len1 = 1.0f/(float)sqrt(len2);
    scale = (float) sqrt(len2 + 1);
    init = 1;
  }

  // map sphere center to ground plane based on light vector
  px -= LIGHTX*pz;
  py -= LIGHTY*pz;

  const float kx = 0.96592582628907f;
  const float ky = 0.25881904510252f;
  float x=radius, y=0;

  glBegin (GL_TRIANGLE_FAN);
  for (int i=0; i<24; i++) {
    // for all points on circle, scale to elongated rotated shadow and draw
    float x2 = (LIGHTX*x*scale - LIGHTY*y)*len1 + px;
    float y2 = (LIGHTY*x*scale + LIGHTX*y)*len1 + py;
    glTexCoord2f (x2*ground_scale+ground_ofsx,y2*ground_scale+ground_ofsy);
    glVertex3f (x2,y2,0);

    // rotate [x,y] vector
    float xtmp = kx*x - ky*y;
    y = ky*x + kx*y;
    x = xtmp;
  }
  glEnd();
}

void ODEWorld::setShadowDrawingMode()
{
  glDisable (GL_LIGHTING);
  if (use_textures) {
    glEnable (GL_TEXTURE_2D);
    ground_texture->bind (1);
    glColor3f (SHADOW_INTENSITY,SHADOW_INTENSITY,SHADOW_INTENSITY);
    glEnable (GL_TEXTURE_2D);
    glEnable (GL_TEXTURE_GEN_S);
    glEnable (GL_TEXTURE_GEN_T);
    glTexGeni (GL_S,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
    glTexGeni (GL_T,GL_TEXTURE_GEN_MODE,GL_EYE_LINEAR);
    static GLfloat s_params[4] = {ground_scale,0,0,ground_ofsx};
    static GLfloat t_params[4] = {0,ground_scale,0,ground_ofsy};
    glTexGenfv (GL_S,GL_EYE_PLANE,s_params);
    glTexGenfv (GL_T,GL_EYE_PLANE,t_params);
  }
  else {
    glDisable (GL_TEXTURE_2D);
    glColor3f (GROUND_R*SHADOW_INTENSITY,GROUND_G*SHADOW_INTENSITY,
	       GROUND_B*SHADOW_INTENSITY);
  }
  glDepthRange (0,0.9999);
}

void ODEWorld::drawBox (const float sides[3])
{
  float lx = sides[0]*0.5f;
  float ly = sides[1]*0.5f;
  float lz = sides[2]*0.5f;

  // sides
  glBegin (GL_TRIANGLE_STRIP);
  glNormal3f (-1,0,0);
  glVertex3f (-lx,-ly,-lz);
  glVertex3f (-lx,-ly,lz);
  glVertex3f (-lx,ly,-lz);
  glVertex3f (-lx,ly,lz);
  glNormal3f (0,1,0);
  glVertex3f (lx,ly,-lz);
  glVertex3f (lx,ly,lz);
  glNormal3f (1,0,0);
  glVertex3f (lx,-ly,-lz);
  glVertex3f (lx,-ly,lz);
  glNormal3f (0,-1,0);
  glVertex3f (-lx,-ly,-lz);
  glVertex3f (-lx,-ly,lz);
  glEnd();

  // top face
  glBegin (GL_TRIANGLE_FAN);
  glNormal3f (0,0,1);
  glVertex3f (-lx,-ly,lz);
  glVertex3f (lx,-ly,lz);
  glVertex3f (lx,ly,lz);
  glVertex3f (-lx,ly,lz);
  glEnd();

  // bottom face
  glBegin (GL_TRIANGLE_FAN);
  glNormal3f (0,0,-1);
  glVertex3f (-lx,-ly,-lz);
  glVertex3f (-lx,ly,-lz);
  glVertex3f (lx,ly,-lz);
  glVertex3f (lx,-ly,-lz);
  glEnd();
}

void ODEWorld::setTransform (const float pos[3], const float R[12])
{
  GLfloat matrix[16];
  matrix[0]=R[0];
  matrix[1]=R[4];
  matrix[2]=R[8];
  matrix[3]=0;
  matrix[4]=R[1];
  matrix[5]=R[5];
  matrix[6]=R[9];
  matrix[7]=0;
  matrix[8]=R[2];
  matrix[9]=R[6];
  matrix[10]=R[10];
  matrix[11]=0;
  matrix[12]=pos[0];
  matrix[13]=pos[1];
  matrix[14]=pos[2];
  matrix[15]=1;
  glPushMatrix();
  glMultMatrixf (matrix);
}

void ODEWorld::setupDrawingMode()
{
  //printf("setupdrawing mode!\n");
  glEnable (GL_LIGHTING);
  if (tnum) {
    if (use_textures) {
      //printf("using textures\n");
      glEnable (GL_TEXTURE_2D);
      wood_texture->bind (1);
      glEnable (GL_TEXTURE_GEN_S);
      glEnable (GL_TEXTURE_GEN_T);
      glTexGeni (GL_S,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
      glTexGeni (GL_T,GL_TEXTURE_GEN_MODE,GL_OBJECT_LINEAR);
      static GLfloat s_params[4] = {1.0f,1.0f,0.0f,1};
      static GLfloat t_params[4] = {0.817f,-0.817f,0.817f,1};
      glTexGenfv (GL_S,GL_OBJECT_PLANE,s_params);
      glTexGenfv (GL_T,GL_OBJECT_PLANE,t_params);
    }
    else {
      glDisable (GL_TEXTURE_2D);
    }
  }
  else {
    glDisable (GL_TEXTURE_2D);
  }
  setColor (color[0],color[1],color[2],color[3]);

  if (color[3] < 1) {
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
  }
  else {
    glDisable (GL_BLEND);
  }
}

void ODEWorld::dsDrawSphereD (const double pos[3], const double R[12], const float radius)
{
 int i;
  float pos2[3],R2[12];
  for (i=0; i<3; i++) pos2[i]=(float)pos[i];
  for (i=0; i<12; i++) R2[i]=(float)R[i];
  dsDrawSphere (pos2,R2,radius);
}

 void ODEWorld::dsDrawSphere (const float pos[3], const float R[12],
			      float radius)
{
  // if (current_state != 2) dsError ("drawing function called outside simulation loop");
  setupDrawingMode();
  glEnable (GL_NORMALIZE);
  glShadeModel (GL_SMOOTH);
  setTransform (pos,R);
  glScaled (radius,radius,radius);
  drawSphere();
  glPopMatrix();
  glDisable (GL_NORMALIZE);

  // draw shadows
  if (use_shadows) {
    glDisable (GL_LIGHTING);
    if (use_textures) {
      ground_texture->bind (1);
      glEnable (GL_TEXTURE_2D);
      glDisable (GL_TEXTURE_GEN_S);
      glDisable (GL_TEXTURE_GEN_T);
      glColor3f (SHADOW_INTENSITY,SHADOW_INTENSITY,SHADOW_INTENSITY);
    }
    else {
      glDisable (GL_TEXTURE_2D);
      glColor3f (GROUND_R*SHADOW_INTENSITY,GROUND_G*SHADOW_INTENSITY,
		 GROUND_B*SHADOW_INTENSITY);
    }
    glShadeModel (GL_FLAT);
    glDepthRange (0,0.9999);
    drawSphereShadow (pos[0],pos[1],pos[2],radius);
    glDepthRange (0,1);
  }
}

void ODEWorld::drawSphere()
{
  // icosahedron data for an icosahedron of radius 1.0
# define ICX 0.525731112119133606f
# define ICZ 0.850650808352039932f
  static GLfloat idata[12][3] = {
    {-ICX, 0, ICZ},
    {ICX, 0, ICZ},
    {-ICX, 0, -ICZ},
    {ICX, 0, -ICZ},
    {0, ICZ, ICX},
    {0, ICZ, -ICX},
    {0, -ICZ, ICX},
    {0, -ICZ, -ICX},
    {ICZ, ICX, 0},
    {-ICZ, ICX, 0},
    {ICZ, -ICX, 0},
    {-ICZ, -ICX, 0}
  };

  static int index[20][3] = {
    {0, 4, 1},	  {0, 9, 4},
    {9, 5, 4},	  {4, 5, 8},
    {4, 8, 1},	  {8, 10, 1},
    {8, 3, 10},   {5, 3, 8},
    {5, 2, 3},	  {2, 7, 3},
    {7, 10, 3},   {7, 6, 10},
    {7, 11, 6},   {11, 0, 6},
    {0, 1, 6},	  {6, 1, 10},
    {9, 0, 11},   {9, 11, 2},
    {9, 2, 5},	  {7, 2, 11},
  };

  static GLuint listnum = 0;
  if (listnum==0) {
    listnum = glGenLists (1);
    glNewList (listnum,GL_COMPILE);
    glBegin (GL_TRIANGLES);
    for (int i=0; i<20; i++) {
      drawPatch (&idata[index[i][2]][0],&idata[index[i][1]][0],
		 &idata[index[i][0]][0],sphere_quality);
    }
    glEnd();
    glEndList();
  }
  glCallList (listnum);
}

void ODEWorld::drawPatch (float p1[3], float p2[3], float p3[3], int level)
{
  int i;
  if (level > 0) {
    float q1[3],q2[3],q3[3];		 // sub-vertices
    for (i=0; i<3; i++) {
      q1[i] = 0.5f*(p1[i]+p2[i]);
      q2[i] = 0.5f*(p2[i]+p3[i]);
      q3[i] = 0.5f*(p3[i]+p1[i]);
    }
    float length1 = (float)(1.0/sqrt(q1[0]*q1[0]+q1[1]*q1[1]+q1[2]*q1[2]));
    float length2 = (float)(1.0/sqrt(q2[0]*q2[0]+q2[1]*q2[1]+q2[2]*q2[2]));
    float length3 = (float)(1.0/sqrt(q3[0]*q3[0]+q3[1]*q3[1]+q3[2]*q3[2]));
    for (i=0; i<3; i++) {
      q1[i] *= length1;
      q2[i] *= length2;
      q3[i] *= length3;
    }
    drawPatch (p1,q1,q3,level-1);
    drawPatch (q1,p2,q2,level-1);
    drawPatch (q1,q2,q3,level-1);
    drawPatch (q3,q2,p3,level-1);
  }
  else {
    glNormal3f (p1[0],p1[1],p1[2]);
    glVertex3f (p1[0],p1[1],p1[2]);
    glNormal3f (p2[0],p2[1],p2[2]);
    glVertex3f (p2[0],p2[1],p2[2]);
    glNormal3f (p3[0],p3[1],p3[2]);
    glVertex3f (p3[0],p3[1],p3[2]);
  }
}

void ODEWorld::dsSetColorAlpha (float red, float green, float blue, float alpha)
{
  color[0] = red;
  color[1] = green;
  color[2] = blue;
  color[3] = alpha;
}

void ODEWorld::dsStartGraphics (int width, int height)
{
  printf("dsStartGraphics()\n");

  char *prefix = DEFAULT_PATH_TO_TEXTURES;
  //  if (fn->version >= 2 && fn->path_to_textures) prefix = fn->path_to_textures;
  char *s = (char*) alloca (strlen(prefix) + 20); 

  strcpy (s,prefix); 
  strcat (s,"/sky.ppm"); 
  sky_texture = new Texture (s); 

  strcpy (s,prefix); 
  strcat (s,"/ground.ppm"); 
  ground_texture = new Texture (s); 

  strcpy (s,prefix); 
  strcat (s,"/wood.ppm"); 
  wood_texture = new Texture (s);

}


Texture::Texture (char *filename)
{
  printf("making texture with filname %s\n",filename);
  image = new Image (filename);
  glGenTextures (1,&name);
  glBindTexture (GL_TEXTURE_2D,name);

  // set pixel unpacking mode
  glPixelStorei (GL_UNPACK_SWAP_BYTES, 0);
  glPixelStorei (GL_UNPACK_ROW_LENGTH, 0);
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);
  glPixelStorei (GL_UNPACK_SKIP_ROWS, 0);
  glPixelStorei (GL_UNPACK_SKIP_PIXELS, 0);

  // glTexImage2D (GL_TEXTURE_2D, 0, 3, image->width(), image->height(), 0,
  //		   GL_RGB, GL_UNSIGNED_BYTE, image->data());
  gluBuild2DMipmaps (GL_TEXTURE_2D, 3, image->width(), image->height(),
		     GL_RGB, GL_UNSIGNED_BYTE, image->data());

  // set texture parameters - will these also be bound to the texture???
  glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

  glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameterf (GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,
		   GL_LINEAR_MIPMAP_LINEAR);

  glTexEnvf (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
}


Texture::~Texture()
{
  delete image;
  glDeleteTextures (1,&name);
}


void Texture::bind (int modulate)
{
  glBindTexture (GL_TEXTURE_2D,name);
  glTexEnvi (GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE,
	     modulate ? GL_MODULATE : GL_DECAL);
}

Image::Image (char *filename)
{
  
  FILE *f = fopen (filename,"rb"); 
  if (!f)
    {
      printf("Can't open image file `%s'",filename);
    }
  
  // read in header
  if (fgetc(f) != 'P' || fgetc(f) != '6') 
    {
      printf("image file \"%s\" is not a binary PPM (no P6 header)",filename);
    }

  skipWhiteSpace (filename,f);
  // read in image parameters
  image_width = readNumber (filename,f); 
  skipWhiteSpace (filename,f); 
  image_height = readNumber (filename,f); 
  skipWhiteSpace (filename,f); 
  int max_value = readNumber (filename,f); 

  // check values
  if (image_width < 1 || image_height < 1) 
    {printf("bad image file \"%s\"",filename);}
  if (max_value != 255) 
    {printf("image file \"%s\" must have color range of 255",filename);}

  // read either nothing, LF (10), or CR,LF (13,10)
  int c = fgetc(f); 
  if (c == 10) { 
    // LF
  }
  else if (c == 13) { 
    // CR
    c = fgetc(f); 
    if (c != 10) ungetc (c,f); 
  }
  else ungetc (c,f); 

  // read in rest of data
  image_data = new byte [image_width*image_height*3]; 
  if (fread (image_data,image_width*image_height*3,1,f) != 1) 
    {printf("Can not read data from image file `%s'",filename);}
  fclose (f);
}


Image::~Image()
{
  delete[] image_data;
}

// call this to update the current camera position. the bits in `mode' say
// if the left (1), middle (2) or right (4) mouse button is pressed, and
// (deltax,deltay) is the amount by which the mouse pointer has moved.

void ODEWorld::dsMotion (int mode, int deltax, int deltay)
{
  float side = 0.01f * float(deltax);
  float fwd = (mode==4) ? (0.01f * float(deltay)) : 0.0f;
  float s = (float) sin (view_hpr[0]*DEG_TO_RAD);
  float c = (float) cos (view_hpr[0]*DEG_TO_RAD);

  if (mode==1) {
    view_hpr[0] += float (deltax) * 0.5f;
    view_hpr[1] += float (deltay) * 0.5f;
  }
  else {
    view_xyz[0] += -s*side + c*fwd;
    view_xyz[1] += c*side + s*fwd;
    if (mode==2 || mode==5) view_xyz[2] += 0.01f * float(deltay);
  }
  wrapCameraAngles();
}

int ODEWorld::dsGetTextures()
{
  return use_textures;
}

int ODEWorld::dsGetShadows()
{
  return use_shadows;
}


void ODEWorld::dsSetShadows (int a)
{
  use_shadows = (a != 0);
}

void ODEWorld::dsGetViewpoint (float xyz[3], float hpr[3])
{
  //if (current_state < 1) dsError ("dsGetViewpoint() called before simulation started");
  if (xyz) {
    xyz[0] = view_xyz[0];
    xyz[1] = view_xyz[1];
    xyz[2] = view_xyz[2];
  }
  if (hpr) {
    hpr[0] = view_hpr[0];
    hpr[1] = view_hpr[1];
    hpr[2] = view_hpr[2];
  }
}

void ODEWorld::dsError(char *msg)
{
  printf("%s",msg);
}

void ODEWorld::wrapCameraAngles()
{
  for (int i=0; i<3; i++) {
    while (view_hpr[i] > 180) view_hpr[i] -= 360;
    while (view_hpr[i] < -180) view_hpr[i] += 360;
  }
}

void ODEWorld::dsSetColor (float red, float green, float blue)
{
  //if (current_state != 2) dsError ("drawing function called outside simulation loop");
  color[0] = red;
  color[1] = green;
  color[2] = blue;
  color[3] = 1;
}

void ODEWorld::set_fitness_mode(int mode)
{
  _Fitness_Mode = mode;
  printf("fitness mode now: %d\n",_Fitness_Mode);
}

int ODEWorld::get_fitness_mode()
{
  return _Fitness_Mode;
}

float ODEWorld::get_gravity_noise()
{
  dVector3 grav;
  dWorldGetGravity (world,grav);
  float noise = grav[0];
  return noise;
}


void ODEWorld::set_gravity_left()
{
  dWorldSetGravity (world,-0.05,0,-0.5); 
}
void ODEWorld::set_gravity_right()
{
  dWorldSetGravity (world,0.05,0,-0.5); 
}

void ODEWorld::set_gravity_down()
{
  dWorldSetGravity(world,0,0,-0.5); 
}

void ODEWorld::set_gravity_noise(float noise)
{
  dWorldSetGravity (world,noise,0,-0.5); 
}
void ODEWorld::set_gravity_random(float range)
{
  float noise = float_rand_in_range(-0.05,0.05);
  dWorldSetGravity (world,noise,0,-0.5); 
}

//int ODEWorld::fscan_config();
int ODEWorld::fscan_config(char *infilename)
{
  FILE *infile = fopen(infilename,"r");
  _Jitter_X = 0;
  _Jitter_A = 0;
  _Tilt_Angle = 0;
  _Horizontal_Gravity_Vector = 0;
  _Frame_Rate = 0;
  _Max_Bricks = 1;
  _Margin = 1;
  printf("ODEWorld::fscan_config() - filename is %s\n",infilename);
  if (!infile)
    {
      printf("cannot open file %s for input\n",infilename);
      return -1;
    }
  int val; 
  char inString[80];
  while (fscanf(infile,"%s = %d;\n",&inString,&val) != EOF)
    {
      if (strcmp(inString,"_Jitter_X") == 0)
	set_jitter_x(val);
      else if (strcmp(inString,"_Jitter_A") == 0)
	set_jitter_a(val);
      else if (strcmp(inString,"_Tilt_Angle") == 0)
	{
	  _Tilt_Angle = (dReal)val*M_PI/180; //convert to rads
	  _Horizontal_Gravity_Vector = 0.5*tan((double)_Tilt_Angle);
	  printf("Tilt Angle: %f, vector: %f\n",_Tilt_Angle,_Horizontal_Gravity_Vector);
	}
      else if (strcmp(inString,"_Max_Bricks") == 0)
	{
	  _Max_Bricks = val;
	}
      //gotta keep this here even tho' it's scanned in multipop
      else if (strcmp(inString,"_Fitness_Mode") == 0)
	_Fitness_Mode = val;
      else if (strcmp(inString,"_Frame_Rate") == 0)
	_Frame_Rate = val;
      else if (strcmp(inString,"_Margin") == 0)
	{
	  dReal added_margin = (val*1./100.);
	  _Margin = _Margin + added_margin;
	  printf("Margin pct is %f of max x (%f)\n",_Margin,MAX_X);
	}
    }
  
  fclose(infile);
  return 0; 
}


void ODEWorld::set_jitter_x(int xjitter)
{
  //printf("setting xjitter to %d (%f)\n",xjitter,(xjitter*1./100.)*BRICK_WIDTH);
  _Jitter_X = (xjitter*1./1000.)*BRICK_WIDTH;
}

void ODEWorld::set_jitter_a(int ajitter)
{
  //  printf("setting ajitter to %f\n",ajitter*1.);
  _Jitter_A = ajitter*1.;
}

void ODEWorld::set_capture(int c)
{
  writeframes = c;
}

void ODEWorld::captureFrame(int num)
{
  fprintf (stderr,"capturing frame %04d\n",num);

  char s[100];
  sprintf (s,"frame/%d.ppm",num);
  FILE *f = fopen (s,"wb");
  //  if (!f) dsError ("can't open \"%s\" for writing",s);
  fprintf (f,"P6\n%d %d\n255\n",width,height);

  /*
  XImage *image = XGetImage (display,win,0,0,width,height,~0,ZPixmap);
  
  int rshift = 7 - getHighBitIndex (image->red_mask);
  int gshift = 7 - getHighBitIndex (image->green_mask);
  int bshift = 7 - getHighBitIndex (image->blue_mask);

  for (int y=0; y<height; y++) {
    for (int x=0; x<width; x++) {
      unsigned long pixel = XGetPixel (image,x,y);
      //printf("pixel is: %ul\n",pixel);
      unsigned char b[3];
      b[0] = SHIFTL(pixel & image->red_mask,rshift);
      b[1] = SHIFTL(pixel & image->green_mask,gshift);
      b[2] = SHIFTL(pixel & image->blue_mask,bshift);
      fwrite (b,3,1,f);
    }
    }
  */
  void *buf = malloc( width * height * 3 );
  glReadPixels( 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, buf );
  
  //printf("sizeof ulong is: %d\n",sizeof(unsigned long));
  //printf("sizeof uint is: %d\n",sizeof(unsigned short));
  for (int y=(height - 1); y>=0; y--) {
    for (int x=0; x<width; x++) {
      //  unsigned long pixel = XGetPixel (image,x,y);
      unsigned char *pixel = ((unsigned char *)buf)+((y*width+ x)*3);
      unsigned char b[3];
      //printf("hello\n");
      b[0] = *pixel;
      b[1] = *(pixel+1);
      b[2] = *(pixel+2);
      // printf("pixel is: %ud %ud %ud\n",(unsigned short)b[0],(unsigned short)b[1],(unsigned short)b[2]);
      fwrite (b,3,1,f);
    }
   }
  free(buf);
  fclose (f);
  //  XDestroyImage (image);
}
