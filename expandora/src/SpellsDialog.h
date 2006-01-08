#ifndef SPELLSDIALOG_H 
#define SPELLSDITDIALOG_H 

#include <QDialog>
#include "ui_spellsdialog.h"
#include "configurator.h"

class SpellsDialog : public QDialog, public Ui::SpellsDialog {
Q_OBJECT

    int editing_index;
    
    vector<TSpell> spells;
    void enableFrame();
    void disableFrame();
public:
    SpellsDialog(QWidget *parent = 0);
    void run();
    void redraw();
public slots:
    void edit_clicked();
    void save_clicked();
    void add_clicked();
    void remove_clicked();
    virtual void accept();
};


#endif
