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

using schemeconfig::SchemeConfig;
using schemeconfig::Ptr;
using schemeconfig::PtrIter;
using schemeconfig::loadValues;

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

namespace {

struct TestSubConfig {
  long x;
  QString y;

  TestSubConfig() {}

  TestSubConfig(PtrIter sexp) { loadValues(sexp, x, y); }
};

struct TestList {

  QList<long> lst;

  TestList() { }

  TestList(PtrIter sexp) {
    std::copy(sexp, sexp.end(), std::back_inserter(lst));
  }
};

struct TestConfig {
  long a;
  long b;
  long c;
  long d;
  TestSubConfig tsc;
  TestList tl;

  TestConfig(PtrIter sexp) { loadValues(sexp, a, b, c, d, tsc, tl); }
};

QDebug operator<<(QDebug dbg, const TestSubConfig &tsc)
{
  dbg.nospace() << "(TestSubConfig " << tsc.x << " " << tsc.y << ")";

  return dbg.space();
}

QDebug operator<<(QDebug dbg, const TestList &tl)
{
  dbg.nospace() << "(TestList " << tl.lst << ")";

  return dbg.space();
}

QDebug operator<<(QDebug dbg, const TestConfig &tc)
{
  dbg.nospace() << "(TestConfig " << tc.a << " " << tc.b << " " << tc.c << " "
                << tc.d << " " << tc.tsc << " " << tc.tl << ")";

  return dbg.space();
}

} // namespace anonymous

void Configuration::on_buttonBox_accepted()
{
  QString configText = ui->txtConfig->toPlainText();
  QByteArray configTextBa = configText.toUtf8();
  SchemeConfig cfg(configTextBa.constData());

  qDebug() << TestConfig(
      PtrIter(cfg.read_eval("'(1 5 10 20 (7 \"wow\") (100 200 300))")));

  emit accept();
}

Configuration::~Configuration()
{
    delete ui;
}
