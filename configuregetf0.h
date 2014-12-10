#ifndef INCLUDED_CONFIGUREGETF0_H
#define INCLUDED_CONFIGUREGETF0_H

namespace GetF0 {
class GetF0;
}

namespace config {
class EspsConfig;
}

void ConfigureGetF0(GetF0::GetF0& f0, const config::EspsConfig& espsConfig);

#endif
