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
    //void parseFile(QString & filename);
  Component * get(QString & id);
  void put(QString & id, Component * component);
  
  void start(QThread::Priority = QThread::InheritPriority);


  //bool characters(const QString& ch);
  bool startElement( const QString&, const QString&, const QString& ,
		     const QXmlAttributes& );
  //bool endElement( const QString&, const QString&, const QString& );
 private:
  Q_OBJECT
  Q_PROPERTY( QString fileName READ getFileName WRITE setFileName ) 

  QString getFileName() const {return property("fileName").toString();}
  void setFileName(QString & name) {/*setProperty("fileName", name);*/}



  QString & signal(QString &);
  QString & slot(QString &);
  void newComponent(const QXmlAttributes & atts);
  void addOption(const QXmlAttributes & atts);
  void connectComponents(const QXmlAttributes & atts);
  map<QString, Component *> components;  
  Component * currentComponent;
  map<QString, QLibrary *> libs;
};

typedef Component * (*componentCreator)();

#endif
