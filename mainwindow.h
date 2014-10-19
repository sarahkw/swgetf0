#include "ui_mainwindow.h"

class MainWindow : public QMainWindow {
  Q_OBJECT

public:

  MainWindow() {
    m_ui.setupUi(this);
  }

private:

  Ui::MainWindow m_ui;

};
