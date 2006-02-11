
/*
  $Id$
*/
#include <cstdlib>
#include <cstring>
#include <QApplication>
#include <QFile>
#include <QXmlDefaultHandler>
#include <QString>

#include "defines.h"


#include "configurator.h"
#include "Map.h"
#include "utils.h"
#include "dispatch.h"


#define XML_ROOMNAME    (1 << 0)
#define XML_DESC        (1 << 1)
#define XML_NOTE        (1 << 2)

class StructureParser: public QXmlDefaultHandler
{
public:
  StructureParser();
  bool characters(const QString& ch);
    
  bool startElement( const QString&, const QString&, const QString& ,
		     const QXmlAttributes& );
  bool endElement( const QString&, const QString&, const QString& );
private:
  /* some flags */
  int flag;

  char data[MAX_DATA_LEN];
  QString s;


  int i;
  CRoom *r;
    
};




void xml_readbase( QString filename)
{
  QFile xmlFile( filename);
  QXmlInputSource source( &xmlFile );

  QXmlSimpleReader reader;

  if (xmlFile.exists() == false) {
      printf("ERROR: The database file %s does NOT exist!\r\n", qPrintable(filename) );
      return;
  }


  StructureParser * handler = new StructureParser();
  reader.setContentHandler( handler );
    
	
    
  printf("reading xml ...");
  fflush(stdout);
  reader.parse( source );
  printf("done.\r\n");

  return;
}


StructureParser::StructureParser()
  : QXmlDefaultHandler()
{
}


bool StructureParser::endElement( const QString& , const QString& , const QString& qName)
{

  if (qName == "room") {
    Map.addroom_nonsorted(r);	/* tada! */
  }        
  flag = 0;    
    
  return TRUE;
}

bool StructureParser::characters( const QString& ch)
{
  if (ch == NULL || ch == "")
    return TRUE;
    
  if (flag == XML_ROOMNAME) {
    strncpy( data, qPrintable( ch) , ch.length() );
    data[ ch.length() ] = 0;
    r->name = strdup(data);
  } else if (flag == XML_DESC) {
    strncpy( data, qPrintable( ch) , ch.length() );
    data[ ch.length() ] = 0;
    if (ch == "" || ch == NULL) {
      r->desc = NULL;
    } else {       
      r->desc = strdup(data);
    }
            
  } else if (flag == XML_NOTE) {
    strncpy( data, qPrintable( ch) , ch.length() );
    data[ ch.length() ] = 0;
    if (ch == "" || ch == NULL) {
      r->desc = NULL;
    } else {       
      r->note = strdup(data);
    }
  }

    
  return TRUE;
} 


bool StructureParser::startElement( const QString& , const QString& , 
                                    const QString& qName, 
                                    const QXmlAttributes& attributes)
{

    
  /*    if (qName == "map") {
	}
  */ 
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
    strncpy( data, qPrintable(s), s.length() );
    data[ s.length() ] = 0;
    dir = numbydir(data[0]);
      
    s = attributes.value("to");
    strncpy( data, qPrintable(s), s.length() );
    data[ s.length() ] = 0;
      
      
    i = 0;
    to = atoi( (const char *) data);
    if (to == 0) 
        while (room_flags[i].name) { 
          if (strcmp(data, room_flags[i].xml_name) == 0) { 
            to = room_flags[i].flag;
          }
          i++;
        }
    r->exits[dir] = to;
      
    s = attributes.value("door");
    strncpy( data, qPrintable( s), s.length() );
    data[ s.length() ] = 0;
      
    if (s.length() != 0) {
      r->doors[dir] = strdup(data);
      if (!r->doors[dir]) {
	printf("XML: Error while allocating memory in readbase function!");
	exit(1);
      }
    }
      
        
  } else if (qName == "roomname") {
    flag = XML_ROOMNAME;
    return TRUE;
  } else if (qName == "desc") {
    flag = XML_DESC;
    return TRUE;
  } else if (qName == "note") {
    flag = XML_NOTE;
    return TRUE;
  } else if (qName == "room") {
      r = new CRoom;

      s = attributes.value("id");
      r->id = s.toInt();
      
      s = attributes.value("x");
      r->x = s.toInt();

      s = attributes.value("y");
      r->y = s.toInt();

      s = attributes.value("z");
      r->z = s.toInt();

      s = attributes.value("terrain");
      r->sector = conf.get_sector_by_desc(s.toAscii());
  }  
    
  return TRUE;
}

/* plain text file alike writing */
void xml_writebase(QString filename)
{
  FILE *f;
  CRoom *p;
  int i;
  char tmp[4028];
  unsigned int z;
    
  print_debug(DEBUG_XML, "in xml_writebase()");
    
  f = fopen(qPrintable(filename), "w");
  if (f == NULL) {
    printf("XML: Error - can not open the file: %s.\r\n", qPrintable(filename));
    return;
  }

  fprintf(f, "<map>\n");
  
    for (z = 0; z < Map.size(); z++) {
        p = Map.rooms[z];
    
        fprintf(f,  "  <room id=\"%i\" x=\"%i\" y=\"%i\" z=\"%i\" "
                "terrain=\"%s\">\n",
                p->id, p->x, p->y, p->z, 
                (const char *) conf.sectors[p->sector].desc);
            
            
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
