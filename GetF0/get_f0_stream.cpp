
#include "get_f0_stream.h"

#include <cstring>

namespace GetF0 {

GetF0Stream::GetF0Stream(SampleFrequency sampleFrequency, DebugLevel debugLevel)
    : GetF0(sampleFrequency, debugLevel), m_buffer(nullptr), m_eof(false)
{
}

GetF0Stream::~GetF0Stream()
{
  if (m_buffer != nullptr)
    delete [] m_buffer;
}

void GetF0Stream::init()
{
  GetF0::init();

  m_buffer = new Sample[streamBufferSize()];

  // If this isn't true we can have out of bounds array access.
  THROW_ERROR(streamBufferSize() < streamOverlapSize(), LogicError,
              "streamBufferSize() needs to be larger than streamOverlapSize()");
}

long GetF0Stream::read_samples(Sample** buffer, long num_records)
{
  THROW_ERROR(streamBufferSize() != num_records, LogicError,
              "not implemented: we can only read streamBufferSize");

  THROW_ERROR(m_eof, LogicError, "still reading after EOF");

  auto bytesFromStream = read_stream_samples(buffer, num_records);

  if (bytesFromStream == num_records) {
    std::memcpy(m_buffer + streamOverlapSize(), *buffer + streamOverlapSize(),
                streamBufferSize() - streamOverlapSize());
  }
  else {
    m_eof = true;
  }

  return bytesFromStream;
}

long GetF0Stream::read_samples_overlap(Sample** buffer, long num_records,
                                       long step)
{
  THROW_ERROR(streamBufferSize() != num_records || streamOverlapSize() != step,
              LogicError,
              "not implemented: we can only read streamBufferSize and overlap "
              "streamOverlapSize");  // TODO not sure if we need to be so
                                     // strict, have to
                                     // think about it

  THROW_ERROR(m_eof, LogicError, "still reading after EOF");

  // TODO assert if we didn't read_samples yet

  auto newSamples = streamOverlapSize();
  auto oldSamples = streamBufferSize() - streamOverlapSize();

  Sample* incomingBuffer;
  auto actualNewSamples = read_stream_samples(&incomingBuffer, newSamples);

  if (actualNewSamples != newSamples) {
    m_eof = true;
  }

  std::memmove(m_buffer, m_buffer + newSamples, oldSamples);
  std::memcpy(m_buffer + oldSamples, incomingBuffer, actualNewSamples);

  return oldSamples + actualNewSamples;
}

} // namespace GetF0
