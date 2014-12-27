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

#include "f0thread.h"

class MainWindow;

class ViewerWidget : public QGLWidget {
  Q_OBJECT

public:
  ViewerWidget(QWidget* parent);

public slots:
  void renderLater();
  void renderNow();

signals:

  void widthChanged(int newWidth);

protected:

  bool event(QEvent *event) override;

  void paintEvent(QPaintEvent *) override;

  void resizeEvent(QResizeEvent* event) override;

private:

  MainWindow *m_parent;

  bool m_update_pending;

};

namespace config {
struct Config;
}

class MainWindow : public QMainWindow {
  Q_OBJECT

public:

  MainWindow(const config::Config& config, F0Thread& f0thread);

  F0Thread& f0thread() { return m_f0thread; }

  const config::Config& config() const { return m_config; }

private slots:

  void on_action_About_triggered();

  void on_viewer_widthChanged(int width);

private:

  Ui::MainWindow m_ui;

  F0Thread& m_f0thread;

  const config::Config& m_config;
};
