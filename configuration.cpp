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

#include "configuration.h"
#include "ui_configuration.h"

#include "configuregetf0.h"
#include "config.h"

#include <QDebug>
#include <QMessageBox>

#include <portaudiocpp/System.hxx>
#include <portaudiocpp/SystemHostApiIterator.hxx>
#include <portaudiocpp/SystemDeviceIterator.hxx>


namespace {

template <class IndexMapType, class ItemIter, class ItemType, class Callable1,
          class Callable2>
static void insertWithDefault(QComboBox *comboBox, IndexMapType &map,
                              ItemIter begin, ItemIter end,
                              ItemType& defaultItem,
			      Callable1 stringGenerator,
                              Callable2 mapItemGenerator) {
  int itemPosition = 0;

  comboBox->clear();
  map.clear();

  {
    map[itemPosition] = mapItemGenerator(defaultItem);

    comboBox->insertItem(itemPosition++,
                         QLatin1String("Default: ") + stringGenerator(defaultItem));
  }

  comboBox->insertSeparator(itemPosition++);

  while (begin != end) {
    ItemType& item = *begin;

    map[itemPosition] = mapItemGenerator(item);

    comboBox->insertItem(itemPosition++, stringGenerator(item));

    ++begin;
  }

}

} // namespace anonymous

Configuration::Configuration(GetF0::GetF0& getf0, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Configuration),
    m_getf0(getf0)
{
  ui->setupUi(this);

  schemeinterface::GetDataFromResource defaultConfigScm(
      ":/scheme/default-config.scm");

  ui->txtConfig->setPlainText(
      QString::fromUtf8(defaultConfigScm.byteArray().data(),
                        defaultConfigScm.byteArray().size()));

  portaudio::System& sys = portaudio::System::instance();

  insertWithDefault(
      ui->cmbAudioHost, m_indexToHostApiTypeId, sys.hostApisBegin(),
      sys.hostApisEnd(), sys.defaultHostApi(),
      [](portaudio::HostApi &hostApi) { return hostApi.name(); },
      [](portaudio::HostApi &hostApi) { return hostApi.typeId(); });
}

PaDeviceIndex Configuration::getDeviceIndex() const
{
  auto index = ui->cmbInputDevice->currentIndex();
  Q_ASSERT(index != -1);
  return m_indexToDeviceIndex[index];
}

config::Config Configuration::getConfig() const
{
  return m_config;
}

portaudio::StreamParameters Configuration::getStreamParameters() const
{
  return m_streamParameters;
}

void Configuration::on_cmbAudioHost_currentIndexChanged(int index)
{
  if (index == -1)
    return;

  portaudio::HostApi &hostApi = portaudio::System::instance().hostApiByTypeId(
      m_indexToHostApiTypeId[index]);

  insertWithDefault(
      ui->cmbInputDevice, m_indexToDeviceIndex,
      hostApi.devicesBegin(), hostApi.devicesEnd(), hostApi.defaultInputDevice(),
      [](portaudio::Device &device) { return device.name(); },
      [](portaudio::Device &device) { return device.index(); });

}

void Configuration::on_buttonBox_accepted()
{
  QString configText = ui->txtConfig->toPlainText();
  QByteArray configTextBa = configText.toUtf8();

  try {
    m_config = config::Config(configTextBa.constData());
  } catch (const schemeinterface::SchemeException& e) {
      QMessageBox msgBox(this);
      msgBox.setText("Error loading configuration.");
      msgBox.setInformativeText(e.error());
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.exec();
      return;
  }

  {
    portaudio::System &sys = portaudio::System::instance();

    portaudio::Device &device = sys.deviceByIndex(getDeviceIndex());

    portaudio::DirectionSpecificStreamParameters isp(
        device,
        1,
        portaudio::INT16,
        true,
        device.defaultLowInputLatency(),
        NULL);

    m_streamParameters = portaudio::StreamParameters(
        isp,
        portaudio::DirectionSpecificStreamParameters::null(),
        m_config.audioConfig.sample_rate,
        paFramesPerBufferUnspecified,
        paNoFlag);


    if (!m_streamParameters.isSupported()) {
      // Test by setting sample rate to something absurd like -1.
      QMessageBox msgBox(this);
      msgBox.setText("Audio configuration not supported.");
      msgBox.setIcon(QMessageBox::Warning);
      msgBox.exec();
      return;
    }
  }

  ConfigureGetF0(m_getf0, m_config.espsConfig);

  try {
    m_getf0.checkParameters(m_config.audioConfig.sample_rate);
  } catch (const GetF0::ParameterError& e) {
    QMessageBox msgBox(this);
    msgBox.setText("Invalid ESPS config.");
    msgBox.setInformativeText(e.what());
    msgBox.setIcon(QMessageBox::Warning);
    msgBox.exec();
    return;
  }

  emit accept();
}

Configuration::~Configuration()
{
    delete ui;
}
