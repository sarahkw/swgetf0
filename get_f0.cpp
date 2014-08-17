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
CREATE_ERROR(AssertionError, LogicError);

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
  GetF0();

  int derp();

protected:

  /// @brief Provide a `buffer` we can read `num_records` samples
  /// from, returning how many samples we can read. Returning less
  /// than requested samples is a termination condition.
  ///
  /// `buffer` is not guaranteed to not be written to. (TODO: check to
  /// see if buffer can be written to.)
  virtual long readSamples(float **buffer, long num_records) { return 0; }

  /// @brief Like `readSamples`, but read `step` samples from previous
  /// buffer.
  virtual long readSamplesOverlap(float **buffer, long num_records, long step)
  {
    return 0;
  }

  virtual void writeOutput(float *f0p, float *vuvp, float *rms_speech,
                           float *acpkp, int vecsize)
  {
  }

private:

  F0_params m_par;

  void init_params();

  static void check_f0_params(F0_params *par, double sample_freq);

};

GetF0::GetF0()
{
  init_params();
}

void GetF0::init_params()
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

int GetF0::derp()
{
  int done;
  long buff_size, actsize;
  double sf, output_starts, frame_rate;
  float *f0p, *vuvp, *rms_speech, *acpkp;
  int i, vecsize;
  long sdstep = 0;


#define SW_CUSTOMIZABLE(x) //TODO(sw)
  SW_CUSTOMIZABLE(debug_level);

  SW_CUSTOMIZABLE(m_par.frame_step);
  SW_CUSTOMIZABLE(m_par.cand_thresh);
  SW_CUSTOMIZABLE(m_par.lag_weight);
  SW_CUSTOMIZABLE(m_par.freq_weight);
  SW_CUSTOMIZABLE(m_par.trans_cost);
  SW_CUSTOMIZABLE(m_par.trans_amp);
  SW_CUSTOMIZABLE(m_par.trans_spec);
  SW_CUSTOMIZABLE(m_par.voice_bias);
  SW_CUSTOMIZABLE(m_par.double_cost);
  SW_CUSTOMIZABLE(m_par.min_f0);
  SW_CUSTOMIZABLE(m_par.max_f0);
  SW_CUSTOMIZABLE(m_par.wind_dur);
  SW_CUSTOMIZABLE(m_par.n_cands);
#undef SW_CUSTOMIZABLE

#define SW_FILE_PARAMS(x, y) //TODO (sw)
  SW_FILE_PARAMS(sf, "sampling frequency");
#undef SW_FILE_PARAMS

  check_f0_params(&m_par, sf);

  /*SW: Removed range restricter, but this may be interesting:
    if (total_samps < ((par->frame_step * 2.0) + par->wind_dur) * sf), then
      input range too small*/

  output_starts = m_par.wind_dur/2.0;
  /* Average delay due to loc. of ref. window center. */
  frame_rate = 1.0 / m_par.frame_step;


  /* Initialize variables in get_f0.c; allocate data structures;
   * determine length and overlap of input frames to read.
   *
   * sw: Looks like init_dp_f0 never returns errors via rcode, but put
   * under assertion.
   */
  THROW_ERROR(init_dp_f0(sf, &m_par, &buff_size, &sdstep) ||
                  buff_size > INT_MAX || sdstep > INT_MAX,
              AssertionError, "problem in init_dp_f0().");

  /*SW: pass sdstep to caller so it knows how much we have to buffer. */

  if (debug_level)
    Fprintf(stderr, "init_dp_f0 returned buff_size %ld, sdstep %ld.\n",
	    buff_size, sdstep);

  float* fdata = nullptr;
  actsize = readSamples(&fdata, buff_size);

  while (1) {

    done = (actsize < buff_size);

    THROW_ERROR(dp_f0(fdata, (int)actsize, (int)sdstep, sf, &m_par, &f0p, &vuvp,
                      &rms_speech, &acpkp, &vecsize, done),
                ProcessingError, "problem in dp_f0().");

    writeOutput(f0p, vuvp, rms_speech, acpkp, vecsize);

    if (done)
      break;

    actsize = readSamplesOverlap(&fdata, buff_size, sdstep);

  }

  exit(0);
}

/*
 * Some consistency checks on parameter values.
 */
void GetF0::check_f0_params(F0_params *par, double sample_freq)
{
  std::vector<std::string> errors;

  if ((par->cand_thresh < 0.01) || (par->cand_thresh > 0.99)) {
    errors.push_back("cand_thresh parameter must be between [0.01, 0.99].");
  }
  if ((par->wind_dur > .1) || (par->wind_dur < .0001)) {
    errors.push_back("wind_dur parameter must be between [0.0001, 0.1].");
  }
  if ((par->n_cands > 100) || (par->n_cands < 3)) {
    errors.push_back("n_cands parameter must be between [3,100].");
  }
  if ((par->max_f0 <= par->min_f0) || (par->max_f0 >= (sample_freq / 2.0)) ||
      (par->min_f0 < (sample_freq / 10000.0))) {
    errors.push_back(
        "min(max)_f0 parameter inconsistent with sampling frequency.");
  }
  double dstep =
      ((double)((int)(0.5 + (sample_freq * par->frame_step)))) / sample_freq;
  if (dstep != par->frame_step) {
    if (debug_level)
      Fprintf(stderr,
              "Frame step set to %f to exactly match signal sample rate.\n",
              dstep);
    par->frame_step = dstep;
  }
  if ((par->frame_step > 0.1) || (par->frame_step < (1.0 / sample_freq))) {
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

} // namespace GetF0
