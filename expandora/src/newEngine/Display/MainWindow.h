#ifndef MAINWINDOW
#define MAINWINDOW

#include <qmainwindow.h> 
#include <qstringlist.h> 
#include <qgl.h>
#include <qthread.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>

#include "Display.h"

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


class MainWindow : public QMainWindow
{
  Q_OBJECT 
    public:
  MainWindow(QWidget *parent, const char *name = 0);
  
  void update_status_bar();
  RendererWidget *renderer;

  private slots:
  void hide_menu();
  void hide_status();
  void hide_roominfo();
  void always_on_top();


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
  int always_on_top_id;


  bool          LeftButtonPressed;
  bool          RightButtonPressed;
  QPoint        old_pos;
};


#endif
