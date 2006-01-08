#ifndef PATTERNEDITDIALOG_H 
#define PATTERNEDITDIALOG_H 

#include <QDialog>
#include "ui_patterndialog.h"
#include "configurator.h"

class PatternEditDialog : public QDialog, public Ui::PatternDialog {
Q_OBJECT

    int editing_index;
    
    vector<TPattern> patterns;
    void enableFrame();
    void disableFrame();
    
public:
    PatternEditDialog(QWidget *parent = 0);
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
