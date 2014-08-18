
#include "get_f0.h"

namespace GetF0 {

class GetF0Stream : public GetF0 {
public:

  GetF0Stream(SampleFrequency sampleFrequency, DebugLevel debugLevel = 0)
      : GetF0(sampleFrequency, debugLevel){};

protected:

  long read_samples(Sample** buffer, long num_records) override;

  long read_samples_overlap(Sample** buffer, long num_records,
                            long step) override;
};

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
