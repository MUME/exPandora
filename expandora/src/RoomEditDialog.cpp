#include <QMessageBox>

#include "Map.h"
#include "configurator.h"
#include "RoomEditDialog.h"

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

int RoomEditDialog::updateExitsInfo(int dir, CRoom *r)
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


void RoomEditDialog::setup_exit_widgets(int dir, CRoom *r)
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
    CRoom *r;
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
    CRoom *r;
    QString name, desc, note;
    int x, y, z;
    int id;
    char terrain;
    unsigned int i;

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
    for (i=0; i< conf.sectors.size(); i++)
        if (comboBox_terrain->currentText() == conf.sectors[i].desc) {
            r->sector = i;
        }
    
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



