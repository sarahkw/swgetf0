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

  class Foo : public GetF0::GetF0Stream {
  public:

    Foo(portaudio::BlockingStream* s, double sampleFrequency)
        : GetF0Stream(sampleFrequency), s(s)
    {
      s->start();
    }

    void setViewer(MainWindow* viewer) { m_viewer = viewer; }

    void write_output_reversed(float* f0p, float* vuvp, float* rms_speech,
                               float* acpkp, int vecsize) override
    {
      std::lock_guard<std::mutex> lockGuard(m_viewer->mutex());

      auto& cb = m_viewer->cb();

      for (int i = vecsize - 1; i >= 0; --i) {
        cb.push_back(f0p[i]);
      }
    }

    long read_stream_samples(short* buffer, long num_records) override {
      s->read(buffer, num_records);
      return num_records;
    }

    MainWindow *m_viewer;

    portaudio::BlockingStream *s;

  };

  Foo* f0;

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

    f0 = new Foo(new portaudio::BlockingStream(sp),
                 config.audioConfig.sample_rate);
  }

  ConfigureGetF0(*f0, config.espsConfig);

  f0->init();

  MainWindow mainWindow(config);
  mainWindow.show();

  f0->setViewer(&mainWindow);

  // Run on new thread
  std::thread inputThread(std::bind(&Foo::run, f0));

  return app.exec();
}
