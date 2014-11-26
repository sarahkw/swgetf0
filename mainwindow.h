/*
  Copyright 2014 Sarah Wong

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

#include "ui_mainwindow.h"

#include <QGLWidget>
#include <QTimer>

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

  QTimer *m_timer;

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

private slots:

  void on_action_About_triggered();

private:

  Ui::MainWindow m_ui;

  std::mutex m_mutex;
  CircularBuffer<Point> m_cb;

};
