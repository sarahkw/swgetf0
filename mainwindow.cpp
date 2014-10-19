#include "mainwindow.h"

ViewerWidget::ViewerWidget(QWidget* parent) : QGLWidget(parent) {}

void ViewerWidget::initializeGL() {}
void ViewerWidget::resizeGL(int w, int h) {}
void ViewerWidget::paintGL() {}

MainWindow::MainWindow() {
  m_ui.setupUi(this);

  ViewerWidget *vw = new ViewerWidget(this);

  setCentralWidget(vw);
}
