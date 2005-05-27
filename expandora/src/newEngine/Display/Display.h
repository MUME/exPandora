#ifndef DISPLAY
#define DISPLAY

#include <qthread.h>
#include <qgl.h>
#include "Component.h"
#include "Room.h"
#include "CachedRoom.h"
#include "Frustum.h"

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
  static const int texture_visibilit_range;
  static const int details_visibility_range;


  GLfloat       colour[4];
  Frustum	frustum;
  GLuint        basic_gllist;
  GLuint        global_list;
  int           curx;
  int           cury;
  int           curz;			/* current rooms position */ 

    
  void drawMarker(Coordinate * pos);
  
  

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
  void viewableAreaChanged(Frustum *); 
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









#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
