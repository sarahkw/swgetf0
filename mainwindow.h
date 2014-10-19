#include "ui_mainwindow.h"

#include <QGLWidget>

#include "CircularBuffer.h"
#include <mutex>

class MainWindow;

class ViewerWidget : public QGLWidget {
  Q_OBJECT

public:
  ViewerWidget(QWidget* parent);

public slots:
  void renderLater();
  void renderNow();

protected:

  bool event(QEvent *event) override;

  void paintEvent(QPaintEvent *) override;

private:

  MainWindow *m_parent;

  bool m_update_pending;
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
