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

#include "configurationdialog.h"
#include "ui_configurationdialog.h"

#include "config.h"
#include "configurationgeneric.h"

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

ConfigurationDialog::ConfigurationDialog(ConfigurationGeneric& configuration,
                                         QWidget* parent)
    : QDialog(parent),
      ui(new Ui::ConfigurationDialog),
      m_configuration(configuration)
{
    ui->setupUi(this);

    ui->txtConfig->setPlainText(ConfigurationGeneric::defaultConfiguration());

    portaudio::System& sys = portaudio::System::instance();

    insertWithDefault(
        ui->cmbAudioHost, m_indexToHostApiTypeId, sys.hostApisBegin(),
        sys.hostApisEnd(), sys.defaultHostApi(),
        [](portaudio::HostApi& hostApi) { return hostApi.name(); },
        [](portaudio::HostApi& hostApi) { return hostApi.typeId(); });
}

PaDeviceIndex ConfigurationDialog::getDeviceIndex() const
{
  auto index = ui->cmbInputDevice->currentIndex();
  Q_ASSERT(index != -1);
  return m_indexToDeviceIndex[index];
}

void ConfigurationDialog::on_cmbAudioHost_currentIndexChanged(int index)
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

void ConfigurationDialog::on_buttonBox_accepted()
{
    QString configText = ui->txtConfig->toPlainText();
    QByteArray configTextBa = configText.toUtf8();

    portaudio::System& sys = portaudio::System::instance();
    portaudio::Device& device = sys.deviceByIndex(getDeviceIndex());

    try {
        m_configuration.configure(configTextBa.constData(), device);

        emit accept();

    } catch (const schemeinterface::SchemeException& e) {
        QMessageBox msgBox(this);
        msgBox.setText("Error loading configuration.");
        msgBox.setInformativeText(e.error());
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    } catch (const ConfigurationGeneric::AudioConfigurationNotSupported& e) {
        // Test by setting sample rate to something absurd like -1.
        QMessageBox msgBox(this);
        msgBox.setText("Audio configuration not supported.");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    } catch (const GetF0::ParameterError& e) {
        QMessageBox msgBox(this);
        msgBox.setText("Invalid ESPS config.");
        msgBox.setInformativeText(e.what());
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.exec();
    }
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}
