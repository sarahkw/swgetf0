#include "Text.h"

#include <SDL.h>
#include <SDL_ttf.h>

#include "../base/Log.h"
#include "../base/Guard.h"
#include "Driver.h"

namespace viewer {
namespace video {

TextException::TextException() :
  std::runtime_error(std::string("SDL_ttf error: ") + SDL_GetError())
{
}

TextException::TextException(const std::string& what) :
  std::runtime_error(std::string("Text error: ") + what)
{
}



Text::Texture::~Texture()
{
  glDeleteTextures(1, &texture_id);
}

Text::Text() throw (TextException)
{
  if (TTF_Init() != 0) {
    throw TextException();
  }
}

Text::~Text()
{
  TTF_Quit();
}

Text::Texture* Text::write(const char* text, int size, Color c) throw (TextException)
{
  typedef base::PtrGuard<TTF_Font, TTF_CloseFont> FontGuard;
  typedef base::PtrGuard<SDL_Surface, SDL_FreeSurface> SurfaceGuard;

  // SDL_ttf uses freetype with the default 72 DPI. 72 points = 1
  // inch. So, 1 point = 1 pixel.

  FontGuard font(TTF_OpenFont("FreeMono.ttf", size));
  if (!font)
    throw TextException();

  SurfaceGuard font_write(TTF_RenderText_Blended(font.get(), text,
						 {c.r, c.g, c.b, 0}));
  if (!font_write)
    throw TextException();

  // "On certain platforms, the GPU prefers that red and blue be
  // swapped (GL_BGRA).  If you supply GL_RGBA, then the driver will
  // do the swapping for you which is slow.  On which platforms?
  // Making a list would be too long but one example is Microsoft
  // Windows."

  // Doing it this way because otherwise endian-ness screws this up.
  const char R[4] = {(char)0x00, (char)0x00, (char)0xFF, (char)0x00};
  const char G[4] = {(char)0x00, (char)0xFF, (char)0x00, (char)0x00};
  const char B[4] = {(char)0xFF, (char)0x00, (char)0xFF, (char)0x00};
  const char A[4] = {(char)0x00, (char)0x00, (char)0x00, (char)0xFF};

  SurfaceGuard font_rdy(SDL_CreateRGBSurface(SDL_SWSURFACE,
					     make_power_of_two(font_write->w),
					     make_power_of_two(font_write->h),
					     32,
					     *(unsigned*)R,
					     *(unsigned*)G,
					     *(unsigned*)B,
					     *(unsigned*)A));
  if (!font_rdy)
    throw TextException();

  // These are for making sure none of the filler part of the texture
  // is shown. Makes everything white so we can see the borders.
  //
  // SDL_Rect fill_rect = {0, 0, (Uint16)font_rdy->w, (Uint16)font_rdy->h};
  // SDL_FillRect(font_rdy.get(), &fill_rect, 0xFFFFFFFF);

  // This disables alpha blending on the blit. If we use alpha
  // blending, we won't get anything because our new surface doesn't
  // have anything with alpha.
  font_write->flags &= ~SDL_SRCALPHA;

  SDL_Rect copy_rect = {
    (Sint16)0, (Sint16)0, (Uint16)font_write->w, (Uint16)font_write->h
  };

  if (SDL_LowerBlit(font_write.get(), &copy_rect, font_rdy.get(), &copy_rect) != 0)
    throw TextException();

  GLuint textureid;
  glGenTextures(1, &textureid);
  glBindTexture(GL_TEXTURE_2D, textureid);

  // Hint for OpenGL n00bie: glTexParameter affects the currently
  // bound texture. Definitely don't call these on the render loop, or
  // else the textures will take some time to appear.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

  SDL_LockSurface(font_rdy.get());
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font_rdy->w, font_rdy->h, 0,
	       GL_BGRA, GL_UNSIGNED_BYTE, font_rdy->pixels);
  SDL_UnlockSurface(font_rdy.get());

  Text::Texture *tex = new Text::Texture;
  tex->texture_id = textureid;
  tex->max_s = float(font_write->w) / font_rdy->w;
  tex->max_t = float(font_write->h) / font_rdy->h;
  tex->w = font_write->w;
  tex->h = font_write->h;
  return tex;
}

unsigned Text::make_power_of_two(unsigned s)
{
  const int BITS_PER_BYTE = 8;

  int i;

  if (s == 0)
    return 1;

  for (i = sizeof(s) * BITS_PER_BYTE - 1; i >= 0; --i) {
    if (s & (1 << i)) {
      break;
    }
  }

  // We know that i >= 0.

  // We know that the i-th most significant bit of s is a 1. If
  // everything besides that are 0, then s is a power of 2.
  if ((((1 << i) - 1) & s) == 0) {
    return s;
  }

  // The left of bit i is the next power of 2.
  ++i;
  return 1 << i;
}


} // namespace video
} // namespace viewer
