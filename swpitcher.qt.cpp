#include <QApplication>

#include "ui_mainwindow.h"

int main(int argc, char* argv[])
{
  QApplication app(argc, argv);

  Ui::MainWindow ui_mainWindow;
  QMainWindow mainWindow;

  ui_mainWindow.setupUi(&mainWindow);

  mainWindow.show();

  return app.exec();
}
