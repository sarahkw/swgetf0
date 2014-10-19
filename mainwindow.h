#include "ui_mainwindow.h"

#include <QGLWidget>

#include "CircularBuffer.h"
#include <mutex>

class ViewerWidget : public QGLWidget {
  Q_OBJECT

public:
  ViewerWidget(QWidget* parent);

protected:

  void initializeGL() override;
  void resizeGL(int w, int h) override;
  void paintGL() override;
};

class MainWindow : public QMainWindow {
  Q_OBJECT

public:

  struct Point {
    float f0;
    float rms;
  };

  MainWindow(std::size_t bufferCapacity);

  CircularBuffer<Point>& cb() { return m_cb; }

  std::mutex& mutex() { return m_mutex; }


private:

  Ui::MainWindow m_ui;

  std::mutex m_mutex;
  CircularBuffer<Point> m_cb;

};
