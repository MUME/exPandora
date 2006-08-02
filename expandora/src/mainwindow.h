#ifndef MAINWINDOW_H 
#define MAINWINDOW_H 

#include <QLabel>
#include <QAction>
#include <QMenu>
#include <QMainWindow>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QKeyEvent>


#include "RoomEditDialog.h"
#include "ConfigWidget.h"
#include "SpellsDialog.h"
#include "renderer.h"

void toggle_renderer_reaction();
void notify_analyzer();

class MainWindow : public QMainWindow
{
  Q_OBJECT

  QLabel        *locationLabel; 
  QLabel        *formulaLabel; 
  QLabel        *modLabel; 


    
    QMenu       *fileMenu;
  QAction       *newAct;
  QAction       *openAct;
  QAction       *reloadAct;
  QAction       *saveAct;
  QAction       *saveAsAct;
  QAction       *quitAct;
  QAction       *publishAct;
  
    QMenu       *mappingMenu;
  QAction       *mappingAct;
  QAction       *automergeAct;
  QAction       *angryLinkerAct;
  
  
    QMenu       *actionsMenu;
  QAction       *roomeditAct;
  QAction       *deleteAct;
  QAction       *mergeAct;
    
    QMenu       *optionsMenu;
  QAction       *always_on_top_action;
  QAction       *calibrateColoursAct;  
  QAction       *saveConfigAct;
  QAction       *saveConfigAsAct;
  QAction       *loadConfigAct;
  QAction       *setupGeneralAct;
  QAction       *spellsAct;
  
  QAction       *emulationAct;
  


  bool          LeftButtonPressed;
  bool          RightButtonPressed;
  QPoint        old_pos;

  void edit_room(unsigned int id);

private:
  ConfigWidget      *generalSettingsDialog;
  RoomEditDialog    *edit_dialog;
  SpellsDialog      *spells_dialog;

public:
    MainWindow(QWidget *parent = 0);

    RendererWidget *renderer;

    void update_status_bar();
    QAction       *hide_status_action;
    QAction       *hide_menu_action;
    

public slots:
  void disable_online_actions();
  void enable_online_actions();

    
private slots:
  void hide_menu();
  void hide_status();
  void always_on_top(bool);
  void newFile();
  void open();
  void reload();
  void quit();
  void save();
  void saveAs();
  void mapping_mode();
  void automerge();
  void angrylinker();
  void calibrateColours();
  void saveConfig();
  void saveAsConfig();
  void loadConfig();
  void delete_room(); 
  void merge_room(); 
  void generalSetting();
  void emulation_mode();  
  void edit_current_room();
  void publish_map();
  void spellsSettings();

  void mousePressEvent( QMouseEvent *);
  void mouseReleaseEvent( QMouseEvent *);
  void mouseMoveEvent( QMouseEvent *);
  void wheelEvent(QWheelEvent *);


  void keyPressEvent( QKeyEvent * );
  void hide();                              /* hide all extra widgets */
};


extern class MainWindow *renderer_window;



#endif
