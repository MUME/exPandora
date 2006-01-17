#ifndef XML_STORAGE
#define XML_STORAGE

#include <qxml.h>
#include <qmutex.h>
#include <stack>

#include "Component.h"
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
}
Exit;


class StructureParser: public QObject, public QXmlDefaultHandler {

	public:
		StructureParser();
		bool characters( const QString& ch );
		bool startElement( const QString&, const QString&, const QString& ,
		                   const QXmlAttributes& );
		bool endElement( const QString&, const QString&, const QString& );
		void addExits();


	private:
		Q_OBJECT
		char terrainFromString(QLatin1String & terrain);
		void buildProperties( char * roomDesc );

		stack<Exit *> exits;

		/* some flags */
		int flag;
		char data[ MAX_DATA_LEN ];
		QString s;
		int i;
		ParseEvent * roomProps;
		Property * prop;
		Coordinate * c;
		double ts;
		char t;
		int id;

	signals:
		void addExit( int, int, int );
		void addRoom( ParseEvent *, Coordinate *, char, int );
};


class XmlStorage: public Component {
	private:
		Q_OBJECT	
		
	public slots:
		void loadFromFile(QString fileName);
	
		
	signals:
		void addExit( int, int, int );
		void addRoom( ParseEvent *, Coordinate *, char, int );
};

#ifdef DMALLOC
#include <mpatrol.h>
#endif
#endif

