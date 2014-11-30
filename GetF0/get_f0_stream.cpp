
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

  auto bytesFromStream = read_stream_samples(m_buffer, num_records);

  if (bytesFromStream != num_records) {
    m_eof = true;
  }

  *buffer = m_buffer;
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

  std::memmove(m_buffer, m_buffer + newSamples, oldSamples * sizeof(Sample));

  auto actualNewSamples = read_stream_samples(m_buffer + oldSamples, newSamples);
  if (actualNewSamples != newSamples) {
    m_eof = true;
  }

  *buffer = m_buffer;
  return oldSamples + actualNewSamples;
}

long GetF0Stream::read_stream_samples(Sample* buffer, long num_records)
{
  short tmpBuf[num_records];
  auto length = read_stream_samples(tmpBuf, num_records);
  for (int i = 0; i < length; ++i) {
    buffer[i] = static_cast<Sample>(tmpBuf[i]);
  }
  return length;
}

} // namespace GetF0
