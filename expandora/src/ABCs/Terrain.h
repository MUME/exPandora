#ifndef TERRAIN
#define TERRAIN
#include <qgl.h>


/** I'm certain this isn't that good so far ... 
    I changed all the public fields to methods, so that we can virtualize them, 
    but as they are renderer-specific that probably won't help. We need to find a way to *only*
    express abstract information about the terrain type and let the renderer find out how to display it.
*/
class Terrain {	
 public:
  virtual char   * getDesc();		/* name of this flag */
  virtual char   * getFilename();      	/* appropriate texture's filename */
  virtual char   getPattern();    	/* appropriate pattern */
  virtual GLuint getTexture();		/* and texture handler for renderer */
  virtual GLuint getGllist();		/* OpenGL display list */
};
#endif
