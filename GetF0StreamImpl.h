#ifndef INCLUDED_GETF0STREAMIMPL
#define INCLUDED_GETF0STREAMIMPL

#include <algorithm>
#include <cstdio>

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
public:
  GetF0StreamImpl(FILE* file, SampleFrequency sampleFrequency,
                  DebugLevel debugLevel = 0)
      : GetF0Stream(sampleFrequency, debugLevel), m_file(file)
  {
  }

  virtual ~GetF0StreamImpl() {}

  long read_stream_samples(Sample* buffer, long num_records) override
  {
    SourceFormat tmpBuffer[num_records];
    auto readSize =
        std::fread(tmpBuffer, sizeof(SourceFormat), num_records, m_file);

    if (readSize != num_records && std::ferror(m_file)) {
      // TODO throw exception
    }

    std::transform(tmpBuffer, tmpBuffer + readSize, buffer,
                   StaticCaster<SourceFormat, Sample>());

    return readSize;
  }

private:
  FILE* m_file;
};

} // namespace GetF0

#endif
