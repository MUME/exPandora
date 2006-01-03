#include "XmlStorage.h"
#include <iostream>


/**
 * this method is called when a component of this type should be
 * created from a library. MY_EXPORT is defined in Component.h
 * and handles platform specific issues
 */
extern "C" MY_EXPORT Component * createComponent() {
    return new XmlStorage;
}



void XmlStorage::xml_readbase() {
    ioMutex.lock();
    QFile xmlFile( fileName );
    QXmlInputSource source( &xmlFile );

    QXmlSimpleReader reader;


    StructureParser handler;
    reader.setContentHandler( &handler );

    connect( &handler, SIGNAL( addExit( int, int, int ) ), this, SIGNAL( addExit( int, int, int ) ) );
    connect( &handler, SIGNAL( addRoom( ParseEvent*, int, Coordinate*, char ) ), this, SIGNAL( addRoom( ParseEvent*, int, Coordinate*, char ) ) );


    printf( "reading xml ...\n" );

    reader.parse( source );

    //pop all the exits and addExit(...) them into the rooms
    handler.addExits();

    ioMutex.unlock();
    return ;
}


void StructureParser::addExits() {
    Exit * e = 0;
    while ( !( exits.empty() ) ) {
        e = exits.top();
        exits.top();
        emit addExit( e->sourceId, e->destId, e->dir );
    }
}


/**
 * properties are separated by "|" in the xml file.
 * @param desc 
 */
void StructureParser::buildProperties( char * desc ) {
    char * lastLineBegin = desc;
    for ( int i = 0; desc[ i ] != 0; i++ ) {
        if ( desc[ i ] == '|' ) {
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
        c( cmm.activate() ),
        ts( 0 ),
        t( 0 ),
id( 0 ) {
    c->clear();
}


bool StructureParser::endElement( const QString& , const QString& , const QString& qName ) {

    if ( qName == "room" ) {
#ifdef DEBUG
        fprintf( stderr, "inserting room: %i\n", id );
#endif

        prop = pmm.activate();
        prop->add
        ( t );
        roomProps->push( prop );
        emit addRoom( roomProps, id, c, t );
        //Room * room = roomAdmin->insertRoom(roomProps, id, c, t);
        // roomCache needed
        //room->hold();

        pemm.deactivate( roomProps );
        roomProps = 0;
        c->clear();
        id = 0;


    }
    flag = 0;

    return TRUE;
}

bool StructureParser::characters( const QString& ch ) {
    strncpy( data,  ch.toAscii(), ch.length() );
    data[ ch.length() ] = 0;

#ifdef DEBUG

    fprintf( stderr, "parsing data: %s\n", data );
#endif

    prop = pmm.activate();
    if ( flag != XML_DESC )
        prop->add
        ( data );
    else
        buildProperties( data );
    if ( roomProps != 0 && flag != 0 ) {
        if ( flag == XML_NOTE )
            roomProps->pushOptional( prop );
        else if ( flag == XML_ROOMNAME )
            roomProps->push( prop );
    } else {
        pmm.deactivate( prop );
        prop = 0;
    }
    return TRUE;

}


bool StructureParser::startElement( const QString& , const QString& ,
                                    const QString& qName,
                                    const QXmlAttributes& attributes ) {


    /*    if (qName == "map") {
    }
    */
    if ( qName == "roomname" ) {
        flag = XML_ROOMNAME;
        return TRUE;
    }
    if ( qName == "desc" ) {
        flag = XML_DESC;
        return TRUE;
    }
    if ( qName == "note" ) {
        flag = XML_NOTE;
        return TRUE;
    }

    if ( qName == "room" ) {

        roomProps = pemm.activate();
    }


    if ( qName == "exit" ) {
        unsigned int to;

        /* special */
        if ( attributes.length() < 3 ) {
            printf( "Not enough exit attributes in XML file!" );
            exit( 1 );
        }

        s = attributes.value( "dir" );
        strncpy( data,  s.toAscii(), s.length() );
        data[ s.length() ] = 0;
        char d = data[ 0 ];


        s = attributes.value( "to" );
        strncpy( data, s.toAscii(), s.length() );
        data[ s.length() ] = 0;

        to = 0;
        if ( !strcmp( data, "UNDEFINED" ) )
            to = INT_MAX;
        else
            to = atoi( ( const char * ) data );

        s = attributes.value( "door" );
        strncpy( data, s.toAscii(), s.length() );
        data[ s.length() ] = 0;


        /* if there is a door, then the exit (represented by d) is an optional property else it is a required property */
        prop = pmm.activate();
        prop->add
        ( d );
        if ( data[0] == 0 ) {
            roomProps->push( prop );
        } else {
            roomProps->pushOptional( prop );
        }
        if ( to != INT_MAX ) {
            Exit * e = new Exit;
            e->sourceId = id;
            e->dir = d;
            e->destId = to;
            exits.push( e );
        }


        if ( s.length() != 0 ) {

            /* all doors are optional properties */
            prop = pmm.activate();
            prop->add
            ( data );
            roomProps->pushOptional( prop );
        }


    }

    if ( attributes.length() > 0 ) {
        for ( int i = 0 ; i < attributes.length(); i++ ) {
            s = attributes.value( i );
            //strcpy(data, (const char *)s);
            strncpy( data, s.toAscii(), s.length() );
            data[ s.length() ] = 0;

            if ( attributes.qName( i ) == "id" ) {
                id = atoi( data );
                continue;
            }
            if ( attributes.qName( i ) == "x" ) {
                c->x = atoi( data ) / 2;
                continue;
            }
            if ( attributes.qName( i ) == "y" ) {
                c->y = atoi( data ) / 2;
                continue;
            }
            if ( attributes.qName( i ) == "z" ) {
                c->z = atoi( data ) / 2;
                continue;
            }

            if ( attributes.qName( i ) == "terrain" ) {
                t = data[ 0 ];
                continue;
            }
            if ( attributes.qName( i ) == "timestamp" ) {
                ts = 0;
                //ts = timeFromString(s);
                continue;
            }


        }
    }

    return TRUE;
}


/**
 * a really ugly hack to get the timestamp out of the most stupid format string
 * why don't we use one of QT's QDateFormat strings?
 * I don't bother to calculate the hours, minutes and seconds as I'm wrong with 365.25 and that matters much more (one day atm?)
 */
double StructureParser::timeFromString( QString & s ) {
    //"dd.MM.yyyy - hh:mm:ss"
    s[ 2 ] = 0;
    s[ 5 ] = 0;
    s[ 10 ] = 0;
    double ret = ( strtod((const char *) s.toAscii() + 6, ( char ** ) 0 ) - 1970 ) * 60 * 60 * 24 * 365.25;
    ret += strtod( (const char *)s.toAscii() + 3, ( char ** ) 0 ) * 60 * 60 * 24 * 30.4375;
    ret += strtod( (const char *)s.toAscii(), ( char ** ) 0 ) * 60 * 60 * 24;
    ret += 60 * 60 * 24; // just add one day to get 2000 not being a Schaltjahr right
    s[ 2 ] = '.';
    s[ 5 ] = '.';
    s[ 10 ] = ' ';
    return ret;
}


/* plain text file alike writing */
void XmlStorage::xml_writebase() {
    FILE * f;
    //Room *p;
    //int i;
    //char tmp[4028];

    f = fopen( fileName.toAscii(), "w" );
    if ( f == NULL ) {
        std::cerr << "XML: Error - can not open the file: " << fileName.toStdString() << std::endl;
        return ;
    }

    fprintf( f, "<map>\n" );
    /*
    for (int i = 0; i <= roomAdmin->lastId(); i++) {
      if (p = roomAdmin->getRoom(i) == 0) continue
          
      fprintf(f,  "  <room id=\"%li\" x=\"%li\" y=\"%li\" z=\"%li\" "
       "terrain=\"%s\" timestamp=\"%s\">\n",
       p->getId, p->getCoordinate()->x, p->getCoordinate()->y, p->getCoordinate()->z, 
       p->getTerrain() ? p->getTerrain()->desc : "NONE", 
       p->timestamp ? p->timestamp : "NONE");
          
          
      fprintf(f, "    <roomname>%s</roomname>\n", p->name ? p->name : "");
      fprintf(f, "    <desc>%s</desc>\n", p->desc ? p->desc : "");
      fprintf(f, "    <note>%s</note>\n", p->note ? p->note : "");
          
      fprintf(f, "    <exits>\n");


          
      for (i = 0; i <= 5; i++) {
        if (p->exits[i] != 0) {

    sprintf(tmp, "%d", p->exits[i]);
    if (p->exits[i] == EXIT_UNDEFINED)
     sprintf(tmp, "%s", "UNDEFINED");
    if (p->exits[i] == EXIT_DEATH)
     sprintf(tmp, "%s", "DEATH");
                  
    fprintf(f, "      <exit dir=\"%c\" to=\"%s\" door=\"%s\"/>\n",
    exitnames[i][0], tmp, p->doors[i] ? p->doors[i] : ""); 
        }

      }
          
      fprintf(f, "    </exits>\n");
      fprintf(f,  "  </room>\n");


    }
    */
    fprintf( f, "</map>\r\n" );
    fflush( f );
    fclose( f );


}

/*
int StructureParser::numbydir(char dir)
{
    if (dir == 'n')
	return NORTH;
    if (dir == 's')
	return SOUTH;
    if (dir == 'e')
	return EAST;
    if (dir == 'w')
	return WEST;
    if (dir == 'u')
	return UP;
    if (dir == 'd')
	return DOWN;
 
    return -1;
}
*/
