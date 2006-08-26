#include "ExampleApplication.h"
#include "SceneManager.h"

#include <QApplication>
#include <QMessageBox>
#include <QMenuBar>
#include <QStatusBar>
#include <QFileDialog>



#include "mainwindow.h"
#include "utils.h"
#include "stacks.h"
#include "engine.h"
#include "userfunc.h"
#include "forwarder.h"
#include "Map.h"
#include "xml2.h"


class MainWindow *renderer_window;

/* global classless */

void notify_analyzer()
{
    QKeyEvent *k = new QKeyEvent(QEvent::KeyPress, Qt::Key_C,0, "c", false , 1);
    QApplication::postEvent( renderer_window, k );
}


/*  globals end */


void MainWindow::newFile()
{
    
}


void MainWindow::disable_online_actions()
{
  mappingAct->setEnabled(false);
  automergeAct->setEnabled(false);
  angryLinkerAct->setEnabled(false);
}


void MainWindow::enable_online_actions()
{
    mappingAct->setEnabled(true);
    automergeAct->setEnabled(true);
    angryLinkerAct->setEnabled(true);
}


void MainWindow::delete_room()
{
    userland_parser.parse_user_input_line("mdelete");
}

void MainWindow::merge_room()
{
    userland_parser.parse_user_input_line("mmerge");
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
    
    
  usercmd_mload(0, 0,  data, data);  
    
  if (!s.isEmpty()) { 
    usercmd_mload(0, 0,  data, data);  
  }  
  QMessageBox::information(this, "Pandora", "Loaded!\n", QMessageBox::Ok);

}

void MainWindow::reload()
{
    userland_parser.parse_user_input_line("mload");
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
    userland_parser.parse_user_input_line("msave");
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
    usercmd_msave(0, 0,  data, data);  
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
    if (!generalSettingsDialog) {
        generalSettingsDialog = new ConfigWidget(this);
    }

    generalSettingsDialog->run();
    generalSettingsDialog->show();
    generalSettingsDialog->raise();
    generalSettingsDialog->activateWindow();
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
        proxy.setMudEmulation( true );
        Engine.setPrompt("-->");
        stacker.put(1);
        stacker.swap();
    } else {
        proxy.setMudEmulation( false );
    
    }

}




void MainWindow::spellsSettings()
{
    if (!spells_dialog) {
        spells_dialog = new SpellsDialog(this);
    }

    spells_dialog->run();
}


void MainWindow::publish_map()
{
    bool mark[MAX_ROOMS];
    CRoom *r;
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
                if (r->isConnected(z) && mark[ r->exits[z]->id  ] != true ) {
                    if ( r->isDoorSecret(z) == true  ) {
                        stacker.put(r->exits[z]->id);
                    }
                }
            }
        }
        stacker.swap();
    }
    
    
    // Stage 1
    {
        int size = Map.size();
        QProgressDialog progress("Removing all secrets, stage 1/2...", "Abort", 0, Map.size(), this);
        for (i = 0; i < Map.size(); i++) {
            progress.setValue(i);
            qApp->processEvents();
    
            if (progress.wasCanceled())
                return;
            
            
            
            r = Map.rooms[i];
            if (r) {
                if (!mark[r->id]) {
                    Map.deleteRoom(r, 0);
                    continue;        
                }
            }
            
        }
        progress.setValue(size);
    }
        
    // Stage 2
    {        
        int size = Map.size();
        QProgressDialog progress("Removing all secrets, stage 2/2...", "Abort", 0, size, this);
        for (i = 0; i < Map.size(); i++) {
            progress.setValue(i);
            qApp->processEvents();
    
            if (progress.wasCanceled())
                return;
            
            r = Map.rooms[i];
            if (r) {
                for (z = 0; z <= 5; z++) {
                    if ( r->isDoorSecret(z) == true ) {
                        printf("Secret door was still in database...\r\n");
                        r->removeDoor(z);
                    }
                }
            }
            
        }
        
        progress.setValue(size);
    }
    
    
    
    
    
    printf("Done!\r\n");
    //    QMessageBox::information(this, "Removing secrets...", "Done!\n", QMessageBox::Ok);
}

MainWindow::MainWindow(QWidget *parent, Qt::WFlags f)
    : QMainWindow( parent)
{
    setWindowTitle("Pandora");
  	
    resize(640, 480);




  connect(&proxy, SIGNAL(connectionEstablished()), this, SLOT(enable_online_actions()));
  connect(&proxy, SIGNAL(connectionLost()), this, SLOT(disable_online_actions()));
  

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

    
  quitAct =  new QAction(tr("&Exit..."), this);
  quitAct->setShortcut(tr("Ctrl+Q"));
  quitAct->setStatusTip(tr("Quit Pandora"));
  connect(quitAct, SIGNAL(triggered()), this, SLOT(quit()));
    
  /* now building a menu and adding actions to menu */
  fileMenu = menuBar()->addMenu(tr("&File"));
  fileMenu->addAction(newAct);
  fileMenu->addSeparator();
  fileMenu->addAction(openAct);  
  fileMenu->addAction(reloadAct);  
  fileMenu->addSeparator();
  fileMenu->addAction(saveAct);  
  fileMenu->addAction(saveAsAct);  
  fileMenu->addSeparator();
  fileMenu->addAction(quitAct);  


  /* Room menu */
  edit_dialog = NULL;
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


/* Map menu */

  
  
  publishAct= new QAction(tr("Remove All Secrets"), this);
  publishAct->setStatusTip(tr("Removes all secret exits and rooms behind them"));
  connect(publishAct, SIGNAL(triggered()), this, SLOT(publish_map()));    

  mapMenu = menuBar()->addMenu(tr("&Map"));
  mapMenu->addAction(publishAct);



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

  
  mappingMenu = menuBar()->addMenu(tr("M&apping"));
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
  
  
  always_on_top_action= new QAction(tr("Always on Top"), this);
  always_on_top_action->setStatusTip(tr("Always on Top"));
  always_on_top_action->setCheckable(true);
  connect(always_on_top_action, SIGNAL(toggled(bool)), this, SLOT(always_on_top(bool)));

  emulationAct= new QAction(tr("Emulation Mode"), this);
  emulationAct->setStatusTip(tr("Offline MUME Emulation"));
  emulationAct->setCheckable(true);
  emulationAct->setChecked(false);
  connect(emulationAct, SIGNAL(triggered()), this, SLOT(emulation_mode()));    


  generalSettingsDialog = NULL;
  setupGeneralAct= new QAction(tr("General Settings ..."), this);
  setupGeneralAct->setStatusTip(tr("Edit general settings"));
  connect(setupGeneralAct, SIGNAL(triggered()), this, SLOT(generalSetting()) );    
  
  spells_dialog = NULL;
  spellsAct= new QAction(tr("Spells Settings"), this);
  spellsAct->setStatusTip(tr("Spells Settings"));
  connect(spellsAct, SIGNAL(triggered()), this, SLOT(spellsSettings()) );    


  saveConfigAct= new QAction(tr("Save Configuration ..."), this);
  saveConfigAct->setStatusTip(tr("Save current configuration"));
  connect(saveConfigAct, SIGNAL(triggered()), this, SLOT(saveConfig()));    

  saveConfigAsAct= new QAction(tr("Save Configuration As ..."), this);
  saveConfigAsAct->setStatusTip(tr("Save current configuration to a different file"));
  connect(saveConfigAsAct, SIGNAL(triggered()), this, SLOT(saveAsConfig()));    

  loadConfigAct= new QAction(tr("Load Configuration"), this);
  loadConfigAct->setStatusTip(tr("Save current configuration to a different file"));
  connect(loadConfigAct, SIGNAL(triggered()), this, SLOT(loadConfig()));    



  optionsMenu = menuBar()->addMenu(tr("&Configuration"));
  optionsMenu->addAction(hide_status_action);
  optionsMenu->addAction(always_on_top_action);  
  optionsMenu->addAction(emulationAct);  
  optionsMenu->addSeparator();
  optionsMenu->addSeparator();
  optionsMenu->addAction(setupGeneralAct);  
  optionsMenu->addAction(spellsAct);  
  optionsMenu->addSeparator();
  optionsMenu->addAction(saveConfigAct);
//  optionsMenu->addAction(saveConfigAsAct);
  optionsMenu->addAction(loadConfigAct);
    
  

  /* status bar magicz */
  locationLabel = new QLabel("NO_SYNC"); 
  locationLabel->setAlignment(Qt::AlignHCenter); 
  locationLabel->setMinimumSize(locationLabel->sizeHint()); 

  formulaLabel = new QLabel(); 
  
  modLabel = new QLabel(tr("     ")); 
  modLabel->setAlignment(Qt::AlignHCenter); 
  modLabel->setMinimumSize(modLabel->sizeHint());      
  modLabel->clear(); 
  
  statusBar()->addWidget(locationLabel); 
  statusBar()->addWidget(formulaLabel, 1); 
  statusBar()->addWidget(modLabel); 

  
  setUpdatesEnabled(true);
  
  disable_online_actions();
  
  
}

void MainWindow::update_status_bar()
{
  char str[20];
  
  if (conf.get_data_mod() )
    modLabel->setText(tr("Data: MOD "));     
  else 
    modLabel->setText(tr("Data: --- "));
  

  stacker.getCurrent(str);
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

void MainWindow::always_on_top(bool set_on_top)
{
  print_debug(DEBUG_INTERFACE, "always_on_top called");

  Qt::WindowFlags flags = windowFlags();
  if( set_on_top )
  {
    flags |= Qt::WindowStaysOnTopHint;
    print_debug(DEBUG_INTERFACE, "setting WindowStaysOnTopHint");
  } else {
    flags &= ~Qt::WindowStaysOnTopHint;
    print_debug(DEBUG_INTERFACE, "clearing WindowStaysOnTopHint");
  }
  setWindowFlags(flags);
  show();
  conf.set_always_on_top(set_on_top);
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
            Engine.exec();
          else 
            userland_parser.parse_command();
          
          break;


         case Qt::CTRL+Qt::Key_Q:
		QApplication::quit();
            renderer_window->hide();
            break;	
         
/*    
         case Qt::Key_R:
           print_debug(DEBUG_RENDERER, "got R (redraw) keypress event");
           update();
           break;
*/           
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
    
    renderer->keyPressEvent( k );
//    renderer->display();
}

void MainWindow::edit_current_room()
{
    if (stacker.amount() != 1) {
        QMessageBox::critical(this, "Room Info Edit",
                              QString("You are not in sync!"));
        return;
    } 
    
    edit_room(stacker.first()->id);
}


void MainWindow::edit_room(unsigned int id)
{
    if (!edit_dialog) {
        edit_dialog = new RoomEditDialog(this);
    }

    edit_dialog->clear_data();
    edit_dialog->load_room_data(id);
        
    edit_dialog->show();
    edit_dialog->raise();
    edit_dialog->activateWindow();
}

