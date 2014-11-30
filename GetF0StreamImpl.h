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

#ifndef INCLUDED_GETF0STREAMIMPL
#define INCLUDED_GETF0STREAMIMPL

#include <algorithm>

#include "GetF0/get_f0_stream.h"

namespace GetF0 {

namespace {
template <class SourceFormat, class DestFormat>
struct StaticCaster {
  DestFormat operator()(SourceFormat from)
  {
    return static_cast<DestFormat>(from);
  }
};
}

template <class SourceFormat>
class GetF0StreamImpl : public GetF0Stream {
protected:

  struct IStream {
    virtual void read(void* buffer, size_t frames) = 0;
    virtual ~IStream() { }
  };

protected:

  GetF0StreamImpl(IStream* stream, SampleFrequency sampleFrequency,
                  DebugLevel debugLevel = 0)
      : GetF0Stream(sampleFrequency, debugLevel), m_stream(stream)
  {
  }

public:

  virtual ~GetF0StreamImpl() { delete m_stream; }

  long read_stream_samples(short* buffer, long num_records) override
  {
    m_stream->read(buffer, num_records);
    return num_records;
  }

private:
  IStream* m_stream;
};

} // namespace GetF0

#endif
