#ifndef TERRAIN
#define TERRAIN
#include <GL/gl.h>
// we should unify this GL thing. arghh

// this is really a lazy class ...
class Terrain {	
	public:
  		char   *desc;		/* name of this flag */
  		char   *filename;      	/* appropriate texture's filename */
  		char   pattern;    	/* appropriate pattern */
  		GLuint texture;		/* and texture handler for renderer */
  		GLuint gllist;		/* OpenGL display list */
  		Terrain *next;
};

#endif
