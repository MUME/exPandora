#ifndef RENDERERWIDGET
#define RENDERERWIDGET
#include <QGLWidget>
#include "RoomRecipient.h"
#include "CachedRoom.h"
#include "Frustum.h"

#define BASE_Z  -6	/* the distance to the "camera" */
#define ROOM_SIZE 0.2f	/* the size of the rooms walls */
#define DIST_Z	1	/* the distance between 2 rooms */
#define MARKER_SIZE           (ROOM_SIZE/2.0)


class RendererWidget : public QGLWidget, public RoomRecipient
{
  Q_OBJECT
private:

  static const GLfloat marker_colour[4];
  static const int texture_visibilit_range;
  static const int details_visibility_range;

  map<int, CachedRoom *> roomsCache;
  Coordinate position;
  GLfloat       colour[4];
  Frustum	frustum;
  GLuint        basic_gllist;
  GLuint        global_list;
  int           curx;
  int           cury;
  int           curz;			/* current rooms position */


  void drawMarker(Coordinate & pos);



public slots:
  void moveMarker(Coordinate, Coordinate);


public:
  GLfloat       angley;
  GLfloat       anglex;
  GLfloat       anglez;
  float           userx;
  float           usery;
  float           userz;		/* additional shift added by user */

  RendererWidget( QWidget *parent);
  void drawExit(Coordinate & from, Coordinate & to, uint dir);
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
#endif

