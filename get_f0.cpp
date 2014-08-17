/*
 * This material contains unpublished, proprietary software of 
 * Entropic Research Laboratory, Inc. Any reproduction, distribution, 
 * or publication of this work must be authorized in writing by Entropic 
 * Research Laboratory, Inc., and must bear the notice: 
 *
 *    "Copyright (c) 1990-1996 Entropic Research Laboratory, Inc. 
 *                   All rights reserved"
 *
 * The copyright notice above does not evidence any actual or intended 
 * publication of this source code.     
 *
 * Written by:  Derek Lin
 * Checked by:
 * Revised by:  David Talkin
 *
 * Brief description:  Estimates F0 using normalized cross correlation and
 *   dynamic programming.
 *
 */

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <stdexcept>
#include <sstream>
#include <vector>

#include "f0.h"

int	    debug_level = 0;

// ----------------------------------------
// Externs
extern "C" {
int init_dp_f0(double freq, F0_params *par, long *buffsize, long *sdstep);
int dp_f0(float *fdata, int buff_size, int sdstep, double freq, F0_params *par,
          float **f0p_pt, float **vuvp_pt, float **rms_speech_pt,
          float **acpkp_pt, int *vecsize, int last_time);
}



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



class GetF0 {
public:

  typedef double SampleFrequency;
  typedef int DebugLevel;

  GetF0(SampleFrequency sampleFrequency, DebugLevel debugLevel = 0);

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

  float& paramCandThresh()   { return m_par.cand_thresh;    } // only correlation peaks above this are considered
  float& paramLagWeight()    { return m_par.lag_weight;     } // degree to which shorter lags are weighted
  float& paramFreqWeight()   { return m_par.freq_weight;    } // weighting given to F0 trajectory smoothness
  float& paramTransCost()    { return m_par.trans_cost;     } // fixed cost for a voicing-state transition
  float& paramTransAmp()     { return m_par.trans_amp;      } // amplitude-change-modulated VUV trans. cost
  float& paramTransSpec()    { return m_par.trans_spec;     } // spectral-change-modulated VUV trans. cost
  float& paramVoiceBias()    { return m_par.voice_bias;     } // fixed bias towards the voiced hypothesis
  float& paramDoubleCost()   { return m_par.double_cost;    } // cost for octave F0 jumps
  float& paramMeanF0()       { return m_par.mean_f0;        } // talker-specific mean F0 (Hz)
  float& paramMeanF0Weight() { return m_par.mean_f0_weight; } // weight to be given to deviations from mean F0
  float& paramMinF0()        { return m_par.min_f0;         } // min. F0 to search for (Hz)
  float& paramMaxF0()        { return m_par.max_f0;         } // max. F0 to search for (Hz)
  float& paramFrameStep()    { return m_par.frame_step;     } // inter-frame-interval (sec)
  float& paramWindDur()      { return m_par.wind_dur;       } // duration of correlation window (sec)
  int  & paramNCands()       { return m_par.n_cands;        } // max. # of F0 cands. to consider at each frame
  int  & paramConditioning() { return m_par.conditioning;   } // Specify optional signal pre-conditioning.

  SampleFrequency &sampleFrequency() { return m_sampleFrequency; };
  DebugLevel &debugLevel() { return m_debugLevel; };

protected:

  /// @brief Provide a `buffer` we can read `num_records` samples
  /// from, returning how many samples we can read. Returning less
  /// than requested samples is a termination condition.
  ///
  /// `buffer` is not guaranteed to not be written to. (TODO: check to
  /// see if buffer can be written to.)
  virtual long read_samples(float **buffer, long num_records) { return 0; }

  /// @brief Like `read_samples`, but read `step` samples from
  /// previous buffer.
  virtual long read_samples_overlap(float **buffer, long num_records, long step)
  {
    return 0;
  }

  virtual void writeOutput(float *f0p, float *vuvp, float *rms_speech,
                           float *acpkp, int vecsize)
  {
  }

private:

  f0_params m_par;
  SampleFrequency m_sampleFrequency;
  DebugLevel m_debugLevel;

  bool m_initialized;

  long m_streamBufferSize;
  long m_streamOverlapSize;

};

GetF0::GetF0(SampleFrequency sampleFrequency, DebugLevel debugLevel)
    : m_sampleFrequency(sampleFrequency),
      m_debugLevel(debugLevel),
      m_initialized(false),
      m_streamBufferSize(0),
      m_streamOverlapSize(0)
{
  resetParameters();
}

void GetF0::resetParameters()
{
  m_par.cand_thresh = 0.3;
  m_par.lag_weight = 0.3;
  m_par.freq_weight = 0.02;
  m_par.trans_cost = 0.005;
  m_par.trans_amp = 0.5;
  m_par.trans_spec = 0.5;
  m_par.voice_bias = 0.0;
  m_par.double_cost = 0.35;
  m_par.min_f0 = 50;
  m_par.max_f0 = 550;
  m_par.frame_step = 0.01;
  m_par.wind_dur = 0.0075;
  m_par.n_cands = 20;
  m_par.mean_f0 = 200;        /* unused */
  m_par.mean_f0_weight = 0.0; /* unused */
  m_par.conditioning = 0;     /*unused */
}

void GetF0::init()
{
  checkParameters();

  /*SW: Removed range restricter, but this may be interesting:
    if (total_samps < ((par->frame_step * 2.0) + par->wind_dur) * sf), then
      input range too small*/

  // double output_starts = m_par.wind_dur/2.0;
  /* Average delay due to loc. of ref. window center. */
  //   SW: I think this is the time delay until output actually
  //       starts. In other words, we'll have some dropped frames.

  // double frame_rate = 1.0 / m_par.frame_step;

  /* Initialize variables in get_f0.c; allocate data structures;
   * determine length and overlap of input frames to read.
   *
   * sw: Looks like init_dp_f0 never returns errors via rcode, but put
   * under assertion.
   */
  THROW_ERROR(init_dp_f0(m_sampleFrequency, &m_par, &m_streamBufferSize,
                         &m_streamOverlapSize) ||
                  m_streamBufferSize > INT_MAX || m_streamOverlapSize > INT_MAX,
              LogicError, "problem in init_dp_f0().");

  m_initialized = true;
}

void GetF0::run()
{
  THROW_ERROR(!m_initialized, LogicError, "Not initialized");

  float *fdata = nullptr;
  float *f0p, *vuvp, *rms_speech, *acpkp;
  int done;
  int i, vecsize;

  long actsize = read_samples(&fdata, m_streamBufferSize);

  while (true) {
    done = (actsize < m_streamBufferSize);

    THROW_ERROR(
        dp_f0(fdata, (int)actsize, (int)m_streamOverlapSize, m_sampleFrequency,
              &m_par, &f0p, &vuvp, &rms_speech, &acpkp, &vecsize, done),
        ProcessingError, "problem in dp_f0().");

    writeOutput(f0p, vuvp, rms_speech, acpkp, vecsize);

    if (done) break;

    actsize =
        read_samples_overlap(&fdata, m_streamBufferSize, m_streamOverlapSize);
  }
}

void GetF0::checkParameters()
{
  std::vector<std::string> errors;

  if ((m_par.cand_thresh < 0.01) || (m_par.cand_thresh > 0.99)) {
    errors.push_back("cand_thresh parameter must be between [0.01, 0.99].");
  }
  if ((m_par.wind_dur > .1) || (m_par.wind_dur < .0001)) {
    errors.push_back("wind_dur parameter must be between [0.0001, 0.1].");
  }
  if ((m_par.n_cands > 100) || (m_par.n_cands < 3)) {
    errors.push_back("n_cands parameter must be between [3,100].");
  }
  if ((m_par.max_f0 <= m_par.min_f0) ||
      (m_par.max_f0 >= (m_sampleFrequency / 2.0)) ||
      (m_par.min_f0 < (m_sampleFrequency / 10000.0))) {
    errors.push_back(
        "min(max)_f0 parameter inconsistent with sampling frequency.");
  }
  double dstep =
      ((double)((int)(0.5 + (m_sampleFrequency * m_par.frame_step)))) /
      m_sampleFrequency;
  if (dstep != m_par.frame_step) {
    if (debug_level)
      Fprintf(stderr,
              "Frame step set to %f to exactly match signal sample rate.\n",
              dstep);
    m_par.frame_step = dstep;
  }
  if ((m_par.frame_step > 0.1) ||
      (m_par.frame_step < (1.0 / m_sampleFrequency))) {
    errors.push_back(
        "frame_step parameter must be between [1/sampling rate, "
        "0.1].");
  }

  if (!errors.empty()) {
    std::stringstream ss;
    bool first = true;
    for (auto &error : errors) {
      if (!first) ss << " ";
      ss << error;
    }

    THROW_ERROR(true, ParameterError, ss.str());
  }
}

long GetF0::streamBufferSize() const
{
  THROW_ERROR(!m_initialized, LogicError, "Not initialized");
  return m_streamBufferSize;
}

long GetF0::streamOverlapSize() const
{
  THROW_ERROR(!m_initialized, LogicError, "Not initialized");
  return m_streamOverlapSize;
}

} // namespace GetF0
