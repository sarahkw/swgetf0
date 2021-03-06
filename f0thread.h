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

#ifndef INCLUDED_F0THREAD
#define INCLUDED_F0THREAD

#include <mutex>

#include <QThread>
#include <QtDebug>

#include <portaudiocpp/Exception.hxx>
#include <portaudiocpp/BlockingStream.hxx>

#include "CircularBuffer.h"
#include "GetF0/get_f0_stream.h"

class F0Thread : public QThread {
  Q_OBJECT

public:
  struct F0Point
  {
      float f0;
      float maxsampleval;
  };

private:

  struct F0StreamImpl : public GetF0::GetF0Stream {

    F0StreamImpl(F0Thread& parent, std::mutex& mutex,
                 CircularBuffer<F0Point>& cb)
        : s_(nullptr), parent_(parent), mutex_i_(mutex), cb_i_(cb), stop_(false)
    {
    }

    void setStream(portaudio::BlockingStream* s)
    {
      s_ = s;
      s_->start();
    }

    void write_output_reversed(float* f0p, float* vuvp, float* rms_speech,
                               float* acpkp, float* maxsamplevalp,
                               int vecsize) override
    {
      {
        std::lock_guard<std::mutex> lockGuard(mutex_i_);

        for (int i = vecsize - 1; i >= 0; --i) {
          cb_i_.push_back({f0p[i], maxsamplevalp[i]});
        }
      }

      emit parent_.updated();
    }

    long read_stream_samples(short* buffer, long num_records) override
    {
      Q_ASSERT(s_ != nullptr);

      if (stop_) {
        return 0;
      } else {
          try {
            s_->read(buffer, num_records);
          } catch (const portaudio::PaException& e) {
            qWarning() << "Error reading from stream: " << e.what();
          }
          return num_records;
      }
    }

    void stop()
    {
      stop_ = true;
    }

    F0Thread& parent_;

    portaudio::BlockingStream *s_;

    std::mutex& mutex_i_;
    CircularBuffer<F0Thread::F0Point>& cb_i_;

    volatile bool stop_;
  };

public:

  F0Thread()
      : cb_(0), f0_(*this, mutex_, cb_)
  {
  }

  CircularBuffer<F0Point>& cb() { return cb_; }

  std::mutex& mutex() { return mutex_; }

  F0StreamImpl& f0() { return f0_; }

signals:

  void updated();

protected:

  void run() override
  {
    f0_.run();
  }

private:

  std::mutex mutex_;
  CircularBuffer<F0Point> cb_;

  F0StreamImpl f0_;
};

#endif
