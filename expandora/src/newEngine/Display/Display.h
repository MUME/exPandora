#ifndef DISPLAY
#define DISPLAY

#include <qthread.h>
#include <qgl.h>
#include "Component.h"
#include "Room.h"
#include "CachedRoom.h"
#include "Frustum.h"
#include "RoomRecipient.h"
#include "RoomAdmin.h"

#define MAX_TEXTURES    100
#define DIST_Z	1	/* the distance between 2 rooms */
#define BASE_Z  -6	/* the distance to the "camera" */
#define ROOM_SIZE 0.2f	/* the size of the rooms walls */
#define MARKER_SIZE           (ROOM_SIZE/2.0)




class RendererWidget : public QGLWidget, public RoomRecipient {
  Q_OBJECT 
 private:
  
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
  
  
 public:
  GLfloat       angley;
  GLfloat       anglex;
  GLfloat       anglez;
  float           userx;
  float           usery;
  float           userz;		/* additional shift added by user */

  RendererWidget( QWidget *parent);
  void drawExit(Coordinate * from, Coordinate * to, unsigned int dir);
  void shiftView();
  void CalculateFrustum();
  void receiveRoom(RoomAdmin *,Room *);
  
 protected:
  void initializeGL();
  void resizeGL( int width, int height );
  void paintGL();

 signals:
  void viewableAreaChanged(RoomRecipient *, Frustum *); 
};



class MainWindow;
class DisplayComponent : public Component {
 Q_OBJECT
  
 signals:
  void lookingForRooms(RoomRecipient *, Frustum *);
  
  
 public slots:
  void playerMoved(Coordinate *, Coordinate *);
  
 public:
  DisplayComponent();
  virtual void init();
  Qt::ConnectionType requiredConnectionType(const QString & str);

 private:
  
  void resolveTerrains();
  QString extractDescription(QString & filename);
  RendererWidget *renderer;
  MainWindow *renderer_window;
  QGLFormat f;
};

#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
