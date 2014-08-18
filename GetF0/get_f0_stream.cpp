
#include <cstring>

#include "get_f0.h"

namespace GetF0 {

class GetF0Stream : public GetF0 {
public:

  GetF0Stream(SampleFrequency sampleFrequency, DebugLevel debugLevel = 0);

  virtual ~GetF0Stream();

  void init() override;

protected:

  long read_samples(Sample** buffer, long num_records) override;

  long read_samples_overlap(Sample** buffer, long num_records,
                            long step) override;

  virtual long read_stream_samples(Sample** buffer, long num_records) = 0;

private:

  Sample* m_buffer;

  bool m_eof;

};



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
              "not implemented: read samples request of length less than "
              "streamBufferSize");

  THROW_ERROR(m_eof, LogicError, "still reading after EOF");

  auto bytesFromStream = read_stream_samples(buffer, num_records);

  if (bytesFromStream == num_records) {
    // Not EOF; we probably might get a read next with an overlap so
    // prepare for it.

    // We'll be doing a memmove later
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
  THROW_ERROR(
      streamBufferSize() != num_records || streamOverlapSize() != step,
      LogicError,
      "not implemented: read samples overlap request of length less than "
      "streamBufferSize");  // TODO not sure if we need to be so strict, have to
                            // think about it

  THROW_ERROR(m_eof, LogicError, "still reading after EOF");

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
