
#include <iostream>
#include <thread>

#include <pulse/simple.h>

#include "viewer/viewer.h"
#include "GetF0StreamImpl.h"

typedef short DiskSample;

int implementation_pulse()
{
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

  using GetF0::GetF0StreamImpl;

  class Foo : public GetF0StreamImpl<DiskSample> {
  public:
    enum { SECONDS = 5 };

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

    Foo(pa_simple* s)
      : GetF0StreamImpl<DiskSample>(new PulseStream(s), 96000),
          m_viewer(pitchFrameRate() * SECONDS)
    {
    }

    void write_output_reversed(float* f0p, float* vuvp, float* rms_speech,
                               float* acpkp, int vecsize) override
    {
      std::lock_guard<std::mutex> lockGuard(m_viewer.mutex());

      auto& cb = m_viewer.cb();

      for (int i = vecsize - 1; i >= 0; --i) {
        cb.push_back({f0p[i], rms_speech[i]});
      }
    }

    viewer::Viewer m_viewer;

  } f0(s);

  f0.init();

  std::thread viewerThread(std::bind(&viewer::Viewer::run, &f0.m_viewer));
  f0.run();

  viewerThread.join();

  return 0;
}

int main(int argc, char* argv[])
{
  return implementation_pulse();
}
