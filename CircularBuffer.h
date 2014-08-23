#ifndef INCLUDED_CIRCULARBUFFER
#define INCLUDED_CIRCULARBUFFER

#include <cstddef>
#include <stdexcept>

template <class T>
class CircularBuffer {
public:
  class iterator {
  public:
    T operator*() { return m_cb.m_data[m_ptr]; }
    iterator& operator++()
    {
      m_ptr++;
      return *this;
    }
    bool operator==(const iterator& o) const { return m_ptr == o.m_ptr; }
    bool operator!=(const iterator& o) const { return !(*this == o); }

  private:
    friend class CircularBuffer;

    CircularBuffer& m_cb;
    size_t m_ptr;

    iterator(CircularBuffer& cb, size_t ptr) : m_cb(cb), m_ptr(ptr) {}
  };

  CircularBuffer(std::size_t capacity)
      : m_capacity(capacity), m_size(0), m_begin(0)
  {
    if (capacity == 0) {
      throw std::invalid_argument("Circular buffer cannot be of zero capacity");
    }

    m_data = new T[capacity];
  }

  iterator begin() { return iterator(*this, m_begin); }

  iterator end() { return iterator(*this, m_size); }

  void push_back(const T& val) {
    m_data[m_size++] = val;
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
  T* m_data;
};

#endif
