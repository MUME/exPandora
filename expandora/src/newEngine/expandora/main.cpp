#include "Component.h"
#include <iostream>

#include <qfiledialog.h>
#include <qapplication.h>
#include <qlibrary.h>
#include <qvariant.h>
#include <qobject.h>
#include <qstringlist.h>

using namespace std;

int main ( int argc, char *argv[] )
{
  try {
  //QApplication::setColorSpec( QApplication::CustomColor );
  QApplication a( argc, argv );
  a.setQuitOnLastWindowClosed (false);
  QString s;
  if ( argc == 1 )
  {
    s = QFileDialog::getOpenFileName(
          0,
          "Choose a configuration",
          QString(),
          "Configuration (*.xml)");
  }
  else
  {
    s = argv[ 1 ];
  }


  if ( !(s.isNull()) )
  {
    QLibrary lib( "Configuration" );
    componentCreator creator = ( componentCreator ) lib.resolve( "createComponent" );
	if ( creator == 0 ) {
		if ( lib.isLoaded() )
			std::cout << "library loaded but creator not found: " << lib.fileName().toStdString() << "\n";
		else
			std::cout << "library can't be loaded: " << lib.fileName().toStdString() << "\n";
		exit(-1) ;
	}
    //componentCreator creator = ( componentCreator ) QLibrary::resolve( QString("Configuration"), "createComponent" );
    Component * config = creator();
    if (!config->setProperty("fileName", s ))
    	throw "couldn't set fileName for configuration";
    config->start();
    a.setQuitOnLastWindowClosed (true);
    a.exec();
  }
  else
  {
    cerr << "no file\n";
  }
  } catch (char const * error) {
    cerr << error << endl;
    exit(-1);
  }
}

