#ifndef MAINWINDOW_H 
#define MAINWINDOW_H 

#include <QMainWindow> 
#include <QStringList> 
#include <QGLWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>
#include <QDockWidget>
#include <QApplication>
#include <QDialog>
#include <QThread>

#include "defines.h"
#include "rooms.h"

#include "ui_roomedit.h"

class QAction; 
class QLabel; 
class FindDialog; 
class Spreadsheet; 
class QLineEdit;
class QLabel;
class QWidget;
class QPushButton;

int renderer_main();


extern int texture_visibilit_range;
extern int details_visibility_range;

class RoomEditDialog : public QDialog, public Ui::roomedit_dialog {
Q_OBJECT

public:
    RoomEditDialog(QWidget *parent = 0);
    QString parse_room_desc(char *);


    void load_room_data(unsigned int id);
public slots:
    virtual void accept();
    virtual void reject();
};

/*
class RoomEditThread : public QThread {
Q_OBJECT
    RoomEditDialog  *dialog;
public:
    virtual void run();
};
*/

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

  QLabel        *locationLabel; 
  QLabel        *formulaLabel; 
  QLabel        *modLabel; 

    QMenu       *optionsMenu;
    QMenu       *fileMenu;
    QMenu       *actionsMenu;

  QAction       *always_on_top_action;
  QAction       *newAct;
  QAction       *quitAct;
  QAction       *openAct;
  QAction       *reloadAct;
  
  QAction       *roomeditAct;
  
  
  
  RoomEditDialog edit_dialog;



  bool          LeftButtonPressed;
  bool          RightButtonPressed;
  QPoint        old_pos;

public:
    MainWindow(QWidget *parent, const char *name = 0);

    RendererWidget *renderer;

    void update_status_bar();
    QAction       *hide_status_action;
    QAction       *hide_menu_action;
private slots:
  void hide_menu();
  void hide_status();
  void always_on_top();
  void newFile();
  void open();
  void reload();
  void quit();
  
    void edit_room();

  void mousePressEvent( QMouseEvent *);
  void mouseReleaseEvent( QMouseEvent *);
  void mouseMoveEvent( QMouseEvent *);
  void wheelEvent(QWheelEvent *);


  void keyPressEvent( QKeyEvent * );
  void hide();                              /* hide all extra widgets */
};


extern class MainWindow *renderer_window;

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
