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

#include <QApplication>

#include <portaudiocpp/AutoSystem.hxx>
#include <portaudiocpp/BlockingStream.hxx>

#include "config.h"
#include "configuration.h"
#include "f0thread.h"
#include "mainwindow.h"

int main(int argc, char* argv[])
{
  portaudio::AutoSystem autoSys;

  QApplication app(argc, argv);

  F0Thread f0;

  Configuration* configDialog = new Configuration(f0.f0());
  int result = configDialog->exec();
  if (result == QDialog::Rejected) {
    delete configDialog;
    return 0;
  }

  PaDeviceIndex paDeviceIndex = configDialog->getDeviceIndex();
  config::Config config = configDialog->getConfig();

  portaudio::BlockingStream* blockingStream =
      new portaudio::BlockingStream(configDialog->getStreamParameters());
  delete configDialog;

  f0.f0().setStream(blockingStream);
  f0.f0().init(config.audioConfig.sample_rate);

  MainWindow mainWindow(config, f0);
  mainWindow.show();

  f0.start();

  app.exec();

  /*TODO

    We recommend that you connect clean-up code to the aboutToQuit()
    signal, instead of putting it in your application's main()
    function because on some platforms the QCoreApplication::exec()
    call may not return. For example, on Windows when the user logs
    off, the system terminates the process after Qt closes all
    top-level windows. Hence, there is no guarantee that the
    application will have time to exit its event loop and execute code
    at the end of the main() function after the
    QCoreApplication::exec() call.
   */

  f0.f0().stop();
  f0.wait();

  return 0;
}
