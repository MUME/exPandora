#include <qxml.h>
#include <stack>

#include "Terrain.h"
#include "ParseEvent.h"
#include "Room.h"
#include "RoomAdmin.h"
#include "Coordinate.h"
#include "Property.h"


#define MAX_DATA_LEN 3072
#define XML_ROOMNAME    1
#define XML_DESC        2
#define XML_NOTE        4

/* buffered reading from file */

typedef struct Exit {
  int sourceId;
  int dir;
  int destId;
} Exit;


class StructureParser: public QXmlDefaultHandler
{
public:
  StructureParser(RoomAdmin * admin);
  bool characters(const QString& ch);
  bool startElement( const QString&, const QString&, const QString& ,
		     const QXmlAttributes& );
  bool endElement( const QString&, const QString&, const QString& );
  void addExits();

private:
  int numbydir(char dir);
  double timeFromString(QString &);
  void buildProperties(char * roomDesc);

  stack<Exit *> exits;
  RoomAdmin * roomAdmin;

  /* some flags */
  int flag;
  char data[MAX_DATA_LEN];
  QString s;
  int i;
  char tid;
  double ts; 
  Terrain * t;
  ParseEvent * roomProps;
  Coordinate * c;    
  Property * prop;
  int id;
};


class XmlStorage {
 public:
  void attachRoomAdmin(RoomAdmin * admin) {roomAdmin = admin;}
  void xml_writebase(char *filename);
  void xml_readbase(char *filename);
 private:
  RoomAdmin * roomAdmin;
};


#ifdef DMALLOC
#include <dmalloc.h>
#endif
