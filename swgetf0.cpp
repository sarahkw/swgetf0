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
#include "inputdevice.h"


////////////////////////////////////////////////////////////////////////////////
//

#include <iostream>
#include <thread>

#include "GetF0StreamImpl.h"
#include "StreamReadBuffer.h"

typedef short DiskSample;

//
////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])
{
  portaudio::AutoSystem autoSys;

  QApplication app(argc, argv);

  InputDevice* inputDevice = new InputDevice();
  int result = inputDevice->exec();
  if (result == QDialog::Rejected) {
    delete inputDevice;
    return 0;
  }

  PaDeviceIndex paDeviceIndex = inputDevice->getDeviceIndex();
  delete inputDevice;


////////////////////////////////////////////////////////////////////////////////
//
  using GetF0::GetF0StreamImpl;

  class Foo : public GetF0StreamImpl<DiskSample> {
  public:

    struct PortStream : public IStream {
      PortStream(portaudio::BlockingStream* s) : s(s) {
	s->start();
      }

      size_t read(void* ptr, size_t size, size_t nmemb) override {
	s->read(ptr, nmemb);
        return nmemb;
      }
      int feof() override { return 0; }
      int ferror() override { return 0; }

      portaudio::BlockingStream* s;
    };

    Foo(portaudio::BlockingStream *s)
        : GetF0StreamImpl<DiskSample>(new PortStream(s), 44100) {}

    void setViewer(MainWindow* viewer) { m_viewer = viewer; }

    void write_output_reversed(float* f0p, float* vuvp, float* rms_speech,
                               float* acpkp, int vecsize) override
    {
      std::lock_guard<std::mutex> lockGuard(m_viewer->mutex());

      auto& cb = m_viewer->cb();

      for (int i = vecsize - 1; i >= 0; --i) {
        cb.push_back({f0p[i], rms_speech[i]});
      }
    }

    MainWindow *m_viewer;

  };

  Foo* f0;

  {
    portaudio::System& sys = portaudio::System::instance();

    portaudio::Device &device = sys.deviceByIndex(paDeviceIndex);
    portaudio::DirectionSpecificStreamParameters isp(
        device, 1, portaudio::INT16, true, device.defaultLowInputLatency(),
        NULL);
    portaudio::StreamParameters sp(
        isp, portaudio::DirectionSpecificStreamParameters::null(), 44100, 2048,
        paNoFlag);

    Q_ASSERT(sp.isSupported());

    f0 = new Foo(new portaudio::BlockingStream(sp));

  }

  f0->init();

//
////////////////////////////////////////////////////////////////////////////////

  enum { SECONDS = 5 };

  MainWindow mainWindow(f0->pitchFrameRate() * SECONDS);
  mainWindow.show();

  f0->setViewer(&mainWindow);

  // Run on new thread
  std::thread inputThread(std::bind(&Foo::run, f0));

  return app.exec();
}
