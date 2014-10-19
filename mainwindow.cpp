#include "mainwindow.h"

#include <QPainter>

namespace {
const float MINNOTE = 100;
const float MAXNOTE = 400;
}

ViewerWidget::ViewerWidget(QWidget* parent)
    : QGLWidget(parent), m_parent(dynamic_cast<MainWindow*>(parent))
{
}

void ViewerWidget::paintEvent(QPaintEvent* event) {
  QPainter painter(this);

  painter.fillRect(rect(), Qt::black);


  int m_width = 1024;
  int m_height = 800;

  std::lock_guard<std::mutex> lockGuard(m_parent->mutex());

  auto noteToPos = [this, m_height](double note) {
    return (note - MINNOTE) * (m_height / (MAXNOTE - MINNOTE));
  };

  const double noteWidth = 2;

#if 0
  double position = 0;
  for (auto note : cb()) {
    if (note.f0 != 0) {  // TODO float compare

      double ypos = noteToPos(note.f0);

      m_driver->draw2DRectangle(position, ypos - 1, position + noteWidth,
                                ypos + 1, video::Color(255, 255, 255, 255));
    }

    {
      double ypos = note.rms * (m_height / 3 / 24000.0) + (2.0 * m_height / 3);

      m_driver->draw2DRectangle(position, ypos - 1, position + noteWidth,
                                ypos + 1, video::Color(0, 100, 100, 255));
    }

    position += noteWidth;
  }
#endif

  // G3
  {
    double ypos = noteToPos(196);
    QPen pen(QColor(100, 0, 0));
    painter.setPen(pen);
    painter.drawLine(0, ypos, m_width, ypos);
  }

#if 0
  // A3
  {
    double ypos = noteToPos(220);
    m_driver->draw2DLine(0, ypos, m_width, ypos, video::Color(255, 0, 0, 255));
  }

  // A#3
  {
    double ypos = noteToPos(233);
    m_driver->draw2DLine(0, ypos, m_width, ypos, video::Color(50, 0, 0, 255));
  }

  // B3
  {
    double ypos = noteToPos(247);
    m_driver->draw2DLine(0, ypos, m_width, ypos, video::Color(100, 0, 0, 255));
  }

  // C4 - wow!
  {
    double ypos = noteToPos(262);
    m_driver->draw2DLine(0, ypos, m_width, ypos, video::Color(100, 0, 0, 255));
  }

  // D4
  {
    double ypos = noteToPos(294);
    m_driver->draw2DLine(0, ypos, m_width, ypos, video::Color(100, 0, 0, 255));
  }

  // E4
  {
    double ypos = noteToPos(330);
    m_driver->draw2DLine(0, ypos, m_width, ypos, video::Color(100, 0, 0, 255));
  }

  // C3
  {
    double ypos = noteToPos(131);
    m_driver->draw2DLine(0, ypos, m_width, ypos, video::Color(100, 0, 0, 255));
  }
#endif
}

MainWindow::MainWindow(std::size_t bufferCapacity) : m_cb(bufferCapacity)
{
  m_ui.setupUi(this);

  ViewerWidget* vw = new ViewerWidget(this);

  setCentralWidget(vw);
}
