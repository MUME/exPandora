#include "Configuration.h"
#include <iostream>
#include <qvariant.h>
#include <qapplication.h>

/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
#ifndef MONOLITHIC
extern "C" MY_EXPORT Configuration * createComponent()
{
  return new Configuration;
}
#else
Initializer<Configuration> configuration("Configuration");
#endif

using namespace Qt;


Configuration::Configuration() : currentComponent( this ) {}

void Configuration::init()
{
  QFile xmlFile( options["fileName"].toString() );
  QXmlInputSource source( &xmlFile );

  QXmlSimpleReader reader;
  reader.setContentHandler( this );
  reader.parse( source );
}

Component * Configuration::get
  ( const QString & id )
{
  return components[id];
}


void Configuration::put( const QString & id, Component * component )
{
  components[id] = component;
}



bool Configuration::startElement( const QString& , const QString& ,
                                  const QString& qName,
                                  const QXmlAttributes& attributes )
{
  if ( qName == "component" )
    newComponent( attributes );
  else if ( qName == "option" )
    addOption( attributes );
  else if ( qName == "connection" )
    connectComponents( attributes );
  else if ( qName == "configuration" )
    return true;
  else
    return false;
  return true;
}


bool Configuration::endElement ( const QString &, const QString &,
                                 const QString & qName )
{
  if (qName == "component")
    currentComponent->start();
  return true;
}

void Configuration::newComponent( const QXmlAttributes & atts )
{
  const QString type = atts.value("type");
  const QString id = atts.value( "id" );
  const QString file = atts.value( "file" );

  if (type.isEmpty())
  {
    if (file.isEmpty())
      throw "no source given for component";
    QLibrary * lib;

    if ( libs.find( file ) == libs.end() )
    {
      lib = new QLibrary( file );
      libs[file] = lib;
    }
    else
      lib = libs[file];

    componentCreator creator = ( componentCreator ) lib->resolve( "createComponent" );
    if ( creator == 0 )
    {
      if ( lib->isLoaded() )
        cerr << "library loaded but creator not found: " << lib->fileName().toStdString() << "\n";
      else
        cerr << "library can't be loaded: " << lib->fileName().toStdString() << "\n";
      throw "Component couldn't be loaded";
    }
    currentComponent = creator();
  }
  else {
    currentComponent = ComponentCreator::creators()[type]->create();
    if (!currentComponent) 
      throw "invalid creator";
  }
  components[id] = currentComponent;
}

void Configuration::addOption( const QXmlAttributes & atts )
{
  QString name = atts.value( "name" );
  QVariant value = atts.value( "value" );
  currentComponent->setOption( name, value );
}

void Configuration::connectComponents( const QXmlAttributes & atts )
{

  Component * from = components[atts.value( "from" )];

  const QString sig = signal(atts.value( "signal" ));

  Component * to = components[atts.value( "to" )];

  const QString sl = slot(atts.value( "slot" ));

  ConnectionType requiredSlot = to->requiredConnectionType(sl);
  ConnectionType requiredSignal = from->requiredConnectionType(sig);

  ConnectionType resultType;
  if (requiredSlot == requiredSignal || requiredSignal == AutoCompatConnection)
    resultType = requiredSlot;
  else if (requiredSlot == AutoCompatConnection)
    resultType = requiredSignal;
  else throw "can't connect non-matching slot and signal";

  QObject::connect(from, sig.toLatin1(), to, sl.toLatin1(), resultType);

}


/**
 * an ugyl hack; let's hope qt itself will contain function replacements 
 * for the SIGNAL and SLOT macros in the future
 * @param name name of the signal
 * @return mangled name of the signal
 */
QString Configuration::signal(const QString & name )
{
  return QString(name).prepend( "2" );
}

QString Configuration::slot(const QString & name )
{
  return QString(name).prepend( "1" );
}
