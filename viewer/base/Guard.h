#ifndef __BASE__GUARD_H__
#define __BASE__GUARD_H__

#include "Common.h"

namespace viewer {
namespace base {

template<class T, void (*Free)(T*) >
class PtrGuard {
public:
  PtrGuard(T *data) : m_data(data) { }
  ~PtrGuard() { if (m_data != NULL) Free(m_data); }
  inline T *operator->() { return m_data;  }
  inline T *get() { return m_data; }
  inline bool null() const { return m_data == NULL; }
  inline bool operator!() const { return m_data == NULL; }

private:
  T *m_data;

  DISALLOW_COPY_AND_ASSIGN(PtrGuard);
};

template<void (*Run)()>
class RunGuard {
public:
  RunGuard() { }
  ~RunGuard() { Run(); }

  DISALLOW_COPY_AND_ASSIGN(RunGuard);
};



} // namespace base
} // namespace viewer

#endif
