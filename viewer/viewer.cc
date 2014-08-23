
#include <cstdlib>
#include <ctime>
#include <memory>
#include <list>
#include <mutex>

#include <GL/gl.h>
#include <SDL.h>

#include "video/Text.h"
#include "video/Driver.h"
#include "base/Log.h"
#include "base/Guard.h"

#include "../CircularBuffer.h"

namespace viewer {

class Viewer {
public:
  Viewer(std::size_t bufferCapacity) : m_cb(bufferCapacity), m_driver(NULL) {}

  virtual ~Viewer()
  {
    if (m_driver != nullptr) delete m_driver;
  }

  CircularBuffer<float>& cb() { return m_cb; }

  std::mutex& mutex() { return m_mutex; }

  int run();

  void tick(Uint32 timeDelta) {}

  void draw();

private:
  std::mutex m_mutex;
  CircularBuffer<float> m_cb;

  video::Driver *m_driver;
  int m_width, m_height;
};


int Viewer::run() {
  typedef base::PtrGuard<SDL_Surface, SDL_FreeSurface> SurfaceGuard;
  typedef base::RunGuard<SDL_Quit> SDLQuitGuard;

  std::unique_ptr<video::Driver> driver;
  Uint32 last_time;
  Uint32 fps = 120;

  if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
    VIEWER_LOG_ERROR << SDL_GetError() << VIEWER_LOG_END;
    return 1;
  }
  SDLQuitGuard sdlQuitGuard;

  SDL_WM_SetCaption("viewer", "viewer");

  SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
  SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
  SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

  SurfaceGuard screen(SDL_SetVideoMode(1024, 600, 24, SDL_OPENGL));
  if (!screen) {
    VIEWER_LOG_ERROR << SDL_GetError() << VIEWER_LOG_END;
    return 1;
  }

#if 0
  SDL_WM_ToggleFullScreen(screen.get());
#endif

  try {
    m_driver = new video::Driver(screen->w, screen->h);
  } catch (const video::TextException& e) {
    VIEWER_LOG_ERROR << e.what() << VIEWER_LOG_END;
    return 1;
  }

  m_width = screen->w;
  m_height = screen->h;

  // Seed random number generator
  srand(time(NULL));

  // Initialize time difference counter
  last_time = SDL_GetTicks();

  // Main loop
  for (;;) {
    Uint32 this_time = SDL_GetTicks();

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
      case SDL_QUIT:
	goto end;
	break;
      default:
	break;
      }
    }

    try {
      // TODO Fix wraparound for time difference
      tick(this_time - last_time);
    } catch (const std::exception& e) {
      VIEWER_LOG_ERROR << e.what() << VIEWER_LOG_END;
      return 1;
    }

    driver->beginScene();
    draw();
    driver->endScene();

    last_time = this_time;

    Sint32 sleep_time = (1000 / fps) - (SDL_GetTicks() - last_time);
    if (sleep_time > 0) {
      SDL_Delay(sleep_time);
    }
  }

 end:
  return 0;
}

void Viewer::draw()
{
  std::lock_guard<std::mutex> lockGuard(mutex());

  const float minNote = 50;
  const float maxNote = 300;

  const double noteWidth = 2;
  // const double noteHeight = 20;

  double position = 0;
  for (auto note : cb()) {
    if (note != 0) {  // TODO float compare

      double ypos = (note - minNote) * (m_height / (maxNote - minNote));

      m_driver->draw2DRectangle(position, ypos - 1, position + noteWidth,
                                ypos + 1, video::Color(255, 255, 255, 255));
    }

    position += noteWidth;
  }

  {
    double ypos = (196 - minNote) * (m_height / (maxNote - minNote));
    m_driver->draw2DLine(0, ypos, m_width, ypos, video::Color(255, 0, 0, 255));
  }
}
}

int main(int argc, char* argv[])
{
  return 0;
}
