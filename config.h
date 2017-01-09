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

#ifndef INCLUDED_CONFIG
#define INCLUDED_CONFIG

#include <QPen>
#include <QList>

#include "schemeinterface.h"

namespace config {

struct AudioConfig {

  AudioConfig() {}
  AudioConfig(schemeinterface::PtrIter sexp) {
    schemeinterface::loadValues(sexp, sample_rate);
  }

  long sample_rate;

};

struct UiConfig {

  UiConfig() {}
  UiConfig(schemeinterface::PtrIter sexp) {
      schemeinterface::loadValues(sexp, width, height, maximized, note_width,
                                  min_note, max_note);
  }

  long width;
  long height;
  bool maximized;
  long note_width;
  long min_note;
  long max_note;

};

struct UiMarkerLines {

  struct Line {

    Line() {}

    Line(schemeinterface::Ptr ptr) {
      long r, g, b;
      schemeinterface::loadValues(schemeinterface::PtrIter(ptr), frequency, r,
                                  g, b);
      pen = QPen(QColor(r, g, b));
    }

    double frequency;
    QPen pen;

  };

  QList<Line> lines;

  UiMarkerLines() {}
  UiMarkerLines(schemeinterface::PtrIter sexp) {
    std::copy(sexp, sexp.end(), std::back_inserter(lines));
  }

};

struct EspsConfig {

  EspsConfig() {}
  EspsConfig(schemeinterface::PtrIter sexp) {
    schemeinterface::loadValues(sexp, cand_thresh, lag_weight, freq_weight,
                                trans_cost, trans_amp, trans_spec, voice_bias,
                                double_cost, min_f0, max_f0, frame_step,
                                wind_dur, n_cands);
  }

  double cand_thresh;
  double lag_weight;
  double freq_weight;
  double trans_cost;
  double trans_amp;
  double trans_spec;
  double voice_bias;
  double double_cost;
  double min_f0;
  double max_f0;
  double frame_step;
  double wind_dur;
  long n_cands;

};

struct Config {

  Config() {}

  Config(const char* configData);

  AudioConfig audioConfig;
  UiConfig uiConfig;
  UiMarkerLines uiMarkerLines;
  EspsConfig espsConfig;

};

} //namespace config

#endif
