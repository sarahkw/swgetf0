/*
  Copyright 2014 Sarah Wong
*/

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
protected:

  struct IStream {
    virtual size_t read(void* ptr, size_t size, size_t nmemb) = 0;
    virtual int feof() = 0;
    virtual int ferror() = 0;
    virtual ~IStream() { }
  };

  struct FileStream : public IStream {
    FileStream(FILE* file) : m_file(file) {}

    size_t read(void* ptr, size_t size, size_t nmemb) override
    {
      return std::fread(ptr, size, nmemb, m_file);
    }

    int feof() override { return std::feof(m_file); }

    int ferror() override { return std::ferror(m_file); }

  private:
    FILE* m_file;
  };

public:

  GetF0StreamImpl(FILE* file, SampleFrequency sampleFrequency,
                  DebugLevel debugLevel = 0)
      : GetF0Stream(sampleFrequency, debugLevel), m_stream(new FileStream(file))
  {
  }

protected:

  GetF0StreamImpl(IStream* stream, SampleFrequency sampleFrequency,
                  DebugLevel debugLevel = 0)
      : GetF0Stream(sampleFrequency, debugLevel), m_stream(stream)
  {
  }

public:

  virtual ~GetF0StreamImpl() { delete m_stream; }

  long read_stream_samples(Sample* buffer, long num_records) override
  {
    SourceFormat tmpBuffer[num_records];

    long totalReadSize = 0;

    while (totalReadSize < num_records) {
      auto requestSize = num_records - totalReadSize;

      auto readSize = m_stream->read(tmpBuffer + totalReadSize,
                                     sizeof(SourceFormat), requestSize);
      totalReadSize += readSize;

      if (readSize != requestSize) {
        if (errno == EINTR) {
          continue;
        }
        else if (m_stream->feof()) {
          break;
        }

        THROW_ERROR(m_stream->ferror(), RuntimeError,
                    "fread returned errno " << std::strerror(errno));
      }
    }

    std::transform(tmpBuffer, tmpBuffer + totalReadSize, buffer,
                   StaticCaster<SourceFormat, Sample>());

    return totalReadSize;
  }

private:
  IStream* m_stream;
};

} // namespace GetF0

#endif
