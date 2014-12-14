#ifndef INCLUDED_CONFIG_H
#define INCLUDED_CONFIG_H

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
    schemeinterface::loadValues(sexp, width, height, note_width, min_note,
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
