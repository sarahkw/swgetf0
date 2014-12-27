/*
  Copyright 2014 Sarah Wong

  Licensed under the Apache License, Version 2.0 (the "License");
  you may not use this file except in compliance with the License.
  You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

  Unless required by applicable law or agreed to in writing, software
  distributed under the License is distributed on an "AS IS" BASIS,
  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  See the License for the specific language governing permissions and
  limitations under the License.
*/

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
