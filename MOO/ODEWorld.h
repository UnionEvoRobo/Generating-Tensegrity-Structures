#ifndef _ODEWORLD_H
#define _ODEWORLD_H
#include <ode/ode.h>
//#include <drawstuff/drawstuff.h>
#include <vector.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <GL/glx.h>
#include <GL/gl.h>
#include <GL/glu.h>



#include <time.h>

#include "Element.h"
#include "String.h"
#include "Graph.h"
#include "Tensegrity.h"

class Element ;

typedef unsigned char byte;
//***************************************************************************
// Texture object.
class Image {
  int image_width,image_height;
  byte *image_data;
public:
  Image (char *filename);
  // load from PPM file
  ~Image();
  int width() { return image_width; }
  int height() { return image_height; }
  byte *data() { return image_data; }
};

class Texture {
  Image *image;
  GLuint name;
public:
  Texture (char *filename);
  ~Texture();
  void bind (int modulate);
};


#ifdef MSVC
#pragma warning(disable:4244 4305)  // for VC++, no precision loss complaints
#endif

#ifdef dDOUBLE
#define dsDrawBox dsDrawBoxD
#define dsDrawSphere dsDrawSphereD
#define dsDrawCylinder dsDrawCylinderD
#define dsDrawCappedCylinder dsDrawCappedCylinderD
#endif

#ifdef WIN32
#define DEFAULT_PATH_TO_TEXTURES "..\\textures\\"
#else
#define DEFAULT_PATH_TO_TEXTURES "./textures"
#endif

#ifndef M_PI
#define M_PI (3.14159265358979323846)
#endif

// constants to convert degrees to radians and the reverse
#define RAD_TO_DEG (180.0/M_PI)
#define DEG_TO_RAD (M_PI/180.0)

// light vector. LIGHTZ is implicitly 1
#define LIGHTX (-0.5f)
//#define LIGHTX (0.40f)
///#define LIGHTY (0.4f)
#define LIGHTY (-1.3f) //this puts the light on the proper side of the camera
//making it bigger makes it brighter in that dimension
// - Y is toward the camera
// and I think +X is to the right 
// ground and sky
#define SHADOW_INTENSITY (0.65f)
/*
#define GROUND_R (0.5f) 	// ground color for when there's no texture 
#define GROUND_G (0.5f) 
#define GROUND_B (0.3f)
*/

#define GROUND_R (0.87f) 	// ground color for when there's no texture
#define GROUND_G (0.87f)
#define GROUND_B (0.87f)

/*
#define SKY_RED 0.0 
#define SKY_GREEN 0.5 
#define SKY_BLUE 1.0
*/

#define SKY_RED 1.0
#define SKY_GREEN 1.0
#define SKY_BLUE 1.0

/*
#define SCAFF_RED  1.0
#define SCAFF_BLUE 0.0 
#define SCAFF_GREEN 0.0 
*/

#define SCAFF_RED  0.75
#define SCAFF_BLUE 0.75
#define SCAFF_GREEN 0.75

#define BRICK_RED 0.0
#define BRICK_GREEN 0.0
#define BRICK_BLUE  1.0

const float ground_scale = 1.0f/1.0f;	// ground texture scale (1/size)
const float ground_ofsx = 0.5;		// offset of ground texture
const float ground_ofsy = 0.5;
const float sky_scale = 1.0f/4.0f;	// sky texture scale (1/size)
const float sky_height = 1.0f;		// sky height above viewpoint
// some constants

#define NUM 50			// max number of objects
#define DENSITY (5.0)		// density of all objects
#define GPB 5			// maximum number of geometries per body
#define MAX_CONTACTS 20		// maximum number of contact points per body

#define BRICK_WIDTH .1 
#define DX 0.02
#define RCH .001
#define STEPS_PER_COMMAND 50
#define MAX_DELAY 450
#define ABSOLUTE_MAX_STEPS 20000
#define MAX_PENETRATION_DEPTH 0.002

#define BAD_BODY -123456789

#define MAX_X 0.5 //since it goes from -Max to +Max
#define MAX_Z 1.0 // since 


static int capped_cylinder_quality = 3;

static int _stopFlag = 0;

static float g_maxforce;
static float g_maxvel;
////
//#define MIN_STEP_DELAY 60 //these work
//#define MIN_VEL 0.0005
//okay, the problem is that sometime a thing has only temporarily stopped!
#define MIN_STEP_DELAY 2 
#define MIN_VEL 0.002
#define MAX_VEL 0.01

#define TURTLE_FWD_2 1
#define TURTLE_FWD_1 0
//from drawstuff internals.h
// dynamics and collision objects
//static void dsPlatformSimLoop (int window_width, int window_height,
//dsFunctions *fn, int initial_pause);
//static void initMotionModel();
// used by platform specific code

//char locase(char c);
//void drawGeom(dGeomID g, const dReal *pos, const dReal *R, int show_aabb);

static int run = 0;

static int _module_count = 1000;

struct MyObject {
  int modID;
  dBodyID body;			// the body
  vector<dGeomID> geom;		// geometries representing this body
  dReal rc;
  dReal gc;
  dReal bc;
  int refcount;
};

#include "MultiAssembly.h"

class ODEWorld
{
 protected:

  //
  
  //************* stolen from drawstuff *******************
// X11 display info
  Display *display;
  int screen;
  XVisualInfo *visual;		// best visual for openGL
  Colormap colormap;		// window's colormap
  Atom wm_protocols_atom;
  Atom wm_delete_window_atom;
  
  // window and openGL
  Window win;			// X11 window, 0 if not initialized
  int width,height;		// window size
  GLXContext glx_context;	// openGL rendering context
  int last_key_pressed;		// last key pressed in the window
  //int run;			// 1 if simulation running
  int pause;			// 1 if in `pause' mode
  int singlestep;		// 1 if single step key pressed
  int writeframes;		// 1 if frame files to be written
  int frame;
  float view_xyz[3];	// position x,y,z
  float view_hpr[3];	// heading, pitch, roll (degrees)
  float color[4];	// current r,g,b,alpha color
  int tnum;			// current texture number
  int use_textures;
  int use_shadows;
  int sphere_quality;
  Texture *sky_texture;
  Texture *ground_texture;
  Texture *wood_texture;
 
  // 
  void createMainWindow (int _width, int _height);
  void dsSetViewpoint (float xyz[3], float hpr[3]);
  void dsSetTextures(int a);
  void dsSetShadows(int a);
  void dsDrawBoxD (const double pos[3], const double R[12], const double sides[3]);
  void dsDrawBox (const float pos[3], const float R[12], const float sides[3]);
  void drawBox (const float sides[3]);
  void drawString(String *);
  void drawStringForce(String *);


  void resetForces();
  void printVelocities();
  void addDrags();
  void drawForce(Element *);
  void drawCapsule (float l, float r);
  void dsDrawCapsule (const float pos[3], const float R[12],
			      float length, float radius);
  void dsDrawSphereD (const double pos[3], const double R[12], const float radius);
  void dsDrawSphere (const float pos[3], const float R[12], float radius);
  void drawSphere();
  void drawSphereShadow (float px, float py, float pz, float radius);
  void dsSetColorAlpha (float red, float green, float blue, float alpha);
  void dsStartGraphics (int width, int height);
  void drawPatch (float p1[3], float p2[3], float p3[3], int level);
  void setupDrawingMode();
  void setShadowDrawingMode();
  void setShadowTransform();
  void setTransform (const float pos[3], const float R[12]);
  void initGraphics();
  void doGraphicsStuff();
  void handleEvent(XEvent &event);
  void command(int cmd);
  void dsDrawFrame (int width, int height);
  void drawSky (float view_xyz[3]);
  void setCamera (float x, float y, float z, float h, float p, float r);
  void drawGround();
  void drawPyramidGrid();
  void setColor (float r, float g, float b, float alpha);
  void dsMotion (int mode, int deltax, int deltay);
  int dsGetTextures();
  int dsGetShadows();
  void dsGetViewpoint (float xyz[3], float hpr[3]);
  void dsError(char *);
  void wrapCameraAngles();
  void drawGeom(dGeomID g, const dReal *pos, const dReal *R, int show_aabb);
  void dsSetColor (float red, float green, float blue);
  
  /*
  void dsGetViewpoint (float xyz[3], float hpr[3]);
  void dsSetTexture (int texture_number);
  void dsDrawSphere (const float pos[3], const float R[12], float radius);
  void dsDrawTriangle (const float pos[3], const float R[12],
		       const float *v0, const float *v1, const float *v2, int solid);
  void dsDrawCylinder (const float pos[3], const float R[12],
		       float length, float radius);
  void dsDrawCappedCylinder (const float pos[3], const float R[12],
			     float length, float radius);
  void dsDrawLine (const float pos1[3], const float pos2[3]);
  
 void dsDrawTriangleD (const double pos[3], const double R[12],
		       const double *v0, const double *v1, const double *v2, int solid);
 void dsDrawCylinderD (const double pos[3], const double R[12],
		       float length, float radius);
 void dsDrawCappedCylinderD (const double pos[3], const double R[12],
			     float length, float radius);
 void dsDrawLineD (const double pos1[3], const double pos2[3]);
 
 void dsSetSphereQuality (int n);		
 void dsSetCappedCylinderQuality (int n);
 */
 
 //************ *********** *********** *********** *********** 
 
 Assembly *cur_assembly;
 

 Tensegrity *cur_tens;

 vector<float> _results;
 vector<float> _intermediate_results;
 vector<dVector3> comArray;

 int _drawCOM;
  int render;
  dWorldID world;
  dSpaceID space;
  MyObject turtle;
  MyObject upperLeftMarker;
  MyObject upperRightMarker;
  vector<MyObject> objvect;

  vector<String *> _strings;
  vector<Element *> _elements;
  // keep modules in here 'cuz saving and restoring objects is a headache 
  vector<MyObject> modvect;

  vector<MyObject> allModules;
  
  dJointGroupID contactgroup;
  dReal xpos;
  dReal ypos;
  dReal zpos;
  dReal epsilon;
  dReal offset;

  int moduleWillCollide;
  vector<dContact> moduleContacts;
  int _grid[(int)(2*MAX_X/DX)][(int)(2*MAX_Z/DX)];
  int goal_grid[(int)(2*MAX_X/DX)][(int)(2*MAX_Z/DX)];
  void captureFrame(int);
  //#define JITTER_X .005
  //#define JITTER_A 20. //degrees

  dReal _Margin;
  dReal _Jitter_X;
  dReal _Jitter_A;
  dReal _Tilt_Angle;
  dReal _Horizontal_Gravity_Vector;
  int _Fitness_Mode; // 0 is compare to goal, 1 is fill grid
  int _Frame_Rate;
  
  int _Max_Bricks;
  int orientation;
  int cur_step;
  int cur_cmd_index;
  int done_evaluating;
  int assembly_completed;
  int done_building;
  int is_glued;
  int have_melted_scaffolding;
  int reject;
  int last_instruction_step;

  int _run_interactive;
  int _enable_ODE;
  int _use_networks;

  dVector3 _currCOM;

  int _collisions_enabled;
  
  int _printlens;

  dReal objects_are_moving();
  int show_aabb;
 
  void init(int do_render = 0, char *configfilename = "block.cfg");
  //void init(int argc, char **argv);
  void remove_all_objects();

  void delete_all_elements();


  void drawObjects();
  void create_turtle();
  void create_markers();
  void create_world();
  float mass_of_objects();

  void print_capsule_ends();
  void print_string_lengths();
  void fprint_string_lengths(char *filename);
  float get_tensegrity_volume();

  void put_capsule(float x =.0, float y=.0, float z=.0);
  void put_sphere(float x =.0, float y=.0, float z=.0);

  int modularizeObject(MyObject inobj);
  MyObject copyObject(MyObject inobj);
  dGeomID copyGeom(dGeomID inobj);
  void describeObjectPos(MyObject inobj);
  void describeObjectRot(MyObject inobj);
  void describeObject(MyObject inobj);
  void describeGeom(dGeomID ingeom, MyObject inobj);
  void describeWorld();

  void resetT3();
  void resetT4();
  void resetT15();
  void resetTens();
  //need some way to get evolved weights down to the networks in the elemens 
  void copyWeightsToElements(Tensegrity *ts);
  
  void disableObject(MyObject inobj);
  void disableGeom(dGeomID ingeom);
  
 
  void print_forces();

  void break_object();
  int reset();
  //  int move_turtle(int val);
  int rotate_turtle(int val);


  float min_distance_between_objects(MyObject b1, MyObject b2);
  float min_distance_between_geoms(dGeomID o1, dGeomID o2);

  //read the .cc file to read why this is a useless fn.
  //void printForceAndTorque();

  // 0:  Build and then wait to settle
  // 1:  Melt Scaffolding and wait to settle
  // 2:  Glue and then wait to settle
  // 3: Done, evaluate.
  enum Assembly_State{S_Building = 0, S_Glueing, S_Melting, S_Done};

  // Goal: compare missing/extra to goal arch
  // Fill: amount of goal area filled
  // Empty: amount of goal area "shaded" from above
  // Meta: empty, but also looking at % difference b/w before and after settle
  // Noisy: for Noisy MOOF - want to collapse results into aggregate
  //         to measure reliability
  enum Fitness_Modes{F_Goal = 0, F_Fill, F_Empty, F_Meta, F_Noisy};

  int cur_state;

  MyObject object_at(dReal xv, dReal yv, dReal zv);

  void calc_fitness();
  vector<float> calc_fill_fitness();
  
  int test_foo;

  
 public:

  ODEWorld();
  ODEWorld(int do_render);
  ODEWorld(int do_render, char *configfilename);

  

  //  ODEWorld(int argc, char **argv);
  ~ODEWorld();
  static void staticNearCallback(void *data, dGeomID o1, dGeomID o2);
  void nearCallback(dGeomID o1, dGeomID o2);

  //these are used just for detecting and keeping track of callbacks
  static void staticNearCallbackCustom(void *data, dGeomID o1, dGeomID o2);
  void nearCallbackCustom(dGeomID o1, dGeomID o2);


  vector<float> evaluateJiggle(Tensegrity *ts, int grow = 0, int do_render = 0, int stopOnFlag = 0, char *prefix = "jiggletemp");
  vector<float> evaluate(Tensegrity *ts, int readFromFile = 0, int do_render = 0, int stopOnFlag = 0);


  void updateStringLabels(Tensegrity *tens);

  MyObject objectAtTurtle();
  MyObject objectOwningGeom(dGeomID ingeom);
  void destroyObject(MyObject inobj);
  int geom_is_in_object(dGeomID ingeom, MyObject inobj);
  int can_place_object(MyObject inobj);



  void gather_data();
  int get_test_foo();
  void set_gravity_random(float range);
  void set_gravity_noise(float noise);
  float get_gravity_noise();
  void set_gravity_left();
  void set_gravity_right();
  void set_gravity_down();

  void set_fitness_mode(int mode);
  int get_fitness_mode();

  void make_capsules();
  void run_interactive();
  void run_tensegrity(int stopOnFlag);
  void run_tensegrity_jiggle(int stopOnFlag, int rate = 1);

  void pull_strings();

  void attachStringsToNets();
  void run_element_networks();

  vector<float> comVectorX; 
  vector<float> comVectorY; 
  vector<float> comVectorZ; 
  void drawCOM();
  void centerOfMass(dVector3 result);
  float distanceTraveled(dVector3 initPos);

  void print_current_tens();

  void calc_nets();
  

  void set_capture(int);

  void activate_string(int i);

  //doubles the margin
  void resetView();
  void zoomout();
  void set_new_margin();
  void set_new_margin(float val);

  int fscan_config(char *infilename);
  void set_jitter_x(int xjitter);
  void set_jitter_a(int ajitter);


  int growAndShowTensegrity(Tensegrity *ints);
  int makeFromGraph(Graph *ingraph, vector<int> rods, int is_static = 0);
  int makeFromTensegrity(Tensegrity *ints, int is_static = 0);
  int makeFromNewTens(Tensegrity *ints, int is_static);
  float getMaxVelocity();
  float getMaxForce();
  
  int countCollisions();
  float contactDepths();
};


static ODEWorld* globalself;

#endif
