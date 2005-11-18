// Interface code 
#include <qevent.h>
#include <qapplication.h>
#include <qwidget.h>
#include <qnamespace.h>
#include <qimage.h>
#include <cstdlib>

#include <qaction.h> 
#include <qcombobox.h> 
#include <qfiledialog.h> 
#include <qlabel.h> 
#include <qlineedit.h> 
#include <qmenubar.h> 
#include <qmessagebox.h> 
#include <qpopupmenu.h> 
#include <qsettings.h> 
#include <qstatusbar.h> 
#include <qlayout.h>
#include <qbutton.h>
#include <qvariant.h>
#include <qwidget.h>
#include <qpushbutton.h>
#include <qtextedit.h>



#include "defines.h"
#include "struct.h"
#include "configurator.h"

#include "renderer.h"
#include "stacks.h"
#include "utils.h"
#include "rooms.h"

#include "userfunc.h"

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

  update_info();
}

void RoomInfo::update_info()
{
  char str[200];
  
  int i;
  struct Troom *r;

  
  
  if (stacker.amount == 0) {
    
    id_label->setText(tr("ID:  Out of sync"));
    terrain_label->setText(tr("Terrain: NONE"));
    
    return;
  }
  
  if (stacker.amount > 1) {
    id_label->setText(tr("ID:  Multiple possibilities"));
    return;
  }

  sprintf(str, "ID: ");
  stacker.get_current( &str[ strlen(str) ] );
  id_label->setText(tr(str));

  r = roomer.getroom(stacker.get(1));
  sprintf(str, "Coord: X: %i, Y %i, Z: %i", r->x, r->y, r->z);
  coord_label->setText( str );
  
  if (r->sector) {
    sprintf(str, " %s ", r->sector->desc);
    
    terrain_label->setText( str );
  } else {
    terrain_label->setText(" T: NONE ");
  }
  if (r->name) {
    name_edit->setText( r->name );
  } else {
    name_edit->setText( "(NULL)" );
  }

  if (r->desc) {
    desc_edit->setText( r->desc );
  } else {
    desc_edit->setText( "(NULL)" );
  }
  desc_edit->setAlignment(Qt::AlignLeft);
  
  if (r->note) {
    note_edit->setText( r->note );
  } else {
    note_edit->setText( "(NULL)" );
  }

  sprintf(str, "Doors:");
  for (i = 0; i <= 5; i++) {
    if (r->doors[i]) {
      sprintf(str+ strlen(str), " %c: %s", dirbynum(i), r->doors[i]);
    }
    
  }
  doors_label->setText( str );

  
  sprintf(str, "Exits:");

  for (i = 0; i <= 5; i++)
      if (r->exits[i] > 0) {
          if (r->exits[i] == EXIT_UNDEFINED) {
              sprintf(str + strlen(str), " #%s#", exitnames[i]);
              continue;
          }
          if (r->exits[i] == EXIT_DEATH) {
              sprintf(str + strlen(str), " !%s!", exitnames[i]);
              continue;
          }
          if (r->doors[i] != NULL) {
              if (strcmp("exit", r->doors[i]) == 0) {
                  sprintf(str + strlen(str), " (%s)", exitnames[i]);
              } else {
                  sprintf(str + strlen(str), " +%s+", exitnames[i]);
              }
          } else {
              sprintf(str + strlen(str), " %s", exitnames[i]);
          }
          sprintf(str + strlen(str), " -[to %i]-", r->exits[i]);
      }
      
  exits_label->setText( str );
  
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

void MainWindow::update_status_bar()
{
  char str[20];
  
  if (conf.get_data_mod() )
    modLabel->setText(tr("Data: MOD "));     
  else 
    modLabel->setText(tr("Data: --- "));
  

  stacker.get_current(str);
  locationLabel->setText(tr(str));
  
  roominfo->update_info();
}


void MainWindow::hide()
{
  print_debug(DEBUG_INTERFACE, "hide/show all function");
  
  menuBar()->hide();
  statusBar()->hide();
  dock->hide();

  
  optionsMenu->setItemChecked(hide_menu_id, true );
  optionsMenu->setItemChecked(hide_status_id, true );
  optionsMenu->setItemChecked(hide_roominfo_id, true );
}


void MainWindow::hide_menu()
{
  print_debug(DEBUG_INTERFACE, "hide/show menu called");
  
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
  print_debug(DEBUG_INTERFACE, "always_on_top called");
  
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
  print_debug(DEBUG_INTERFACE, "hide/show status called");

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
  print_debug(DEBUG_INTERFACE, "hide/show room info called");

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
    switch ( LOWER(k->ascii()) ) {
        case 'c':
          if (userland_parser.is_empty())
            engine();
          else 
            userland_parser.parse_command();
          
          break;
         
        case 'x':
            renderer->userz += 1;
            glredraw = 1;
            break;
    
         case 'y':
            renderer->userz -= 1;
            glredraw = 1;
            break;
    
         case 'q':
            renderer->userx -= 1;
            glredraw = 1;
            break;
         
         case 'w':
            renderer->userx += 1;
            glredraw = 1;
            break;
         
         case 'a':
            renderer->usery += 1;
            glredraw = 1;
            break;
    
         case 's':
            renderer->usery -= 1;
            glredraw = 1;
            break;
    
         case 'r':
           print_debug(DEBUG_RENDERER, "got R (redraw) keypress event");
           glredraw = 1;
           break;
    
    }        
    
    switch ( k->key() ) {
        case Key_Up:
          renderer->anglex += 5;
          glredraw = 1;
          break;
        case Key_Down:
          renderer->anglex -= 5;
          glredraw = 1;
          break;
        case Key_Left:
          renderer->angley -= 5;
          glredraw = 1;
          break;
        case Key_Right:
          renderer->angley += 5;
          glredraw = 1;
          break;
        case Key_PageUp:
          renderer->anglez += 5;
          glredraw = 1;
          break;
        case Key_PageDown:
          renderer->anglez -= 5;
          glredraw = 1;
          break;
        
         case Key_Escape:           
            renderer->angley = 0;
            renderer->anglex = 0;
            renderer->anglez = 0;
            renderer->userx = 0;
            renderer->usery = 0;
            renderer->userz = BASE_Z;		
            glredraw = 1;
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
    
    renderer->display();
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
  
  print_debug(DEBUG_INTERFACE, "mouseEvent. LeftMouse %s, RightMouse %s. Dist_x %i, dist_y %i.",
      ON_OFF(LeftButtonPressed), ON_OFF(RightButtonPressed), dist_x, dist_y);
  
  if (LeftButtonPressed) {
      renderer->userx += (float) dist_x / 10.0;
      renderer->usery -= (float) dist_y / 10.0;
      glredraw = 1;

      renderer->display();
      old_pos = pos;
  } else if (RightButtonPressed) {
    renderer->anglex += dist_y;
    renderer->angley += dist_x;
    glredraw = 1;
    
    
    renderer->display();
    old_pos = pos;

  }

  
  
}

void MainWindow::wheelEvent(QWheelEvent *e)
{
  int delta;

  delta = e->delta();

  renderer->userz += delta / 120;
  glredraw = 1;
  renderer->display();
}
