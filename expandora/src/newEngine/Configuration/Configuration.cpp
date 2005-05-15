#include "Configuration.h"
#include <utility>
#include <qvariant.h>

/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
extern "C" MY_EXPORT Configuration * createComponent() {
  return new Configuration;
}

Configuration::Configuration()  : currentComponent(this) {
  put(*(new QString("configuration")), this);
}

void Configuration::parseFile(QString & filename) {
  QFile xmlFile( filename);
  QXmlInputSource source( &xmlFile );

  QXmlSimpleReader reader;
  reader.setContentHandler(this);
  reader.parse( source );
}

Component * Configuration::get(QString & id) {
  return (*components.find(id)).second;
}


void Configuration::put(QString & id, Component * component) {
  pair<QString , Component *> val;
  val.first = id;
  val.second = component;
  components.insert(val);
}



bool Configuration::startElement( const QString& , const QString& , 
                                    const QString& qName, 
                                    const QXmlAttributes& attributes)
{
  if (qName == "component") newComponent(attributes);
  else if (qName == "option") addOption(attributes);
  else if (qName == "connection") connectComponents(attributes);
  else return false;
  return true;
}

void Configuration::newComponent(const QXmlAttributes & atts) {
  QString id = atts.value("id");
  QString file = atts.value("file");
  QLibrary * lib;
  
  if (libs.find(file) == libs.end()) {
    lib = new QLibrary(file);
    libs.insert(make_pair(file, lib));
  }
  else lib = (*libs.find(file)).second;
  
  componentCreator creator = (componentCreator) lib->resolve("createComponent");
  currentComponent = creator();
  put(id, currentComponent);
}

void Configuration::addOption(const QXmlAttributes & atts) {
  QString  name = atts.value("name");
  QString  value = atts.value("value");
  currentComponent->setProperty((const char*)name, *(new QVariant(value)));
}

void Configuration::connectComponents(const QXmlAttributes & atts) {
  QString from = atts.value("from");
  QString signal = atts.value("signal");
  QString to = atts.value("to");
  QString slot = atts.value("slot");
  
  get(to)->connect(get(from), signal, slot);
}

