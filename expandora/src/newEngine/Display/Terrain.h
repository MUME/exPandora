#ifndef TERRAIN
#define TERRAIN
#include <qgl.h>
#include <map>
using namespace std;

// this is really a lazy class ...
class Terrain {	
	public:
  		char   *desc;		/* name of this flag */
  		char   *filename;      	/* appropriate texture's filename */
  		char   pattern;    	/* appropriate pattern */
  		GLuint texture;		/* and texture handler for renderer */
  		GLuint gllist;		/* OpenGL display list */
  		//Terrain *next;
  		static map<char, Terrain *> terrains;
		static map<QString, char> terrainIDs;
};
#endif

#ifdef DMALLOC
#include <dmalloc.h>
#endif
