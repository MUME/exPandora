
/*
  $Id$
*/
#include <cstdlib>
#include <cstring>
#include <QApplication>
#include <QFile>
#include <QXmlDefaultHandler>
#include <QString>
#include <QProgressDialog>

#include "defines.h"


#include "configurator.h"
#include "Map.h"
#include "utils.h"
#include "dispatch.h"
#include "mainwindow.h"

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
  bool readingRegion;
  float version;

  char data[MAX_DATA_LEN];
  QString s;


  int i;
  CRoom *r;
  CRegion *region;
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
    
	
  
  fflush(stdout);
  reader.parse( source );
  
  // Now fix the preloaded ID' in exits 
  
  for (unsigned int i = 0; i < Map.size(); i++) {
        CRoom *r = Map.rooms[i];
        for (int exit = 0; exit <= 5; exit++)
            if (r->exits[ exit ] > 0)
                r->exits[exit] = Map.getRoom( (unsigned long) r->exits[exit] );
  }
  return;
}


StructureParser::StructureParser()
  : QXmlDefaultHandler()
{
    flag = 0;
    readingRegion = false;
    version = 1.0;
}


bool StructureParser::endElement( const QString& , const QString& , const QString& qName)
{
  if (qName == "room") {
      Map.addRoomNonsorted(r);	/* tada! */
  }  
  if (qName == "region" && readingRegion)  {
      Map.addRegion( region );
      region = NULL;      
      readingRegion = false;
  }        
  flag = 0;    
    
  return TRUE;
}

bool StructureParser::characters( const QString& ch)
{
  if (ch == NULL || ch == "")
    return TRUE;
    
  if (flag == XML_ROOMNAME) {
    r->setName(ch.toAscii());
  } else if (flag == XML_DESC) {
      r->setDesc(ch.toAscii());
  } else if (flag == XML_NOTE) {
      r->setNote(ch.toAscii());
  }
  return TRUE;
} 


bool StructureParser::startElement( const QString& , const QString& , 
                                    const QString& qName, 
                                    const QXmlAttributes& attributes)
{
    if (readingRegion == true) {
        if (qName == "alias") {
            QByteArray alias, door;
            
            s = attributes.value("name");
            alias = s.toAscii();
            
            s = attributes.value("door");
            door = s.toAscii();
            
            if (door != "" && alias != "")
                region->addDoor(alias, door);
            return TRUE;
        } 
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
    dir = numbydir(s.toAscii().at(0));
      
    s = attributes.value("to");
      
    i = 0;
    bool NoError = false;
    to = s.toInt(&NoError);
    if (NoError == false) {
        while (room_flags[i].name != "") { 
          if (s == room_flags[i].xml_name) { 
              r->setExitFlags(dir, room_flags[i].flag);
              break;
          }
          i++;
        }
    } else {
//        r->setExit(dir, to);    
        // Ugly preload of ID's to pointers 
        r->exits[dir] = (CRoom *) to;        
    }
      
    s = attributes.value("door");
    r->setDoor(dir, s.toAscii());
    
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

      s = attributes.value("id");
      r = new CRoom( s.toInt() );
      
      s = attributes.value("x");
      if (version == 1.0) {
        r->setX( s.toInt() * 10);
      } else {
        r->setX( s.toInt() );        
      }
        
      s = attributes.value("y");
      if (version == 1.0) {
        r->setY( s.toInt() * 10);
      } else {
        r->setY( s.toInt() );        
      }

      s = attributes.value("z");
      if (version == 1.0) {
        r->setZ( s.toInt() * 10);
      } else {
        r->setZ( s.toInt() );        
      }

      s = attributes.value("terrain");
      r->setSector( conf.get_sector_by_desc(s.toAscii()) );
      
      s = attributes.value("region");
      r->setRegion(s.toAscii());
  } else if (qName == "region") {
     region = new CRegion;
            
     readingRegion = true;
     s = attributes.value("name");
     region->setName(s.toAscii());
  }  else if (qName == "map") {
     s = attributes.value("name");
     if (s != "") 
        version = s.toFloat();
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

  

  fprintf(f, "<map version=\"2.0\">\n");
  
  
  
  {
        // SAVE REGIONS DATA
        CRegion    *region;
        QList<CRegion *> regions;
        QMap<QByteArray, QByteArray> doors;        
        
        regions = Map.getAllRegions();
        fprintf(f, "  <regions>\n");
        for (int i=0; i < regions.size(); i++) {
            region = regions[i];
            if (region->getName() == "default")
                continue;   // skip the default region -> its always in memory as the first one anyway!
            fprintf(f, "    <region name=\"%s\">\n", (const char *) region->getName() );
            
            doors = region->getAllDoors();
            QMapIterator<QByteArray, QByteArray> i(doors);
            while (i.hasNext()) {
                i.next();
                fprintf(f, "      <alias name=\"%s\" door=\"%s\"/>\n",  (const char *)  i.key(), (const char *) i.value() );
            }
            fprintf(f, "    </region>\n");
        }
        fprintf(f, "  </regions>\n");
  
  
  
  
  }
  
  
    for (z = 0; z < Map.size(); z++) {
        p = Map.rooms[z];
    
        fprintf(f,  "  <room id=\"%i\" x=\"%i\" y=\"%i\" z=\"%i\" "
                "terrain=\"%s\" region=\"%s\">\n",
                p->id, p->getX(), p->getY(), p->getZ(), 
                (const char *) conf.sectors[ p->getTerrain() ].desc, 
                (const char *) p->getRegionName());
            
            
        fprintf(f, "    <roomname>%s</roomname>\n", (const char *) p->getName());
        fprintf(f, "    <desc>%s</desc>\n",  (const char *) p->getDesc() );
        fprintf(f, "    <note>%s</note>\n",  (const char *) p->getNote() );
            
        fprintf(f, "    <exits>\n");
    
    
            
        for (i = 0; i <= 5; i++) {
            if (p->isExitPresent(i) == true) {
    
                if (p->isExitNormal(i) == true) {
                    sprintf(tmp, "%d", p->exits[i]->id);
                } else {
                    if (p->isExitUndefined(i) == true)
                        sprintf(tmp, "%s", "UNDEFINED");
                    else if (p->isExitDeath(i) == true)
                        sprintf(tmp, "%s", "DEATH");
                }
                                    
                fprintf(f, "      <exit dir=\"%c\" to=\"%s\" door=\"%s\"/>\n",
                        exitnames[i][0], tmp, (const char *) p->getDoor(i) ); 
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
