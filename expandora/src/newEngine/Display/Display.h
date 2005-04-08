#ifndef DISPLAY
#define DISPLAY

#include <qthread.h>

#include "Room.h"
#include "RoomAdmin.h"
#include "Parser.h"

#define MAX_TEXTURES    100
#define DIST_Z	1	/* the distance between 2 rooms */
#define BASE_Z  -6	/* the distance to the "camera" */
#define ROOM_SIZE 0.2f	/* the size of the rooms walls */
#define MARKER_SIZE           (ROOM_SIZE/2.0)

class QAction; 
class QLabel; 
class FindDialog; 
class Spreadsheet; 
class QLineEdit;
class QLabel;
class QWidget;
class QPushButton;

// We create an enum of the sides so we don't have to call each side 0 or 1.
// This way it makes it more understandable and readable when dealing with frustum sides.
enum FrustumSide
  {
    RIGHT   = 0,        // The RIGHT side of the frustum
    LEFT    = 1,        // The LEFT  side of the frustum
    BOTTOM  = 2,        // The BOTTOM side of the frustum
    TOP     = 3,        // The TOP side of the frustum
    BACK    = 4,        // The BACK side of the frustum
    FRONT   = 5         // The FRONT side of the frustum
  }; 

// Like above, instead of saying a number for the ABC and D of the plane, we
// want to be more descriptive.
enum PlaneData
  {
    A = 0,              // The X value of the plane's normal
    B = 1,              // The Y value of the plane's normal
    C = 2,              // The Z value of the plane's normal
    D = 3               // The distance the plane is from the origin
  };



class RendererWidget : public QGLWidget
{
  Q_OBJECT public:

  GLfloat       angley;
  GLfloat       anglex;
  GLfloat       anglez;
  int           userx;
  int           usery;
  int           userz;		/* additional shift added by user */

  RendererWidget( QWidget *parent, const char *name=0 );

  void attachParser(Parser * _parser) {parser = _parser;}
  void attachRoomAdmin(RoomAdmin * admin) {roomAdmin = admin;}

  void display(void);

  void scheduleRedraw() {glredraw = 1;}
  void draw(void);
  void CalculateFrustum();


 protected:
  void initializeGL();
  void resizeGL( int width, int height );
  void paintGL();

 private:
  Parser * parser;
  RoomAdmin * roomAdmin;

  static const GLfloat marker_colour[4];
  static const int texture_visibilit_range = 300;
  static const int details_visibility_range = 500;

  GLfloat       colour[4];
  float         m_Frustum[6][4];
  GLuint        basic_gllist;
  GLuint        global_list;
  int           glredraw;
  int           curx;
  int           cury;
  int           curz;			/* current rooms position */ 

  void NormalizePlane(float frustum[6][4], int side);  
  void glDrawMarkers();
  void glDrawRoom(Room * pr);
  bool PointInFrustum(float x, float y, float z);
};



class MainWindow;
class Display : public QThread {
 public:
  void run();
  void toggle_renderer_reaction();
  void attachParser(Parser * parser); 
  void attachRoomAdmin(RoomAdmin * admin); 
  void CalculateFrustum();


 private:
  MainWindow *renderer_window;
};








#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
