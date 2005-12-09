// Interface code 
#include <QEvent>
#include <QApplication>
#include <QImage>
#include <cstdlib>

#include <QAction> 
#include <QComboBox> 
#include <QFileDialog> 
#include <QLabel> 
#include <QLineEdit> 
#include <QSettings> 
#include <QLayout>
#include <QVariant>
#include <QWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QMenuBar>
#include <QStatusBar>
#include <QKeyEvent>


#include "defines.h"
#include "struct.h"
#include "configurator.h"

#include "renderer.h"
#include "stacks.h"
#include "utils.h"
#include "rooms.h"

#include "userfunc.h"


MainWindow::MainWindow(QWidget *parent, const char *name)
    : QMainWindow( parent)
{
  renderer =  new RendererWidget( this, name);
  setCentralWidget( renderer );

  /* creating actions and connecting them here */
  newAct = new QAction(tr("&New"), this);
  newAct->setShortcut(tr("Ctrl+N"));
  newAct->setStatusTip(tr("Create a new map"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));    
  
  openAct = new QAction(tr("&Open..."), this);
  openAct->setShortcut(tr("Ctrl+O"));
  openAct->setStatusTip(tr("Open an existing map"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));
  
  quitAct =  new QAction(tr("&Exit..."), this);
  quitAct->setShortcut(tr("Ctrl+Q"));
  openAct->setStatusTip(tr("Quit Pandora"));
  connect(quitAct, SIGNAL(triggered()), this, SLOT(quit()));
    
  /* now building a menu and adding actions to menu */
  QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);  
  fileMenu->addAction(quitAct);  



/*
  optionsMenu = new QPopupMenu( 0, "optionsMenu" );
  optionsMenu->insertItem( tr("Hide All"), this, SLOT( hide() ), CTRL+Key_H );
  optionsMenu->setCheckable(true);
  hide_menu_id = optionsMenu->insertItem( tr("Hide/Show Menu"), this, SLOT( hide_menu() ), Key_F12);
  hide_status_id = optionsMenu->insertItem( tr("Hide/Show Status"), this, SLOT( hide_status()), Key_F11 );
  hide_roominfo_id = optionsMenu->insertItem( tr("Hide/Show Room info"), this, SLOT( hide_roominfo()), Key_F10);

  optionsMenu->insertSeparator();

  always_on_top_id = optionsMenu->insertItem( tr("Always on top"), this, SLOT( always_on_top()), Key_F9);

  
  menuBar()->insertItem( tr("&Options"), optionsMenu);
  

  
  optionsMenu->setItemChecked(hide_menu_id, false );
  optionsMenu->setItemChecked(hide_status_id, false );
  optionsMenu->setItemChecked(hide_roominfo_id, true );
 
  optionsMenu->setItemChecked(always_on_top_id, true );
*/  
//  this->setWFlags(Qt::WStyle_Customize | Qt::WStyle_StaysOnTop);

   Qt::WindowFlags flags = windowFlags();
#ifndef Q_OS_MACX
   if ( flags && Qt::WindowStaysOnTopHint) {
      flags ^= Qt::WindowStaysOnTopHint;
   }
   else{
      flags |= Qt::WindowStaysOnTopHint;
   }
#else
   if( flags && Qt::WindowStaysOnTopHint){
      flags ^= Qt::WindowStaysOnTopHint;
   
}
#endif
/*
   QPoint p(geometry().x(),geometry().y());
   setParent(0,flags,p,true);
*/
  setWindowFlags(flags);


  /* status bar magicz */
  locationLabel = new QLabel("NO_SYNC", this); 
  locationLabel->setAlignment(Qt::AlignHCenter); 
  locationLabel->setMinimumSize(locationLabel->sizeHint()); 

  formulaLabel = new QLabel(this); 
  
  modLabel = new QLabel(tr("     "), this); 
  modLabel->setAlignment(Qt::AlignHCenter); 
  modLabel->setMinimumSize(modLabel->sizeHint());      
  modLabel->clear(); 
  
  statusBar()->addWidget(locationLabel); 
  statusBar()->addWidget(formulaLabel, 1); 
  statusBar()->addWidget(modLabel); 

  

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
}


void MainWindow::hide()
{
  print_debug(DEBUG_INTERFACE, "hide/show all function");
  
  menuBar()->hide();
  statusBar()->hide();

  
  hide_menu_action->setChecked(true);
  hide_status_action->setChecked(true);
}


void MainWindow::hide_menu()
{
  print_debug(DEBUG_INTERFACE, "hide/show menu called");
  
  if (hide_menu_action->isChecked()) 
  {
    menuBar()->show();  
    hide_menu_action->setChecked(false);
  } else {
    menuBar()->hide();
    hide_menu_action->setChecked(true);
  }
}

void MainWindow::always_on_top()
{
  print_debug(DEBUG_INTERFACE, "always_on_top called");

  Qt::WindowFlags flags = windowFlags();
  if( flags && Qt::WindowStaysOnTopHint )
  {
    flags ^= Qt::WindowStaysOnTopHint;
    always_on_top_action->setChecked(false);
  } else {
    flags |= Qt::WindowStaysOnTopHint;
    always_on_top_action->setChecked(true);
  }
  setWindowFlags(flags);
}


void MainWindow::hide_status()
{
  print_debug(DEBUG_INTERFACE, "hide/show status called");

  if (hide_status_action->isChecked() ) 
  {
    statusBar()->show();  
    hide_status_action->setChecked(false);
  } else {
    statusBar()->hide();
    hide_status_action->setChecked(true);
  }
  
}

void MainWindow::keyPressEvent( QKeyEvent *k )
{
    switch ( k->key() ) {
        case Qt::Key_C :
          if (userland_parser.is_empty())
            engine();
          else 
            userland_parser.parse_command();
          
          break;
         
        case Qt::Key_X :
            renderer->userz += 1;
            glredraw = 1;
            break;
    
         case Qt::Key_Y:
            renderer->userz -= 1;
            glredraw = 1;
            break;
    
         case Qt::Key_Q:
            renderer->userx -= 1;
            glredraw = 1;
            break;
         
         case Qt::Key_W:
            renderer->userx += 1;
            glredraw = 1;
            break;
         
         case Qt::Key_A:
            renderer->usery += 1;
            glredraw = 1;
            break;
    
         case Qt::Key_S:
            renderer->usery -= 1;
            glredraw = 1;
            break;
    
         case Qt::Key_R:
           print_debug(DEBUG_RENDERER, "got R (redraw) keypress event");
           glredraw = 1;
           break;
        case Qt::Key_Up:
          renderer->anglex += 5;
          glredraw = 1;
          break;
        case Qt::Key_Down:
          renderer->anglex -= 5;
          glredraw = 1;
          break;
        case Qt::Key_Left:
          renderer->angley -= 5;
          glredraw = 1;
          break;
        case Qt::Key_Right:
          renderer->angley += 5;
          glredraw = 1;
          break;
        case Qt::Key_PageUp:
          renderer->anglez += 5;
          glredraw = 1;
          break;
        case Qt::Key_PageDown:
          renderer->anglez -= 5;
          glredraw = 1;
          break;
        
         case Qt::Key_Escape:           
            renderer->angley = 0;
            renderer->anglex = 0;
            renderer->anglez = 0;
            renderer->userx = 0;
            renderer->usery = 0;
            renderer->userz = BASE_Z;		
            glredraw = 1;
            break;				

         case Qt::Key_F12:
            //hide_menu();
            break;
         case Qt::Key_F11:
            //hide_status();
            break;				
         case Qt::Key_F10:
            //hide_roominfo();
            break;				

         case Qt::CTRL+Qt::Key_H:
            //hide();
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
