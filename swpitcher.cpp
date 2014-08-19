
#include <algorithm>
#include <iostream>
#include <vector>
#include <cstdio>
#include <cstring>
#include <cassert>

#include "GetF0/get_f0.h"

#include "GetF0VectorImpl.h"

namespace {

template <class SourceFormat, class DestFormat>
struct StaticCaster {
  DestFormat operator()(SourceFormat from)
  {
    return static_cast<DestFormat>(from);
  }
};

template <class SourceFormat, class DestFormat>
int readFile(const std::string& fileName, std::vector<DestFormat>& output)
{
  std::FILE* inputFile = std::fopen(fileName.c_str(), "rb");
  if (inputFile == nullptr) {
    std::perror("Cannot open input");
    return 1;
  }

  enum { BUFFER_SIZE = 4096 };
  SourceFormat buffer[BUFFER_SIZE];
  size_t readSize;

  do {
    readSize = std::fread(buffer, sizeof(SourceFormat), BUFFER_SIZE, inputFile);

    std::transform(buffer, buffer + readSize, std::back_inserter(output),
                   StaticCaster<SourceFormat, DestFormat>());

  } while (readSize == BUFFER_SIZE);

  if (std::ferror(inputFile) != 0) {
    std::perror("Error reading file");
    return 1;
  }

  if (std::fclose(inputFile) != 0) {
    std::perror("Error closing file");
    return 1;
  }

  return 0;
}

}  // namespace anonymous

int main(int argc, char* argv[])
{
  typedef short DiskSample;

  GetF0VectorImpl::SampleVector samples;
  if (readFile<DiskSample, GetF0VectorImpl::Sample>(argv[1], samples) != 0) {
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

  std::cout << std::endl;

  for (int i = 0; i < f0.m_outputVector.size(); ++i) {
    if (i > 0 && i % 10 == 0) {
      std::cout << std::endl;
    }

    std::cout << f0.m_outputVector[i] << " ";
  }
  std::cout << std::endl;

  return 0;
}
