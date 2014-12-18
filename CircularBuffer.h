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

#ifndef INCLUDED_CIRCULARBUFFER
#define INCLUDED_CIRCULARBUFFER

#include <stdexcept>
#include <vector>

template <class T>
class CircularBuffer {
public:

  class iterator : public std::iterator<T, std::forward_iterator_tag> {
  public:

    T& operator*() { return m_cb.m_data[m_ptr]; }
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
      return m_oneMoreLoop == o.m_oneMoreLoop && m_ptr == o.m_ptr;
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

    m_data.resize(capacity);
  }

  iterator begin() { return iterator(*this, m_begin, m_capacity == m_size); }

  iterator end() { return iterator(*this, m_ptr, false); }

  void push_back(const T& val)
  {
    m_data[m_ptr++] = val;

    if (m_size < m_capacity)
      m_size++;
    else
      m_begin = (m_begin + 1) % m_capacity;

    if (m_ptr == m_capacity) {
      m_ptr = 0;
    }
  }

  size_t size() const { return m_size; }

  virtual ~CircularBuffer() { }

private:
  // Disable copy and assign
  CircularBuffer(CircularBuffer&);
  CircularBuffer& operator=(CircularBuffer&);

  friend class iterator;

  size_t m_capacity;

  size_t m_size;
  size_t m_begin;
  size_t m_ptr;

  std::vector<T> m_data;
};

#endif
