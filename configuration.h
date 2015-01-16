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

#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <QDialog>
#include <QMap>

#include <portaudiocpp/Device.hxx>
#include <portaudiocpp/HostApi.hxx>
#include <portaudiocpp/StreamParameters.hxx>

#include "GetF0/get_f0.h"
#include "config.h"

namespace Ui {
class Configuration;
}

class Configuration : public QDialog
{
  Q_OBJECT

public:
  explicit Configuration(GetF0::GetF0& getf0, QWidget *parent = 0);
  ~Configuration();

  PaDeviceIndex getDeviceIndex() const;

  config::Config getConfig() const;

  portaudio::StreamParameters getStreamParameters() const;

public slots:

  void on_cmbAudioHost_currentIndexChanged(int index);

  void on_buttonBox_accepted();

private:
  Ui::Configuration *ui;

  GetF0::GetF0& m_getf0;

  QMap<int, PaHostApiTypeId> m_indexToHostApiTypeId;
  QMap<int, PaDeviceIndex> m_indexToDeviceIndex;

  config::Config m_config;

  portaudio::StreamParameters m_streamParameters;
};

#endif // CONFIGURATION_H
