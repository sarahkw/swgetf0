#include "Color.h"

#include <ostream>

namespace viewer {
namespace video {

Color Color::multiply(const Color& o)
{
  return Color(r * o.r / 255,
	       g * o.g / 255,
	       b * o.b / 255,
	       a * o.a / 255);
}

Color Color::screen(const Color& o)
{
#define SC(x) (255 - (((255 - x)*(255 - o.x ))/255))
  return Color(SC(r),
	       SC(g),
	       SC(b),
	       SC(a));
#undef SC
}

#ifndef NDEBUG
std::ostream& operator<<(std::ostream& o, const Color& c)
{
  o << "{" << (unsigned)c.r << ", " << (unsigned)c.g << ", "
    << (unsigned)c.b << ", " << (unsigned)c.a << "}";
  return o;
}
#endif


} // namespace video
} // namespace viewer
