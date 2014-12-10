#include "configuregetf0.h"

#include "GetF0/get_f0.h"
#include "config.h"

void ConfigureGetF0(GetF0::GetF0& f0, const config::EspsConfig& espsConfig) {
  f0.paramCandThresh() = espsConfig.cand_thresh;
  f0.paramLagWeight()  = espsConfig.lag_weight;
  f0.paramFreqWeight() = espsConfig.freq_weight;
  f0.paramTransCost()  = espsConfig.trans_cost;
  f0.paramTransAmp()   = espsConfig.trans_amp;
  f0.paramTransSpec()  = espsConfig.trans_spec;
  f0.paramVoiceBias()  = espsConfig.voice_bias;
  f0.paramDoubleCost() = espsConfig.double_cost;
  f0.paramMinF0()      = espsConfig.min_f0;
  f0.paramMaxF0()      = espsConfig.max_f0;
  f0.paramFrameStep()  = espsConfig.frame_step;
  f0.paramWindDur()    = espsConfig.wind_dur;
  f0.paramNCands()     = espsConfig.n_cands;
}
