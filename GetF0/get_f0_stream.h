#ifndef INCLUDED_GET_F0_STREAM
#define INCLUDED_GET_F0_STREAM

#include "get_f0.h"

namespace GetF0 {

/// The ESPS algorithm needs an overlap buffer. This handles the
/// buffer so we can calculate real time pitch from a stream.
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

} // namespace GetF0

#endif
