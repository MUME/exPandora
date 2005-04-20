
#include <cstdio>
#include <cmath>
#include <qgl.h>
#include <qimage.h>
#include <qapplication.h>
#include <qdatetime.h>

#include "Terrain.h"
#include "Display.h"
#include "RoomCollection.h"
#include "MainWindow.h"


const GLfloat RendererWidget::marker_colour[]  =  {1.0, 0.2, 0.0, 0.6};


RendererWidget::RendererWidget( QWidget *parent, const char *name )
  : QGLWidget( parent, name )
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

  glShadeModel(GL_SMOOTH);
  glClearColor (0.0, 0.0, 0.0, 0.0);	/* This Will Clear The Background Color To Black */
  glPointSize (4.0);		/* Add point size, to make it clear */
  glLineWidth (2.0);		/* Add line width,   ditto */

  glEnable(GL_BLEND);
  glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  
  glEnable(GL_LINE_SMOOTH);    
  glEnable(GL_POLYGON_SMOOTH);  

  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
  glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
  glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);
  glPixelStorei(GL_RGBA, 1);
    
  basic_gllist = glGenLists(1);
  if (basic_gllist != 0) {
    glNewList(basic_gllist, GL_COMPILE);

    glBegin(GL_QUADS);
    glVertex3f( ROOM_SIZE,  ROOM_SIZE, 0.0f);
    glVertex3f( ROOM_SIZE, -ROOM_SIZE, 0.0f);
    glVertex3f(-ROOM_SIZE, -ROOM_SIZE, 0.0f);
    glVertex3f(-ROOM_SIZE,  ROOM_SIZE, 0.0f);
    glEnd();
      
    glEndList();
  }

  for (map<char, Terrain *>::iterator i = terrains.begin(); i != terrains.end(); i++) {
    p = (*i).second;

    glGenTextures(1, &p->texture);

    buf1.load( p->filename );
    tex1 = QGLWidget::convertToGLFormat( buf1 ); // flipped 32bit RGBA

    glGenTextures(1, &p->texture );
    glBindTexture(GL_TEXTURE_2D, p->texture );
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex1.width(), tex1.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, tex1.bits() );
        
      
      
    p->gllist = glGenLists(1);
    if (p->gllist != 0) {
      glNewList(p->gllist, GL_COMPILE);

      glEnable(GL_TEXTURE_2D);
      
      glBindTexture(GL_TEXTURE_2D, p->texture);
        
      glBegin(GL_QUADS);
      glTexCoord2f(0.0, 1.0);
      glVertex3f(-ROOM_SIZE,  ROOM_SIZE, 0.0f);
      glTexCoord2f(0.0, 0.0);
      glVertex3f(-ROOM_SIZE, -ROOM_SIZE, 0.0f);
      glTexCoord2f(1.0, 0.0);
      glVertex3f( ROOM_SIZE, -ROOM_SIZE, 0.0f);
      glTexCoord2f(1.0, 1.0);
      glVertex3f( ROOM_SIZE,  ROOM_SIZE, 0.0f);

      glEnd();
      glDisable(GL_TEXTURE_2D);
        
        
        
      glEndList();
    }
  }
    

}


void RendererWidget::resizeGL( int width, int height )
{
  
  glViewport (0, 0, (GLint) width, (GLint) height);	
  glMatrixMode (GL_PROJECTION);	
  glLoadIdentity ();		
  gluPerspective(50.0f, (GLfloat) width / (GLfloat) height, 0.5f, 1000.0f);
  glMatrixMode (GL_MODELVIEW);	
  shiftView();
}


void Display::run()
{
  printf("Starting renderer ...\n");

  QApplication::setColorSpec( QApplication::CustomColor );
  if ( !QGLFormat::hasOpenGL() ) {
    qWarning( "This system has no OpenGL support. Exiting." );
    return;
  }


  
  renderer_window = new MainWindow( 0, "MainWindow" );
  qApp->setMainWidget( renderer_window );
  
  QGLFormat f;
  f.setDoubleBuffer( TRUE );                 
  f.setDirectRendering( TRUE );              
  f.setRgba( TRUE );
  f.setDepth( TRUE );

  QGLFormat::setDefaultFormat( f );

  renderer_window->show();

  printf("Renderer: ready and awaiting for events...\r\n");
  
  return;
}

void RendererWidget::moveMarker(Coordinate * oldPos, Coordinate * newPos)
{
  if (oldPos != 0) {
    glColor4f(0,0,0,0);
    drawMarker(oldPos);
  }

  if (newPos != 0) {
    glColor4f(marker_colour[0],marker_colour[1],marker_colour[2],marker_colour[3]);
    drawMarker(newPos);
  }
}


void RendererWidget::drawMarker(Coordinate * pos) {
  int dx, dy, dz;
  dx = pos->x - curx;
  dy = pos->y - cury;
  dz = (pos->z - curz) * DIST_Z;

      
  /* upper */
  glBegin(GL_TRIANGLES);
  glVertex3f(               dx, MARKER_SIZE + dy + ROOM_SIZE,  0.0f + dz);
  glVertex3f(-MARKER_SIZE + dx,               dy + ROOM_SIZE,  0.0f + dz);
  glVertex3f(+MARKER_SIZE + dx,               dy + ROOM_SIZE,  0.0f + dz);
  glEnd();

  /* lower */
  glBegin(GL_TRIANGLES);
  glVertex3f(               dx,-MARKER_SIZE + dy - ROOM_SIZE,  0.0f + dz);
  glVertex3f(-MARKER_SIZE + dx,               dy - ROOM_SIZE,  0.0f + dz);
  glVertex3f(+MARKER_SIZE + dx,               dy - ROOM_SIZE,  0.0f + dz);
  glEnd();

  /* right */
  glBegin(GL_TRIANGLES);
  glVertex3f(               dx + ROOM_SIZE, +MARKER_SIZE + dy, 0.0f + dz);
  glVertex3f( MARKER_SIZE + dx + ROOM_SIZE,            dy,     0.0f + dz);
  glVertex3f(               dx + ROOM_SIZE, -MARKER_SIZE + dy, 0.0f + dz);
  glEnd();

  /* left */
  glBegin(GL_TRIANGLES);
  glVertex3f(               dx - ROOM_SIZE, +MARKER_SIZE + dy, 0.0f + dz);
  glVertex3f(-MARKER_SIZE + dx - ROOM_SIZE,            dy,     0.0f + dz);
  glVertex3f(               dx - ROOM_SIZE, -MARKER_SIZE + dy, 0.0f + dz);
  glEnd();

}



void RendererWidget::drawRoom(Room * pr)
{
  GLfloat dx, dy, dz;
  set<int> * rc;
  Coordinate * r;
  Coordinate * p = pr->getCoordinate();
  unsigned int k;
  char lines, texture;
  float distance;

  roomCache[pr->getId()] = p;

  dx = p->x - curx;
  dy = p->y - cury;
  dz = (p->z - curz) * DIST_Z;
  lines = 1;
  texture = 1;
    
  if (PointInFrustum(dx, dy, dz) != true)
    return;

  int z = p->z - curz;
  
  if (z <= -14) {
    colour[0] = 0; colour[1] = 0; colour[2] = 0; colour[3] = 0; 
  }
  else if (z <= -10) {
    colour[0] = 0.1; colour[1] = 0.1; colour[2] = 0.1; colour[3] = 0.1; 
  }
  else if (z <= -5) {
    colour[0] = 0.3; colour[1] = 0.3; colour[2] = 0.3; colour[3] = 0.2; 
  }
  else if (z < -1) {
    colour[0] = 0.4; colour[1] = 0.4; colour[2] = 0.4; colour[3] = 0.3; 
  }
  else if (z == -1) {
    colour[0] = 0.5; colour[1] = 0.5; colour[2] = 0.5; colour[3] = 0.4; 
  }
  else if (z == 0) {
    colour[0] = 0.1; colour[1] = 0.8; colour[2] = 0.8; colour[3] = 0.6; 
  }
  else if (z == 1) {
    colour[0] = 0.0; colour[1] = 0.5; colour[2] = 0.9; colour[3] = 0.2; 
  }
  else {
    colour[0] = 0.0; colour[1] = 0.5; colour[2] = 0.9; colour[3] = 0.1; 
  }
  

  distance = m_Frustum[FRONT][A] * dx + m_Frustum[FRONT][B] * dy + 
    m_Frustum[FRONT][C] * dz + m_Frustum[FRONT][D];
    
  if (distance >= details_visibility_range) 
    lines = 0;

  if (distance >= texture_visibilit_range) 
    texture = 0;

    
  glTranslatef(dx, dy, dz);
  if (pr->getTerrain() != NULL && texture) {
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, (terrains.find(pr->getTerrain()))->second->texture);

    glBegin(GL_QUADS);
      
    glTexCoord2f(0.0, 1.0);
    glVertex3f(-ROOM_SIZE,  ROOM_SIZE, 0.0f);
    glTexCoord2f(0.0, 0.0);
    glVertex3f(-ROOM_SIZE, -ROOM_SIZE, 0.0f);
    glTexCoord2f(1.0, 0.0);
    glVertex3f( ROOM_SIZE, -ROOM_SIZE, 0.0f);
    glTexCoord2f(1.0, 1.0);
    glVertex3f( ROOM_SIZE,  ROOM_SIZE, 0.0f);
      
    glEnd();
    glDisable(GL_TEXTURE_2D);

  } else {
    glCallList(basic_gllist);
  }              
    
  glTranslatef(-dx, -dy, -dz);

  if (lines == 0)
    return;
    
  for (k = 0; k < pr->numExits(); k++) 

    {
      rc = pr->getNeighbours(k);
      if (rc == 0) continue;
      for(set<int>::iterator i = rc->begin(); i != rc->end(); i++) {
	if (halfDrawnSources.find(*i) == halfDrawnSources.end()) {
	  HalfExit * h = undrawnExits[pr->getId()];
	  if (h == 0) {
	    h =  new HalfExit(pr->getCoordinate());
	    undrawnExits[pr->getId()] = h;
	  }
	  h[k].insert(*i);
	}
	else Coordinate * r = undrawnExits[*i]->from;
	
	drawExit(dx, dy, dz, r);
	
      } 


    }

        
    
}


void RendererWidget::drawExit(int dx, int dy, int dz, Coordinate * r) {
  GLfloat kx, ky, kz;
  GLfloat sx, sy;

  int dx2 = r->x - curx;
  int dy2 = r->y - cury;
  int dz2 = (r->z - curz) * DIST_Z;
  
  dx2 = (dx + dx2) / 2;
  dy2 = (dy + dy2) / 2;
  dz2 = (dz + dz2) / 2;


  kx = 0;
  ky = 0;
  kz = 0;
  
  sx = 0;
  sy = 0;
	
  Coordinate * dir = 0;
  if (k < Coordinate::stdMoves.size()) 
    dir = Coordinate::stdMoves[k];
  else 
    dir = cmm.activate();

  if (dir->y > 0) {
    ky = +(ROOM_SIZE + 0.2);
    sy = +ROOM_SIZE;
  } 
  else if (dir->y < 0) {
    ky = -(ROOM_SIZE + 0.2);
    sy = -ROOM_SIZE;
  }
  if (dir->x > 0) {
    kx = +(ROOM_SIZE + 0.2);
    sx = +ROOM_SIZE;
  } 
  else if (dir->x < 0) {
    kx = -(ROOM_SIZE + 0.2);
    sx = -ROOM_SIZE;
  }
  if (dir->z > 0) {
    kz = +(ROOM_SIZE + 0.2);
  } else if (dir->z < 0) {
    kz = -(ROOM_SIZE + 0.2);
  } 

  glColor4f(0, 1.0, 0.0, colour[3] + 0.2);

                
  glBegin(GL_LINES);
  glVertex3f(dx + sx, dy + sy, dz);
  glVertex3f(dx + kx, dy + ky, dz + kz);
  glVertex3f(dx + kx, dy + ky, dz + kz);
  glVertex3f(dx2, dy2, dz2);
  glEnd();

  glColor4f(colour[0], colour[1], colour[2], colour[3]);

}


void RendererWidget::shiftView()
{
    
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glLoadIdentity();

  glEnable(GL_TEXTURE_2D);

    
  glColor3ub(255, 0, 0);

  glTranslatef(0, 0, userz);

  glRotatef(anglex, 1.0f, 0.0f, 0.0f);
  glRotatef(angley, 0.0f, 1.0f, 0.0f);
  glRotatef(anglez, 0.0f, 0.0f, 1.0f);

  glTranslatef(userx, usery, 0);

  CalculateFrustum();
    
  glColor4f(0.1, 0.8, 0.8, 0.4);

  colour[0] = 0.1; colour[1] = 0.8; colour[2] = 0.8; colour[3] = 0.4; 
  

  emit viewableAreaChanged((float **)m_Frustum); 
}



void Display::toggle_renderer_reaction()
{
  QKeyEvent * k = new QKeyEvent(QEvent::KeyPress, 0, 'r', 0, NULL, FALSE, 0);
  QApplication::postEvent( renderer_window->renderer, k );
}




void RendererWidget::NormalizePlane(float frustum[6][4], int side)
{
  // Here we calculate the magnitude of the normal to the plane (point A B C)
  // Remember that (A, B, C) is that same thing as the normal's (X, Y, Z).
  // To calculate magnitude you use the equation:  magnitude = sqrt( x^2 + y^2 + z^2)
  float magnitude = (float)sqrt( frustum[side][A] * frustum[side][A] + 
				 frustum[side][B] * frustum[side][B] + 
				 frustum[side][C] * frustum[side][C] );

  // Then we divide the plane's values by it's magnitude.
  // This makes it easier to work with.
  frustum[side][A] /= magnitude;
  frustum[side][B] /= magnitude;
  frustum[side][C] /= magnitude;
  frustum[side][D] /= magnitude; 
}

void RendererWidget::CalculateFrustum()
{    
  float   proj[16];                               // This will hold our projection matrix
  float   modl[16];                               // This will hold our modelview matrix
  float   clip[16];                               // This will hold the clipping planes

  glGetFloatv( GL_PROJECTION_MATRIX, proj );

  glGetFloatv( GL_MODELVIEW_MATRIX, modl );

  // Now that we have our modelview and projection matrix, if we combine these 2 matrices,
  // it will give us our clipping planes.  To combine 2 matrices, we multiply them.

  clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
  clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
  clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
  clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

  clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
  clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
  clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
  clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

  clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
  clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
  clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
  clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

  clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
  clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
  clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
  clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];
    
  // Now we actually want to get the sides of the frustum.  To do this we take
  // the clipping planes we received above and extract the sides from them.

  // This will extract the RIGHT side of the frustum
  m_Frustum[RIGHT][A] = clip[ 3] - clip[ 0];
  m_Frustum[RIGHT][B] = clip[ 7] - clip[ 4];
  m_Frustum[RIGHT][C] = clip[11] - clip[ 8];
  m_Frustum[RIGHT][D] = clip[15] - clip[12];

  // Now that we have a normal (A,B,C) and a distance (D) to the plane,
  // we want to normalize that normal and distance.

  // Normalize the RIGHT side
  NormalizePlane(m_Frustum, RIGHT);

  // This will extract the LEFT side of the frustum
  m_Frustum[LEFT][A] = clip[ 3] + clip[ 0];
  m_Frustum[LEFT][B] = clip[ 7] + clip[ 4];
  m_Frustum[LEFT][C] = clip[11] + clip[ 8];
  m_Frustum[LEFT][D] = clip[15] + clip[12];

  // Normalize the LEFT side
  NormalizePlane(m_Frustum, LEFT);

  // This will extract the BOTTOM side of the frustum
  m_Frustum[BOTTOM][A] = clip[ 3] + clip[ 1];
  m_Frustum[BOTTOM][B] = clip[ 7] + clip[ 5];
  m_Frustum[BOTTOM][C] = clip[11] + clip[ 9];
  m_Frustum[BOTTOM][D] = clip[15] + clip[13];

  // Normalize the BOTTOM side
  NormalizePlane(m_Frustum, BOTTOM);

  // This will extract the TOP side of the frustum
  m_Frustum[TOP][A] = clip[ 3] - clip[ 1];
  m_Frustum[TOP][B] = clip[ 7] - clip[ 5];
  m_Frustum[TOP][C] = clip[11] - clip[ 9];
  m_Frustum[TOP][D] = clip[15] - clip[13];

  // Normalize the TOP side
  NormalizePlane(m_Frustum, TOP);

  // This will extract the BACK side of the frustum
  m_Frustum[BACK][A] = clip[ 3] - clip[ 2];
  m_Frustum[BACK][B] = clip[ 7] - clip[ 6];
  m_Frustum[BACK][C] = clip[11] - clip[10];
  m_Frustum[BACK][D] = clip[15] - clip[14];

  // Normalize the BACK side
  NormalizePlane(m_Frustum, BACK);

  // This will extract the FRONT side of the frustum
  m_Frustum[FRONT][A] = clip[ 3] + clip[ 2];
  m_Frustum[FRONT][B] = clip[ 7] + clip[ 6];
  m_Frustum[FRONT][C] = clip[11] + clip[10];
  m_Frustum[FRONT][D] = clip[15] + clip[14];

  // Normalize the FRONT side
  NormalizePlane(m_Frustum, FRONT);
    
}

bool RendererWidget::PointInFrustum( float x, float y, float z )
{
  // Go through all the sides of the frustum
  for(int i = 0; i < 6; i++ )
    {
      // Calculate the plane equation and check if the point is behind a side of the frustum
      if(m_Frustum[i][A] * x + m_Frustum[i][B] * y + m_Frustum[i][C] * z + m_Frustum[i][D] <= 0)
        {
	  // The point was behind a side, so it ISN'T in the frustum
	  return false;
        }
    }

  // The point was inside of the frustum (In front of ALL the sides of the frustum)
  return true;
}
