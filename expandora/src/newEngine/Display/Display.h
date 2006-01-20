#ifndef DISPLAY
#define DISPLAY

#include <qgl.h>
#include "Component.h"
#include "Room.h"
#include "CachedRoom.h"
#include "Frustum.h"
#include "RoomRecipient.h"
#include "RoomAdmin.h"
#include "RendererWidget.h"
#include "MainWindow.h"

class DisplayComponent : public Component {
 Q_OBJECT
  
 signals:
  void lookingForRooms(RoomRecipient *, Frustum *);
  void openMap(QString);
  void deleteRoom();
  
 public slots:
  void playerMoved(Coordinate, Coordinate);
  
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
