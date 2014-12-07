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

  Ptr nil           ()          { return P(sc_->NIL); }

  bool is_nil       ()          { return p_ == sc_->NIL; }

  bool is_string    ()          { return sc_->vptr->is_string(p_); }
  char *string_value()          { return sc_->vptr->string_value(p_); }
  bool is_number    ()          { return sc_->vptr->is_number(p_); }
  num nvalue        ()          { return sc_->vptr->nvalue(p_); }
  long ivalue       ()          { return sc_->vptr->ivalue(p_); }
  double rvalue     ()          { return sc_->vptr->rvalue(p_); }
  bool is_integer   ()          { return sc_->vptr->is_integer(p_); }
  bool is_real      ()          { return sc_->vptr->is_real(p_); }
  bool is_character ()          { return sc_->vptr->is_character(p_); }
  long charvalue    ()          { return sc_->vptr->charvalue(p_); }
  bool is_list      ()          { return sc_->vptr->is_list(sc_, p_); }
  bool is_vector    ()          { return sc_->vptr->is_vector(p_); }
  int list_length   ()          { return sc_->vptr->list_length(sc_, p_); }
  long vector_length()          { return sc_->vptr->vector_length(p_); }
  Ptr vector_elem   (int ielem) { return P(sc_->vptr->vector_elem(p_, ielem)); }
  bool is_pair      ()          { return sc_->vptr->is_pair(p_); }
  Ptr car           ()          { return P(sc_->vptr->pair_car(p_)); }
  Ptr cdr           ()          { return P(sc_->vptr->pair_cdr(p_)); }

  bool is_symbol    ()          { return sc_->vptr->is_symbol(p_); }
  char *symname     ()          { return sc_->vptr->symname(p_); }

#undef P

  operator long() { return ivalue(); }
  operator double() { return rvalue(); }
  operator const char*() { return string_value(); }

};

struct PtrIter : public std::iterator<Ptr, std::forward_iterator_tag> {

  PtrIter(Ptr ptr) : ptr_(ptr) {}

  PtrIter begin() { return *this; }

  PtrIter end() { return PtrIter(ptr_.nil()); }

  PtrIter& operator++() { ptr_ = ptr_.cdr(); return *this; }

  Ptr operator*() { return ptr_.car(); }

  bool operator!=(const PtrIter &other) const
  {
    return ptr_.p_ != other.ptr_.p_;
  }

  Ptr ptr_;

};

void loadValues(PtrIter iter) {}

template <typename Arg1, typename... Args>
void loadValues(PtrIter iter, Arg1 &arg1, Args &... args)
{
  arg1 = static_cast<Arg1>(*iter);
  loadValues(++iter, args...);
}

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

struct Config {
  Config(const char *configScript) : sc_(scheme_init_new())
  {
    Q_ASSERT(sc_ != nullptr);

    scheme_set_output_port_file(sc_, stdout);

    loadResource(":/tinyscheme/init.scm");
    loadResource(":/tinyscheme/config-helper.scm");

    qDebug() << TestConfig(
        PtrIter(read_eval("'(1 5 10 20 (7 \"wow\") (100 200 300))")));

    scheme_load_string(sc_, configScript);
    if (sc_->retcode != 0) qDebug() << "Scheme failed" << __LINE__;

    Ptr ret = read_eval("(cdr (assv ':sample-rate (cdr (assv 'audio-config config))))");
    qDebug() << ret.ivalue();
  }

  void loadResource(const char *resource)
  {
    GetDataFromResource gdfr(resource);
    scheme_load_string(sc_, gdfr.byteArray().data());
  }

  Ptr read_eval(const char* script)
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
    return Ptr(sc_, scheme_call(sc_, fun, _cons(sc_, arg, sc_->NIL, 0)));
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
