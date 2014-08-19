#include "GetF0VectorImpl.h"

#include <algorithm>

GetF0VectorImpl::GetF0VectorImpl(SampleFrequency sampleFrequency,
                                 SampleVector& samples)
    : GetF0(sampleFrequency), m_samples(samples), m_position(0)
{
}

long GetF0VectorImpl::read_samples(float** buffer, long num_records)
{
  auto range = std::min<long>(num_records, m_samples.size() - m_position);
  *buffer = &m_samples[m_position];
  m_position += range;
  return range;
}

long GetF0VectorImpl::read_samples_overlap(float** buffer, long num_records,
                                           long step)
{
  // (num_records - step) old records
  // (step) new records

  m_position -= (num_records - step);
  return read_samples(buffer, num_records);
}

void GetF0VectorImpl::write_output_reversed(float* f0p, float* vuvp,
                                            float* rms_speech, float* acpkp,
                                            int vecsize)
{
  std::reverse_copy(f0p, f0p + vecsize, std::back_inserter(m_outputVector));
}
