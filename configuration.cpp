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
#include <QResource>

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

struct GetDataFromResource {

  GetDataFromResource(const char *file)
  {
    QResource resource(file);
    Q_ASSERT(resource.isValid());
    if (resource.isCompressed())
      m_byteArray = qUncompress(resource.data(), resource.size());
    else
      m_byteArray = QByteArray(reinterpret_cast<const char*>(resource.data()), resource.size());
  }

  const QByteArray& byteArray() const { return m_byteArray; }

  QByteArray m_byteArray;
};

} // namespace anonymous

Configuration::Configuration(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Configuration)
{
  ui->setupUi(this);

  GetDataFromResource defaultConfigScm(":/tinyscheme/default-config.scm");
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

struct Ptr {
  scheme *sc_;
  pointer p_;

  Ptr(scheme *sc, pointer p) : sc_(sc), p_(p) {}

#define P(v) Ptr(sc_, v)

  bool is_nil         (pointer p)              { return p == sc_->NIL; }

  bool is_string      (pointer p)              { return sc_->vptr->is_string(p); }
  char *string_value  (pointer p)              { return sc_->vptr->string_value(p); }
  bool is_number      (pointer p)              { return sc_->vptr->is_number(p); }
  num nvalue          (pointer p)              { return sc_->vptr->nvalue(p); }
  long ivalue         (pointer p)              { return sc_->vptr->ivalue(p); }
  double rvalue       (pointer p)              { return sc_->vptr->rvalue(p); }
  bool is_integer     (pointer p)              { return sc_->vptr->is_integer(p); }
  bool is_real        (pointer p)              { return sc_->vptr->is_real(p); }
  bool is_character   (pointer p)              { return sc_->vptr->is_character(p); }
  long charvalue      (pointer p)              { return sc_->vptr->charvalue(p); }
  bool is_list        (pointer p)              { return sc_->vptr->is_list(sc_, p); }
  bool is_vector      (pointer p)              { return sc_->vptr->is_vector(p); }
  int list_length     (pointer vec)            { return sc_->vptr->list_length(sc_, vec); }
  long vector_length  (pointer vec)            { return sc_->vptr->vector_length(vec); }
  Ptr vector_elem     (pointer vec, int ielem) { return P(sc_->vptr->vector_elem(vec, ielem)); }
  bool is_pair        (pointer p)              { return sc_->vptr->is_pair(p); }
  Ptr car             (pointer p)              { return P(sc_->vptr->pair_car(p)); }
  Ptr cdr             (pointer p)              { return P(sc_->vptr->pair_cdr(p)); }

  bool is_symbol      (pointer p)              { return sc_->vptr->is_symbol(p); }
  char *symname       (pointer p)              { return sc_->vptr->symname(p); }

#undef P

};

struct Config {
  Config(const char *configScript) : sc_(scheme_init_new())
  {
    Q_ASSERT(sc_ != nullptr);

    scheme_set_output_port_file(sc_, stdout);

    loadResource(":/tinyscheme/init.scm");
    loadResource(":/tinyscheme/config-helper.scm");

    read_eval("(begin (display 1337) (newline))");

    scheme_load_string(sc_, configScript);
    if (sc_->retcode != 0) qDebug() << "Scheme failed" << __LINE__;

    pointer ret = read_eval("(cdr (assv ':sample-rate (cdr (assv 'audio-config config))))");
    qDebug() << sc_->vptr->ivalue(ret);
  }

  void loadResource(const char *resource)
  {
    GetDataFromResource gdfr(resource);
    scheme_load_string(sc_, gdfr.byteArray().data());
  }

  pointer read_eval(const char* script)
  {
    // tinyscheme bug: When executing
    //
    //   (read (open-input-string script))
    //
    // sc_->inport needs to be set, or there will be an error when
    // read is restoring the previous inport value when returning.
    scheme_set_input_port_file(sc_, stdin); // TODO stdin?

    pointer fun = scheme_eval(sc_, mk_symbol(sc_, "read-eval"));
    pointer arg = mk_string(sc_, script);
    return scheme_call(sc_, fun, _cons(sc_, arg, sc_->NIL, 0));
  }

  scheme *sc_;
};

} // namespace anonymous

void Configuration::on_buttonBox_accepted()
{
  QString configText = ui->txtConfig->toPlainText();
  QByteArray configTextBa = configText.toUtf8();
  Config cfg(configTextBa.constData());

  emit accept();
}

Configuration::~Configuration()
{
    delete ui;
}
