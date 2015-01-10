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
#include <QDebug>

#include <portaudiocpp/AutoSystem.hxx>
#include <portaudiocpp/StreamParameters.hxx>
#include <portaudiocpp/BlockingStream.hxx>

#include "mainwindow.h"
#include "configuration.h"
#include "config.h"
#include "configuregetf0.h"
#include "f0thread.h"

int main(int argc, char* argv[])
{
  portaudio::AutoSystem autoSys;

  QApplication app(argc, argv);

  Configuration* inputDevice = new Configuration();
  int result = inputDevice->exec();
  if (result == QDialog::Rejected) {
    delete inputDevice;
    return 0;
  }

  PaDeviceIndex paDeviceIndex = inputDevice->getDeviceIndex();
  config::Config config = inputDevice->getConfig();

  portaudio::BlockingStream* blockingStream =
      new portaudio::BlockingStream(inputDevice->getStreamParameters());
  delete inputDevice;

  F0Thread f0(blockingStream, config.audioConfig.sample_rate);

  ConfigureGetF0(f0.f0(), config.espsConfig);

  f0.f0().init();

  MainWindow mainWindow(config, f0);
  mainWindow.show();

  f0.start();

  return app.exec();
}
