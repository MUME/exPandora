#include "Configuration.h"
#include <iostream>
#include <qvariant.h>
#include <qapplication.h>

/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
extern "C" MY_EXPORT Component * createComponent() {
	return new Configuration;
}

Configuration::Configuration() : currentComponent( this ) {
	put( *( new QString( "configuration" ) ), this );
}

void Configuration::start( QThread::Priority ) {
	QFile xmlFile( property( "fileName" ).toString() );
	QXmlInputSource source( &xmlFile );

	QXmlSimpleReader reader;
	reader.setContentHandler( this );
	reader.parse( source );
}

Component * Configuration::get
	( QString & id ) {
	return ( *components.find( id ) ).second;
}


void Configuration::put( QString & id, Component * component ) {
	pair<QString , Component *> val;
	val.first = id;
	val.second = component;
	components.insert( val );
}



bool Configuration::startElement( const QString& , const QString& ,
                                  const QString& qName,
                                  const QXmlAttributes& attributes ) {
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
                                 const QString & qName ) {
	if ( qName == "component" )
		currentComponent->start();
	return true;
}

void Configuration::newComponent( const QXmlAttributes & atts ) {
	QString id = atts.value( "id" );
	QString file = atts.value( "file" );
	QLibrary * lib;

	if ( libs.find( file ) == libs.end() ) {
		lib = new QLibrary( file );
		libs.insert( make_pair( file, lib ) );
	} else
		lib = ( *libs.find( file ) ).second;

	componentCreator creator = ( componentCreator ) lib->resolve( "createComponent" );
	if ( creator == 0 ) {
		if ( lib->isLoaded() )
			cout << "library loaded but creator not found: " << lib->library() << "\n";
		else
			cout << "library can't be loaded: " << lib->library() << "\n";
		exit(-1) ;
	}
	currentComponent = creator();
	put( id, currentComponent );
}

void Configuration::addOption( const QXmlAttributes & atts ) {
	QString name = atts.value( "name" );
	QString value = atts.value( "value" );
	currentComponent->setProperty( ( const char* ) name, value );
}

void Configuration::connectComponents( const QXmlAttributes & atts ) {
	QString from = atts.value( "from" );
	QString sig = atts.value( "signal" );
	QString to = atts.value( "to" );
	QString sl = atts.value( "slot" );


	get
		( to ) ->connect( get
			                  ( from ), signal( sig ), slot( sl ) );
}


/**
 * an ugyl hack; let's hope qt itself will contain function replacements 
 * for the SIGNAL and SLOT macros in the future
 * @param name name of the signal
 * @return mangled name of the signal
 */
QString & Configuration::signal( QString & name ) {
	return name.prepend( "2" );
}

QString & Configuration::slot( QString & name ) {
	return name.prepend( "1" );
}
