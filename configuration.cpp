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

#include "tinyscheme/scheme-private.h"
#include "tinyscheme/scheme.h"

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

  ui->txtSampleRate->setValidator(new QIntValidator(this));

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
  QFile initFile(":/tinyscheme/init.scm");
  Q_ASSERT(initFile.open(QFile::ReadOnly));
  QByteArray initFileTextBa(initFile.readAll());

  scheme* sc = scheme_init_new();
  Q_ASSERT(sc != nullptr);

  scheme_set_input_port_file(sc, stdin);
  scheme_set_output_port_file(sc, stdout);


  QString configText = ui->txtConfig->toPlainText();
  QByteArray configTextBa = configText.toLatin1();
  scheme_load_string(sc, configTextBa.data());

  // init.scm
  scheme_load_string(sc, initFileTextBa.data());

  scheme_load_string(sc, "(define (get-sample-rate) (cadr (assv 'sample-rate config)))");

  pointer func = scheme_eval(sc, mk_symbol(sc, "get-sample-rate"));
  pointer ret = scheme_call(sc, func, sc->NIL);

  qDebug() << ret->_flag;
  qDebug() << sc->vptr->ivalue(ret);

  emit accept();
}

Configuration::~Configuration()
{
    delete ui;
}
