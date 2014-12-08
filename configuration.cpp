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

#include <QDebug>

#include <portaudiocpp/System.hxx>
#include <portaudiocpp/SystemHostApiIterator.hxx>
#include <portaudiocpp/SystemDeviceIterator.hxx>

#include "schemeconfig.h"
#include "config.h"

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

Configuration::Configuration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Configuration)
{
  ui->setupUi(this);

  schemeconfig::GetDataFromResource defaultConfigScm(
      ":/tinyscheme/default-config.scm");

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
  schemeconfig::SchemeConfig scfg(configTextBa.constData());

  auto p = scfg.read_eval(
       "(let* ((get-key (lambda (key alist) (cdr (assv key alist))))           \n"
       "       (get-items (lambda (keys alist)                                 \n"
       "                    (let ((getter (lambda (key) (get-key key alist)))) \n"
       "                      (map getter keys)))))                            \n"
       "  (list                                                                \n"
       "   (get-items '(:sample-rate)                                          \n"
       "              (get-key 'audio-config config))                          \n"
       "   (get-items '(:width :height :note-width :min-note :max-note)        \n"
       "              (get-key 'ui-config config))                             \n"
       "   (get-key 'ui-marker-lines config)                                   \n"
       "   (get-items '(:cand-thresh :lag-weight :freq-weight :trans-cost      \n"
       "                             :trans-amp :trans-spec :voice-bias        \n"
       "                             :double-cost :min-f0 :max-f0 :frame-step  \n"
       "                             :wind-dur :n-cands)                       \n"
       "              (get-key 'esps-config config))))                         \n");

  m_config = config::Config(p);

  emit accept();
}

Configuration::~Configuration()
{
    delete ui;
}
