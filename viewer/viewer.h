#ifndef INCLUDED_VIEWER
#define INCLUDED_VIEWER

#include <mutex>

#include <SDL.h>

#include "../CircularBuffer.h"
#include "video/Driver.h"

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

}

#endif
