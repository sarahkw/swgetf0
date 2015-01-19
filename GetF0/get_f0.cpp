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

#include "get_f0.h"

#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <limits.h>
#include <vector>

#include "f0.h"


// ----------------------------------------
// Externs
extern "C" {
int init_dp_f0(double freq, F0_params *par, long *buffsize, long *sdstep);
int dp_f0(float *fdata, int buff_size, int sdstep, double freq, F0_params *par,
          float **f0p_pt, float **vuvp_pt, float **rms_speech_pt,
          float **acpkp_pt, int *vecsize, int last_time);

int	    debug_level = 0;
}



namespace GetF0 {

GetF0::GetF0()
    : m_par(new f0_params),
      m_sampleFrequency(0),
      m_initialized(false),
      m_streamBufferSize(0),
      m_streamOverlapSize(0)
{
  resetParameters();
}

GetF0::~GetF0() { delete m_par; }

void GetF0::resetParameters()
{
  m_par->cand_thresh = 0.3;
  m_par->lag_weight = 0.3;
  m_par->freq_weight = 0.02;
  m_par->trans_cost = 0.005;
  m_par->trans_amp = 0.5;
  m_par->trans_spec = 0.5;
  m_par->voice_bias = 0.0;
  m_par->double_cost = 0.35;
  m_par->min_f0 = 50;
  m_par->max_f0 = 550;
  m_par->frame_step = 0.01;
  m_par->wind_dur = 0.0075;
  m_par->n_cands = 20;
}

void GetF0::init(SampleFrequency sampleFrequency)
{
  checkParameters(sampleFrequency);

  m_sampleFrequency = sampleFrequency;

  /*SW: Removed range restricter, but this may be interesting:
    if (total_samps < ((par->frame_step * 2.0) + par->wind_dur) * sf), then
      input range too small*/

  // double output_starts = m_par.wind_dur/2.0;
  /* Average delay due to loc. of ref. window center. */
  //   SW: I think this is the time delay until output actually
  //       starts. In other words, we'll have some dropped frames.

  /* Initialize variables in get_f0.c; allocate data structures;
   * determine length and overlap of input frames to read.
   *
   * sw: Looks like init_dp_f0 never returns errors via rcode, but put
   * under assertion.
   */
  THROW_ERROR(init_dp_f0(m_sampleFrequency, m_par, &m_streamBufferSize,
                         &m_streamOverlapSize) ||
                  m_streamBufferSize > INT_MAX || m_streamOverlapSize > INT_MAX,
              LogicError, "problem in init_dp_f0().");

  m_initialized = true;
}

void GetF0::run()
{
  THROW_ERROR(!m_initialized, LogicError, "Not initialized");

  float* fdata = nullptr;
  float* f0p, *vuvp, *rms_speech, *acpkp;
  int done;
  int vecsize;

  long actsize = read_samples(&fdata, m_streamBufferSize);

  while (true) {
    done = (actsize < m_streamBufferSize);

    THROW_ERROR(
        dp_f0(fdata, (int)actsize, (int)m_streamOverlapSize, m_sampleFrequency,
              m_par, &f0p, &vuvp, &rms_speech, &acpkp, &vecsize, done),
        ProcessingError, "problem in dp_f0().");

    write_output_reversed(f0p, vuvp, rms_speech, acpkp, vecsize);

    if (done) break;

    actsize =
        read_samples_overlap(&fdata, m_streamBufferSize, m_streamOverlapSize);
  }
}

void GetF0::checkParameters(SampleFrequency sampleFrequency)
{
  std::vector<std::string> errors;

  if ((m_par->cand_thresh < 0.01) || (m_par->cand_thresh > 0.99)) {
    errors.push_back("cand_thresh parameter must be between [0.01, 0.99].");
  }
  if ((m_par->wind_dur > .1) || (m_par->wind_dur < .0001)) {
    errors.push_back("wind_dur parameter must be between [0.0001, 0.1].");
  }
  if ((m_par->n_cands > 100) || (m_par->n_cands < 3)) {
    errors.push_back("n_cands parameter must be between [3,100].");
  }
  if ((m_par->max_f0 <= m_par->min_f0) ||
      (m_par->max_f0 >= (sampleFrequency / 2.0)) ||
      (m_par->min_f0 < (sampleFrequency / 10000.0))) {
    errors.push_back(
        "min(max)_f0 parameter inconsistent with sampling frequency.");
  }
  double dstep =
      ((double)((int)(0.5 + (sampleFrequency * m_par->frame_step)))) /
      sampleFrequency;
  if (dstep != m_par->frame_step) {
    if (debug_level)
      Fprintf(stderr,
              "Frame step set to %f to exactly match signal sample rate.\n",
              dstep);
    m_par->frame_step = dstep;
  }
  if ((m_par->frame_step > 0.1) ||
      (m_par->frame_step < (1.0 / sampleFrequency))) {
    errors.push_back(
        "frame_step parameter must be between [1/sampling rate, "
        "0.1].");
  }

  if (!errors.empty()) {
    std::stringstream ss;
    bool first = true;
    for (auto& error : errors) {
      if (!first) ss << "\n";
      ss << error;
      first = false;
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

double GetF0::pitchFrameRate() const
{
  return 1.0 / m_par->frame_step;
}

float& GetF0::paramCandThresh()   { return m_par->cand_thresh;    }
float& GetF0::paramLagWeight()    { return m_par->lag_weight;     }
float& GetF0::paramFreqWeight()   { return m_par->freq_weight;    }
float& GetF0::paramTransCost()    { return m_par->trans_cost;     }
float& GetF0::paramTransAmp()     { return m_par->trans_amp;      }
float& GetF0::paramTransSpec()    { return m_par->trans_spec;     }
float& GetF0::paramVoiceBias()    { return m_par->voice_bias;     }
float& GetF0::paramDoubleCost()   { return m_par->double_cost;    }
float& GetF0::paramMinF0()        { return m_par->min_f0;         }
float& GetF0::paramMaxF0()        { return m_par->max_f0;         }
float& GetF0::paramFrameStep()    { return m_par->frame_step;     }
float& GetF0::paramWindDur()      { return m_par->wind_dur;       }
int  & GetF0::paramNCands()       { return m_par->n_cands;        }


} // namespace GetF0
