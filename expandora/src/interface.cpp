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
#include <QDialog>
#include <QMessageBox>

#include "defines.h"
#include "struct.h"
#include "configurator.h"

#include "renderer.h"
#include "stacks.h"
#include "utils.h"
#include "rooms.h"

#include "userfunc.h"



void MainWindow::newFile()
{
    
}

void MainWindow::open()
{
  QString s = QFileDialog::getOpenFileName(
                    this,
                    "Choose a database",
                    "",
                    "XML files (*.xml)");    
  char data[MAX_STR_LEN];
    
  strcpy(data, qPrintable(s));
    
  if (!s.isEmpty()) { 
    usercmd_mload(0, 0,  data);  
  }  
}

void MainWindow::reload()
{
    usercmd_mload(0,0, "");    
}

void MainWindow::quit()
{
    QApplication::quit();
}

MainWindow::MainWindow(QWidget *parent, const char *name)
    : QMainWindow( parent)
{
  renderer =  new RendererWidget( this, name);
  setCentralWidget( renderer );
  resize(640, 480);

  /* creating actions and connecting them here */
  newAct = new QAction(tr("&New"), this);
  newAct->setShortcut(tr("Ctrl+N"));
  newAct->setStatusTip(tr("Create a new map"));
  connect(newAct, SIGNAL(triggered()), this, SLOT(newFile()));    
  
  openAct = new QAction(tr("&Load..."), this);
  openAct->setShortcut(tr("Ctrl+L"));
  openAct->setStatusTip(tr("Open an existing map"));
  connect(openAct, SIGNAL(triggered()), this, SLOT(open()));

  reloadAct = new QAction(tr("&Reload..."), this);
  reloadAct->setShortcut(tr("Ctrl+R"));
  reloadAct->setStatusTip(tr("Reload current map"));
  connect(reloadAct, SIGNAL(triggered()), this, SLOT(reload()));
    
  quitAct =  new QAction(tr("&Exit..."), this);
  quitAct->setShortcut(tr("Ctrl+Q"));
  quitAct->setStatusTip(tr("Quit Pandora"));
  connect(quitAct, SIGNAL(triggered()), this, SLOT(quit()));
    
  /* now building a menu and adding actions to menu */
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAct);
  fileMenu->addAction(openAct);  
  fileMenu->addAction(quitAct);  


/* options menu bar */
  
  
  hide_status_action = new QAction(tr("Hide Status Bar"), this);
  hide_status_action->setShortcut(tr("F11"));
  hide_status_action->setStatusTip(tr("Hides the Statusbar"));
  hide_status_action->setCheckable(true);
  hide_status_action->setChecked(false);
  connect(hide_status_action, SIGNAL(triggered()), this, SLOT(hide_status()));    

  hide_menu_action= new QAction(tr("Hide Menu Bar"), this);
  hide_menu_action->setShortcut(tr("F12"));
  hide_menu_action->setStatusTip(tr("Hides the Menubar"));
  hide_menu_action->setCheckable(true);
  hide_menu_action->setChecked(false);
  connect(hide_menu_action, SIGNAL(triggered()), this, SLOT(hide_menu()));    
  

  always_on_top_action= new QAction(tr("Always on Top"), this);
  always_on_top_action->setStatusTip(tr("Always on Top"));
  always_on_top_action->setCheckable(true);
  always_on_top_action->setChecked(true);
  connect(always_on_top_action, SIGNAL(triggered()), this, SLOT(always_on_top()));    

  optionsMenu = menuBar()->addMenu(tr("&Options"));
  optionsMenu->addAction(hide_status_action);
  optionsMenu->addAction(hide_menu_action);  
  optionsMenu->addAction(always_on_top_action);  

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

  /* Actions menu */
  roomeditAct= new QAction(tr("Room Info"), this);
  roomeditAct->setStatusTip(tr("View/Edit current Room's info"));
  connect(roomeditAct, SIGNAL(triggered()), this, SLOT(edit_room()));    

  actionsMenu = menuBar()->addMenu(tr("&Actions"));
  actionsMenu->addAction(roomeditAct);
  

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
  if (hide_menu_action->isChecked()) 
  {
    menuBar()->hide();  
  } else {
    menuBar()->show();
  }
}

void MainWindow::always_on_top()
{
  print_debug(DEBUG_INTERFACE, "always_on_top called");

  Qt::WindowFlags flags = windowFlags();
  if( flags && Qt::WindowStaysOnTopHint )
  {
    flags ^= Qt::WindowStaysOnTopHint;
  } else {
    flags |= Qt::WindowStaysOnTopHint;
  }
  printf("Setting flag.\r\n");
//  setWindowFlags(flags);
  printf("The flag is set.\r\n");
}


void MainWindow::hide_status()
{
  print_debug(DEBUG_INTERFACE, "hide/show status called");

  if (hide_status_action->isChecked() ) 
  {
    statusBar()->hide();  
  } else {
    statusBar()->show();
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


         case Qt::CTRL+Qt::Key_Q:
		QApplication::quit();
//            renderer_window->hide();
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
	    hide_menu_action->setChecked(!hide_menu_action->isChecked());
            hide_menu();
            break;
         case Qt::Key_F11:
	    hide_status_action->setChecked(!hide_status_action->isChecked());
            hide_status();
            break;				
         case Qt::Key_F10:
            //hide_roominfo();
            break;				

         case Qt::CTRL+Qt::Key_H:
//            renderer_window->hide();
            break;	
    }
    renderer_window->renderer->display();
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

void MainWindow::edit_room()
{
    printf("Staring edit room action!\r\n");
    if (stacker.amount != 1) {
        QMessageBox::critical(0, "Room Info Edit",
                              QString("You are not in sync!"));
        
            
        return;
    } 
    
    edit_dialog.load_room_data(stacker.get(1));
    
    edit_dialog.show();
    edit_dialog.raise();
    edit_dialog.activateWindow();
}

/* ROOM EDIT DIALOG */
RoomEditDialog::RoomEditDialog(QWidget *parent) : 
                                    QDialog(parent)
{
    setupUi(this);
    
//    setModal(true);

    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
}


void RoomEditDialog::load_room_data(unsigned int id)
{
    Troom *r;
    unsigned int i;
    
    /* stuff dialog with room data */    
    r = roomer.getroom(id);
    lineEdit_doornorth->setText(r->doors[NORTH]);        
    lineEdit_doorsouth->setText(r->doors[SOUTH]);        
    lineEdit_dooreast->setText(r->doors[EAST]);        
    lineEdit_doorwest->setText(r->doors[WEST]);        
    lineEdit_doorup->setText(r->doors[UP]);        
    lineEdit_doordown->setText(r->doors[DOWN]);        
    lineEdit_name->setText(r->name);
    lineEdit_coordx->setText(QString("%1").arg(r->x));
    lineEdit_coordy->setText(QString("%1").arg(r->y));
    lineEdit_coordz->setText(QString("%1").arg(r->z));
    
    QString desc = r->desc;
    desc.replace("|", "\n");
    textEdit_desc->append( desc );
    label_roomid->setText(QString("%1").arg(r->id) );
    textEdit_note->append(r->note);
        
    for (i=0; i< conf.sectors.size(); i++)
        comboBox_terrain->insertItem(i, conf.sectors[i].desc);    
        
    comboBox_terrain->setCurrentIndex(r->sector);
}


void RoomEditDialog::accept()
{
    Troom *r;
    QString name, desc, note;
    QString doorn, doore, doors, doorw, dooru, doord;
    int x, y, z;
    int id;
    char terrain;

    printf("Accepted!\r\n");
    /* OK! Now check the data */
    
    id = label_roomid->text().toInt();
    
    r = roomer.getroom(stacker.get(id));
    if (r == NULL) {
        QMessageBox::critical(0, "Room Info Edit",
                              QString("The room with this ID does not exist anymore."));
        return;    
    }
    
    name = lineEdit_name->text();
    if (name.length() == 0 || name.length() > 80) {
        QMessageBox::critical(0, "Room Info Edit",
                              QString("Bad room name!"));
        return;    
    }
    
    doorn = lineEdit_doornorth->text();
    if (doorn.length() > 40) {
        QMessageBox::critical(0, "Room Info Edit",
                              QString("Bad door to the north!"));
        return;    
    }
    
    doors = lineEdit_doorsouth->text();
    if (doors.length() > 40) {
        QMessageBox::critical(0, "Room Info Edit",
                              QString("Bad door to the south!"));
        return;    
    }
    doore = lineEdit_dooreast->text();
    if (doore.length() > 40) {
        QMessageBox::critical(0, "Room Info Edit",
                              QString("Bad door to the east!"));
        return;    
    }
    doorw = lineEdit_doorwest->text();
    if (doorw.length() > 40) {
        QMessageBox::critical(0, "Room Info Edit",
                              QString("Bad door to the west!"));
        return;    
    }
    
    dooru = lineEdit_doorup->text();
    if (dooru.length() > 40) {
        QMessageBox::critical(0, "Room Info Edit",
                              QString("Bad door to the up!"));
        return;    
    }
    
    doord = lineEdit_doordown->text();
    if (doord.length() > 40) {
        QMessageBox::critical(0, "Room Info Edit",
                              QString("Bad door to the down!"));
        return;    
    }
    
    x = lineEdit_coordx->text().toInt();
    y = lineEdit_coordy->text().toInt();
    z = lineEdit_coordz->text().toInt();
    
    desc = textEdit_desc->toPlainText();
    desc.replace("\n", "|");
    terrain = comboBox_terrain->currentIndex();
            
    note = textEdit_note->toPlainText();
            
            
    if (r->name != name) {
        printf(" REfreshing roomname.\r\n");
        printf(" Old: %s\r\n", r->name);
        printf(" New: %s\r\n", qPrintable(name));
        roomer.refresh_roomname(id, name.toAscii());            
    }
    if (r->desc != desc) {
        printf(" REfreshing desc.\r\n");
        printf(" Old: %s\r\n", r->desc);
        printf(" New: %s\r\n", qPrintable(desc));
        roomer.refresh_desc(id, desc.toAscii());            
    }
    if (r->note != note) {
        printf(" Refreshing note.\r\n");
        printf(" Old: %s\r\n", r->note);
        printf(" New: %s\r\n", qPrintable(note));
        roomer.refresh_note(id, note.toAscii());            
    }

    roomer.refresh_door(id, NORTH, doorn.toAscii());    
    roomer.refresh_door(id, EAST, doore.toAscii());    
    roomer.refresh_door(id, SOUTH, doors.toAscii());    
    roomer.refresh_door(id, WEST, doorw.toAscii());    
    roomer.refresh_door(id, UP, dooru.toAscii());    
    roomer.refresh_door(id, DOWN, doord.toAscii());    
            
    done(Accepted);
}

void RoomEditDialog::reject()
{
    printf("Rejected!\r\n");
    done(Rejected);
}








