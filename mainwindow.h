/*
    Copyright (C) 2014  Sarah Wong

    This file is part of swgetf0.

    swgetf0 is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
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
