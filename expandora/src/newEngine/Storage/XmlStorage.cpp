#include "XmlStorage.h"
#include <iostream>

using namespace std;
/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
#ifndef MONOLITHIC
extern "C" MY_EXPORT Component * createComponent()
{
  return new XmlStorage;
}
#else
Initializer<XmlStorage> xmlStorage("Storage");
#endif


void XmlStorage::loadFromFile(QString fileName)
{

  map<QString, Coordinate> myMoves;
  myMoves["n"] = Coordinate(0,1,0);
  myMoves["s"] = Coordinate(0,-1,0);
  myMoves["w"] = Coordinate(-1,0,0);
  myMoves["e"] = Coordinate(1,0,0);
  myMoves["u"] = Coordinate(0,0,1);
  myMoves["d"] = Coordinate(0,0,-1);

  Coordinate::insertMoves(myMoves);
  
  QFile xmlFile(fileName);
  //QFile xmlFile( options["fileName"].toString() );
  QXmlInputSource source( &xmlFile );

  QXmlSimpleReader reader;


  StructureParser handler;
  reader.setContentHandler( &handler );

  connect( &handler, SIGNAL( addExit( int, int, uint ) ), this, SIGNAL( addExit( int, int, uint ) ) );
  connect( &handler, SIGNAL( addRoom( ParseEvent*, Coordinate, char, int ) ), this, SIGNAL( addRoom( ParseEvent*, Coordinate, char, int ) ) );




  reader.parse( source );

  //pop all the exits and addExit(...) them into the rooms
  handler.addExits();


  return ;
}


void StructureParser::addExits()
{
  

  
  Exit * e = 0;
  while ( !( exits.empty() ) )
  {
    e = exits.top();
    exits.pop();
    emit addExit( e->sourceId, e->destId, e->dir );
  }
}


/**
 * properties are separated by "|" in the xml file.
 * @param desc 
 */
void StructureParser::buildProperties( char * desc )
{
  char * lastLineBegin = desc;
  for ( int i = 0; desc[ i ] != 0; i++ )
  {
    if ( desc[ i ] == '|' )
    {
      desc[ i ] = 0;
      prop->add
      ( lastLineBegin );
      roomProps->push( prop );
      prop = pmm.activate();
      lastLineBegin = desc + i + 1;
    }
  }
}



StructureParser::StructureParser()
    : QXmlDefaultHandler(),
    roomProps( 0 ),
    prop( 0 ),
    ts( 0 ),
    t( 0 ),
    id( 0 )
{}


bool StructureParser::endElement( const QString& , const QString& , const QString& qName )
{

  if ( qName == "room" )
  {
    prop = pmm.activate();
    prop->add
    ( t );
    roomProps->push( prop );
    emit addRoom( roomProps, c, t, id );
    
    pemm.deactivate( roomProps );
    roomProps = 0;
    c.clear();
    id = 0;


  }
  flag = 0;

  return true;
}

bool StructureParser::characters( const QString& ch )
{
  strncpy( data,  ch.toLatin1(), ch.length() );
  data[ ch.length() ] = 0;

  prop = pmm.activate();
  if ( flag != XML_DESC )
    prop->add
    ( data );
  else
    buildProperties( data );
  if ( roomProps != 0 && flag != 0 )
  {
    if ( flag == XML_NOTE )
      roomProps->pushOptional( prop );
    else if ( flag == XML_ROOMNAME )
      roomProps->push( prop );
  }
  else
  {
    pmm.deactivate( prop );
    prop = 0;
  }
  return true;

}


bool StructureParser::startElement( const QString& , const QString& ,
                                    const QString& qName,
                                    const QXmlAttributes& attributes )
{


  /*    if (qName == "map") {
  }
  */
  if ( qName == "roomname" )
  {
    flag = XML_ROOMNAME;
    return true;
  }
  if ( qName == "desc" )
  {
    flag = XML_DESC;
    return true;
  }
  if ( qName == "note" )
  {
    flag = XML_NOTE;
    return true;
  }

  if ( qName == "room" )
  {

    roomProps = pemm.activate();
  }


  if ( qName == "exit" )
  {
    unsigned int to;

    /* special */
    if ( attributes.length() < 3 )
    {
      throw "Not enough exit attributes in XML file!" ;
    }

    s = attributes.value( "dir" );
    strncpy( data,  s.toLatin1(), s.length() );
    data[ s.length() ] = 0;
    char d = data[0];


    s = attributes.value( "to" );
    strncpy( data, s.toLatin1(), s.length() );
    data[ s.length() ] = 0;

    to = 0;
    if ( !strcmp( data, "UNDEFINED" ) )
      to = INT_MAX;
    else
      to = atoi( ( const char * ) data );

    s = attributes.value( "door" );
    strncpy( data, s.toLatin1(), s.length() );
    data[ s.length() ] = 0;


    /* if there is a door, then the exit (represented by d) is an optional property else it is a required property */
    prop = pmm.activate();
    prop->add
    ( d );
    if ( data[0] == 0 )
    {
      roomProps->push( prop );
    }
    else
    {
      roomProps->pushOptional( prop );
    }
    if ( to != INT_MAX )
    {
      Exit * e = new Exit;
      e->sourceId = id;
      e->dir = Coordinate::moveCodes[QChar(d)];
      e->destId = to;
      exits.push( e );
    }


    if ( s.length() != 0 )
    {

      /* all doors are optional properties */
      prop = pmm.activate();
      prop->add
      ( data );
      roomProps->pushOptional( prop );
    }


  }

  if ( attributes.length() > 0 )
  {
    for ( int i = 0 ; i < attributes.length(); i++ )
    {
      s = attributes.value( i );
      //strcpy(data, (const char *)s);
      strncpy( data, s.toLatin1(), s.length() );
      data[ s.length() ] = 0;

      if ( attributes.qName( i ) == "id" )
      {
        id = atoi( data );
        continue;
      }
      if ( attributes.qName( i ) == "x" )
      {
        c.x = atoi( data ) / 2;
        continue;
      }
      if ( attributes.qName( i ) == "y" )
      {
        c.y = atoi( data ) / 2;
        continue;
      }
      if ( attributes.qName( i ) == "z" )
      {
        c.z = atoi( data ) / 2;
        continue;
      }

      if ( attributes.qName( i ) == "terrain" )
      {
	QLatin1String terrainString(data);
        t = terrainFromString(terrainString);
        continue;
      }
      if ( attributes.qName( i ) == "timestamp" )
      {
        ts = 0;
        //ts = timeFromString(s);
        continue;
      }


    }
  }

  return true;
}


char StructureParser::terrainFromString(QLatin1String & terrain) {
  if (terrain == "SHALLOWWATER")
    return '%';
  else if (terrain == "INDOORS")
    return '[';
  else if (terrain == "FIELD")
    return '.';
  else if (terrain == "HILLS")
    return '(';
  else if (terrain == "FOREST")
    return 'f';
  else if (terrain == "ROAD")
    return '+';
  else if (terrain == "CITY")
    return '#';
  else if (terrain == "MOUNTAINS")
    return '<';
  else if (terrain == "WATER")
    return '~';
  else if (terrain == "RAPIDS")
    return 'W';
  else if (terrain == "BRUSH")
    return ':';
  // "UNDERWATER", "TUNNEL" and "CAVE" don't occur ... ??
  else return 0;
  

}



