#ifndef INCLUDED_VIEWER
#define INCLUDED_VIEWER

#include <mutex>

#include "../CircularBuffer.h"
#include "video/Driver.h"

namespace viewer {

class Viewer {
public:

  struct Point {
    float f0;
    float rms;
  };

  Viewer(std::size_t bufferCapacity) : m_cb(bufferCapacity), m_driver(NULL) {}

  virtual ~Viewer()
  {
    if (m_driver != nullptr) delete m_driver;
  }

  CircularBuffer<Point>& cb() { return m_cb; }

  std::mutex& mutex() { return m_mutex; }

  int run();

  void tick(unsigned timeDelta) {}

  void draw();

private:
  std::mutex m_mutex;
  CircularBuffer<Point> m_cb;

  video::Driver *m_driver;
  int m_width, m_height;
};

}

#endif
