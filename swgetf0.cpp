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
#include "GetF0/get_f0_stream.h"
#include "configuregetf0.h"
#include "f0thread.h"


////////////////////////////////////////////////////////////////////////////////
//

#include <iostream>
#include <thread>

//
////////////////////////////////////////////////////////////////////////////////


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
  delete inputDevice;

  F0Thread* f0;

  {
    portaudio::System& sys = portaudio::System::instance();

    portaudio::Device &device = sys.deviceByIndex(paDeviceIndex);
    portaudio::DirectionSpecificStreamParameters isp(
        device, 1, portaudio::INT16, true, device.defaultLowInputLatency(),
        NULL);
    portaudio::StreamParameters sp(
        isp, portaudio::DirectionSpecificStreamParameters::null(),
        config.audioConfig.sample_rate, paFramesPerBufferUnspecified, paNoFlag);

    Q_ASSERT(sp.isSupported());

    f0 = new F0Thread(new portaudio::BlockingStream(sp),
                      config.audioConfig.sample_rate);
  }

  ConfigureGetF0(f0->f0(), config.espsConfig);

  f0->f0().init();

  MainWindow mainWindow(config, *f0);
  mainWindow.show();

  f0->start();

  return app.exec();
}
