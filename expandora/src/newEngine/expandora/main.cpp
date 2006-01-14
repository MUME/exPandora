#include <iostream>
#include <qfiledialog.h>
#include <qapplication.h>
#include <qlibrary.h>
#include <qvariant.h>
#include <qobject.h>
#include <qstringlist.h>
#include <QGLWidget>
#include "Component.h"
#ifdef DMALLOC
#include <mpatrol.h>
#endif
using namespace std;

int main ( int argc, char *argv[] )
{

  try
  {
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
      Component * config;
      if (ComponentCreator::creators().find("Configuration") != ComponentCreator::creators().end()) {
	ComponentCreator * creator = (ComponentCreator::creators())["Configuration"];
	config = creator->create();
      }
      else
      {
        QLibrary lib( "Configuration" );
        componentCreator creator = ( componentCreator ) lib.resolve( "createComponent" );
        if ( creator == 0 )
        {
          if ( lib.isLoaded() )
            std::cout << "library loaded but creator not found: " << lib.fileName().toStdString() << "\n";
          else
            std::cout << "library can't be loaded: " << lib.fileName().toStdString() << "\n";
          exit(-1) ;
        }
        config = creator();
      }
      QString f("fileName");
      QVariant v(s);
      config->setOption(f, v );
      config->start();
      a.setQuitOnLastWindowClosed (true);
      a.exec();
    }
    else
    {
      cerr << "no file\n";
    }
  }
  catch (char const * error)
  {
    cerr << error << endl;
    exit(-1);
  }
}

