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

#include "schemeconfig.h"

#include <QDebug>

#include "tinyscheme/scheme-private.h"
#include "tinyscheme/scheme.h"

using namespace schemeconfig;



GetDataFromResource::GetDataFromResource(const char* file)
{
  QResource resource(file);
  Q_ASSERT(resource.isValid());
  if (resource.isCompressed())
    m_byteArray = qUncompress(resource.data(), resource.size());
  else
    m_byteArray = QByteArray(reinterpret_cast<const char*>(resource.data()),
                             resource.size());
}

const QByteArray& GetDataFromResource::byteArray() const { return m_byteArray; }



Ptr::Ptr(scheme *sc, pointer p) : sc_(sc), p_(p) {}

#define P(v) Ptr(sc_, v)

Ptr Ptr::nil           ()          { return P(sc_->NIL); }

bool Ptr::is_nil       ()          { return p_ == sc_->NIL; }

bool Ptr::is_string    ()          { return sc_->vptr->is_string(p_); }
char *Ptr::string_value()          { return sc_->vptr->string_value(p_); }
bool Ptr::is_number    ()          { return sc_->vptr->is_number(p_); }
long Ptr::ivalue       ()          { return sc_->vptr->ivalue(p_); }
double Ptr::rvalue     ()          { return sc_->vptr->rvalue(p_); }
bool Ptr::is_integer   ()          { return sc_->vptr->is_integer(p_); }
bool Ptr::is_real      ()          { return sc_->vptr->is_real(p_); }
bool Ptr::is_character ()          { return sc_->vptr->is_character(p_); }
long Ptr::charvalue    ()          { return sc_->vptr->charvalue(p_); }
bool Ptr::is_list      ()          { return sc_->vptr->is_list(sc_, p_); }
bool Ptr::is_vector    ()          { return sc_->vptr->is_vector(p_); }
int Ptr::list_length   ()          { return sc_->vptr->list_length(sc_, p_); }
long Ptr::vector_length()          { return sc_->vptr->vector_length(p_); }
Ptr Ptr::vector_elem   (int ielem) { return P(sc_->vptr->vector_elem(p_, ielem)); }
bool Ptr::is_pair      ()          { return sc_->vptr->is_pair(p_); }
Ptr Ptr::car           ()          { return P(sc_->vptr->pair_car(p_)); }
Ptr Ptr::cdr           ()          { return P(sc_->vptr->pair_cdr(p_)); }

bool Ptr::is_symbol    ()          { return sc_->vptr->is_symbol(p_); }
char *Ptr::symname     ()          { return sc_->vptr->symname(p_); }

#undef P

Ptr::operator long() { return ivalue(); }
Ptr::operator double() { return rvalue(); }
Ptr::operator const char*() { return string_value(); }



Config::Config(const char *configScript) : sc_(scheme_init_new())
{
  Q_ASSERT(sc_ != nullptr);

  scheme_set_output_port_file(sc_, stdout);

  loadResource(":/tinyscheme/init.scm");
  loadResource(":/tinyscheme/config-helper.scm");

  scheme_load_string(sc_, configScript);
  if (sc_->retcode != 0) qDebug() << "Scheme failed" << __LINE__;

  Ptr ret =
      read_eval("(cdr (assv ':sample-rate (cdr (assv 'audio-config config))))");
  qDebug() << ret.ivalue();
}

void Config::loadResource(const char *resource)
{
  schemeconfig::GetDataFromResource gdfr(resource);
  scheme_load_string(sc_, gdfr.byteArray().data());
}

Ptr Config::read_eval(const char* script)
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
