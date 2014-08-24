#ifndef INCLUDED_GETF0STREAMIMPL
#define INCLUDED_GETF0STREAMIMPL

#include <algorithm>
#include <cstdio>
#include <cstring>
#include <cerrno>

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

    long totalReadSize = 0;

    while (totalReadSize < num_records) {
      auto requestSize = num_records - totalReadSize;

      auto readSize = std::fread(tmpBuffer + totalReadSize,
                                 sizeof(SourceFormat), requestSize, m_file);
      totalReadSize += readSize;

      if (readSize != requestSize) {
        if (errno == EINTR) {
          continue;
        }
        else if (std::feof(m_file)) {
          break;
        }

        THROW_ERROR(std::ferror(m_file), RuntimeError,
                    "fread returned errno " << std::strerror(errno));
      }
    }

    std::transform(tmpBuffer, tmpBuffer + totalReadSize, buffer,
                   StaticCaster<SourceFormat, Sample>());

    return totalReadSize;
  }

private:
  FILE* m_file;
};

} // namespace GetF0

#endif
