#ifndef RENDERER
#define RENDERER

class Map;

class Renderer {
 public:
  virtual void attachMap(Map * m);

  /** scan all atached maps and display their content */
  virtual void redraw();
}
