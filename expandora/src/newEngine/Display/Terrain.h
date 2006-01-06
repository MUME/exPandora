#ifndef TERRAIN
#define TERRAIN
#include <qgl.h>
#include <map>
using namespace std;

// this is really a lazy class ...
class Terrain {	
	public:
  		QString desc;		/* name of this flag */
  		QString filename;      	/* appropriate texture's filename */
  		char   pattern;    	/* appropriate pattern */
  		GLuint texture;		/* and texture handler for renderer */
  		GLuint gllist;		/* OpenGL display list */
  		static vector<Terrain *> terrains;
		static map<QString, char> terrainIDs;
};
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
