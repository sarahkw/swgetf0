#ifndef __BASE__COLORPALETTE_H__
#define __BASE__COLORPALETTE_H__

#include <vector>

#include "../video/Color.h"

namespace viewer {
namespace base {

/**
 * ColorPalette takes a count of colors we need, and generates them in
 * advance.
 */
class ColorPalette {
public:

  ColorPalette(unsigned color_count);

  const video::Color& color(unsigned i);

private:
  std::vector<video::Color> m_colors;

  static const GLubyte K_DARKEST;
  static const GLubyte K_BRIGHTEST;
};

} // namespace base
} // namespace viewer

#endif
