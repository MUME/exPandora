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
 Q_PROPERTY( QString fileName READ getFileName WRITE setFileName ) 
 
 public:
  Configuration();
  QString getFileName() const {return fileName;}
  void setFileName(QString & name) {fileName = name;}

    //void parseFile(QString & filename);
  Component * get(QString & id);
  void put(QString & id, Component * component);
  
  void start(QThread::Priority = QThread::InheritPriority);


  //bool characters(const QString& ch);
  bool startElement( const QString&, const QString&, const QString& ,
		     const QXmlAttributes& );
  bool endElement( const QString&, const QString&, const QString& );
 private:
  Q_OBJECT

  QString & signal(QString &);
  QString & slot(QString &);
	void setPath(const QXmlAttributes & atts);
  void newComponent(const QXmlAttributes & atts);
  void addOption(const QXmlAttributes & atts);
  void connectComponents(const QXmlAttributes & atts);
  map<QString, Component *> components;  
  Component * currentComponent;
  map<QString, QLibrary *> libs;
  QString fileName;
};



#endif
