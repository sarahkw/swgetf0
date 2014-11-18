/*
    Copyright (C) 2014  Sarah Wong

    This file is part of swgetf0.

    swgetf0 is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <QApplication>
#include <QAudioInput> // not final
#include <QAudioDeviceInfo> // not final

#include "mainwindow.h"
#include "inputdevice.h"


////////////////////////////////////////////////////////////////////////////////
//

#include <iostream>
#include <thread>

#include <pulse/simple.h>

#include "GetF0StreamImpl.h"

typedef short DiskSample;

//
////////////////////////////////////////////////////////////////////////////////


int main(int argc, char* argv[])
{
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

    struct QIOStream : public IStream {
      QIOStream(QIODevice* ioDevice) : m_ioDevice(ioDevice) {}

      size_t read(void* ptr, size_t size, size_t nmemb) override
      {
        return m_ioDevice->read(static_cast<char*>(ptr), size * nmemb) / size;
      }

      int feof() override { return 0; }

      int ferror() override
      {
        return 0;  // TODO
      }

      QIODevice* m_ioDevice;
    };

    Foo(pa_simple* s) : GetF0StreamImpl<DiskSample>(new PulseStream(s), 96000)
    {
    }

    Foo(QIODevice* ioDevice)
        : GetF0StreamImpl<DiskSample>(new QIOStream(ioDevice), 96000)
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

  if (std::string(argv[1]) == "q") {
    std::cout << "QT Audio" << std::endl;

    QAudioInput* audio;
    {
      QAudioFormat format;
      // Set up the desired format, for example:
      format.setSampleRate(96000);
      format.setChannelCount(1);
      format.setSampleSize(16);
      format.setCodec("audio/pcm");
      format.setByteOrder(QAudioFormat::LittleEndian);
      format.setSampleType(QAudioFormat::SignedInt);

      audio = new QAudioInput(format);
    }

    f0 = new Foo(audio->start());

  } else if (std::string(argv[1]) == "p") {
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
