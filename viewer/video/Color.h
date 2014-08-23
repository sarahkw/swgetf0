#ifndef __VIDEO__COLOR_H__
#define __VIDEO__COLOR_H__

#include <GL/gl.h>

#ifndef NDEBUG
#include <iosfwd>
#endif

namespace viewer {
namespace video {

struct Color {
  GLubyte r;
  GLubyte g;
  GLubyte b;
  GLubyte a;

  Color(GLubyte r, GLubyte g, GLubyte b, GLubyte a)
    : r(r), g(g), b(b), a(a)
  { }

  unsigned toUint() const
  {
    return a | (b << 8) | (g << 16) | (r << 24);
  }

  Color multiply(const Color& o);

  Color screen(const Color& o);
};

#ifndef NDEBUG
std::ostream& operator<<(std::ostream& o, const Color& c);
#endif

} // namespace video
} // namespace viewer

#endif
