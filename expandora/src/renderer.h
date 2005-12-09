#ifndef MAINWINDOW_H 
#define MAINWINDOW_H 

#include <QMainWindow> 
#include <QStringList> 
#include <QGLWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>
#include <QDockWidget>


#include "defines.h"
#include "rooms.h"

class QAction; 
class QLabel; 
class FindDialog; 
class Spreadsheet; 

int renderer_main();


extern int texture_visibilit_range;
extern int details_visibility_range;

class QLineEdit;
class QLabel;
class QWidget;
class QPushButton;

class RendererWidget : public QGLWidget
{
  Q_OBJECT
public:
  GLfloat       angley;
  GLfloat       anglex;
  GLfloat       anglez;
  float         userx;
  float         usery;
  float         userz;		/* additional shift added by user */

  int           current_plane_z;        /* to avoid arguments usage */
  GLuint        basic_gllist;


    
  int           rooms_drawn_csquare;
  int           rooms_drawn_total;
  int           square_frustum_checks;

  RendererWidget( QWidget *parent, const char *name=0 );

  void display(void);

  void draw(void);
  void CalculateFrustum();
  


protected:
  void initializeGL();
  void resizeGL( int width, int height );
  void paintGL();

private:
  GLfloat       colour[4];
  float         m_Frustum[6][4];
  GLuint        global_list;
  int           curx;
  int           cury;
  int           curz;			/* current rooms position */ 
  


  void glDrawMarkers();
  void glDrawRoom(struct Troom *p);
  
  void glDrawCSquare(CSquare *p);
  bool PointInFrustum(float x, float y, float z);
  bool SquareInFrustum(CSquare *p);
  
};


class MainWindow : public QMainWindow
{
  Q_OBJECT
public:
    MainWindow(QWidget *parent, const char *name = 0);

    RendererWidget *renderer;

    void update_status_bar();

private slots:
  void hide_menu();
  void hide_status();
  void always_on_top();
  void newFile() {  } ;
  void open() { };

private:
  void mousePressEvent( QMouseEvent *);
  void mouseReleaseEvent( QMouseEvent *);
  void mouseMoveEvent( QMouseEvent *);
  void wheelEvent(QWheelEvent *);

  void keyPressEvent( QKeyEvent * );
  void hide();                              /* hide all extra widgets */

  QLabel        *locationLabel; 
  QLabel        *formulaLabel; 
  QLabel        *modLabel; 
  QMenu         *optionsMenu;

  QAction       *hide_status_action;
  QAction       *hide_menu_action;
  QAction       *always_on_top_action;
  QAction       *newAct;
  QAction       *quitAct;
  QAction       *openAct;



  bool          LeftButtonPressed;
  bool          RightButtonPressed;
  QPoint        old_pos;
};


extern class MainWindow *renderer_window;

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
