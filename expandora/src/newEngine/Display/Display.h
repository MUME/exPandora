#ifndef DISPLAY
#define DISPLAY

#include <qthread.h>
#include <qgl.h>
#include "Component.h"
#include "Room.h"
#include "CachedRoom.h"

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


class RendererWidget : public QGLWidget {

 private:
  Q_OBJECT 
  static const GLfloat marker_colour[4];
  static const int texture_visibilit_range = 300;
  static const int details_visibility_range = 500;


  GLfloat       colour[4];
  float         m_Frustum[6][4];
  GLuint        basic_gllist;
  GLuint        global_list;
  int           curx;
  int           cury;
  int           curz;			/* current rooms position */ 

  void NormalizePlane(float frustum[6][4], int side);  
  void drawMarker(Coordinate * pos);
  
  bool PointInFrustum(float x, float y, float z);

 public slots:
  void moveMarker(Coordinate *, Coordinate *);
  void drawRoom(QObject *,Room *);
  
 public:
  GLfloat       angley;
  GLfloat       anglex;
  GLfloat       anglez;
  int           userx;
  int           usery;
  int           userz;		/* additional shift added by user */

  RendererWidget( QWidget *parent, const char *name=0 );
  void drawExit(Coordinate * from, Coordinate * to, unsigned int dir);
  void shiftView();
  void CalculateFrustum();

 protected:
  void initializeGL();
  void resizeGL( int width, int height );

 signals:
  void viewableAreaChanged(float**); 
};



class MainWindow;
class DisplayThread : public QThread, public Component {
 public:
  void run();
  void start(Priority priority = InheritPriority) {QThread::start(priority);}
  void toggle_renderer_reaction();
  void CalculateFrustum();


 private:
  MainWindow *renderer_window;

};


// We create an enum of the sides so we don't have to call each side 0 or 1.
// This way it makes it more understandable and readable when dealing with frustum sides.
enum FrustumSide {
    RIGHT   = 0,        // The RIGHT side of the frustum
    LEFT    = 1,        // The LEFT  side of the frustum
    BOTTOM  = 2,        // The BOTTOM side of the frustum
    TOP     = 3,        // The TOP side of the frustum
    BACK    = 4,        // The BACK side of the frustum
    FRONT   = 5         // The FRONT side of the frustum
}; 

// Like above, instead of saying a number for the ABC and D of the plane, we
// want to be more descriptive.
enum PlaneData {
    A = 0,              // The X value of the plane's normal
    B = 1,              // The Y value of the plane's normal
    C = 2,              // The Z value of the plane's normal
    D = 3               // The distance the plane is from the origin
};






#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
