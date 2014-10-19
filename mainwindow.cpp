#include "mainwindow.h"

#include <QLabel>

MainWindow::MainWindow() {
  m_ui.setupUi(this);

  QLabel *label1 = new QLabel("Hi", this);

  setCentralWidget(label1);
}
