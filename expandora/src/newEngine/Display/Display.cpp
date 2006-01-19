
#include <cstdio>

#include <qgl.h>
#include <qimage.h>
#include <qapplication.h>
#include <qdatetime.h>
#include <QKeyEvent>
#include <QChar>

#include "Terrain.h"
#include "Display.h"
#include "MainWindow.h"
#include "CachedRoom.h"


using namespace Qt;

/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
#ifndef MONOLITHIC
extern "C" MY_EXPORT Component * createComponent()
{
  return new DisplayComponent;
}
#else
Initializer<DisplayComponent> display("Display");
#endif


ConnectionType DisplayComponent::requiredConnectionType(const QString & str) {
  if (str == SLOT(playerMoved(Coordinate *, Coordinate *)))
    return QueuedConnection;
  else 
    return DirectConnection;
}




DisplayComponent::DisplayComponent() {
  renderer = 0;
  renderer_window = 0;
  f.setDoubleBuffer( TRUE );
  f.setDirectRendering( TRUE );
  f.setRgba( TRUE );
  f.setDepth( TRUE );

  QGLFormat::setDefaultFormat( f );
  
}

void DisplayComponent::init()
{
  if ( !QGLFormat::hasOpenGL() )
  {
    throw "This system has no OpenGL support.";
  }

  resolveTerrains();

  renderer_window = new MainWindow( 0 );
  renderer = renderer_window->renderer;


  QObject::connect(renderer, SIGNAL(viewableAreaChanged(RoomRecipient *,Frustum *)), this, SIGNAL(lookingForRooms(RoomRecipient *,Frustum *)), DirectConnection);
  QObject::connect(renderer_window, SIGNAL(openMap(QString)), this, SIGNAL(openMap(QString)));

}

void DisplayComponent::resolveTerrains()
{
  QString terrainIds = options["terrains"].toString();
  for (int i = 0; i<terrainIds.size(); ++i)
  {
    QChar code = (terrainIds[i]);
    
    Terrain * terrain = new Terrain();
    
    QString terrainFile = options[QString(code)].toString();
    terrain->filename = terrainFile;
    terrain->desc = extractDescription(terrainFile);
    char codeChar = code.toLatin1();
    terrain->pattern = codeChar;
    if (Terrain::terrains.size() <= codeChar) Terrain::terrains.resize(codeChar+1, 0);
    Terrain::terrains[codeChar] = terrain;
  }

}

QString DisplayComponent::extractDescription(QString & fileName)
{
  QString result = "";
  bool dotFound = false;
  for(int i = fileName.size() -1; i >= 0; --i)
  {
    if ((!dotFound) && fileName[i] == '.') dotFound = true;
    else if (dotFound)
    {
      if (fileName[i] == '/' || fileName[i] == '\\') return result;
      else result.prepend(fileName[i]);
    }
  }
  return result;
}


void DisplayComponent::playerMoved(Coordinate from, Coordinate to)
{
  renderer->moveMarker(from, to);
}


