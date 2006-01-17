#include <qlayout.h>
#include <qapplication.h>
#include <qmenubar.h>
#include <qstatusbar.h>
#include <QKeyEvent>
#include <QFileDialog>

#include "MainWindow.h"
#include "Room.h"
#include "Coordinate.h"
#include "Terrain.h"


#define ON_OFF(flag) ( (flag) ? "ON" : "OFF" )

using namespace Qt;






MainWindow::MainWindow(QWidget *parent)
    : QMainWindow( parent)
{

  resize(640, 480);
  renderer =  new RendererWidget( this );
  setCentralWidget( renderer );


  QMenu *fileMenu = new QMenu( "File", 0 );
  fileMenu->addAction( tr("&Exit"), qApp, SLOT( quit() ), Qt::CTRL+Qt::Key_Q );
  fileMenu->addAction( tr("&Open..."), this, SLOT( open() ));
  menuBar()->addMenu( fileMenu );


  optionsMenu = new QMenu( "Options", 0 );


  /*  hide_status_id = optionsMenu->addAction( tr("Hide/Show Status"), this, SLOT( hide_status()), Qt::Key_F11 );
    hide_status_id->setCheckable(true);
    hide_roominfo_id = optionsMenu->addAction( tr("Hide/Show Room info"), this, SLOT( hide_roominfo()), Qt::Key_F10);
    hide_roominfo_id->setCheckable(true);
  */

  optionsMenu->addSeparator();

  always_on_top_id = optionsMenu->addAction( tr("Always on top"), this, SLOT( always_on_top()), Qt::Key_F9);
  always_on_top_id->setCheckable(true);

  menuBar()->addMenu( optionsMenu);

  /*locationLabel = new QLabel("NO_SYNC");
  locationLabel->setAlignment(AlignHCenter);
  locationLabel->setMinimumSize(locationLabel->sizeHint());*/

  formulaLabel = new QLabel();

  modLabel = new QLabel(tr("     "));
  modLabel->setAlignment(AlignHCenter);
  modLabel->setMinimumSize(modLabel->sizeHint());
  modLabel->clear();

  //  statusBar()->addWidget(locationLabel);
  statusBar()->addWidget(formulaLabel, 1);
  statusBar()->addWidget(modLabel);



  /*  hide_roominfo_id->setChecked(true);
    hide_status_id->setChecked(true);*/
  always_on_top_id->setChecked(false);


  //dock = new QDockWidget("Room Info", this);
  //addDockWidget(Qt::BottomDockWidgetArea, dock);

  //roominfo = new RoomInfo(dock);
  //dock->setWidget(roominfo);




  LeftButtonPressed = false;
  RightButtonPressed = false;

  show();
}

void MainWindow::update_status_bar(Room *)
{

  //  locationLabel->setText("silly location bar");

  //  roominfo->update_info(rr);
}


void MainWindow::open()
{
  QString s = QFileDialog::getOpenFileName(
                0,
                "Choose a map file",
                QString(),
                "Map (*.xml)");
  if(!s.isNull())
  {
    emit openMap(s);
  }
}




void MainWindow::always_on_top()
{

  Qt::WindowFlags flags = windowFlags();
  if(flags && Qt::WindowStaysOnTopHint)
  {
    flags ^= Qt::WindowStaysOnTopHint;

  }
  else
  {
    flags |= Qt::WindowStaysOnTopHint;

  }
  setWindowFlags(flags);
  show();
}


void MainWindow::hide_status()
{

  /*  if (hide_status_id->isChecked())
    {
      statusBar()->show();
    }
    else
    {
      statusBar()->hide();
    }
  */
}


void MainWindow::hide_roominfo()
{

  /*  if (hide_roominfo_id->isChecked())
    {
      dock->show();
    }
    else
    {
      dock->hide();
    }*/
}


void MainWindow::keyPressEvent( QKeyEvent *k )
{

  switch ( tolower(k->key()) )
  {
  case 'c':
    break;

  case 'x':
    renderer->userz += 1;
    renderer->shiftView();
    break;

  case 'y':
    renderer->userz -= 1;
    renderer->shiftView();
    break;

  case 'q':
    renderer->userx -= 1;
    renderer->shiftView();
    break;

  case 'w':
    renderer->userx += 1;
    renderer->shiftView();
    break;

  case 'a':
    renderer->usery += 1;
    renderer->shiftView();
    break;

  case 's':
    renderer->usery -= 1;
    renderer->shiftView();
    break;

  case 'r':
    renderer->shiftView();
    break;

  }

  switch ( k->key() )
  {
  case Key_Up:
    renderer->anglex += 5;
    renderer->shiftView();
    break;
  case Key_Down:
    renderer->anglex -= 5;
    renderer->shiftView();
    break;
  case Key_Left:
    renderer->angley -= 5;
    renderer->shiftView();
    break;
  case Key_Right:
    renderer->angley += 5;
    renderer->shiftView();
    break;
  case Key_PageUp:
    renderer->anglez += 5;
    renderer->shiftView();
    break;
  case Key_PageDown:
    renderer->anglez -= 5;
    renderer->shiftView();
    break;

  case Key_Escape:
    renderer->angley = 0;
    renderer->anglex = 0;
    renderer->anglez = 0;
    renderer->userx = 0;
    renderer->usery = 0;
    renderer->userz = BASE_Z;
    renderer->shiftView();
    break;
  }


}

void MainWindow::mousePressEvent( QMouseEvent *e)
{

  if (e->button() == Qt::LeftButton)
  {
    LeftButtonPressed = true;
  }
  else
  {
    RightButtonPressed = true;
  }
  old_pos = e->pos();


}

void MainWindow::mouseReleaseEvent( QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton)
  {
    LeftButtonPressed = false;
  }
  else
  {
    RightButtonPressed = false;
  }
}

void MainWindow::mouseMoveEvent( QMouseEvent *e)
{
  QPoint pos;
  float dist_x, dist_y;

  pos = e->pos();
  dist_x = pos.x() - old_pos.x();
  dist_y = pos.y() - old_pos.y();


  if (LeftButtonPressed)
  {

    //if ( ((dist_x * dist_x) + (dist_y * dist_y)) >= 100)
    //{

    renderer->userx += dist_x/10.0;
    renderer->usery -= dist_y/10.0;
    renderer->shiftView();
    old_pos = pos;
    //}

  }
  else if (RightButtonPressed)
  {
    renderer->anglex += dist_y;
    renderer->angley += dist_x;
    renderer->shiftView();
    old_pos = pos;

  }



}

void MainWindow::wheelEvent(QWheelEvent *e)
{
  int delta;

  delta = e->delta();

  renderer->userz += delta / 120;
  renderer->shiftView();
}
