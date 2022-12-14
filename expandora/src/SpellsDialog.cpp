
#include <QByteArray>
#include <QMutex>
#include <vector>


#include "SpellsDialog.h"
         
/* --------------------------- SpellsDialog  ------------------------------ */

SpellsDialog::SpellsDialog (QWidget *parent) : QDialog(parent)
{
    setupUi(this);

    connect(pushButton_add, SIGNAL(clicked()), this, SLOT(add_clicked()) );
    connect(pushButton_save, SIGNAL(clicked()), this, SLOT(save_clicked()) );
    connect(pushButton_remove, SIGNAL(clicked()), this, SLOT(remove_clicked()) );
    connect(pushButton_edit, SIGNAL(clicked()), this, SLOT(edit_clicked()) );
}

void SpellsDialog::run()
{
    unsigned int    i;

    if (isVisible())
        return;

    spells.clear();
    disableFrame();
    editing_index = 0;
    
    for (i = 0; i < conf.spells.size(); i++) 
        spells.push_back( conf.spells[i] ); 
    
    redraw();
    show();
    raise();
    activateWindow();
}


void SpellsDialog::edit_clicked()
{
    editing_index = listWidget->currentRow();
    enableFrame();

    lineEdit_name->setText(spells[editing_index].name);
    lineEdit_up_mes->setText(spells[editing_index].up_mes);
    lineEdit_refresh_mes->setText(spells[editing_index].refresh_mes);
    lineEdit_down_mes->setText(spells[editing_index].down_mes);
    checkBox_addon->setChecked( spells[editing_index].addon );
}

void SpellsDialog::redraw()
{
    unsigned int i;
    QString s;
    QString tmp, tmp2;
    
    listWidget->clear();
    for (i = 0; i < spells.size(); i++) {
/*        tmp = Events[patterns[i].event.type].data;
        tmp2 = patterns[i].pattern;
        s = QString("%1 %2 %3")
            .arg( patterns[i].is_regexp ? "REGEXP" : "WILDCARD", -10)
            .arg( tmp, -15)
            .arg( tmp2, -90 );
*/
        listWidget->addItem(spells[i].name);   
    }
    listWidget->setCurrentRow(0);
    repaint();
}


void SpellsDialog::save_clicked()
{
    TSpell p;
    
    disableFrame();
    
    if (editing_index == -1) {
        editing_index = spells.size();
        p.up = false;
        spells.push_back(p);
    }
    

    spells[editing_index].name = lineEdit_name->text().toAscii();
    spells[editing_index].up_mes = lineEdit_up_mes->text().toAscii();
    spells[editing_index].refresh_mes = lineEdit_refresh_mes->text().toAscii();
    spells[editing_index].down_mes = lineEdit_down_mes->text().toAscii();
    spells[editing_index].addon =  checkBox_addon->isChecked();
    
    redraw();
}

void SpellsDialog::add_clicked()
{
    if (frame->isEnabled()) {
        editing_index = -1;    
        disableFrame();
        save_clicked();
    } else {
        enableFrame();
        editing_index = -1;
        
        lineEdit_name->setText("");
        lineEdit_up_mes->setText("");
        lineEdit_refresh_mes->setText("");
        lineEdit_down_mes->setText("");
        checkBox_addon->setChecked( false );

    }
}

void SpellsDialog::remove_clicked()
{
    vector<TSpell>::iterator p;
    int i;
    
    i = 0;
    for (p=spells.begin(); p != spells.end(); ++p) {
        if (i == listWidget->currentRow()) {
            p = spells.erase(p);
            break;
        }
        i++;
    }
    redraw();
}


void SpellsDialog::accept()
{
    unsigned int i;
    
    conf.spells.clear();
    for (i = 0; i < spells.size(); i++) {
        conf.add_spell(spells[i]);
    }
    spells.clear();        
    done(Accepted);
}


void SpellsDialog::enableFrame()
{
    frame->setEnabled(true);
    pushButton_remove->setEnabled(false);
}

void SpellsDialog::disableFrame()
{    
    frame->setEnabled(false);
    pushButton_remove->setEnabled(true);
}

