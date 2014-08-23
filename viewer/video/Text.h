#ifndef __VIDEO__TEXT_H__
#define __VIDEO__TEXT_H__

#include <stdexcept>

#include <GL/gl.h>

#include "../base/Common.h"

namespace viewer {
namespace video {

struct Color;

class TextException : public std::runtime_error {
public:
  TextException();
  TextException(const std::string& what);
};

class Text {
public:

  struct Texture {
    GLuint texture_id;
    float max_s;
    float max_t;
    int w;
    int h;

    Texture() { }
    virtual ~Texture();

    DISALLOW_COPY_AND_ASSIGN(Texture);
  };

  Text() throw (TextException);
  ~Text();

  Texture* write(const char* text, int size, Color c) throw (TextException);

private:
  /**
   * Ensures input is a power of 2 by increasing it to the next power
   * of 2 if it is not a power of 2.
   */
  static unsigned make_power_of_two(unsigned s);
};

} // namespace video
} // namespace viewer

#endif
