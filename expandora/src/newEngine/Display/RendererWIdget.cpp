#include <QImage>
#include "RendererWidget.h"
#include "Terrain.h"
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
  glClearColor ( 0.0, 0.0, 0.0, 0.0 );	// This Will Clear The Background Color To Black 
  glPointSize ( 4.0 );		// Add point size, to make it clear 
  glLineWidth ( 2.0 );		// Add line width,   ditto 

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

void RendererWidget::moveMarker( Coordinate, Coordinate newPos )
{
    position = newPos;
    /*glColor4f( 0, 0, 0, 0 );
    drawMarker( oldPos );*/


    
    userx = -newPos.x;
    usery = -newPos.y;
    userz = -newPos.z + BASE_Z;
    shiftView();
    //glColor4f( marker_colour[ 0 ], marker_colour[ 1 ], marker_colour[ 2 ], marker_colour[ 3 ] );
    //drawMarker( position );
    swapBuffers();
    
  
  
}


void RendererWidget::drawMarker( Coordinate & pos )
{
  int dx, dy, dz;
  dx = pos.x;
  dy = pos.y ;
  dz = ( pos.z ) * DIST_Z;


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



void RendererWidget::receiveRoom( RoomAdmin * owner, Room * pr )
{



  Coordinate p = pr->getCoordinate();

  char lines, texture;
  float distance;

  CachedRoom * cr = new CachedRoom( pr, owner, this, roomsCache );

  Coordinate d( p.x, p.y , ( p.z ) * DIST_Z );


  lines = 1;
  texture = 1;

  
  
  int z = p.z;

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
  

  distance = frustum.getDistance( d );

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
  
  //owner->releaseRoom(this, pr->getId());
  

   cr->drawExits( this ); //this is only allowed because we are sure no one else called 
			  //drawExits(RendererWidget *) on any CachedRoom since the 
			  //Construction of this one

}


void RendererWidget::drawExit( Coordinate & from, Coordinate & to, unsigned int k )
{
  GLfloat kx = 0.0;
  GLfloat ky = 0.0;

  GLfloat dx = from.x;
  GLfloat dy = from.y;
  GLfloat dz = ( from.z ) * DIST_Z;

  GLfloat dx2 = ( dx + ((float)to.x)) / 2.0;
  GLfloat dy2 = ( dy + ((float)to.y)) / 2.0;
  GLfloat dz2 = ( dz + ((float)to.z) * DIST_Z ) / 2.0;


  
  Coordinate dir;
  if ( k < Coordinate::stdMoves.size() )
    dir = Coordinate::stdMoves[ k ];
  

  if ( dir.y > 0 )
  {
    ky = ROOM_SIZE;
  }
  else if ( dir.y < 0 )
  {
    ky = -ROOM_SIZE;
  }
  if ( dir.x > 0 )
  {
    kx = ROOM_SIZE;
  }
  else if ( dir.x < 0 )
  {
    kx = -ROOM_SIZE;
  }


  glColor4f( 0, 1.0, 0.0, colour[ 3 ] + 0.2 );


  glBegin( GL_LINES );
  glVertex3f( dx + kx, dy + ky, dz );
  glVertex3f( dx2, dy2, dz2 );
  glEnd();

  glColor4f( colour[ 0 ], colour[ 1 ], colour[ 2 ], colour[ 3 ] );

}


void RendererWidget::shiftView()
{
  swapBuffers();
  glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

  glLoadIdentity();

  glEnable( GL_TEXTURE_2D );


  glColor3ub( 255, 0, 0 );

  

  glRotatef( anglex, 1.0f, 0.0f, 0.0f );
  glRotatef( angley, 0.0f, 1.0f, 0.0f );
  glRotatef( anglez, 0.0f, 0.0f, 1.0f );

  glTranslatef( userx, usery, userz );

  CalculateFrustum();

  glColor4f( 0.1, 0.8, 0.8, 0.4 );

  colour[ 0 ] = 0.1;
  colour[ 1 ] = 0.8;
  colour[ 2 ] = 0.8;
  colour[ 3 ] = 0.4;


  emit viewableAreaChanged( this, &frustum );
  glColor4f( marker_colour[ 0 ], marker_colour[ 1 ], marker_colour[ 2 ], marker_colour[ 3 ] );
  drawMarker( position );
  
  
}


void RendererWidget::CalculateFrustum()
{
    float   proj[16];                               // This will hold our projection matrix
    float   modl[16];                               // This will hold our modelview matrix
    float   clip[16];                               // This will hold the clipping planes

    glPushMatrix ();
    
    glGetFloatv(GL_PROJECTION_MATRIX, proj);

    glGetFloatv(GL_MODELVIEW_MATRIX, modl);
    
    glLoadMatrixf (proj);
    glMultMatrixf (modl);
    
    glGetFloatv(GL_MODELVIEW_MATRIX, clip);
    glPopMatrix (); 
    
    frustum.rebuild(clip);

}

void RendererWidget::paintGL() {
  shiftView();
  
}
