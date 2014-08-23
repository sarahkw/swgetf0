#include "ColorPalette.h"

#include <stdexcept>
#include <cassert>

namespace viewer {
namespace base {

ColorPalette::ColorPalette(unsigned color_count)
{
  const unsigned RGB_COLORS = 3;

  unsigned increment = (K_BRIGHTEST - K_DARKEST) * RGB_COLORS / color_count;
  if (increment == 0) {
    throw std::out_of_range("Too many colors");
  }

  GLubyte current[RGB_COLORS] = {K_BRIGHTEST, K_BRIGHTEST, K_BRIGHTEST};
  for (unsigned i = 0; i < color_count; ++i) {
    current[i % RGB_COLORS] -= increment;
    m_colors.push_back(video::Color(current[0], current[1], current[2], 255));
  }
}

const video::Color& ColorPalette::color(unsigned i)
{
  assert(i < m_colors.size());
  return m_colors[i];
}

const GLubyte ColorPalette::K_DARKEST = 100;
const GLubyte ColorPalette::K_BRIGHTEST = 200;

} // namespace base
} // namespace viewer
