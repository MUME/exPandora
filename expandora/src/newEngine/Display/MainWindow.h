#ifndef MAINWINDOW
#define MAINWINDOW

#include <qmainwindow.h>
#include <qstringlist.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qpushbutton.h>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>
#include <QDockWidget>

#include "Display.h"

class RoomInfo : public QWidget
{
  Q_OBJECT
public:

  RoomInfo(QWidget *parent);

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

  void update_info(Room * rr);

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
  MainWindow(QWidget *parent);

  void update_status_bar(Room * rr);
  RendererWidget *renderer;

private slots:

  void hide_status();
  void hide_roominfo();
  void always_on_top();


private:
  void mousePressEvent( QMouseEvent *);
  void mouseReleaseEvent( QMouseEvent *);
  void mouseMoveEvent( QMouseEvent *);
  void wheelEvent(QWheelEvent *);

  void keyPressEvent( QKeyEvent * );



  QLabel        *locationLabel;
  QLabel        *formulaLabel;
  QLabel        *modLabel;
  QMenu    	*optionsMenu;
  QDockWidget   *dock;
  RoomInfo      *roominfo;

  QAction * hide_status_id;
  QAction * hide_roominfo_id;
  QAction * always_on_top_id;


  bool          LeftButtonPressed;
  bool          RightButtonPressed;
  QPoint        old_pos;
};

#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif
