#include <qxml.h>
#include <qobject.h>
#include <stack>


#include "ParseEvent.h"
#include "Room.h"

#include "Coordinate.h"
#include "Property.h"


#define MAX_DATA_LEN 3072
#define XML_ROOMNAME    1
#define XML_DESC        2
#define XML_NOTE        4

/* buffered reading from file */
/* finally I changed the syntax: 
   - exit pattern is now a character representation of the offset in the std exits
   - terrain pattern is now a character that maps to the respective terrain (in mume: the terrain character in the prompt)
   it should be quite easy to convert existing maps.
*/

typedef struct Exit {
  int sourceId;
  int dir;
  int destId;
} Exit;


class StructureParser: public QObject, public QXmlDefaultHandler {
public:
  StructureParser();
  bool characters(const QString& ch);
  bool startElement( const QString&, const QString&, const QString& ,
		     const QXmlAttributes& );
  bool endElement( const QString&, const QString&, const QString& );
  void addExits();

private:
  Q_OBJECT
  double timeFromString(QString &);
  void buildProperties(char * roomDesc);

  stack<Exit *> exits;

  /* some flags */
  int flag;
  char data[MAX_DATA_LEN];
  QString s;
  int i;
  double ts; 
  char t;
  ParseEvent * roomProps;
  Coordinate * c;    
  Property * prop;
  int id;

 signals:
  void addExit(int, int, int);
  void addRoom(ParseEvent *, int, Coordinate *, char);
};


class XmlStorage {
 public:
  void xml_writebase(char *filename);
  void xml_readbase(char *filename);
};


#ifdef DMALLOC
#include <dmalloc.h>
#endif
