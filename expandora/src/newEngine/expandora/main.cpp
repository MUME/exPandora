#include "Component.h"
#include <iostream>
using std::cout;
#include <qfiledialog.h>
#include <qapplication.h>
#include <qlibrary.h>
#include <qvariant.h>
#include <qobject.h>

int main ( int argc, char *argv[] ) {
	QApplication a( argc, argv );
	QString s;
	if ( argc == 1 ) {
		s = QFileDialog::getOpenFileName(
		        QString::null,
		        "Configuration (*.xml)",
		        0,
		        "open file dialog",
		        "Choose a Configuration" );
		} else
		s = argv[ 1 ];

	if ( s ) {
		a.addLibraryPath( a.applicationDirPath() );
		componentCreator creator = ( componentCreator ) QLibrary::resolve( "Configuration", "createComponent" );
		Component * config = creator();
		config->setProperty("fileName", s );
		config->start();
		a.exec();
		} else {
		cout << "no file\n";
		}
	}

