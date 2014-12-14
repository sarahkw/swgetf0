#include "config.h"

using namespace config;

Config::Config(const char* configData)
{
  schemeinterface::SchemeInterface si;
  si.loadResource(":/scheme/config.scm");
  si.load_string(configData);

  auto p = si.read_eval("(config/serialize-config)");

  schemeinterface::loadValues(p, audioConfig, uiConfig, uiMarkerLines,
                              espsConfig);
}
