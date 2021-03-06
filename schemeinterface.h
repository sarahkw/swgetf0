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

#ifndef INCLUDED_SCHEMEINTERFACE
#define INCLUDED_SCHEMEINTERFACE

#include <QString>
#include <QResource>

// forward declare
struct scheme;
struct cell;
typedef struct scheme scheme;
typedef struct cell *pointer;

namespace schemeinterface {

struct Ptr;

class SchemeException {
public:
  SchemeException(int retcode, QString error) : retcode_(retcode), error_(error)
  {
  }

  int retcode() const {
    return retcode_;
  }

  const QString& error() const {
    return error_;
  }

protected:

  int retcode_;

  QString error_;

};

struct GetDataFromResource {

  GetDataFromResource(const char *file);

  const QByteArray& byteArray() const;

  QByteArray m_byteArray;
};

struct Ptr {
  scheme *sc_;
  pointer p_;

  Ptr(scheme *sc, pointer p);

  Ptr nil();

  bool is_nil();

  bool is_string();
  char *string_value();
  bool is_number();
  long ivalue();
  double rvalue();
  bool is_integer();
  bool is_real();
  bool is_character();
  long charvalue();
  bool is_list();
  bool is_vector();
  int list_length();
  long vector_length();
  Ptr vector_elem(int ielem);
  bool is_pair();
  Ptr car();
  Ptr cdr();

  bool is_symbol();
  char *symname();

  operator pointer();
  operator long();
  operator double();
  operator const char *();
  operator bool();
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

inline void loadValues(PtrIter iter) {}

template <typename Arg1, typename... Args>
inline void loadValues(PtrIter iter, Arg1 &arg1, Args &... args)
{
  arg1 = static_cast<Arg1>(*iter);
  loadValues(++iter, args...);
}

struct SchemeInterface {
  SchemeInterface();

  virtual ~SchemeInterface();

  void loadResource(const char *resource);

  Ptr read_eval(const char* script);

  // Wrappers around functions that execute code. Throw exception if
  // retcode isn't 0.
  void load_file(FILE *fin);
  void load_named_file(FILE *fin, const char *filename);
  void load_string(const char *cmd);
  Ptr apply0(const char *procname);
  Ptr call(pointer func, pointer args);
  Ptr eval(pointer obj);

  scheme *sc_;

  QString lastError_;

private:

  void check_retcode();

};

} // namespace schemeinterface

#endif
