#include "Driver.h"

#include <SDL.h>

namespace viewer {
namespace video {

Driver::Driver(int width, int height) throw (TextException)
  : m_width(width), m_height(height)
{
  // Init OpenGL
  glClearColor(0, 0, 0, 0);
  glViewport(0, 0, width, height);
  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();
  glOrtho(-1, width + 1, -1, height + 1, -1, 1);

  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void Driver::beginScene()
{
  glClear(GL_COLOR_BUFFER_BIT);
  glMatrixMode(GL_MODELVIEW);
  glLoadIdentity();
}

void Driver::endScene()
{
  SDL_GL_SwapBuffers();
}

void Driver::draw2DLine(float x1, float y1, float x2, float y2, Color c)
{
  glColor4ub(c.r, c.g, c.b, c.a);

  glBegin(GL_LINES);
  glVertex2f(x1, y1);
  glVertex2f(x2, y2);
  glEnd();
}

void Driver::draw2DRectangle(float x1, float y1, float x2, float y2, Color c)
{
  glColor4ub(c.r, c.g, c.b, c.a);

  glBegin(GL_QUADS);
  glVertex2f(x1, y1);
  glVertex2f(x2, y1);
  glVertex2f(x2, y2);
  glVertex2f(x1, y2);
  glEnd();
}

void Driver::draw2DRectangleGrad(float x1, float y1, float x2, float y2, Color c_l, Color c_r)
{
  // TODO Maybe rewrite these so we have a quad, but with fewer glColor calls.
  glBegin(GL_QUADS);
  glColor4ub(c_l.r, c_l.g, c_l.b, c_l.a);
  glVertex2f(x1, y1);
  glColor4ub(c_r.r, c_r.g, c_r.b, c_r.a);
  glVertex2f(x2, y1);
  glColor4ub(c_r.r, c_r.g, c_r.b, c_r.a);
  glVertex2f(x2, y2);
  glColor4ub(c_l.r, c_l.g, c_l.b, c_l.a);
  glVertex2f(x1, y2);
  glEnd();
}

void Driver::draw2DRectangleOutline(float x1, float y1, float x2, float y2, Color c, GLfloat width)
{
  glColor4ub(c.r, c.g, c.b, c.a);

  glLineWidth(width);

  glBegin(GL_LINE_LOOP);
  glVertex2f(x1, y1);
  glVertex2f(x2, y1);
  glVertex2f(x2, y2);
  glVertex2f(x1, y2);
  glEnd();

  // Reset
  glLineWidth(1);
}

void Driver::draw2DText(Text::Texture *tex, float x, float y)
{
  glColor4f(1, 1, 1, 1);
  glBindTexture(GL_TEXTURE_2D, tex->texture_id);
  glEnable(GL_TEXTURE_2D);
  glBegin(GL_QUADS);
  glTexCoord2f(0, tex->max_t); glVertex2f(x + 0, y + 0);
  glTexCoord2f(tex->max_s, tex->max_t); glVertex2f(x + tex->w, y + 0);
  glTexCoord2f(tex->max_s, 0); glVertex2f(x + tex->w, y + tex->h);
  glTexCoord2f(0, 0); glVertex2f(x + 0, y + tex->h);
  glEnd();
  glDisable(GL_TEXTURE_2D);
}

Text::Texture* Driver::renderText(const char* text, int size, Color c) throw (TextException)
{
  return m_text.write(text, size, c);
}


} // namespace video
} // namespace viewer
