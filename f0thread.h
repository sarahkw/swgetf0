#ifndef INCLUDED_F0THREAD
#define INCLUDED_F0THREAD

#include <mutex>
#include <QThread>

#include <portaudiocpp/BlockingStream.hxx>

#include "CircularBuffer.h"
#include "GetF0/get_f0_stream.h"

class F0Thread : public QThread, private GetF0::GetF0Stream {
  Q_OBJECT
public:

  F0Thread(portaudio::BlockingStream* s, double sampleFrequency)
      : GetF0Stream(sampleFrequency), s_(s), cb_(0)
  {
    s_->start();
  }

  void write_output_reversed(float* f0p, float* vuvp, float* rms_speech,
                             float* acpkp, int vecsize) override
  {
    std::lock_guard<std::mutex> lockGuard(mutex_);

    for (int i = vecsize - 1; i >= 0; --i) {
      cb_.push_back(f0p[i]);
    }
  }

  long read_stream_samples(short* buffer, long num_records) override
  {
    s_->read(buffer, num_records);
    return num_records;
  }

protected:
  void run() override
  {
    GetF0Stream::run();
  }

private:

  portaudio::BlockingStream *s_;

  std::mutex mutex_;
  CircularBuffer<float> cb_;


};

#endif
