
#include <algorithm>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cassert>

#include "GetF0/get_f0.h"

#include "GetF0VectorImpl.h"
#include "GetF0StreamImpl.h"

namespace {

template <class SourceFormat, class DestFormat>
struct StaticCaster {
  DestFormat operator()(SourceFormat from)
  {
    return static_cast<DestFormat>(from);
  }
};

template <class SourceFormat, class DestFormat>
int readSamples(std::vector<DestFormat>& output)
{
  enum { BUFFER_SIZE = 4096 };
  SourceFormat buffer[BUFFER_SIZE];
  size_t readSize;

  do {
    readSize = std::fread(buffer, sizeof(SourceFormat), BUFFER_SIZE, stdin);

    std::transform(buffer, buffer + readSize, std::back_inserter(output),
                   StaticCaster<SourceFormat, DestFormat>());

  } while (readSize == BUFFER_SIZE);

  if (std::ferror(stdin) != 0) {
    std::perror("Error reading samples");
    return 1;
  }

  return 0;
}

}  // namespace anonymous

typedef short DiskSample;

int implementation_vector()
{
  using GetF0::GetF0VectorImpl;

  GetF0VectorImpl::SampleVector samples;
  if (readSamples<DiskSample, GetF0VectorImpl::Sample>(samples) != 0) {
    return 1;
  }

  std::cout << "Read " << samples.size() << " samples." << std::endl;

  GetF0VectorImpl::SampleFrequency freq = 16000;
  GetF0VectorImpl f0(freq, samples);
  f0.init();
  f0.run();

  std::cout << "Returned " << f0.m_outputVector.size() << " data points."
            << std::endl;
  std::cout << "streamBufferSize " << f0.streamBufferSize() << std::endl;
  std::cout << "streamOverlapSize " << f0.streamOverlapSize() << std::endl;

  std::cout << std::endl; // Blank line for debug drawer

  for (int i = 0; i < f0.m_outputVector.size(); ++i) {
    if (i > 0 && i % 10 == 0) {
      std::cout << std::endl;
    }

    std::cout << f0.m_outputVector[i] << " ";
  }
  std::cout << std::endl;

  return 0;
}

int implementation_stream()
{
  using GetF0::GetF0StreamImpl;

  class Foo : public GetF0StreamImpl<DiskSample> {
  public:
    Foo() : GetF0StreamImpl<DiskSample>(stdin, 16000) {}

    void write_output_reversed(float* f0p, float* vuvp, float* rms_speech,
                               float* acpkp, int vecsize) override
    {
      for (int i = vecsize; i >= 0; --i) {
	std::cout << f0p[i] << " ";
      }
    }

  } f0;

  std::cout << std::endl; // Blank line for debug drawer

  f0.init();
  f0.run();

  return 0;
}

int main(int argc, char* argv[])
{
  auto implementation = atoi(argv[1]);

  if (implementation == 0) {
    return implementation_vector();
  }
  else if (implementation == 1) {
    return implementation_stream();
  }
  else {
    std::cerr << "Bad implementation." << std::endl;
    return 1;
  }
}
