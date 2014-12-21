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

  typedef T value_type;

  class const_iterator : public std::iterator<T, std::forward_iterator_tag>
  {
  public:

    const T& operator*() { return m_cb.m_data[m_ptr]; }
    const_iterator& operator++()
    {
      m_ptr++;
      if (m_ptr == m_cb.m_workingSet) {
        m_ptr = 0;
        m_oneMoreLoop = false;
      }
      return *this;
    }
    bool operator==(const const_iterator& o) const
    {
      return m_oneMoreLoop == o.m_oneMoreLoop && m_ptr == o.m_ptr;
    }
    bool operator!=(const const_iterator& o) const { return !(*this == o); }

  private:
    friend class CircularBuffer;

    const CircularBuffer& m_cb;
    size_t m_ptr;
    bool m_oneMoreLoop;

    const_iterator(const CircularBuffer& cb, size_t ptr, bool oneMoreLoop)
        : m_cb(cb), m_ptr(ptr), m_oneMoreLoop(oneMoreLoop)
    {
    }
  };

  // XXX gtest's pretty printer will only think we're a container if
  //     we have both ::iterator and ::const_iterator. At the moment
  //     we don't need non-const iterator.
  typedef struct DummyType {
  } iterator;

  CircularBuffer(std::size_t workingSet)
      : m_workingSet(workingSet), m_ptr(0), m_full(false), m_skip(0)
  {
    m_data.resize(workingSet);

    if (workingSet == 0) {
      m_full = true;
    }
  }

  void expand(std::size_t expandCount)
  {
    if (m_skip > 0) {
      // m_skip > 0 ==> m_full is true

      auto unskipCount = std::min(expandCount, m_skip);

      auto erasePtr = m_ptr + m_skip - unskipCount;
      while (expandCount > 0 && m_skip > 0) {
        if (erasePtr >= m_workingSet) {
          erasePtr -= m_workingSet;
        }

        m_data[erasePtr] = T();

        ++erasePtr;
        --m_skip;
        --expandCount;
      }

      if (expandCount == 0) {
        return;
      }
    }

    // No need for padding if the buffer hasn't filled up yet.
    if (!m_full) {
      m_data.resize(m_workingSet += expandCount);
      return;
    }

    /*
      Padding to be added in the beginning.

      | 4 | 5 | 1 | 2 | 3 |   |   |
              ^ ptr
                          ^ workingSet

               Padding
              vvvvvvvvv
      | 4 | 5 | 0 | 0 | 1 | 2 | 3 |
              ^ ptr
                                  ^ workingSet
    */

    auto oldSize = m_workingSet;
    m_data.resize(m_workingSet += expandCount);

    T* data = m_data.data();
    std::move_backward(data + m_ptr, data + oldSize, data + m_workingSet);
    std::fill(data + m_ptr, data + m_ptr + expandCount, T());
  }

  void shrink(std::size_t shrinkCount)
  {
    if (!m_full) {
      // We will chop off the right-most part of the working set size.

      auto discardSlots = std::min(m_workingSet - m_ptr, shrinkCount);
      shrinkCount -= discardSlots;
      m_workingSet -= discardSlots;
      m_data.resize(m_workingSet); // vector probably won't do anything

      if (m_ptr == m_workingSet) {
        m_ptr = 0;
        m_full = true;
      }
    }

    // shrinkCount nonzero means we're full
    m_skip += shrinkCount;
  }

  const_iterator begin() const
  {
    size_t beginpos;
    bool oneMoreLoop;
    if (m_full) {
      beginpos = m_ptr + m_skip;
      oneMoreLoop = true;

      if (beginpos >= m_workingSet) {
        beginpos -= m_workingSet;
        oneMoreLoop = false;
      }
    } else {
      // Not full

      auto amountOfExpectedItems = m_workingSet - m_skip;
      if (m_ptr > amountOfExpectedItems) {
        beginpos = m_ptr - amountOfExpectedItems;
      } else {
        beginpos = 0;
      }

      oneMoreLoop = false;
    }

    return const_iterator(*this, beginpos, oneMoreLoop);
  }

  const_iterator end() const { return const_iterator(*this, m_ptr, false); }

  void push_back(const T& val)
  {
    if (m_workingSet == 0) {
      return;
    }

    m_data[m_ptr++] = val;

    if (m_ptr == m_workingSet) {
      m_ptr = 0;
      m_full = true;
    }
  }

  size_t size() const
  {
    return m_full ? m_workingSet - m_skip
                  : std::min(m_ptr, m_workingSet - m_skip);
  }

  virtual ~CircularBuffer() { }

private:
  // Disable copy and assign
  CircularBuffer(CircularBuffer&);
  CircularBuffer& operator=(CircularBuffer&);

  size_t m_workingSet;
  size_t m_ptr;
  bool m_full;

  size_t m_skip;

  std::vector<T> m_data;
};

#endif
