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

#include "schemeinterface.h"

#include <QDebug>

#include "tinyscheme/scheme-private.h"
#include "tinyscheme/scheme.h"

using namespace schemeinterface;



GetDataFromResource::GetDataFromResource(const char* file)
{
  QResource resource(file);
  Q_ASSERT(resource.isValid());
  if (resource.isCompressed())
    m_byteArray = qUncompress(resource.data(), resource.size());
  else
    m_byteArray = QByteArray(reinterpret_cast<const char *>(resource.data()),
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

Ptr::operator pointer() { return p_; }
Ptr::operator long() { return ivalue(); }
Ptr::operator double() { return rvalue(); }
Ptr::operator const char*() { return string_value(); }



namespace {

pointer ff_report_error(scheme * sc, pointer args)
{
  SchemeInterface *si = reinterpret_cast<SchemeInterface *>(sc->ext_data);
  si->lastError_ = args;
}

} // namespace anonymous



SchemeInterface::SchemeInterface() : sc_(scheme_init_new()), lastError_(NULL)
{
  Q_ASSERT(sc_ != nullptr);

  scheme_set_external_data(sc_, reinterpret_cast<void *>(this));

  scheme_set_output_port_file(sc_, stdout);

  scheme_registerable foreignFuncs[] = {{ff_report_error, "si/report-error"}};
  scheme_register_foreign_func_list(
      sc_, foreignFuncs, sizeof(foreignFuncs) / sizeof(*foreignFuncs));

  loadResource(":/tinyscheme/init.scm");
  loadResource(":/scheme/schemeinterface.scm");
}

SchemeInterface::~SchemeInterface()
{
  scheme_deinit(sc_);
}

void SchemeInterface::loadResource(const char *resource)
{
  GetDataFromResource gdfr(resource);
  load_string(gdfr.byteArray().data());
}

Ptr SchemeInterface::read_eval(const char* script)
{
  // tinyscheme bug: When executing
  //
  //   (read (open-input-string script))
  //
  // sc_->inport needs to be set, or there will be an error when
  // read is restoring the previous inport value when returning.
  scheme_set_input_port_file(sc_, stdin); // TODO stdin?

  pointer fun = eval(mk_symbol(sc_, "si/read-eval"));
  pointer arg = mk_string(sc_, script);
  Ptr ret = call(fun, _cons(sc_, arg, sc_->NIL, 0));

  return ret;
}

void SchemeInterface::load_file(FILE *fin)
{
  scheme_load_file(sc_, fin);
  check_retcode();
}

void SchemeInterface::load_named_file(FILE *fin, const char *filename)
{
  scheme_load_named_file(sc_, fin, filename);
  check_retcode();
}

void SchemeInterface::load_string(const char *cmd)
{
  scheme_load_string(sc_, cmd);
  check_retcode();
}

Ptr SchemeInterface::apply0(const char *procname)
{
  pointer ret = scheme_apply0(sc_, procname);
  check_retcode();
  return Ptr(sc_, ret);
}

Ptr SchemeInterface::call(pointer func, pointer args)
{
  pointer ret = scheme_call(sc_, func, args);
  check_retcode();
  return Ptr(sc_, ret);
}

Ptr SchemeInterface::eval(pointer obj)
{
  pointer ret = scheme_eval(sc_, obj);
  check_retcode();
  return Ptr(sc_, ret);
}

void SchemeInterface::check_retcode()
{
  if (sc_->retcode != 0) {
    pointer errorToThrow = lastError_;
    lastError_ = NULL;

    // We're expecting errorToThrow to be a list of 1 item, with an
    // error string.
    PtrIter pIter(Ptr(sc_, errorToThrow));
    QString errorMessage;

    if (pIter.begin() != pIter.end() && (*pIter).is_string()) {
      errorMessage = (*pIter).string_value();
    } else {
      // TODO Can we do something better here?
      qDebug()
          << "Got an error that's not a list with a string as a first item.";
    }

    throw SchemeException(sc_->retcode, errorMessage);
  }
}
