
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

};



GetF0Stream::GetF0Stream(SampleFrequency sampleFrequency, DebugLevel debugLevel)
    : GetF0(sampleFrequency, debugLevel), m_buffer(nullptr)
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
}

long GetF0Stream::read_samples(Sample** buffer, long num_records)
{
  return 0;
}

long GetF0Stream::read_samples_overlap(Sample** buffer, long num_records,
                                       long step)
{
  return 0;
}

} // namespace GetF0
