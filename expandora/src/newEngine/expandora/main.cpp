#include "Configuration.h"
#include <iostream>
#include <qfiledialog.h>
#include <qapplication.h>

int main (int argc, char *argv[]) {
  QApplication a( argc, argv );
  QString s;
  if (argc == 1) {
    s = QFileDialog::getOpenFileName(
				     QString::null,
				     "Configuration (*.xml)",
				     0,
				     "open file dialog",
				     "Choose a Configuration" );
  }
  else s = argv[1];
  
  if (s) {
    Configuration config;
    config.setFileName(s);
    config.start();
  }
  else {
    cout << "no file\n";
  }
}

