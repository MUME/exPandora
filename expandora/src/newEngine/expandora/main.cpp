#include "Component.h"
#include <iostream>
using std::cerr;
#include <qfiledialog.h>
#include <qapplication.h>
#include <qlibrary.h>
#include <qvariant.h>
#include <qobject.h>
#include <qstringlist.h>

int main ( int argc, char *argv[] ) {
	QApplication::setColorSpec( QApplication::CustomColor );
	QApplication a( argc, argv );
	QString s;
	if ( argc == 1 ) {
		s = QFileDialog::getOpenFileName(
				0,
				"Choose a configuration",
		        	QString(),
		        	"Configuration (*.xml)");
		} 
	else
		s = argv[ 1 ];

	if ( !(s.isNull()) ) {
		componentCreator creator = ( componentCreator ) QLibrary::resolve( "Configuration", "createComponent" );
		Component * config = creator();
		config->setProperty("fileName", s );
		config->start();
		a.exec();
		} else {
		cerr << "no file\n";
		}
	}

