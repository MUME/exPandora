
#include <QByteArray>
#include <QMutex>
#include <vector>


#include "PatternEditDialog.h"
#include "event.h"
         
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

    if (isVisible())
        return;

    patterns.clear();
    frame->setEnabled(false);
    editing_index = 0;
    
    for (i = 0; i < conf.patterns.size(); i++) 
        patterns.push_back( conf.patterns[i] ); 
    
    for (i = 0; i < Events.size(); i++)
        comboBox_type->addItem(Events[i].data);

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
    comboBox_type->setCurrentIndex(patterns[editing_index].event.type);
    lineEdit_data->setText(patterns[editing_index].event.data);
}

void PatternEditDialog::redraw()
{
    unsigned int i;
    QString s;
    QString tmp, tmp2;
    
    listWidget->clear();
    for (i = 0; i < patterns.size(); i++) {
        tmp = Events[patterns[i].event.type].data;
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
    patterns[editing_index].event.type = comboBox_type->currentIndex();
    patterns[editing_index].event.data = lineEdit_data->text().toAscii();
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
        conf.add_pattern(patterns[i].pattern, patterns[i].event.data, 
                              Events[patterns[i].event.type].group, 
                            patterns[i].event.type, patterns[i].is_regexp);                        
    }
    patterns.clear();        
    done(Accepted);
}
