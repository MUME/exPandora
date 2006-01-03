#ifndef ROOMEDITDIALOG_H 
#define ROOMEDITDIALOG_H 

#include <QDialog>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QString>

#include "defines.h"
#include "CRoom.h"
#include "ui_roomedit.h"


class RoomEditDialog : public QDialog, public Ui::roomedit_dialog {
Q_OBJECT

    /* context for exit functions; */
    QLineEdit *door;
    QLineEdit *leads;
    QComboBox *flags;
    QCheckBox *box;
    
    void set_door_context(int dir);
    void setup_exit_widgets(int dir, CRoom *r);
    int updateExitsInfo(int dir, CRoom *r);
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

#endif
