#ifndef INCLUDED_CONFIG_H
#define INCLUDED_CONFIG_H

#include <QColor>
#include <QList>

#include "schemeconfig.h"

namespace config {

struct AudioConfig {

  AudioConfig() {}
  AudioConfig(schemeconfig::PtrIter sexp) {
    schemeconfig::loadValues(sexp, sample_rate);
  }

  long sample_rate;

};

struct UiConfig {

  UiConfig() {}
  UiConfig(schemeconfig::PtrIter sexp) {
    schemeconfig::loadValues(sexp, width, height, note_width, min_note,
                             max_note);
  }

  long width;
  long height;
  long note_width;
  long min_note;
  long max_note;

};

struct UiMarkerLines {

  struct Line {

    Line() {}

    Line(schemeconfig::Ptr ptr) {
      long r, g, b;
      schemeconfig::loadValues(schemeconfig::PtrIter(ptr), frequency, r, g, b);
      color = QColor(r, g, b);
    }

    double frequency;
    QColor color;

  };

  QList<Line> lines;

  UiMarkerLines() {}
  UiMarkerLines(schemeconfig::PtrIter sexp) {
    std::copy(sexp, sexp.end(), std::back_inserter(lines));
  }

};

struct EspsConfig {

  EspsConfig() {}
  EspsConfig(schemeconfig::PtrIter sexp) {
    schemeconfig::loadValues(sexp, cand_thresh, lag_weight, freq_weight,
                             trans_cost, trans_amp, trans_spec, voice_bias,
                             double_cost, min_f0, max_f0, frame_step, wind_dur,
                             n_cands);
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

  Config() { }
  Config(schemeconfig::PtrIter sexp) {
    schemeconfig::loadValues(sexp, audioConfig, uiConfig, uiMarkerLines, espsConfig);
  }

  AudioConfig audioConfig;
  UiConfig uiConfig;
  UiMarkerLines uiMarkerLines;
  EspsConfig espsConfig;

};

} //namespace config

#endif
