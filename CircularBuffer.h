#ifndef INCLUDED_CIRCULARBUFFER
#define INCLUDED_CIRCULARBUFFER

#include <cstddef>
#include <stdexcept>
#include <cassert>

template <class T>
class CircularBuffer {
public:
  class iterator {
  public:
    T operator*() { return m_cb.m_data[m_ptr]; }
    iterator& operator++()
    {
      m_ptr++;
      if (m_ptr == m_cb.m_capacity) {
        m_ptr = 0;
        m_oneMoreLoop = false;
      }
      return *this;
    }
    bool operator==(const iterator& o) const
    {
      if (m_oneMoreLoop) return false;
      return m_ptr == o.m_ptr;
    }
    bool operator!=(const iterator& o) const { return !(*this == o); }

  private:
    friend class CircularBuffer;

    CircularBuffer& m_cb;
    size_t m_ptr;
    bool m_oneMoreLoop;

    iterator(CircularBuffer& cb, size_t ptr, bool oneMoreLoop)
        : m_cb(cb), m_ptr(ptr), m_oneMoreLoop(oneMoreLoop)
    {
    }
  };

  CircularBuffer(std::size_t capacity)
      : m_capacity(capacity), m_size(0), m_begin(0), m_ptr(0)
  {
    if (capacity == 0) {
      throw std::invalid_argument("Circular buffer cannot be of zero capacity");
    }

    m_data = new T[capacity];
  }

  iterator begin() { return iterator(*this, m_begin, m_capacity == m_size); }

  iterator end() { return iterator(*this, m_ptr, false); }

  void push_back(const T& val)
  {
    if (m_size < m_capacity)
      m_size++;
    else
      m_begin = (m_begin + 1) % m_capacity;

    m_data[m_ptr++] = val;

    if (m_ptr == m_capacity) {
      m_ptr = 0;
    }
  }

  virtual ~CircularBuffer() { delete m_data; }

private:
  // Disable copy and assign
  CircularBuffer(CircularBuffer&);
  CircularBuffer& operator=(CircularBuffer&);

  friend class iterator;

  size_t m_capacity;
  size_t m_size;

  size_t m_begin;
  size_t m_ptr;
  T* m_data;
};

#endif
