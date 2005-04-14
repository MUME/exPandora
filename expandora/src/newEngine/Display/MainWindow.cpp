#include <qlayout.h>
#include <qpopupmenu.h>
#include <qapplication.h>
#include <qmenubar.h> 
#include <qstatusbar.h> 


#include "MainWindow.h"
#include "Display.h"
#include "Room.h"
#include "Coordinate.h"
#include "Terrain.h"


#define ON_OFF(flag) ( (flag) ? "ON" : "OFF" )

RoomInfo::RoomInfo(QWidget* parent, const char* name)
 : QWidget(parent, name)
{
  /* ------- */
  id_label = new QLabel("ID: Out of sync", this);
  terrain_label = new QLabel("T: NONE", this);
  coord_label = new QLabel("COORD:", this);
  
  topLayout = new QHBoxLayout; 
  topLayout->addWidget(id_label); 
  topLayout->addStretch(2);
  topLayout->addWidget(terrain_label);
  topLayout->addStretch(2);
  topLayout->addWidget(coord_label);
  /* ------- */  
  
  /* ------- */
  name_label= new QLabel("RN ", this);
  name_edit = new QLineEdit(this);

  name_edit->setAlignment(Qt::AlignLeft);
  
  nameLayout = new QHBoxLayout; 
  nameLayout->addWidget(name_label); 
//  nameLayout->addStretch(1);
  nameLayout->addWidget(name_edit);
  /* ------- */
  desc_label= new QLabel("D ", this);
  
  desc_edit = new QLineEdit(this);
  
  descLayout = new QHBoxLayout; 
  descLayout->addWidget(desc_label); 
//  descLayout->addStretch(1);
  descLayout->addWidget(desc_edit);

  /* --- */
  note_label= new QLabel("Note ", this);
  
  note_edit = new QLineEdit(this);
  
  noteLayout = new QHBoxLayout; 
  noteLayout->addWidget(note_label); 
//  noteLayout->addStretch(1);
  noteLayout->addWidget(note_edit);
  /* --- */
  
  exits_label = new QLabel("Exits:", this);
  doors_label = new QLabel("Doors:", this);
  
  
  mainLayout = new QVBoxLayout(this); 
  mainLayout->addLayout(topLayout); 
  mainLayout->addLayout(nameLayout);
  mainLayout->addLayout(descLayout);
  mainLayout->addLayout(noteLayout);
  mainLayout->addWidget(doors_label);
  mainLayout->addWidget(exits_label);
  

  mainLayout->setMargin(11);
  mainLayout->setSpacing(6); 

}

void RoomInfo::update_info(Room * rr)
{
  char str[200];
  
  Coordinate * r;

  if (rr) r = rr->getCoordinate();
  else r = new Coordinate(0,0,0);

  sprintf(str, "Coord: X: %i, Y %i, Z: %i", r->x, r->y, r->z);
  coord_label->setText( str );
  

  if(rr) {
    sprintf(str, " %s ", terrains.find(rr->getTerrain())->second->desc);

    
    terrain_label->setText( str );
  } else {
    terrain_label->setText(" T: NONE ");
  }
  
}

  



MainWindow::MainWindow(QWidget *parent, const char *name)
    : QMainWindow( parent, name)
{
  renderer =  new RendererWidget( this, "Renderer");
  setCentralWidget( renderer );

  QPopupMenu *fileMenu = new QPopupMenu( 0, "fileMenu" );
  fileMenu->insertItem( tr("&Exit"), qApp, SLOT( quit() ), CTRL+Key_Q );
  menuBar()->insertItem( tr("&File"), fileMenu );

  
  optionsMenu = new QPopupMenu( 0, "optionsMenu" );
  optionsMenu->insertItem( tr("Hide All"), this, SLOT( hide() ), CTRL+Key_H );
  optionsMenu->setCheckable(true);
  hide_menu_id = optionsMenu->insertItem( tr("Hide/Show Menu"), this, SLOT( hide_menu() ), Key_F12);
  hide_status_id = optionsMenu->insertItem( tr("Hide/Show Status"), this, SLOT( hide_status()), Key_F11 );
  hide_roominfo_id = optionsMenu->insertItem( tr("Hide/Show Room info"), this, SLOT( hide_roominfo()), Key_F10);

  optionsMenu->insertSeparator();

  always_on_top_id = optionsMenu->insertItem( tr("Always on top"), this, SLOT( always_on_top()), Key_F9);

  
  menuBar()->insertItem( tr("&Options"), optionsMenu);
  
  locationLabel = new QLabel("NO_SYNC", this); 
  locationLabel->setAlignment(AlignHCenter); 
  locationLabel->setMinimumSize(locationLabel->sizeHint()); 
  
  formulaLabel = new QLabel(this); 
  
  modLabel = new QLabel(tr("     "), this); 
  modLabel->setAlignment(AlignHCenter); 
  modLabel->setMinimumSize(modLabel->sizeHint());      
  modLabel->clear(); 
  
  statusBar()->addWidget(locationLabel); 
  statusBar()->addWidget(formulaLabel, 1); 
  statusBar()->addWidget(modLabel); 
  
  
  optionsMenu->setItemChecked(hide_menu_id, false );
  optionsMenu->setItemChecked(hide_status_id, false );
  optionsMenu->setItemChecked(hide_roominfo_id, true );
 
  optionsMenu->setItemChecked(always_on_top_id, true );
//  this->setWFlags(Qt::WStyle_Customize | Qt::WStyle_StaysOnTop);

  dock = new QDockWindow(this, "dock");
  dock->setCaption("Room Info");
  dock->setResizeEnabled( TRUE );
//  dock->setVerticalStretchable( TRUE );
//  dock->setHorizontalStretchable( TRUE );
  addDockWindow(dock, DockTornOff, FALSE);
  
  roominfo = new RoomInfo(dock, "roominfo");
  dock->setWidget(roominfo);


   int flags = getWFlags();
#ifndef Q_OS_MACX
   if(testWFlags(Qt::WStyle_StaysOnTop)){
      flags ^= Qt::WStyle_StaysOnTop;
   }
   else{
      flags |= Qt::WStyle_StaysOnTop;
   }
#else
   if(testWFlags(Qt::WStyle_StaysOnTop)){
      flags ^= Qt::WStyle_StaysOnTop;
   
}
#endif
   QPoint p(geometry().x(),geometry().y());
   reparent(0,flags,p,true);

//  this->reparent(NULL, this->getWFlags() | WStyle_StaysOnTop, QPoint(0,0),true);  
    /* ---- */
//  setDockEnabled(DockBottom, false);
//  setDockEnabled(DockTop, false);
//  setDockEnabled(DockLeft, false);
//  setDockEnabled(DockRight, false);
  



  LeftButtonPressed = false;
  RightButtonPressed = false;


}

void MainWindow::update_status_bar(Room * rr)
{  

  locationLabel->setText("silly location bar");
  
  roominfo->update_info(rr);
}


void MainWindow::hide()
{
  
  
  menuBar()->hide();
  statusBar()->hide();
  dock->hide();

  
  optionsMenu->setItemChecked(hide_menu_id, true );
  optionsMenu->setItemChecked(hide_status_id, true );
  optionsMenu->setItemChecked(hide_roominfo_id, true );
}


void MainWindow::hide_menu()
{
  
  if (optionsMenu->isItemChecked(hide_menu_id)) 
  {
    menuBar()->show();  
    optionsMenu->setItemChecked(hide_menu_id, false);
  } else {
    menuBar()->hide();
    optionsMenu->setItemChecked(hide_menu_id, true);
  }
}

void MainWindow::always_on_top()
{
  
  WFlags flags = getWFlags();
  if(testWFlags(Qt::WStyle_StaysOnTop))
  {
    flags ^= Qt::WStyle_StaysOnTop;
    optionsMenu->setItemChecked(always_on_top_id, false);
  } else {
    flags |= Qt::WStyle_StaysOnTop;
    optionsMenu->setItemChecked(always_on_top_id, true);
  }
  QPoint p(geometry().x(),geometry().y());
  reparent(0,flags,p,true);

}


void MainWindow::hide_status()
{

  if (optionsMenu->isItemChecked(hide_status_id)) 
  {
    statusBar()->show();  
    optionsMenu->setItemChecked(hide_status_id, false);
  } else {
    statusBar()->hide();
    optionsMenu->setItemChecked(hide_status_id, true);
  }
  
}


void MainWindow::hide_roominfo()
{

  if (optionsMenu->isItemChecked(hide_roominfo_id)) 
  {
    dock->show();
    optionsMenu->setItemChecked(hide_roominfo_id, false);
  } else {
    dock->hide();
    optionsMenu->setItemChecked(hide_roominfo_id, true);
  }
}


void MainWindow::keyPressEvent( QKeyEvent *k )
{
    switch ( tolower(k->ascii()) ) {
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
    
    switch ( k->key() ) {
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

         case Key_F12:
            hide_menu();
            break;
         case Key_F11:
            hide_status();
            break;				
         case Key_F10:
            hide_roominfo();
            break;				

         case CTRL+Key_H:
            hide();
            break;				

         
    }
    
}

void MainWindow::mousePressEvent( QMouseEvent *e)
{
  
  if (e->button() == Qt::LeftButton) {
    LeftButtonPressed = true;
  } else {
    RightButtonPressed = true;
  }
  old_pos = e->pos();
  
  
}

void MainWindow::mouseReleaseEvent( QMouseEvent *e)
{
  if (e->button() == Qt::LeftButton) {
    LeftButtonPressed = false;
  } else {
    RightButtonPressed = false;
  }
}

void MainWindow::mouseMoveEvent( QMouseEvent *e)
{
  QPoint pos;
  int dist_x, dist_y;

  pos = e->pos();
  dist_x = pos.x() - old_pos.x();
  dist_y = pos.y() - old_pos.y();
  
  
  if (LeftButtonPressed) {
    
    if ( ((dist_x * dist_x) + (dist_y * dist_y)) >= 100) {
      
      renderer->userx -= dist_x;
      renderer->usery += dist_y;
      renderer->shiftView();
      old_pos = pos;
    }
    
  } else if (RightButtonPressed) {
    renderer->anglex -= dist_y;
    renderer->angley -= dist_x;
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
