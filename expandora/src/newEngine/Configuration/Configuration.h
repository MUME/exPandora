#ifndef CONFIGURATION
#define CONFIGURATION

#include "Component.h"
#include <qxml.h>
#include <qstring.h>
#include <map>
#include <qlibrary.h>
#include <qobject.h>
using namespace std;



class Configuration : public QXmlDefaultHandler, public Component {
 public:
  Configuration();
  void parseFile(QString & filename);
  QObject * get(QString & id);
  void put(QString & id, QObject * component);


  //bool characters(const QString& ch);
  bool startElement( const QString&, const QString&, const QString& ,
		     const QXmlAttributes& );
  //bool endElement( const QString&, const QString&, const QString& );
 private:

  void newComponent(const QXmlAttributes & atts);
  void addOption(const QXmlAttributes & atts);
  void connectComponents(const QXmlAttributes & atts);
  map<QString, QObject *> components;  
  Component * currentComponent;
  map<QString, QLibrary *> libs;
};

typedef Component * (*componentCreator)();

#endif
