
/*
  $Id$
*/
#include <string.h>
#include <qapplication.h>
#include <qfile.h>
#include <qxml.h>
//#include <qwindowdefs.h>
#include <qstring.h>

#include "defines.h"
#include "struct.h"

#include "rooms.h"
#include "utils.h"
#include "dispatch.h"

#ifdef NEW_ENGINE
#include <stack>
using namespace std;
#include "Terrain.h"
#include "ParseEvent.h"
#include "Room.h"
#include "RoomAdmin.h"
#include "Coordinate.h"
#include "Property.h"
#endif

#define XML_ROOMNAME    (1 << 0)
#define XML_DESC        (1 << 1)
#define XML_NOTE        (1 << 2)

#ifdef NEW_ENGINE
typedef struct Exit {
  int sourceId;
  int dir;
  int destId;
} Exit;
#endif

class StructureParser: public QXmlDefaultHandler
{
public:
  StructureParser();
  bool characters(const QString& ch);
    
  bool startElement( const QString&, const QString&, const QString& ,
		     const QXmlAttributes& );
  bool endElement( const QString&, const QString&, const QString& );
#ifdef NEW_ENGINE
  void addExits();

#endif
private:
  /* some flags */
  int flag;

  char data[MAX_DATA_LEN];
  QString s;


  int i;
#ifndef NEW_ENGINE
  struct Troom *r;
    
#else
  double timeFromString(QString &);
  void buildProperties(char * roomDesc);
   
  char tid;
  double ts; 
  Terrain * t;
  ParseEvent * roomProps;
  Coordinate * c;    
  stack<Exit *> exits;
  Property * prop;
  int id;
#endif
};




void xml_readbase( char *filename)
{
  QFile xmlFile( filename);
  QXmlInputSource source( &xmlFile );

  QXmlSimpleReader reader;

  StructureParser * handler = new StructureParser();
  reader.setContentHandler( handler );
    
	
    
  printf("reading xml ...\n");
    
  reader.parse( source );

#ifndef NEW_ENGINE
  printf("sorting rooms... \n");
  roomer.resort_rooms();
  printf("done sorting. \n");
#else
  //pop all the exits and addExit(...) them into the rooms
  handler->addExits();	  
#endif
  return;
}

#ifdef NEW_ENGINE
void StructureParser::addExits() {
  Exit * e;
  Room * from;
  Room * to;
  while (!(exits.empty())) {
    e = exits.top();
    exits.pop();
    from = roomAdmin.getRoom(e->sourceId);
    to = roomAdmin.getRoom(e->destId);
	    
    from->addExit(e->dir, to);
		
  }
}

void StructureParser::buildProperties(char * desc) {
  char * lastLineBegin = desc;
  for (int i = 0; desc[i] != 0; i++) {
    if (desc[i] == '|') {
      desc[i] = 0;
      prop->add(lastLineBegin);
      roomProps->push(prop);
      prop = pmm.activate();
      lastLineBegin = desc+i+1;
    }
  }
  //prop->add(lastLineBegin);
  //roomProps->push(prop);
  //prop = pmm.activate();
}
#endif


StructureParser::StructureParser()
  : QXmlDefaultHandler()
{
#ifdef NEW_ENGINE
  roomProps = 0;
  prop = 0;
  c = cmm.activate();
  c->clear();
  ts = 0;
  t = 0;
  id = 0;
  tid = 0;
#endif
}


bool StructureParser::endElement( const QString& , const QString& , const QString& qName)
{

  if (qName == "room") {
#ifndef NEW_ENGINE
    roomer.addroom_nonsorted(r);	/* tada! */
#else
    prop = pmm.activate();
    prop->add(tid);
    roomProps->push(prop);
    Room * room = roomAdmin.insertRoom(roomProps, id, c, t);
    room->resetTime(ts);
    pemm.deactivate(roomProps);
    roomProps = 0;
    c->clear();
    id = 0;
    tid = 0;
#endif
  }        
  flag = 0;    
    
  return TRUE;
}

bool StructureParser::characters( const QString& ch)
{
  strncpy( data, (const char*) ch, ch.length() );
  data[ ch.length() ] = 0;
#ifdef NEW_ENGINE	
  prop = pmm.activate();
  if (flag != XML_DESC) prop->add(data);	
  else buildProperties(data);
  if (roomProps != 0 && flag != 0) {
    if (flag == XML_NOTE) roomProps->pushOptional(prop);
    else if (flag == XML_ROOMNAME) roomProps->push(prop);
  }
  else {
    pmm.deactivate(prop);
    prop = 0;
  }
  return TRUE;
#else
  if (ch == NULL || ch == "")
    return TRUE;
    
  if (flag == XML_ROOMNAME) {
    r->name = strdup(data);
  }
  if (flag == XML_DESC) {
    if (ch == "" || ch == NULL) {
      r->desc = NULL;
    } else {       
      r->desc = strdup(data);
    }
            
  }
  if (flag == XML_NOTE) {
    if (ch == "" || ch == NULL) {
      r->desc = NULL;
    } else {       
      r->note = strdup(data);
    }
  }

    
  return TRUE;
#endif
} 


bool StructureParser::startElement( const QString& , const QString& , 
                                    const QString& qName, 
                                    const QXmlAttributes& attributes)
{

    
  /*    if (qName == "map") {
	}
  */    
  if (qName == "roomname") {
    flag = XML_ROOMNAME;
    return TRUE;
  }
  if (qName == "desc") {
    flag = XML_DESC;
    return TRUE;
  }
  if (qName == "note") {
    flag = XML_NOTE;
    return TRUE;
  }
    
  if (qName == "room") {
#ifdef NEW_ENGINE
    roomProps = pemm.activate();
 	
#else
    r = new Troom;
    reset_room(r);
#endif
  }
    
    
  if (qName == "exit") {
    unsigned int dir;
    unsigned int to;
    int i;
        
    /* special */
    if (attributes.length() < 3) {
      printf("Not enough exit attributes in XML file!");
      exit(1);
    }        
      
    s = attributes.value("dir");
    strncpy( data, (const char*) s, s.length() );
    data[ s.length() ] = 0;
#ifdef NEW_ENGINE
    char d = data[0];
#endif
    dir = numbydir(data[0]);
      
    s = attributes.value("to");
    strncpy( data, (const char*) s, s.length() );
    data[ s.length() ] = 0;
      
      
    i = 0;
    to = 0;
    while (room_flags[i].name) { 
      if (strcmp(data, room_flags[i].xml_name) == 0) { // EVIL!!! completely unnecessary strcmp in inner loop; time to redesign the xml syntax ...
	to = room_flags[i].flag;
      }
      i++;
    }
    if (!to)
      to = atoi( (const char *) data);
#ifdef NEW_ENGINE
    else to = INT_MAX;
#else
    r->exits[dir] = to;
#endif
      
      
    s = attributes.value("door");
    strncpy( data, (const char*) s, s.length() );
    data[ s.length() ] = 0;
      
#ifdef NEW_ENGINE
    /* if there is a door, then the exit (represented by d) is an optional property else it is a required property */
    prop = pmm.activate();
    prop->add(d); 
    if (data[0] == 0) {
      roomProps->push(prop);
    }
    else {
      roomProps->pushOptional(prop);
    }
    if (to != INT_MAX) { 
      Exit  * e = new Exit;
      e->sourceId = id;
      e->dir = dir;
      e->destId = to;
      exits.push(e);
    }
#endif 
      
    if (s.length() != 0) {
#ifdef NEW_ENGINE
      /* all doors are optional properties */
      prop = pmm.activate();
      prop->add(data);
      roomProps->pushOptional(prop);
#else
      r->doors[dir] = strdup(data);
      if (!r->doors[dir]) {
	printf("XML: Error while allocating memory in readbase function!");
	exit(1);
      }
#endif     
          
    }
      
        
  }
    
  if ( attributes.length() > 0 ) {
    for ( int i = 0 ; i < attributes.length(); i++ ) {
      s = attributes.value(i);
      //strcpy(data, (const char *)s);
      strncpy( data, (const char*) s, s.length() );
      data[ s.length() ] = 0;

      if (attributes.qName(i) == "id") {
#ifndef NEW_ENGINE
	r->id = atoi( data );
#else
	id = atoi(data);
#endif
	continue;
      }
      if (attributes.qName(i) == "x") {
#ifndef NEW_ENGINE
	r->x = atoi( data );
#else
	c->x = atoi(data);
#endif
	continue;
      }
      if (attributes.qName(i) == "y") {
#ifndef NEW_ENGINE
	r->y = atoi( data );
#else
	c->y = atoi(data);
#endif
	continue;
      }
      if (attributes.qName(i) == "z") {
#ifndef NEW_ENGINE
	r->z = atoi( data );
#else
	c->z = atoi(data);
#endif
	continue;
      }

      if (attributes.qName(i) == "terrain") {
#ifdef NEW_ENGINE
	tid = terrainIDs.find(s)->second;
	t = terrains.find(tid)->second;
#else   
	struct room_sectors_data *p;
		
	r->sector = NULL;
	p = room_sectors;
	while (p) {
	  if (strcmp(p->desc, data) == 0) {
	    // this could be somewhat faster if we used another data structure (for example the map in Terrain.cpp)
	    // atm we traverse all the terrains for each inserted room -> effort: n(rooms)*n(terrains)
	    // with a proper structure we could reduce it to n(rooms)*1 or at least n(rooms)*log(n(terrains))
	    // perhaps we need another map<QString &, char> to find the terrain abbrevation characters from the names used in the xml
	    // in Room's properties the abbrevations are stored
	    // 
	    // and if we stayed like this, I wouldn't use strcmp in an inner loop - especially when I only want to know if the strings differ at all
	    r->sector = p;
	  }
	  p = p->next;
	}
#endif
	continue;
      }
      if (attributes.qName(i) == "timestamp") {
#ifndef NEW_ENGINE
	strcpy(r->timestamp, data);
#else
	//ts = 0;
	ts = timeFromString(s);
#endif
	continue;
      }
  
            
    }
  }

  return TRUE;
}

#ifdef NEW_ENGINE
/**
 * a really ugly hack to get the timestamp out of the most stupid format string
 * why don't we use one of QT's QDateFormat strings?
 * I don't bother to calculate the hours, minutes and seconds as I'm wrong with 365.25 and that matters much more (one day atm?)
 */
double StructureParser::timeFromString(QString & s) {
  //"dd.MM.yyyy - hh:mm:ss"
  s[2] = 0;
  s[5] = 0;
  s[10] = 0;
  double ret = (strtod((const char *)s + 6, (char **)0) - 1970) * 60 * 60 * 24 * 365.25;
  ret += strtod((const char *)s + 3, (char **)0) * 60 * 60 * 24 * 30.4375;
  ret += strtod((const char *)s, (char **)0) * 60 * 60 * 24;
  ret += 60 * 60 * 24; // just add one day to get 2000 not being a Schaltjahr right
  s[2] = '.';
  s[5] = '.';
  s[10] = ' ';
  return ret;
}
#endif

/* plain text file alike writing */
void xml_writebase(char *filename)
{
  FILE *f;
  struct Troom *p;
  int i;
  char tmp[4028];

    
  print_debug(DEBUG_XML, "in xml_writebase()");
    
  p = roomer.getroom(0);	/* get the pointer to the base of array */
  f = fopen(filename, "w");
  if (f == NULL) {
    printf("XML: Error - can not open the file: %s.\r\n", filename);
    return;
  }

  fprintf(f, "<map>\n");
    
  while (p->next != NULL) {
    p = p->next;
        
    fprintf(f,  "  <room id=\"%li\" x=\"%li\" y=\"%li\" z=\"%li\" "
	    "terrain=\"%s\" timestamp=\"%s\">\n",
	    p->id, p->x, p->y, p->z, 
	    p->sector ? p->sector->desc : "NONE", 
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

  fprintf(f, "</map>\r\n");
  fflush(f);
  fclose(f);
    
  print_debug(DEBUG_XML, "xml_writebase() is done.\r\n");
}