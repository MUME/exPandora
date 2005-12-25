#ifndef MAINWINDOW_H 
#define MAINWINDOW_H 

#include <QMainWindow> 
#include <QStringList> 
#include <QGLWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QMenu>
#include <QDockWidget>
#include <QApplication>
#include <QDialog>
#include <QThread>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>

#include "defines.h"
#include "rooms.h"

#include "ui_roomedit.h"
#include "ui_configedit.h"

class QAction; 
class QLabel; 
class FindDialog; 
class Spreadsheet; 
class QLineEdit;
class QLabel;
class QWidget;
class QPushButton;

int renderer_main();


//extern int texture_visibilit_range;
//extern int details_visibility_range;

class RoomEditDialog : public QDialog, public Ui::roomedit_dialog {
Q_OBJECT

    /* context for exit functions; */
    QLineEdit *door;
    QLineEdit *leads;
    QComboBox *flags;
    QCheckBox *box;
    
    void set_door_context(int dir);
    void setup_exit_widgets(int dir, Troom *r);
    int updateExitsInfo(int dir, Troom *r);
public:
    RoomEditDialog(QWidget *parent = 0);
    QString parse_room_desc(char *);

    void  changedExitsFlag(int dir, int index);
    void  changedExitsState(int dir, bool state);

    void load_room_data(unsigned int id);
    void clear_data();
public slots:
    virtual void accept();
    virtual void reject();
    void changedExitsFlagN(int index) {changedExitsFlag(NORTH, index); }
    void changedExitsFlagS(int index) {changedExitsFlag(SOUTH, index); }
    void changedExitsFlagE(int index) {changedExitsFlag(EAST, index); }
    void changedExitsFlagW(int index) {changedExitsFlag(WEST, index); }
    void changedExitsFlagU(int index) {changedExitsFlag(UP, index); }
    void changedExitsFlagD(int index) {changedExitsFlag(DOWN, index); }
    
    void changedExitsStateN(bool state) { changedExitsState(NORTH, state); }
    void changedExitsStateS(bool state) { changedExitsState(SOUTH, state); }
    void changedExitsStateE(bool state) { changedExitsState(EAST, state); }
    void changedExitsStateW(bool state) { changedExitsState(WEST, state); }
    void changedExitsStateU(bool state) { changedExitsState(UP, state); }
    void changedExitsStateD(bool state) { changedExitsState(DOWN, state); }

};

class ConfigWidget : public QDialog, public Ui::ConfigDialog {
Q_OBJECT
public:
    ConfigWidget (QWidget *parent = 0);
    
    void run();
public slots:
    virtual void accept();
    void autorefreshUpdated(bool);
};

class RendererWidget : public QGLWidget
{
  Q_OBJECT
public:
  GLfloat       angley;
  GLfloat       anglex;
  GLfloat       anglez;
  float         userx;
  float         usery;
  float         userz;		/* additional shift added by user */

  int           current_plane_z;        /* to avoid arguments usage */
  GLuint        basic_gllist;


    
  int           rooms_drawn_csquare;
  int           rooms_drawn_total;
  int           square_frustum_checks;

  RendererWidget( QWidget *parent, const char *name=0 );

  void display(void);

  void draw(void);
  void CalculateFrustum();
  


protected:
  void initializeGL();
  void resizeGL( int width, int height );
  void paintGL();

private:
  GLfloat       colour[4];
  float         m_Frustum[6][4];
  GLuint        global_list;
  int           curx;
  int           cury;
  int           curz;			/* current rooms position */ 
  


  void glDrawMarkers();
  void glDrawRoom(struct Troom *p);
  
  void glDrawCSquare(CSquare *p);
  bool PointInFrustum(float x, float y, float z);
  bool SquareInFrustum(CSquare *p);
  
};


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
  
  QAction       *setupNetAct;
  QAction       *setupOpenGLAct;
  QAction       *setupGeneralAct;
  
  RoomEditDialog    edit_dialog;
  ConfigWidget      analyser_dialog;


  bool          LeftButtonPressed;
  bool          RightButtonPressed;
  QPoint        old_pos;

  void edit_room(unsigned int id);

public:
    MainWindow(QWidget *parent, const char *name = 0);

    RendererWidget *renderer;

    void update_status_bar();
    QAction       *hide_status_action;
    QAction       *hide_menu_action;
    
    void disable_online_actions();
    void enable_online_actions();

    
private slots:
  void hide_menu();
  void hide_status();
  void always_on_top();
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
  
  void edit_current_room();

  void mousePressEvent( QMouseEvent *);
  void mouseReleaseEvent( QMouseEvent *);
  void mouseMoveEvent( QMouseEvent *);
  void wheelEvent(QWheelEvent *);


  void keyPressEvent( QKeyEvent * );
  void hide();                              /* hide all extra widgets */
};


extern class MainWindow *renderer_window;

#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
