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
#include "configurator.h"

#include "stacks.h"
#include "utils.h"
#include "rooms.h"
#include "renderer.h"
#include "userfunc.h"
#include "event.h"
#include "dispatch.h"
#include "engine.h"



void MainWindow::newFile()
{
    
}


void MainWindow::disable_online_actions()
{
  mappingAct->setEnabled(false);
  automergeAct->setEnabled(false);
  angryLinkerAct->setEnabled(false);
  calibrateColoursAct->setEnabled(false);
}


void MainWindow::enable_online_actions()
{
  mappingAct->setEnabled(true);
  automergeAct->setEnabled(true);
  angryLinkerAct->setEnabled(true);
  calibrateColoursAct->setEnabled(true);
}



void MainWindow::open()
{
  QString s = QFileDialog::getOpenFileName(
                    this,
                    "Choose a database",
                    "database/",
                    "XML files (*.xml)");    
  char data[MAX_STR_LEN];
    
  strcpy(data, qPrintable(s));
    
  if (!s.isEmpty()) { 
    usercmd_mload(0, 0,  data);  
  }  
  QMessageBox::information(this, "Pandora", "Loaded!\n", QMessageBox::Ok);

}

void MainWindow::reload()
{
    usercmd_mload(0, 0, "");    
}

void MainWindow::quit()
{
    if (conf.get_data_mod()) {
        switch(QMessageBox::information(this, "Pandora",
                                        "The map contains unsaved changes\n"
                                        "Do you want to save the changes before exiting?",
                                        "&Save", "&Discard", "Cancel",
                                        0,      // Enter == button 0
                                        2)) { // Escape == button 2
        case 0: // Save clicked or Alt+S pressed or Enter pressed.
            save();
            break;
        case 1: // Discard clicked or Alt+D pressed
            // don't save but exit
            break;
        case 2: // Cancel clicked or Escape pressed
            return;// don't exit
            break;
        }    
    
    } 
    
    if (conf.get_conf_mod()) {
        switch(QMessageBox::information(this, "Pandora",
                                        "The configuration was changed\n"
                                        "Do you want to write it down on disc before exiting?",
                                        "&Save", "&Discard", "Cancel",
                                        0,      // Enter == button 0
                                        2)) { // Escape == button 2
        case 0: // Save clicked or Alt+S pressed or Enter pressed.
            conf.save_config();
            break;
        case 1: // Discard clicked or Alt+D pressed
            // don't save but exit
            break;
        case 2: // Cancel clicked or Escape pressed
            return;// don't exit
            break;
        }    
    
    
    }
    QApplication::quit();
}

void MainWindow::save()
{
    usercmd_msave(0, 0, "");
    QMessageBox::information(this, "Saving...", "Saved!\n", QMessageBox::Ok);
                                    
}

void MainWindow::saveAs()
{
  char data[MAX_STR_LEN];
  
  QString s = QFileDialog::getSaveFileName(
                    this,
                    "Choose a filename to save under",
                    "database/",
                    "XML database files (*.xml)");
                    
  strcpy(data, qPrintable(s));
    
  if (!s.isEmpty()) { 
    usercmd_msave(0, 0,  data);  
  }  
  
  QMessageBox::information(this, "Saving...", "Saved!\n", QMessageBox::Ok);

}


void MainWindow::mapping_mode()
{
  if (mappingAct->isChecked()) 
  {
    userland_parser.parse_user_input_line("mmap on");
  } else {
    userland_parser.parse_user_input_line("mmap off");
  }
}

void MainWindow::automerge()
{
  if (automergeAct->isChecked()) 
  {
    userland_parser.parse_user_input_line("mautomerge on");
  } else {
    userland_parser.parse_user_input_line("mautomerge off");
  }
}

void MainWindow::angrylinker()
{
  if (angryLinkerAct->isChecked()) 
  {
    userland_parser.parse_user_input_line("mangrylinker on");
  } else {
    userland_parser.parse_user_input_line("mangrylinker off");
  }
}

void MainWindow::calibrateColours()
{
    userland_parser.parse_user_input_line("mcalibrate");
}


void MainWindow::saveConfig()
{
    conf.save_config();
    QMessageBox::information(this, "Saving...", "Saved!\n", QMessageBox::Ok);
}

void MainWindow::delete_room()
{
    userland_parser.parse_user_input_line("mdelete");
}

void MainWindow::merge_room()
{
    userland_parser.parse_user_input_line("mmerge");
}


void MainWindow::saveAsConfig()
{
    QString s = QFileDialog::getSaveFileName(
                    this,
                    "Choose a filename to save under",
                    "configs/",
                    "XML config files (*.xml)");
    if (s.isEmpty())
        return;
    conf.save_config_as("", s.toAscii());
    QMessageBox::information(this, "Saving...", "Saved!\n", QMessageBox::Ok);
}

void MainWindow::loadConfig()
{
  QString s = QFileDialog::getOpenFileName(
                    this,
                    "Choose another configuration file to load",
                    "configs/",
                    "XML config files (*.xml)");    
  if (s.isEmpty())
        return;

  conf.load_config("", s.toAscii());
  QMessageBox::information(this, "Pandora", "Loaded!\n", QMessageBox::Ok);

}    

void MainWindow::generalSetting()
{
    analyser_dialog.run();
}

void MainWindow::emulation_mode()
{
    if (emulationAct->isChecked()) {
        /* cannot turn it on if we have anyone connected to us already */
        /* tricky check - if online_actions are on, then we have connections */
        if (mappingAct->isChecked()) {
            emulationAct->setChecked(false); 
            QMessageBox::critical(this, "Pandora",
                              QString("You have to disconnect from the game first!"));
            return;        
        }
        mud_emulation = true;
        strcpy(last_prompt, "-->");
        stacker.put(1);
        stacker.swap();

        
    } else {
        mud_emulation = false;
    
    }

}


void MainWindow::editPatterns()
{
    pattern_dialog.run();
}


void MainWindow::publish_map()
{
    bool mark[MAX_ROOMS];
    Croom *r;
    unsigned int i;
    unsigned int z;
    
    if (mappingAct->isChecked()) {
        emulationAct->setChecked(false); 
        QMessageBox::critical(this, "Pandora",
                          QString("You have to disconnect first!"));
        return;        
    }
        
    memset(mark, 0, MAX_ROOMS);
    stacker.reset();
    stacker.put(1);
    stacker.swap();
    while (stacker.amount() != 0) {
        for (i = 0; i < stacker.amount(); i++) {
            r = stacker.get(i);
            mark[r->id] = true;
            for (z = 0; z <= 5; z++) {
                if (r->is_connected(z) && mark[r->exits[z]] != true ) {
                    if ( (r->doors[z] == NULL) || (r->doors[z] && (strcmp(r->doors[z], "exit") == 0)) ) {
                        stacker.put(r->exits[z]);
                    }
                }
            }
        }
        stacker.swap();
    }
    
    
    for (i = 0; i < Map.size(); i++) {
        r = Map.rooms[i];
        if (r) {
            if (!mark[r->id]) {
                Map.delete_room(r, 0);
                continue;        
            }
        }
        
    }
    
    for (i = 0; i < Map.size(); i++) {
        r = Map.rooms[i];
        if (r) {
            for (z = 0; z <= 5; z++) {
                if (r->doors[z] && (strcmp(r->doors[z], "exit") != 0)) {
                    printf("Secret door was still in database...\r\n");
                    r->remove_door(z);
                }
            }
        }
        
    }

    
    
    
    
    
    printf("Done!\r\n");
    //    QMessageBox::information(this, "Removing secrets...", "Done!\n", QMessageBox::Ok);
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

  saveAct = new QAction(tr("&Save..."), this);
  saveAct->setShortcut(tr("Ctrl+S"));
  saveAct->setStatusTip(tr("Save currently opened map"));
  connect(saveAct, SIGNAL(triggered()), this, SLOT(save()));

  saveAsAct = new QAction(tr("Save As..."), this);
  saveAsAct->setStatusTip(tr("Save the map As"));
  connect(saveAsAct, SIGNAL(triggered()), this, SLOT(saveAs()));

  publishAct= new QAction(tr("Clear secrets"), this);
  publishAct->setStatusTip(tr("Removes all secret exits and rooms behind them"));
  connect(publishAct, SIGNAL(triggered()), this, SLOT(publish_map()));    

    
  quitAct =  new QAction(tr("&Exit..."), this);
  quitAct->setShortcut(tr("Ctrl+Q"));
  quitAct->setStatusTip(tr("Quit Pandora"));
  connect(quitAct, SIGNAL(triggered()), this, SLOT(quit()));
    
  /* now building a menu and adding actions to menu */
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAct);
  fileMenu->addAction(publishAct);
  fileMenu->addSeparator();
  fileMenu->addAction(openAct);  
  fileMenu->addAction(reloadAct);  
  fileMenu->addSeparator();
  fileMenu->addAction(saveAct);  
  fileMenu->addAction(saveAsAct);  
  fileMenu->addSeparator();
  fileMenu->addAction(quitAct);  


  /* Room menu */
  roomeditAct= new QAction(tr("Edit Current Room"), this);
  roomeditAct->setStatusTip(tr("View/Edit current Room's info"));
  connect(roomeditAct, SIGNAL(triggered()), this, SLOT(edit_current_room()));    

  deleteAct= new QAction(tr("Delete current room"), this);
  deleteAct->setStatusTip(tr("Deletes this room"));
  connect(deleteAct, SIGNAL(triggered()), this, SLOT(delete_room()));    

  mergeAct= new QAction(tr("Merge"), this);
  mergeAct->setStatusTip(tr("Tries to find twin room and merge them"));
  connect(mergeAct, SIGNAL(triggered()), this, SLOT(merge_room()));    



  actionsMenu = menuBar()->addMenu(tr("&Room"));
  actionsMenu->addAction(roomeditAct);
  actionsMenu->addAction(deleteAct);
  actionsMenu->addAction(mergeAct);



  /* Mapping menu */
  
  
  mappingAct= new QAction(tr("Mapping"), this);
  mappingAct->setCheckable(true);
  mappingAct->setChecked(false);
  mappingAct->setStatusTip(tr("Switch mapping mode on/off"));
  connect(mappingAct, SIGNAL(triggered()), this, SLOT(mapping_mode()));    
  
  automergeAct= new QAction(tr("AutoMerge"), this);
  automergeAct->setCheckable(true);
  conf.get_automerge() ? automergeAct->setChecked(true) : automergeAct->setChecked(false);
  automergeAct->setStatusTip(tr("Automatically merge twin (same name/desc) rooms"));
  connect(automergeAct, SIGNAL(triggered()), this, SLOT(automerge()));    

  angryLinkerAct= new QAction(tr("AngryLinker"), this);
  angryLinkerAct->setCheckable(true);
  conf.get_angrylinker() ? angryLinkerAct->setChecked(false) : angryLinkerAct->setChecked(false);
  angryLinkerAct->setStatusTip(tr("Auto-link neightbour rooms"));
  connect(angryLinkerAct, SIGNAL(triggered()), this, SLOT(angrylinker()));    

  
  mappingMenu = menuBar()->addMenu(tr("&Mapping"));
  mappingMenu->addAction(mappingAct);
  mappingMenu->addAction(automergeAct);
  mappingMenu->addAction(angryLinkerAct);


/* Configuration menu bar */
  
  
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
  
  
  calibrateColoursAct= new QAction(tr("Calibrate Colours"), this);
  calibrateColoursAct->setStatusTip(tr("Sends change colour command and parses the output"));
  connect(calibrateColoursAct, SIGNAL(triggered()), this, SLOT(calibrateColours()));    

  always_on_top_action= new QAction(tr("Always on Top"), this);
  always_on_top_action->setStatusTip(tr("Always on Top"));
  always_on_top_action->setCheckable(true);
  always_on_top_action->setChecked(true);
  connect(always_on_top_action, SIGNAL(triggered()), this, SLOT(always_on_top()));    

  emulationAct= new QAction(tr("Emulation Mode"), this);
  emulationAct->setStatusTip(tr("Offline MUME Emulation"));
  emulationAct->setCheckable(true);
  emulationAct->setChecked(false);
  connect(emulationAct, SIGNAL(triggered()), this, SLOT(emulation_mode()));    


  setupGeneralAct= new QAction(tr("General Settings ..."), this);
  setupGeneralAct->setStatusTip(tr("Edit general settings"));
  connect(setupGeneralAct, SIGNAL(triggered()), this, SLOT(generalSetting()) );    
  
  
  patternEditAct= new QAction(tr("Edit Patterns"), this);
  patternEditAct->setStatusTip(tr("Edit Patterns"));
  connect(patternEditAct, SIGNAL(triggered()), this, SLOT(editPatterns()) );    

  saveConfigAct= new QAction(tr("Save Configuration ..."), this);
  saveConfigAct->setStatusTip(tr("Save current configuration"));
  connect(saveConfigAct, SIGNAL(triggered()), this, SLOT(saveConfig()));    

  saveConfigAsAct= new QAction(tr("Save Configuration As ..."), this);
  saveConfigAsAct->setStatusTip(tr("Save current configuration to a different file"));
  connect(saveConfigAsAct, SIGNAL(triggered()), this, SLOT(saveConfigAs()));    

  loadConfigAct= new QAction(tr("Load Configuration"), this);
  loadConfigAct->setStatusTip(tr("Save current configuration to a different file"));
  connect(loadConfigAct, SIGNAL(triggered()), this, SLOT(loadConfig()));    



  optionsMenu = menuBar()->addMenu(tr("&Configuration"));
  optionsMenu->addAction(hide_status_action);
  optionsMenu->addAction(always_on_top_action);  
  optionsMenu->addAction(emulationAct);  
  optionsMenu->addSeparator();
  optionsMenu->addAction(calibrateColoursAct);  
  optionsMenu->addSeparator();
  optionsMenu->addAction(setupGeneralAct);  
  optionsMenu->addAction(patternEditAct);  
  optionsMenu->addSeparator();
  optionsMenu->addAction(saveConfigAct);
  optionsMenu->addAction(saveConfigAsAct);
  optionsMenu->addAction(loadConfigAct);
    
  


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
  
  
  disable_online_actions();
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

void MainWindow::edit_current_room()
{
    printf("Staring edit room action!\r\n");
    if (stacker.amount() != 1) {
        QMessageBox::critical(this, "Room Info Edit",
                              QString("You are not in sync!"));
        return;
    } 
    
    edit_room(stacker.first()->id);
}


void MainWindow::edit_room(unsigned int id)
{
    edit_dialog.clear_data();
    edit_dialog.load_room_data(id);
        
    edit_dialog.show();
    edit_dialog.raise();
    edit_dialog.activateWindow();
}

/* ROOM EDIT DIALOG */
RoomEditDialog::RoomEditDialog(QWidget *parent) : 
                                    QDialog(parent)
{
    setupUi(this);
    
    comboBox_N->insertItem(ROOMFLAG_UNDEFINED, "Undefined Exit");    
    comboBox_E->insertItem(ROOMFLAG_UNDEFINED, "Undefined Exit");    
    comboBox_S->insertItem(ROOMFLAG_UNDEFINED, "Undefined Exit");    
    comboBox_W->insertItem(ROOMFLAG_UNDEFINED, "Undefined Exit");    
    comboBox_U->insertItem(ROOMFLAG_UNDEFINED, "Undefined Exit");    
    comboBox_D->insertItem(ROOMFLAG_UNDEFINED, "Undefined Exit");    

    comboBox_N->insertItem(ROOMFLAG_DEATH, "DEATH");    
    comboBox_E->insertItem(ROOMFLAG_DEATH, "DEATH");    
    comboBox_S->insertItem(ROOMFLAG_DEATH, "DEATH");    
    comboBox_W->insertItem(ROOMFLAG_DEATH, "DEATH");    
    comboBox_U->insertItem(ROOMFLAG_DEATH, "DEATH");    
    comboBox_D->insertItem(ROOMFLAG_DEATH, "DEATH");    

    comboBox_N->insertItem(ROOMFLAG_NONE, "NONE");
    comboBox_E->insertItem(ROOMFLAG_NONE, "NONE");
    comboBox_S->insertItem(ROOMFLAG_NONE, "NONE");
    comboBox_W->insertItem(ROOMFLAG_NONE, "NONE");
    comboBox_U->insertItem(ROOMFLAG_NONE, "NONE");
    comboBox_D->insertItem(ROOMFLAG_NONE, "NONE");
    
    
    connect(comboBox_N, SIGNAL(activated(int)), this, SLOT(changedExitsFlagN(int)) );
    connect(comboBox_S, SIGNAL(activated(int)), this, SLOT(changedExitsFlagS(int)) );
    connect(comboBox_E, SIGNAL(activated(int)), this, SLOT(changedExitsFlagE(int)) );
    connect(comboBox_W, SIGNAL(activated(int)), this, SLOT(changedExitsFlagW(int)) );
    connect(comboBox_U, SIGNAL(activated(int)), this, SLOT(changedExitsFlagU(int)) );
    connect(comboBox_D, SIGNAL(activated(int)), this, SLOT(changedExitsFlagD(int)) );
    
    connect(checkBox_N, SIGNAL(toggled(bool)), this, SLOT(changedExitsStateN(bool)) );
    connect(checkBox_S, SIGNAL(toggled(bool)), this, SLOT(changedExitsStateS(bool)) );
    connect(checkBox_E, SIGNAL(toggled(bool)), this, SLOT(changedExitsStateE(bool)) );
    connect(checkBox_W, SIGNAL(toggled(bool)), this, SLOT(changedExitsStateW(bool)) );
    connect(checkBox_U, SIGNAL(toggled(bool)), this, SLOT(changedExitsStateU(bool)) );
    connect(checkBox_D, SIGNAL(toggled(bool)), this, SLOT(changedExitsStateD(bool)) );

/*    
    connect(okButton, SIGNAL(clicked()), this, SLOT(accept()));
    connect(cancelButton, SIGNAL(clicked()), this, SLOT(reject()));
*/
}

int RoomEditDialog::updateExitsInfo(int dir, Croom *r)
{
    QString dname;
    unsigned int lead;
    int flag;
    
    set_door_context(dir);
    if (box->isChecked()) {
        flag = flags->currentIndex();
        if (flag == ROOMFLAG_UNDEFINED) {
            lead = EXIT_UNDEFINED;
        } else if (flag == ROOMFLAG_DEATH) {
            lead = EXIT_DEATH;
        } else  {        
            lead = leads->text().toInt();
            if (Map.getroom(lead) == NULL) {
                QMessageBox::critical(this, "Room Info Edit",
                              QString("Bad door to the north!"));
                return -1;
            }
        }
        r->exits[dir] = lead;
    
        dname = door->text();
        if (dname.length() > 40) {
            QMessageBox::critical(this, "Room Info Edit",
                              QString("Bad door to the north!"));
            return -1;    
        }
        r->refresh_door(dir, dname.toAscii());    
    } else {
        if (r->doors[dir])
            free(r->doors[dir]);
        r->exits[dir] = 0;
        return 0;        
    }
    
    return 0;
}

void RoomEditDialog::clear_data()
{
    textEdit_note->clear();
    textEdit_desc->clear();
}


void    RoomEditDialog::set_door_context(int dir)
{
    switch (dir) {
        case NORTH: door = lineEdit_doornorth;
                    leads = lineEdit_leadN;
                    flags = comboBox_N;
                    box = checkBox_N;
                    break;
    
        case SOUTH: door = lineEdit_doorsouth;
                    leads = lineEdit_leadS;
                    flags = comboBox_S;
                    box = checkBox_S;
                    break;
    
        case EAST : door = lineEdit_dooreast; 
                    leads = lineEdit_leadE;
                    flags = comboBox_E;
                    box = checkBox_E;
                    break;
    
        case WEST : door = lineEdit_doorwest;
                    leads = lineEdit_leadW;
                    flags = comboBox_W;
                    box = checkBox_W;
                    break;
    
        case UP   : door = lineEdit_doorup;   
                    leads = lineEdit_leadU;
                    flags = comboBox_U;
                    box = checkBox_U;
                    break;
    
        case DOWN : door = lineEdit_doordown; 
                    leads = lineEdit_leadD;
                    flags = comboBox_D;
                    box = checkBox_D;
                    break;
    
        default:
            return;    
    }
}

void  RoomEditDialog::changedExitsFlag(int dir, int index)
{
    set_door_context(dir);  
      
    if (index == ROOMFLAG_NONE) {
        leads->setEnabled(true);
        leads->setText("??ID??");
        flags->setCurrentIndex(ROOMFLAG_NONE);
    } else if (index == ROOMFLAG_UNDEFINED) {
        leads->setText("UNDEF");
        leads->setEnabled(false);
        flags->setCurrentIndex(ROOMFLAG_UNDEFINED);
    } else if (index == ROOMFLAG_DEATH) {
        leads->setText("DEATH");
        leads->setEnabled(false);
        flags->setCurrentIndex(ROOMFLAG_DEATH);
    } 
    
}


void RoomEditDialog::setup_exit_widgets(int dir, Croom *r)
{
    set_door_context(dir);
    
    
    /* Exit North */
    door->setText(r->doors[dir]);        
    flags->setCurrentIndex(ROOMFLAG_NONE);
    
    if (r->exits[dir] == 0) {
        box->setChecked(false);
        changedExitsState(dir, false);
    } else {
        box->setChecked(true);
        changedExitsState(dir, true);
                
        if (r->exits[dir] == EXIT_UNDEFINED) {
            changedExitsFlag(dir, ROOMFLAG_UNDEFINED);
        } else if (r->exits[dir] == EXIT_DEATH) {
            changedExitsFlag(dir, ROOMFLAG_UNDEFINED);
        } else {
            changedExitsFlag(dir, ROOMFLAG_NONE);
            leads->setText(QString("%1").arg(r->exits[dir]) );
            leads->setEnabled(true);
        }
        
                
    }
}


void  RoomEditDialog::changedExitsState(int dir, bool state)
{
    set_door_context(dir);
    if (state) {
        door->setEnabled(true);
        leads->setEnabled(false);
        flags->setEnabled(true);
        flags->setCurrentIndex(ROOMFLAG_UNDEFINED);
        leads->setText("UNDEF");
    } else {
        door->setEnabled(false);
        leads->setEnabled(false);
        flags->setEnabled(false);
        flags->setCurrentIndex(ROOMFLAG_NONE);
    }
}

void RoomEditDialog::load_room_data(unsigned int id)
{
    Croom *r;
    unsigned int i;
    
    /* stuff dialog with room data */    
    r = Map.getroom(id);
    setup_exit_widgets(NORTH, r);
    setup_exit_widgets(EAST, r);
    setup_exit_widgets(SOUTH, r);
    setup_exit_widgets(WEST, r);
    setup_exit_widgets(UP, r);
    setup_exit_widgets(DOWN, r);
    
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
    Croom *r;
    QString name, desc, note;
    int x, y, z;
    int id;
    char terrain;

    printf("Accepted!\r\n");
    /* OK! Now check the data */
    
    id = label_roomid->text().toInt();
    printf("Room id : %i\r\n", id);
    
    r = Map.getroom(id);
    if (r == NULL) {
        QMessageBox::critical(this, "Room Info Edit",
                              QString("The room with this ID does not exist anymore."));
        return;    
    }
    
    name = lineEdit_name->text();
    if (name.length() == 0 || name.length() > 80) {
        QMessageBox::critical(this, "Room Info Edit",
                              QString("Bad room name!"));
        return;    
    }
    
    
    x = lineEdit_coordx->text().toInt();
    y = lineEdit_coordy->text().toInt();
    z = lineEdit_coordz->text().toInt();
    
    desc = textEdit_desc->toPlainText();
    desc.replace("\n", "|");
    terrain = comboBox_terrain->currentIndex();
            
    note = textEdit_note->toPlainText();
            
            
    if (r->name != name) 
        r->refresh_roomname(name.toAscii());            
    if (r->desc != desc) 
        r->refresh_desc(desc.toAscii());            
    if (r->note != note) 
        r->refresh_note(note.toAscii());            

    if (updateExitsInfo(NORTH, r) == -1) return;
    if (updateExitsInfo(EAST, r) == -1) return;
    if (updateExitsInfo(SOUTH, r) == -1) return;
    if (updateExitsInfo(WEST, r) == -1) return;
    if (updateExitsInfo(UP, r) == -1) return;
    if (updateExitsInfo(DOWN, r) == -1) return;
            
    toggle_renderer_reaction();
    done(Accepted);
}

void RoomEditDialog::reject()
{
    printf("Rejected!\r\n");
    done(Rejected);
}



/* ----------------------- AnalyserConfigWidget -----------------------------*/
 
ConfigWidget::ConfigWidget (QWidget *parent) : QDialog(parent)
{
    setupUi(this);                        
    
    connect(checkBox_autorefresh, SIGNAL(toggled(bool)), this, SLOT(autorefreshUpdated(bool)) );

}

void ConfigWidget::autorefreshUpdated(bool state)
{
    if (state) {
        spinBox_namequote->setEnabled(true);
        spinBox_descquote->setEnabled(true);
    } else {
        spinBox_namequote->setEnabled(false);
        spinBox_descquote->setEnabled(false);
    }
}


void ConfigWidget::run()
{

    if (isVisible())
        return;
    if (conf.get_brief_mode()) 
        checkBox_brief->setChecked(true);
    else 
        checkBox_brief->setChecked(false);
              
       
    lineEdit_remoteport->setText(QString("%1").arg(conf.get_remote_port()) );
    lineEdit_remotehost->setText(conf.get_remote_host());
    
    lineEdit_localport->setText(QString("%1").arg(conf.get_local_port()) );

    spinBox_namequote->setValue(conf.get_name_quote());
    spinBox_descquote->setValue(conf.get_desc_quote());
    if (conf.get_autorefresh()) {
        checkBox_autorefresh->setChecked(true);
        spinBox_namequote->setEnabled(true);
        spinBox_descquote->setEnabled(true);
    } else {
        checkBox_autorefresh->setChecked(false);
        spinBox_namequote->setEnabled(false);
        spinBox_descquote->setEnabled(false);
    }
    
    if (conf.get_automerge()) 
        checkBox_automerge->setChecked(true);
    else
        checkBox_automerge->setChecked(false);
    
    if (conf.get_angrylinker()) 
        checkBox_angrylinker->setChecked(true);
    else
        checkBox_angrylinker->setChecked(false);

    if (conf.get_exits_check()) 
        checkBox_exits->setChecked(true);
    else
        checkBox_exits->setChecked(false);

    if (conf.get_terrain_check()) 
        checkBox_terrain->setChecked(true);
    else
        checkBox_terrain->setChecked(false);

    
    
        
    lineEdit_visrange->setText(QString("%1").arg(conf.get_texture_vis()) );
    lineEdit_detrange->setText(QString("%1").arg(conf.get_details_vis()) );
    
    show();
    raise();
    activateWindow();
}

void ConfigWidget::accept()
{
    int i;
    
    if (conf.get_brief_mode() !=  checkBox_brief->isChecked() ) 
        conf.set_brief_mode( checkBox_brief->isChecked() );
       
    if (conf.get_remote_host() != lineEdit_remotehost->text().toAscii() ) 
        conf.set_remote_host(lineEdit_remotehost->text().toAscii());
    
    i = lineEdit_remoteport->text().toInt();
    if (i == 0) {
            QMessageBox::critical(this, "Cofiguration",
                              QString("Bad remote port!"));
            return;    
    }
    if (conf.get_remote_port() != i)
        conf.set_remote_port(i);
        
    
    i = lineEdit_localport->text().toInt();
    if (i == 0) {
            QMessageBox::critical(this, "Cofiguration",
                              QString("Bad local port!"));
            return;    
    }
    if (conf.get_local_port() != i)
        conf.set_local_port(i);


    if (conf.get_name_quote() != spinBox_namequote->value())
        conf.set_name_quote( spinBox_namequote->value() ); 
    if (conf.get_desc_quote() != spinBox_descquote->value() )        
        conf.set_desc_quote( spinBox_descquote->value() ); 
    
    if (conf.get_autorefresh() != checkBox_autorefresh->isChecked())
        conf.set_autorefresh( checkBox_autorefresh->isChecked() );
    
    
    if (conf.get_automerge() != checkBox_automerge->isChecked()) 
        conf.set_automerge(checkBox_automerge->isChecked());
    
    if (conf.get_angrylinker() != checkBox_angrylinker->isChecked() )
        conf.set_angrylinker(checkBox_angrylinker->isChecked() );
 
    if (conf.get_exits_check() != checkBox_exits->isChecked() ) 
        conf.set_exits_check(checkBox_exits->isChecked() );
        
    if (conf.get_terrain_check() != checkBox_terrain->isChecked() )
        conf.set_terrain_check( checkBox_terrain->isChecked() );
    
        
    
    i = lineEdit_visrange->text().toInt();
    if (i == 0) {
            QMessageBox::critical(this, "Cofiguration",
                              QString("Bad textures visibility range port!"));
            return;    
    }
    if (conf.get_texture_vis() != i)
        conf.set_texture_vis(i);


    i = lineEdit_detrange->text().toInt();
    if (i == 0) {
            QMessageBox::critical(this, "Cofiguration",
                              QString("Bad details visibility range!"));
            return;    
    }
    if (conf.get_details_vis() != i)
        conf.set_details_vis(i);

    done(Accepted);
}


/* --------------------------- PatternEditDialog ------------------------------ */

PatternEditDialog::PatternEditDialog(QWidget *parent) : QDialog(parent)
{
    setupUi(this);

    connect(pushButton_add, SIGNAL(clicked()), this, SLOT(add_clicked()) );
    connect(pushButton_save, SIGNAL(clicked()), this, SLOT(save_clicked()) );
    connect(pushButton_remove, SIGNAL(clicked()), this, SLOT(remove_clicked()) );
    connect(pushButton_edit, SIGNAL(clicked()), this, SLOT(edit_clicked()) );
}

void PatternEditDialog::run()
{
    unsigned int    i;
    QString s;

    if (isVisible())
        return;

    patterns.clear();
    frame->setEnabled(false);
    editing_index = 0;
    
    for (i = 0; i < conf.patterns.size(); i++) 
        patterns.push_back( conf.patterns[i] ); 
    
    for (i = 0; i < event_types.size(); i++)
        comboBox_type->addItem(event_types[i].name);

    redraw();
    show();
    raise();
    activateWindow();
}


void PatternEditDialog::edit_clicked()
{
    editing_index = listWidget->currentRow();
    frame->setEnabled(true);

    checkBox_regexp->setChecked(patterns[editing_index].is_regexp);
    lineEdit_expr->setText(patterns[editing_index].pattern);
    comboBox_type->setCurrentIndex(patterns[editing_index].type);
    lineEdit_data->setText(patterns[editing_index].data);
}

void PatternEditDialog::redraw()
{
    unsigned int i;
    QString s;
    QString tmp, tmp2;
    
    listWidget->clear();
    for (i = 0; i < patterns.size(); i++) {
        tmp = event_types[patterns[i].type].name;
        tmp2 = patterns[i].pattern;
        s = QString("%1 %2 %3")
            .arg( patterns[i].is_regexp ? "REGEXP" : "WILDCARD", -10)
            .arg( tmp, -15)
            .arg( tmp2, -90 );
        listWidget->addItem(s);   
    }
    repaint();
}


void PatternEditDialog::save_clicked()
{
    frame->setEnabled(false);
    TPattern p;
    
    if (editing_index == -1) {
        editing_index = patterns.size();
        patterns.push_back(p);
    }
    
    patterns[editing_index].is_regexp = checkBox_regexp->isChecked();
    patterns[editing_index].pattern = lineEdit_expr->text().toAscii();
    patterns[editing_index].type = comboBox_type->currentIndex();
    patterns[editing_index].data = lineEdit_data->text().toAscii();
    redraw();
}

void PatternEditDialog::add_clicked()
{
    frame->setEnabled(true);
    editing_index = -1;
}

void PatternEditDialog::remove_clicked()
{
    vector<TPattern>::iterator p;
    int i;
    
    i = 0;
    for (p=patterns.begin(); p != patterns.end(); ++p) {
        if (i == listWidget->currentRow()) {
            p = patterns.erase(p);
            break;
        }
        i++;
    }
    redraw();
}


void PatternEditDialog::accept()
{
    unsigned int i;
    
    conf.patterns.clear();
    for (i = 0; i < patterns.size(); i++) {
        conf.add_pattern(patterns[i].pattern, patterns[i].data, event_types[patterns[i].type].group, 
                            patterns[i].type, patterns[i].is_regexp);                        
    }
    patterns.clear();        
    done(Accepted);
}






