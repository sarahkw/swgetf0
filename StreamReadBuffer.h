/*
  Copyright 2014 Sarah Wong
*/

#ifndef INCLUDED_STREAMREADBUFFER
#define INCLUDED_STREAMREADBUFFER

#include <cstddef>
#include <functional>

/*! Read from a source in large chunks, letting another source read
    from the buffer in smaller chunks.

    This is to appease Qt's audio interface, as multiple small reads
    eats too much CPU. */
class StreamReadBuffer {
public:

  typedef std::function<ssize_t(void*, size_t)> Reader;

  StreamReadBuffer(size_t minimumReadSize, Reader reader)
      : m_minimumReadSize(minimumReadSize), m_reader(reader)
  {
    reader(NULL, 100);
  }

private:

  size_t m_minimumReadSize;

  Reader m_reader;

};

#endif
