/*
  Copyright 2016 Sarah Wong

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

#include <QString>

#include <portaudiocpp/StreamParameters.hxx>

#include "config.h"

namespace GetF0 {
class GetF0;
}

#ifndef CONFIGURATIONGENERIC_H
#define CONFIGURATIONGENERIC_H

class ConfigurationGeneric {
public:
    static QString defaultConfiguration();

    ConfigurationGeneric(GetF0::GetF0& target) : m_target(target) {}

    ///
    /// @throws schemeinterface::SchemeException If there's an error loading the
    /// config
    /// @throws AudioConfigurationNotSupported
    /// @throws GetF0::ParameterError If ESPS says the config is invalid.
    ///
    void configure(const char* configScript, const portaudio::Device& device);

    const config::Config& config() const { return m_config; }
    const portaudio::StreamParameters& streamParameters() const
    {
        return m_streamParameters;
    }

    class AudioConfigurationNotSupported : public std::exception {};

private:

    GetF0::GetF0& m_target;

    config::Config m_config;
    portaudio::StreamParameters m_streamParameters;

    ///
    /// @throws AudioConfigurationNotSupported
    ///
    static portaudio::StreamParameters buildStreamParameters(
        const portaudio::Device& device, double sampleRate);
};

#endif // CONFIGURATIONGENERIC_H
