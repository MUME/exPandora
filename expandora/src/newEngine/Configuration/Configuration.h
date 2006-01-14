#ifndef CONFIGURATION
#define CONFIGURATION

#include "Component.h"
#include <qxml.h>
#include <qstring.h>
#include <map>
#include <qlibrary.h>
#include <qobject.h>
#include <qvariant.h>

using namespace std;



class Configuration : public Component, public QXmlDefaultHandler {
 
 public:
  Configuration();

  Component * get(const QString & id);
  void put(const QString & id, Component * component);
  
  virtual void init();


  //bool characters(const QString& ch);
  bool startElement( const QString&, const QString&, const QString& ,
		     const QXmlAttributes& );
  bool endElement( const QString&, const QString&, const QString& );
 private:
  Q_OBJECT

  QString signal(const QString &);
  QString slot(const QString &);
  void newComponent(const QXmlAttributes & atts);
  void addOption(const QXmlAttributes & atts);
  void connectComponents(const QXmlAttributes & atts);
  map<QString, Component *> components;  
  Component * currentComponent;
  map<QString, QLibrary *> libs;
};

#ifdef DMALLOC
#include <mpatrol.h>
#endif

#endif
