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

#include "configurationgeneric.h"

#include "GetF0/get_f0.h"
#include "config.h"
#include "configuregetf0.h"

#include <portaudiocpp/Device.hxx>
#include <portaudiocpp/DirectionSpecificStreamParameters.hxx>

QString ConfigurationGeneric::defaultConfiguration()
{
    schemeinterface::GetDataFromResource defaultConfigScm(
        ":/scheme/default-config.scm");
    return QString::fromUtf8(defaultConfigScm.byteArray().data(),
                             defaultConfigScm.byteArray().size());
}

void ConfigurationGeneric::configure(const char* configScript,
                                     const portaudio::Device& device)
{
    m_config = config::Config(configScript);
    m_streamParameters = ConfigurationGeneric::buildStreamParameters(
        device, m_config.audioConfig.sample_rate);
    ConfigureGetF0(m_target, m_config.espsConfig);
    m_target.checkParameters(m_config.audioConfig.sample_rate);
}

portaudio::StreamParameters ConfigurationGeneric::buildStreamParameters(
    const portaudio::Device& device, double sampleRate)
{
    portaudio::DirectionSpecificStreamParameters isp(
        device, 1, portaudio::INT16, true, device.defaultLowInputLatency(),
        NULL);
    portaudio::StreamParameters ret = portaudio::StreamParameters(
        isp, portaudio::DirectionSpecificStreamParameters::null(), sampleRate,
        paFramesPerBufferUnspecified, paNoFlag);
    if (!ret.isSupported()) {
        throw AudioConfigurationNotSupported();
    }
    return ret;
}
