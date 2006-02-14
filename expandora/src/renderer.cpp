#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <vector>

#include <QGLWidget>
#include <QImage>
#include <QApplication>
#include <QDateTime>
#include <QKeyEvent>

#include "renderer.h"
#include "configurator.h"


#include "stacks.h"
#include "utils.h"
#include "Map.h"

#include "userfunc.h"

using namespace std;

#if defined(Q_CC_MSVC)
#pragma warning(disable:4305) // init: truncation from const double to float
#endif

GLfloat marker_colour[4] =  {1.0, 0.1, 0.1, 0.9};


#define MARKER_SIZE           (ROOM_SIZE/2.0)

class MainWindow *renderer_window;


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

RendererWidget::RendererWidget( QWidget *parent )
     : QGLWidget( parent )
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
  
  last_drawn_marker = 0;
  last_drawn_trail = 0;

}


void RendererWidget::initializeGL()
{
  unsigned int i;
  
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

    
//    glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_TEXTURE_ENV_MODE_REPLACE);
  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
  glPixelStorei(GL_RGBA, 1);


    print_debug(DEBUG_RENDERER, "in init()");
    

    
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

    for (i = 0; i < conf.sectors.size(); i++) {
        conf.load_texture(&conf.sectors[i]);
    }
}


void RendererWidget::resizeGL( int width, int height )
{
    print_debug(DEBUG_RENDERER, "in resizeGL()");
  
    glViewport (0, 0, (GLint) width, (GLint) height);	
    glMatrixMode (GL_PROJECTION);	
    glLoadIdentity ();		
    gluPerspective(50.0f, (GLfloat) width / (GLfloat) height, 0.5f, 1000.0f);
    glMatrixMode (GL_MODELVIEW);	
  
    glredraw = 1;
//    display();
}


void RendererWidget::paintGL()
{
    print_debug(DEBUG_RENDERER, "in paintGL()");
//  QTime t;
//  t.start();

  display();
  draw();

    
//  printf("Rendering's done. Time elapsed %d ms\r\n", t.elapsed());
}


/* mode 1 = full marker, mode 2 - partial marker */
void RendererWidget::drawMarker(int dx, int dy, int dz, int mode)
{
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
    

      if (mode == 1) {
        /* left */
        glBegin(GL_QUADS);
        glVertex3f(dx - ROOM_SIZE - (MARKER_SIZE / 3.5), dy + ROOM_SIZE + (MARKER_SIZE / 3.5), 0.0f);
        glVertex3f(dx - ROOM_SIZE - (MARKER_SIZE / 3.5), dy - ROOM_SIZE                      , 0.0f);
        glVertex3f(dx - ROOM_SIZE                      , dy - ROOM_SIZE                      , 0.0f);
        glVertex3f(dx - ROOM_SIZE                      , dy + ROOM_SIZE + (MARKER_SIZE / 3.5), 0.0f);
        glEnd();

        /* right */
        glBegin(GL_QUADS);
        glVertex3f(dx + ROOM_SIZE                      , dy + ROOM_SIZE + (MARKER_SIZE / 3.5), 0.0f);
        glVertex3f(dx + ROOM_SIZE                      , dy - ROOM_SIZE                      , 0.0f);
        glVertex3f(dx + ROOM_SIZE + (MARKER_SIZE / 3.5), dy - ROOM_SIZE                      , 0.0f);
        glVertex3f(dx + ROOM_SIZE + (MARKER_SIZE / 3.5), dy + ROOM_SIZE + (MARKER_SIZE / 3.5), 0.0f);
        glEnd();

        /* upper */
        glBegin(GL_QUADS);
        glVertex3f(dx - ROOM_SIZE - (MARKER_SIZE / 3.5), dy + ROOM_SIZE + (MARKER_SIZE / 3.5), 0.0f);
        glVertex3f(dx - ROOM_SIZE - (MARKER_SIZE / 3.5), dy + ROOM_SIZE                      , 0.0f);
        glVertex3f(dx + ROOM_SIZE + (MARKER_SIZE / 3.5), dy + ROOM_SIZE                      , 0.0f);
        glVertex3f(dx + ROOM_SIZE + (MARKER_SIZE / 3.5), dy + ROOM_SIZE + (MARKER_SIZE / 3.5), 0.0f);
        glEnd();

        /* lower */
        glBegin(GL_QUADS);
        glVertex3f(dx - ROOM_SIZE - (MARKER_SIZE / 3.5), dy - ROOM_SIZE                      , 0.0f);
        glVertex3f(dx - ROOM_SIZE - (MARKER_SIZE / 3.5), dy - ROOM_SIZE + (MARKER_SIZE / 3.5), 0.0f);
        glVertex3f(dx + ROOM_SIZE + (MARKER_SIZE / 3.5), dy - ROOM_SIZE + (MARKER_SIZE / 3.5), 0.0f);
        glVertex3f(dx + ROOM_SIZE + (MARKER_SIZE / 3.5), dy - ROOM_SIZE                      , 0.0f);
        glEnd();
    }
}


void RendererWidget::glDrawMarkers()
{
    CRoom *p;
    unsigned int k;
    
    int dx, dy, dz;

    if (stacker.amount() == 0)
        return;
    
    
    glColor4f(marker_colour[0],marker_colour[1],marker_colour[2],marker_colour[3]);
    for (k = 0; k < stacker.amount(); k++) {
        
        p = stacker.get(k);
    
        if (p == NULL) {
            printf("RENDERER ERROR: Stuck upon corrupted room while drawing red pointers.\r\n");
            continue;
        }
    
        dx = p->x - curx;
        dy = p->y - cury;
        dz = (p->z - curz) /* * DIST_Z */;
    

        drawMarker(dx, dy, dz, 1);
    }
    
    
    if (last_drawn_marker != stacker.first()->id) {
        last_drawn_trail = last_drawn_marker;
        last_drawn_marker = stacker.first()->id;
    }

    if (last_drawn_trail) {
        glColor4f(marker_colour[0] / 1.1, marker_colour[1] / 1.5, marker_colour[2] / 1.5, marker_colour[3] / 1.5);
        p = Map.getroom(last_drawn_trail);
        if (p != NULL) {
            dx = p->x - curx;
            dy = p->y - cury;
            dz = (p->z - curz) ;
            drawMarker(dx, dy, dz, 2);
        }
    }
    
}




void RendererWidget::glDrawRoom(CRoom *p)
{
    GLfloat dx, dx2, dy, dy2, dz, dz2;
    CRoom *r;
    int k;
    float distance;
    bool lines, texture;    

    rooms_drawn_csquare++;
    
    dx = p->x - curx;
    dy = p->y - cury;
    dz = (p->z - curz) /* * DIST_Z */;
    dx2 = 0;
    dy2 = 0;
    dz2 = 0;
    lines = 1;
    texture = 1;
    
    if (PointInFrustum(dx, dy, dz) != true)
      return;
    
    rooms_drawn_total++;


    distance = m_Frustum[FRONT][A] * dx + m_Frustum[FRONT][B] * dy + 
               m_Frustum[FRONT][C] * dz + m_Frustum[FRONT][D];
    
    if (distance >= conf.get_details_vis()) 
      lines = 0;

    if (distance >= conf.get_texture_vis()) 
      texture = 0;

    
    glTranslatef(dx, dy, dz);
    if (p->sector && texture) {
      glEnable(GL_TEXTURE_2D);
      glBindTexture(GL_TEXTURE_2D, conf.sectors[ p->sector].texture);
      glCallList(conf.sectors[ p->sector].gllist);  
      glDisable(GL_TEXTURE_2D);
    } else {
      glCallList(basic_gllist);
    }              
    
    glTranslatef(-dx, -dy, -dz);

    if (lines == 0)
      return;
    
    for (k = 0; k <= 5; k++) 
      if (p->exits[k] != 0) {
        if (p->exits[k] < EXIT_UNDEFINED) {
            GLfloat kx, ky, kz;
            GLfloat sx, sy;

            r = Map.ids[p->exits[k]];

            dx2 = r->x - curx;
            dy2 = r->y - cury;
            dz2 = (r->z - curz) /* * DIST_Z */;

            dx2 = (dx + dx2) / 2;
            dy2 = (dy + dy2) / 2;
            dz2 = (dz + dz2) / 2;

            if (k == NORTH) {
                kx = 0;
                ky = +(ROOM_SIZE + 0.2);
                kz = 0;
                sx = 0;
                sy = +ROOM_SIZE;
            } else if (k == EAST) {
                kx = +(ROOM_SIZE + 0.2);
                ky = 0;
                kz = 0;
                sx = +ROOM_SIZE;
                sy = 0;
            } else if (k == SOUTH) {
                kx = 0;
                ky = -(ROOM_SIZE + 0.2);
                kz = 0;
                sx = 0;
                sy = -ROOM_SIZE;
            } else if (k == WEST) {
                kx = -(ROOM_SIZE + 0.2);
                ky = 0;
                kz = 0;
                sx = -ROOM_SIZE;
                sy = 0;
            } else if (k == UP) {
                kx = 0;
                ky = 0;
                kz = +(ROOM_SIZE + 0.2);
                sx = 0;
                sy = 0;
            } else /*if (k == DOWN) */{
                kx = 0;
                ky = 0;
                kz = -(ROOM_SIZE + 0.2);
                sx = 0;
                sy = 0;
            } 
            if (p->doors[k] != NULL) {
                if (strcmp(p->doors[k], "exit") == 0) {
                    glColor4f(0, 1.0, 0.0, colour[3] + 0.2);
                } else {
                    glColor4f(1.0, 0.0, 0.0, colour[3] + 0.2);
                }
            }
                
            glBegin(GL_LINES);
            glVertex3f(dx + sx, dy + sy, dz);
            glVertex3f(dx + kx, dy + ky, dz + kz);
            glVertex3f(dx + kx, dy + ky, dz + kz);
            glVertex3f(dx2, dy2, dz2);
            glEnd();

            glColor4f(colour[0], colour[1], colour[2], colour[3]);

        } 

	  else {
            GLfloat kx, ky, kz;

            if (k == NORTH) {
                dx2 = dx;
                dy2 = dy + 0.5;
                dz2 = dz;
            } else if (k == EAST) {
                dx2 = dx + 0.5;
                dy2 = dy;
                dz2 = dz;
            } else if (k == SOUTH) {
                dx2 = dx;
                dy2 = dy - 0.5;
                dz2 = dz;
            } else if (k == WEST) {
                dx2 = dx - 0.5;
                dy2 = dy;
                dz2 = dz;
            } else if (k == UP) {
                dx2 = dx;
                dy2 = dy;
                dz2 = dz + 0.5;
            } else if (k == DOWN) {
                dx2 = dx;
                dy2 = dy;
                dz2 = dz - 0.5;
            }

            kx = 0;
            ky = 0;
            kz = 0;


            if (k == NORTH) {
                ky = +(ROOM_SIZE);
            } else if (k == EAST) {
                kx = +(ROOM_SIZE);
            } else if (k == SOUTH) {
                ky = -(ROOM_SIZE);
            } else if (k == WEST) {
                kx = -(ROOM_SIZE);
            } else if (k == UP) {
                kz = 0;
            } else if (k == DOWN) {
                kz = 0;
            }


            if (p->exits[k] == EXIT_UNDEFINED) {
              glColor4f(1.0, 1.0, 0.0, colour[3] + 0.2);
            } 
            
            if (p->exits[k] == EXIT_DEATH) {
                glColor4f(1.0, 0.0, 0.0, colour[3] + 0.2);
            } 


            glBegin(GL_LINES);
            glVertex3f(dx + kx, dy + ky, dz);
            glVertex3f(dx2 + kx, dy2 + ky, dz2);
            glEnd();
            
            GLuint death_terrain = conf.get_texture_by_desc("DEATH");
            if (death_terrain && p->exits[k] == EXIT_DEATH) {
              glTranslatef(dx2 + kx, dy2 + ky, dz2);
              
              glEnable(GL_TEXTURE_2D);
              glBindTexture(GL_TEXTURE_2D, death_terrain);
        
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

              glTranslatef(-(dx2 + kx), -(dy2 + ky), -dz2);
              
            }

            glColor4f(colour[0], colour[1], colour[2], colour[3]);
            
        }
    }

        
    
}



void RendererWidget::glDrawCSquare(CSquare *p)
{
    unsigned int k;
    
    if (!SquareInFrustum(p)) {
        return; // this square is not in view 
    }
        
    if (p->to_be_passed()) {
//         go deeper 
        if (p->subsquares[ Left_Upper ])
            glDrawCSquare( p->subsquares[ Left_Upper ]);
        if (p->subsquares[ Right_Upper ])
            glDrawCSquare( p->subsquares[ Right_Upper ]);
        if (p->subsquares[ Left_Lower ])
            glDrawCSquare( p->subsquares[ Left_Lower ]);
        if (p->subsquares[ Right_Lower ])
            glDrawCSquare( p->subsquares[ Right_Lower ]);
    } else {
        for (k = 0; k < p->rooms.size(); k++) {
            glDrawRoom(p->rooms[k]);
        } 
    }
}



void RendererWidget::draw(void)
{
    CRoom *p;
    CPlane *plane;  

    
    rooms_drawn_csquare=0;
    rooms_drawn_total=0;
    square_frustum_checks = 0;
    
    int z = 0;
    
    print_debug(DEBUG_RENDERER, "in draw()");
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glLoadIdentity();

    glEnable(GL_TEXTURE_2D);
//    glEnable(GL_DEPTH_TEST);    
    
    glColor3ub(255, 0, 0);

    glTranslatef(0, 0, userz);

    glRotatef(anglex, 1.0f, 0.0f, 0.0f);
    glRotatef(angley, 0.0f, 1.0f, 0.0f);
    glRotatef(anglez, 0.0f, 0.0f, 1.0f);
    glTranslatef(userx, usery, 0);

 
//    print_debug(DEBUG_RENDERER, "taking base coordinates");
    if (stacker.amount() >= 1) {
	p = stacker.first();
        if (p != NULL) {
            curx = p->x;
            cury = p->y;
            curz = p->z;
        } else {
            printf("RENDERER ERROR: cant get base coordinates.\r\n");
        }
    }


    CalculateFrustum();
    
    
//    print_debug(DEBUG_RENDERER, "drawing %i rooms", Map.size());

    glColor4f(0.1, 0.8, 0.8, 0.4);
    colour[0] = 0.1; colour[1] = 0.8; colour[2] = 0.8; colour[3] = 0.4; 

    plane = Map.planes;
    while (plane) {
        
        z = plane->z - curz;
        
/*        
        if (z == 0) {
          colour[0] = 0.1; colour[1] = 0.8; colour[2] = 0.8; colour[3] = 0.6; 
        } else if (z > 1) {
          colour[0] = 0.0; colour[1] = 0.5; colour[2] = 0.9; colour[3] = 0.1; 
        } else if (z < -1) {
          colour[0] = 0.4; colour[1] = 0.4; colour[2] = 0.4; colour[3] = 0.3; 
        } else if (z == -1) {
          colour[0] = 0.5; colour[1] = 0.5; colour[2] = 0.5; colour[3] = 0.4; 
        } else if (z == 1) {
          colour[0] = 0.0; colour[1] = 0.5; colour[2] = 0.9; colour[3] = 0.2; 
        } else if (z <= -5) {
          colour[0] = 0.3; colour[1] = 0.3; colour[2] = 0.3; colour[3] = 0.2; 
        } else if (z <= -10) {
          colour[0] = 0.1; colour[1] = 0.1; colour[2] = 0.1; colour[3] = 0.1; 
        } else if (z <= -14) {
          colour[0] = 0; colour[1] = 0; colour[2] = 0; colour[3] = 0; 
        }
*/        
        if (z == 0) {
          colour[0] = 1; colour[1] = 1; colour[2] = 1; colour[3] = 0.8; 
        } else if (z > 1) {
          colour[0] = 1; colour[1] = 1; colour[2] = 1; colour[3] = 0.1; 
        } else if (z < -1) {
          colour[0] = 1; colour[1] = 1; colour[2] = 1; colour[3] = 0.3; 
        } else if (z == -1) {
          colour[0] = 1; colour[1] = 1; colour[2] = 1; colour[3] = 0.4; 
        } else if (z == 1) {
          colour[0] = 1; colour[1] = 1; colour[2] = 1; colour[3] = 0.2; 
        } else if (z <= -5) {
          colour[0] = 1; colour[1] = 1; colour[2] = 1; colour[3] = 0.2; 
        } else if (z <= -10) {
          colour[0] = 1; colour[1] = 1; colour[2] = 1; colour[3] = 0.1; 
        } else if (z <= -14) {
          colour[0] = 1; colour[1] = 1; colour[2] = 1; colour[3] = 0; 
        }
        glColor4f(colour[0], colour[1], colour[2], colour[3]);
        
        current_plane_z = plane->z;
        
        glDrawCSquare(plane->squares);
        plane = plane->next;
    }
    
    
    print_debug(DEBUG_RENDERER, "Drawn %i rooms, after dot elimination %i, %i square frustum checks done", 
            rooms_drawn_csquare, rooms_drawn_total, square_frustum_checks);
//    print_debug(DEBUG_RENDERER, "Drawing markers");

    glDrawMarkers();
    
//    print_debug(DEBUG_RENDERER, "draw() done");
  
    this->swapBuffers();
    glredraw = 0;
}

void RendererWidget::display(void)
{
  

  if (glredraw) {
    QTime t;
    t.start();

    draw();
    print_debug(DEBUG_RENDERER, "Rendering's done. Time elapsed %d ms", t.elapsed());
  }  
  
}

void NormalizePlane(float frustum[6][4], int side)
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

    glPushMatrix ();
    
    glGetFloatv(GL_PROJECTION_MATRIX, proj);

    glGetFloatv(GL_MODELVIEW_MATRIX, modl);
    
    glLoadMatrixf (proj);
    glMultMatrixf (modl);
    
    glGetFloatv(GL_MODELVIEW_MATRIX, clip);
    glPopMatrix (); 

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


bool RendererWidget::SquareInFrustum(CSquare *p)
{
    float x, y, z, size;


    square_frustum_checks++;
    
    /* normalize the coordinates to the real view coordinates */
    /* see glDrawRoom for similar functions */
    x = p->centerx -curx;
    y = p->centery - cury;
    z = (current_plane_z - curz) /* * DIST_Z */;
    size = ( (p->rightx - p->leftx) / 2 /*- curz */) /* * DIST_Z */;

    for(int i = 0; i < 6; i++ )
    {
        if(m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (float) z + m_Frustum[i][D] >= 0)
           continue;
        if(m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y - size) + m_Frustum[i][C] * (float) z + m_Frustum[i][D] >= 0)
           continue;
        if(m_Frustum[i][A] * (x - size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (float) z + m_Frustum[i][D] >= 0)
           continue;
        if(m_Frustum[i][A] * (x + size) + m_Frustum[i][B] * (y + size) + m_Frustum[i][C] * (float) z + m_Frustum[i][D] >= 0)
           continue;

        // If we get here, it isn't in the frustum
        return false;
    }

    return true;
}
