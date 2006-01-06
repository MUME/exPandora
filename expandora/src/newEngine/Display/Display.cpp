
#include <cstdio>

#include <qgl.h>
#include <qimage.h>
#include <qapplication.h>
#include <qdatetime.h>
#include <QKeyEvent>
#include <QChar>

#include "Terrain.h"
#include "Display.h"
#include "RoomCollection.h"
#include "MainWindow.h"
#include "CachedRoom.h"

/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
extern "C" MY_EXPORT Component * createComponent()
{
  return new DisplayComponent;
}


const GLfloat RendererWidget::marker_colour[] =
  {
    1.0, 0.2, 0.0, 0.6
  };
const int RendererWidget::texture_visibilit_range = 300;
const int RendererWidget::details_visibility_range = 500;


RendererWidget::RendererWidget( QWidget *parent)
    : QGLWidget( parent)
{
  angley = 0;
  anglex = 0;
  anglez = 0;
  userx = 0;
  usery = 0;
  userz = BASE_Z;		/* additional shift added by user */
  curx = 0;
  cury = 0;
  curz = 0;			/* current rooms position */
}


void RendererWidget::initializeGL()
{
  QImage tex1, buf1;
  Terrain * p;

  glShadeModel( GL_SMOOTH );
  glClearColor ( 0.0, 0.0, 0.0, 0.0 );	/* This Will Clear The Background Color To Black */
  glPointSize ( 4.0 );		/* Add point size, to make it clear */
  glLineWidth ( 2.0 );		/* Add line width,   ditto */

  glEnable( GL_BLEND );
  glBlendFunc ( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

  glEnable( GL_LINE_SMOOTH );
  glEnable( GL_POLYGON_SMOOTH );

  glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
  glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
  glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );


  glTexEnvf( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
  glPixelStorei( GL_RGBA, 1 );

  basic_gllist = glGenLists( 1 );
  if ( basic_gllist != 0 )
  {
    glNewList( basic_gllist, GL_COMPILE );

    glBegin( GL_QUADS );
    glVertex3f( ROOM_SIZE, ROOM_SIZE, 0.0f );
    glVertex3f( ROOM_SIZE, -ROOM_SIZE, 0.0f );
    glVertex3f( -ROOM_SIZE, -ROOM_SIZE, 0.0f );
    glVertex3f( -ROOM_SIZE, ROOM_SIZE, 0.0f );
    glEnd();

    glEndList();
  }

  for ( vector<Terrain *>::iterator i = Terrain::terrains.begin(); i != Terrain::terrains.end(); ++i )
  {
    p = ( *i );
    if (!p) continue;

    glGenTextures( 1, &p->texture );

    buf1.load( p->filename );
    tex1 = QGLWidget::convertToGLFormat( buf1 ); // flipped 32bit RGBA

    glGenTextures( 1, &p->texture );
    glBindTexture( GL_TEXTURE_2D, p->texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );

    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA, tex1.width(), tex1.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex1.bits() );



    p->gllist = glGenLists( 1 );
    if ( p->gllist != 0 )
    {
      glNewList( p->gllist, GL_COMPILE );

      glEnable( GL_TEXTURE_2D );

      glBindTexture( GL_TEXTURE_2D, p->texture );

      glBegin( GL_QUADS );
      glTexCoord2f( 0.0, 1.0 );
      glVertex3f( -ROOM_SIZE, ROOM_SIZE, 0.0f );
      glTexCoord2f( 0.0, 0.0 );
      glVertex3f( -ROOM_SIZE, -ROOM_SIZE, 0.0f );
      glTexCoord2f( 1.0, 0.0 );
      glVertex3f( ROOM_SIZE, -ROOM_SIZE, 0.0f );
      glTexCoord2f( 1.0, 1.0 );
      glVertex3f( ROOM_SIZE, ROOM_SIZE, 0.0f );

      glEnd();
      glDisable( GL_TEXTURE_2D );



      glEndList();
    }
  }


}


void RendererWidget::resizeGL( int width, int height )
{

  glViewport ( 0, 0, ( GLint ) width, ( GLint ) height );
  glMatrixMode ( GL_PROJECTION );
  glLoadIdentity ();
  gluPerspective( 50.0f, ( GLfloat ) width / ( GLfloat ) height, 0.5f, 1000.0f );
  glMatrixMode ( GL_MODELVIEW );
  shiftView();
}

void DisplayComponent::start()
{
  if ( !QGLFormat::hasOpenGL() )
  {
    throw "This system has no OpenGL support.";
  }
  resolveTerrains();

  f.setDoubleBuffer( TRUE );
  f.setDirectRendering( TRUE );
  f.setRgba( TRUE );
  f.setDepth( TRUE );

  QGLFormat::setDefaultFormat( f );

  renderer_window = new MainWindow( 0 );
  renderer =  new RendererWidget( renderer_window );
  QObject::connect(renderer, SIGNAL(viewableAreaChanged(QObject *,Frustum *)), this, SIGNAL(lookingForRooms(QObject *,Frustum *)));
  renderer_window->setCentralWidget( renderer );
  renderer_window->renderer = renderer;
  renderer_window->show();

  Component::start();

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


void DisplayComponent::playerMoved(Coordinate * from, Coordinate * to)
{
  renderer->moveMarker(from, to);
}


void RendererWidget::moveMarker( Coordinate * oldPos, Coordinate * newPos )
{
  if ( oldPos != 0 )
  {
    glColor4f( 0, 0, 0, 0 );
    drawMarker( oldPos );
  }

  if ( newPos != 0 )
  {
    glColor4f( marker_colour[ 0 ], marker_colour[ 1 ], marker_colour[ 2 ], marker_colour[ 3 ] );
    drawMarker( newPos );
  }
}


void RendererWidget::drawMarker( Coordinate * pos )
{
  int dx, dy, dz;
  dx = pos->x - curx;
  dy = pos->y - cury;
  dz = ( pos->z - curz ) * DIST_Z;


  /* upper */
  glBegin( GL_TRIANGLES );
  glVertex3f( dx, MARKER_SIZE + dy + ROOM_SIZE, 0.0f + dz );
  glVertex3f( -MARKER_SIZE + dx, dy + ROOM_SIZE, 0.0f + dz );
  glVertex3f( + MARKER_SIZE + dx, dy + ROOM_SIZE, 0.0f + dz );
  glEnd();

  /* lower */
  glBegin( GL_TRIANGLES );
  glVertex3f( dx, -MARKER_SIZE + dy - ROOM_SIZE, 0.0f + dz );
  glVertex3f( -MARKER_SIZE + dx, dy - ROOM_SIZE, 0.0f + dz );
  glVertex3f( + MARKER_SIZE + dx, dy - ROOM_SIZE, 0.0f + dz );
  glEnd();

  /* right */
  glBegin( GL_TRIANGLES );
  glVertex3f( dx + ROOM_SIZE, + MARKER_SIZE + dy, 0.0f + dz );
  glVertex3f( MARKER_SIZE + dx + ROOM_SIZE, dy, 0.0f + dz );
  glVertex3f( dx + ROOM_SIZE, -MARKER_SIZE + dy, 0.0f + dz );
  glEnd();

  /* left */
  glBegin( GL_TRIANGLES );
  glVertex3f( dx - ROOM_SIZE, + MARKER_SIZE + dy, 0.0f + dz );
  glVertex3f( -MARKER_SIZE + dx - ROOM_SIZE, dy, 0.0f + dz );
  glVertex3f( dx - ROOM_SIZE, -MARKER_SIZE + dy, 0.0f + dz );
  glEnd();

}



void RendererWidget::receiveRoom( QObject * owner, Room * pr )
{



  Coordinate * p = pr->getCoordinate();

  char lines, texture;
  float distance;

  CachedRoom * cr = new CachedRoom( pr, owner );

  Coordinate d( p->x - curx, p->y - cury, ( p->z - curz ) * DIST_Z );


  lines = 1;
  texture = 1;

  if ( !frustum.PointInFrustum( &d ) )
    return ;

  int z = p->z - curz;

  if ( z <= -14 )
  {
    colour[ 0 ] = 0;
    colour[ 1 ] = 0;
    colour[ 2 ] = 0;
    colour[ 3 ] = 0;
  }
  else if ( z <= -10 )
  {
    colour[ 0 ] = 0.1;
    colour[ 1 ] = 0.1;
    colour[ 2 ] = 0.1;
    colour[ 3 ] = 0.1;
  }
  else if ( z <= -5 )
  {
    colour[ 0 ] = 0.3;
    colour[ 1 ] = 0.3;
    colour[ 2 ] = 0.3;
    colour[ 3 ] = 0.2;
  }
  else if ( z < -1 )
  {
    colour[ 0 ] = 0.4;
    colour[ 1 ] = 0.4;
    colour[ 2 ] = 0.4;
    colour[ 3 ] = 0.3;
  }
  else if ( z == -1 )
  {
    colour[ 0 ] = 0.5;
    colour[ 1 ] = 0.5;
    colour[ 2 ] = 0.5;
    colour[ 3 ] = 0.4;
  }
  else if ( z == 0 )
  {
    colour[ 0 ] = 0.1;
    colour[ 1 ] = 0.8;
    colour[ 2 ] = 0.8;
    colour[ 3 ] = 0.6;
  }
  else if ( z == 1 )
  {
    colour[ 0 ] = 0.0;
    colour[ 1 ] = 0.5;
    colour[ 2 ] = 0.9;
    colour[ 3 ] = 0.2;
  }
  else
  {
    colour[ 0 ] = 0.0;
    colour[ 1 ] = 0.5;
    colour[ 2 ] = 0.9;
    colour[ 3 ] = 0.1;
  }


  distance = frustum.getDistance( &d );

  if ( distance >= details_visibility_range )
    lines = 0;

  if ( distance >= texture_visibilit_range )
    texture = 0;


  glTranslatef( d.x, d.y, d.z );
  if ( ( pr->getTerrain() != 0 ) && texture )
  {
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, ( Terrain::terrains[pr->getTerrain()] ->texture ));

    glBegin( GL_QUADS );

    glTexCoord2f( 0.0, 1.0 );
    glVertex3f( -ROOM_SIZE, ROOM_SIZE, 0.0f );
    glTexCoord2f( 0.0, 0.0 );
    glVertex3f( -ROOM_SIZE, -ROOM_SIZE, 0.0f );
    glTexCoord2f( 1.0, 0.0 );
    glVertex3f( ROOM_SIZE, -ROOM_SIZE, 0.0f );
    glTexCoord2f( 1.0, 1.0 );
    glVertex3f( ROOM_SIZE, ROOM_SIZE, 0.0f );

    glEnd();
    glDisable( GL_TEXTURE_2D );

  }
  else
  {
    glCallList( basic_gllist );
  }




  glTranslatef( -d.x, -d.y, -d.z );

  if ( lines == 0 )
    return ;

  // make the cached room draw its exits
  cr->drawExits( this );

}


void RendererWidget::drawExit( Coordinate * from, Coordinate * to, unsigned int k )
{
  GLfloat kx, ky, kz;
  GLfloat sx, sy;

  int dx = from->x - curx;
  int dy = from->y - cury;
  int dz = ( from->z - curz ) * DIST_Z;

  int dx2 = to->x - curx;
  int dy2 = to->y - cury;
  int dz2 = ( to->z - curz ) * DIST_Z;

  dx2 = ( dx + dx2 ) / 2;
  dy2 = ( dy + dy2 ) / 2;
  dz2 = ( dz + dz2 ) / 2;


  kx = 0;
  ky = 0;
  kz = 0;

  sx = 0;
  sy = 0;

  Coordinate * dir = 0;
  if ( k < Coordinate::stdMoves.size() )
    dir = Coordinate::stdMoves[ k ];
  else
    dir = cmm.activate();

  if ( dir->y > 0 )
  {
    ky = + ( ROOM_SIZE + 0.2 );
    sy = + ROOM_SIZE;
  }
  else if ( dir->y < 0 )
  {
    ky = -( ROOM_SIZE + 0.2 );
    sy = -ROOM_SIZE;
  }
  if ( dir->x > 0 )
  {
    kx = + ( ROOM_SIZE + 0.2 );
    sx = + ROOM_SIZE;
  }
  else if ( dir->x < 0 )
  {
    kx = -( ROOM_SIZE + 0.2 );
    sx = -ROOM_SIZE;
  }
  if ( dir->z > 0 )
  {
    kz = + ( ROOM_SIZE + 0.2 );
  }
  else if ( dir->z < 0 )
  {
    kz = -( ROOM_SIZE + 0.2 );
  }

  glColor4f( 0, 1.0, 0.0, colour[ 3 ] + 0.2 );


  glBegin( GL_LINES );
  glVertex3f( dx + sx, dy + sy, dz );
  glVertex3f( dx + kx, dy + ky, dz + kz );
  glVertex3f( dx + kx, dy + ky, dz + kz );
  glVertex3f( dx2, dy2, dz2 );
  glEnd();

  glColor4f( colour[ 0 ], colour[ 1 ], colour[ 2 ], colour[ 3 ] );

}


void RendererWidget::shiftView()
{

  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glLoadIdentity();

  glEnable( GL_TEXTURE_2D );


  glColor3ub( 255, 0, 0 );

  glTranslatef( 0, 0, userz );

  glRotatef( anglex, 1.0f, 0.0f, 0.0f );
  glRotatef( angley, 0.0f, 1.0f, 0.0f );
  glRotatef( anglez, 0.0f, 0.0f, 1.0f );

  glTranslatef( userx, usery, 0 );

  CalculateFrustum();

  glColor4f( 0.1, 0.8, 0.8, 0.4 );

  colour[ 0 ] = 0.1;
  colour[ 1 ] = 0.8;
  colour[ 2 ] = 0.8;
  colour[ 3 ] = 0.4;


  emit viewableAreaChanged( this, &frustum );
}



void DisplayComponent::toggle_renderer_reaction()
{
  QKeyEvent * k = new QKeyEvent( QEvent::KeyPress, 'r', Qt::NoModifier );
  QApplication::postEvent( renderer, k );
}






void RendererWidget::CalculateFrustum()
{
  float proj[ 16 ];                               // This will hold our projection matrix
  float modl[ 16 ];                               // This will hold our modelview matrix

  glGetFloatv( GL_PROJECTION_MATRIX, proj );

  glGetFloatv( GL_MODELVIEW_MATRIX, modl );


  // Now that we have our modelview and projection matrix, if we combine these 2 matrices,
  // it will give us our clipping planes.
  frustum.rebuild( proj, modl );

}


