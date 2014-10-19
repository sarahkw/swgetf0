#include "ui_mainwindow.h"

#include <QGLWidget>

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

  MainWindow();

private:

  Ui::MainWindow m_ui;

};
