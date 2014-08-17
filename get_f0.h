#ifndef INCLUDED_GET_F0_H
#define INCLUDED_GET_F0_H

#include <sstream>
#include <stdexcept>

struct f0_params;

namespace GetF0 {

// EXCEPTIONS

#define CREATE_ERROR(_Name, _Base)                                   \
  class _Name : public _Base {                                       \
  public:                                                            \
    explicit _Name(const std::string &what_arg) : _Base(what_arg) {} \
  };

CREATE_ERROR(RuntimeError, std::runtime_error);
CREATE_ERROR(LogicError, std::logic_error);
CREATE_ERROR(ParameterError, RuntimeError);
CREATE_ERROR(ProcessingError, RuntimeError);

#undef CREATE_ERROR


#define THROW_ERROR(condition, exception, s) \
  do {                                       \
    if (condition) {                         \
      std::stringstream ss;                  \
      ss << s;                               \
      throw exception(ss.str());             \
    }                                        \
  } while (0);



/// Wraps ESPS get_f0. Make sure you only have one of these active in
/// each address space because it uses global memory.
class GetF0 {
public:

  typedef double SampleFrequency;
  typedef int DebugLevel;

  GetF0(SampleFrequency sampleFrequency, DebugLevel debugLevel = 0);
  virtual ~GetF0();

  void resetParameters();

  /// @brief Some consistency checks on parameter values. Throws
  /// ParameterError if there's something wrong.
  void checkParameters();

  void init();
  void run();

  // ----------------------------------------
  // Calculations available after init
  long streamBufferSize() const;
  long streamOverlapSize() const;

  // ----------------------------------------
  // Getters/setters

  float& paramCandThresh();   // only correlation peaks above this are considered
  float& paramLagWeight();    // degree to which shorter lags are weighted
  float& paramFreqWeight();   // weighting given to F0 trajectory smoothness
  float& paramTransCost();    // fixed cost for a voicing-state transition
  float& paramTransAmp();     // amplitude-change-modulated VUV trans. cost
  float& paramTransSpec();    // spectral-change-modulated VUV trans. cost
  float& paramVoiceBias();    // fixed bias towards the voiced hypothesis
  float& paramDoubleCost();   // cost for octave F0 jumps
  float& paramMeanF0();       // talker-specific mean F0 (Hz)
  float& paramMeanF0Weight(); // weight to be given to deviations from mean F0
  float& paramMinF0();        // min. F0 to search for (Hz)
  float& paramMaxF0();        // max. F0 to search for (Hz)
  float& paramFrameStep();    // inter-frame-interval (sec)
  float& paramWindDur();      // duration of correlation window (sec)
  int  & paramNCands();       // max. # of F0 cands. to consider at each frame
  int  & paramConditioning(); // Specify optional signal pre-conditioning.

  SampleFrequency &sampleFrequency() { return m_sampleFrequency; };
  DebugLevel &debugLevel() { return m_debugLevel; };

protected:

  /// @brief Provide a `buffer` we can read `num_records` samples
  /// from, returning how many samples we can read. Returning less
  /// than requested samples is a termination condition.
  ///
  /// `buffer` is not guaranteed to not be written to. (TODO: check to
  /// see if buffer can be written to.)
  virtual long read_samples(float **buffer, long num_records) = 0;

  /// @brief Like `read_samples`, but read `step` samples from
  /// previous buffer.
  virtual long read_samples_overlap(float** buffer, long num_records,
                                    long step) = 0;

  virtual void write_output(float* f0p, float* vuvp, float* rms_speech,
                            float* acpkp, int vecsize) = 0;

private:

  f0_params* m_par;
  SampleFrequency m_sampleFrequency;
  DebugLevel m_debugLevel;

  bool m_initialized;

  long m_streamBufferSize;
  long m_streamOverlapSize;

};

} // namespace

#endif
