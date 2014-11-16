/*
    Copyright (C) 2014  Sarah Wong

    This file is part of swgetf0.

    swgetf0 is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef INCLUDED_CIRCULARBUFFER
#define INCLUDED_CIRCULARBUFFER

#include <cstddef>
#include <stdexcept>
#include <cassert>

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

    m_data = new T[capacity];
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

  virtual ~CircularBuffer() { delete [] m_data; }

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
