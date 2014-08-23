#ifndef __VIDEO__DRIVER_H__
#define __VIDEO__DRIVER_H__

#include <GL/gl.h>

#include "Text.h"
#include "Color.h"

namespace viewer {
namespace video {

class Driver {
public:
  Driver(int width, int height) throw (TextException);

  void beginScene();
  void endScene();

  void draw2DLine(float x1, float y1, float x2, float y2, Color c);
  void draw2DRectangle(float x1, float y1, float x2, float y2, Color c);
  void draw2DRectangleGrad(float x1, float y1, float x2, float y2, Color c_l, Color c_r);
  void draw2DRectangleOutline(float x1, float y1, float x2, float y2, Color c, GLfloat width);
  void draw2DText(Text::Texture *tex, float x, float y);

  Text::Texture* renderText(const char* text, int size, Color c) throw (TextException);

  int width() const { return m_width; }
  int height() const { return m_height; }

private:
  Text m_text;

  int m_width, m_height;
};

} // namespace video
} // namespace viewer

#endif
