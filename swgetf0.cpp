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

#include "mainwindow.h"
#include "inputdevice.h"


////////////////////////////////////////////////////////////////////////////////
//

#include <iostream>
#include <thread>

#include <pulse/simple.h>

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

  delete inputDevice;


////////////////////////////////////////////////////////////////////////////////
//
  using GetF0::GetF0StreamImpl;

  class Foo : public GetF0StreamImpl<DiskSample> {
  public:
    struct PulseStream : public IStream {
      PulseStream(pa_simple* s) : s(s) { }

      size_t read(void* ptr, size_t size, size_t nmemb) override {
        pa_simple_read(s, ptr, size * nmemb, NULL);  // TODO this is mess. fix.
        return nmemb;
      }
      int feof() override { return 0; }
      int ferror() override { return 0; }

      pa_simple* s;
    };

    Foo(pa_simple* s) : GetF0StreamImpl<DiskSample>(new PulseStream(s), 96000)
    {
    }

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

  if (argc != 2) {
    std::cerr << "Give me an argument. Read the source." << std::endl;
    return 1;
  }

  if (std::string(argv[1]) == "p") {
    std::cout << "PulseAudio" << std::endl;

    pa_simple* s;
    pa_sample_spec ss;
    ss.format = PA_SAMPLE_S16LE;
    ss.channels = 1;
    ss.rate = 96000;
    s = pa_simple_new(NULL,         // Use the default server.
                      "swpitcher",  // Our application's name.
                      PA_STREAM_RECORD,
                      NULL,      // Use the default device.
                      "Record",  // Description of our stream.
                      &ss,       // Our sample format.
                      NULL,      // Use default channel map
                      NULL,      // Use default buffering attributes.
                      NULL       // Ignore error code.
                      );
    if (s == nullptr) {
      std::cerr << "Pulse failed." << std::endl;
    }

    f0 = new Foo(s);

  } else {
    return 1;
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
