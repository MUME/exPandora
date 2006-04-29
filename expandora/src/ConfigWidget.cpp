#include <QMessageBox>

#include "configurator.h"
#include "ConfigWidget.h"

/* ----------------------- AnalyserConfigWidget -----------------------------*/
 
ConfigWidget::ConfigWidget (QWidget *parent) : QDialog(parent)
{
    setupUi(this);                        
    setWindowTitle(tr("General Settings"));
    
    connect(checkBox_autorefresh, SIGNAL(toggled(bool)), this, SLOT(autorefreshUpdated(bool)) );
    connect(checkBox_promptIAC, SIGNAL(toggled(bool)), this, SLOT(promptIACclicked(bool)) );
}


void ConfigWidget::run()
{

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

/*    checkBox_promptIAC->setChecked( conf.is_prompt_IAC() );
    checkBox_forwardIAC->setChecked( conf.is_forward_IAC() );
    promptIACclicked( conf.is_prompt_IAC() );
    checkBox_forwardPromptColour->setChecked( conf.is_forwardPromptColour() );
*/
    checkBox_promptIAC->setEnabled( false );
    checkBox_forwardIAC->setEnabled( false );
    checkBox_forwardPromptColour->setEnabled( false );     
        
    lineEdit_visrange->setText(QString("%1").arg(conf.get_texture_vis()) );
    lineEdit_detrange->setText(QString("%1").arg(conf.get_details_vis()) );
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

void ConfigWidget::promptIACclicked(bool state)
{
    if (state) {
        checkBox_forwardIAC->setEnabled(true);
    } else {
        checkBox_forwardIAC->setEnabled(false);
    }
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

