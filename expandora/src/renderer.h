#ifndef MAINWINDOW_H 
#define MAINWINDOW_H

#include <cmath>
#include <qmainwindow.h> 
#include <qstringlist.h> 
#include <qgl.h>

class QAction; 
class QLabel; 
class FindDialog; 
class Spreadsheet; 

#define MAX_TEXTURES    100

#define DIST_Z	2	/* the distance between 2 rooms */
#define BASE_Z  -12	/* the distance to the "camera" */
#define ROOM_SIZE 0.4f	/* the size of the rooms walls */

  
int renderer_main();


extern int texture_visibilit_range;
extern int details_visibility_range;

class QLineEdit;
class QLabel;
class QWidget;
class QPushButton;

class RoomInfo : public QWidget
{
    Q_OBJECT

public:
    RoomInfo(QWidget *parent, const char *name = 0);

    QLabel*     id_label;
    QLabel*     terrain_label;
    QLabel*     coord_label;

    QLabel*     name_label;
    QLineEdit*  name_edit;

    QLabel*     desc_label;
    QLineEdit*  desc_edit;

    QLabel*     note_label;
    QLineEdit*  note_edit;

    QLabel*     exits_label;
    QLabel*     doors_label;

    QPushButton* apply_button;

    void update_info();

private:
  QHBoxLayout   *topLayout;
  QHBoxLayout   *noteLayout; 
  QHBoxLayout   *nameLayout; 
  QHBoxLayout   *descLayout; 
  QVBoxLayout   *mainLayout; 



};


class RendererWidget : public QGLWidget
{
  Q_OBJECT
public:
  GLfloat       angley;
  GLfloat       anglex;
  GLfloat       anglez;
  int           userx;
  int           usery;
  int           userz;		/* additional shift added by user */

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
  GLuint        basic_gllist;
  GLuint        global_list;
  int           curx;
  int           cury;
  int           curz;			/* current rooms position */ 
  
  void glDrawMarkers();
  void glDrawRoom(struct Troom *p);
  bool PointInFrustum(float x, float y, float z);
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
  void hide_roominfo();


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
  QPopupMenu    *optionsMenu;
  QDockWindow   *dock;
  RoomInfo      *roominfo;

  int hide_status_id;
  int hide_menu_id;
  int hide_roominfo_id;

  bool          LeftButtonPressed;
  bool          RightButtonPressed;
  QPoint        old_pos;
};


extern class MainWindow *renderer_window;

#endif
