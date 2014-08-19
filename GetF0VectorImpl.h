#ifndef INCLUDED_GETF0VECTORIMPL
#define INCLUDED_GETF0VECTORIMPL

#include <vector>
#include "GetF0/get_f0.h"

class GetF0VectorImpl : public GetF0::GetF0 {
public:
  typedef std::vector<Sample> SampleVector;
  typedef std::vector<float> OutputVector;

  GetF0VectorImpl(SampleFrequency sampleFrequency, SampleVector& samples);

protected:
  long read_samples(float** buffer, long num_records) override;

  long read_samples_overlap(float** buffer, long num_records,
                            long step) override;

  void write_output_reversed(float* f0p, float* vuvp, float* rms_speech,
                             float* acpkp, int vecsize) override;

private:
  SampleVector& m_samples;

  unsigned m_position;

public:
  OutputVector m_outputVector;
};

#endif
