/*
  Copyright 2014 Sarah Wong
*/

#ifndef INCLUDED_STREAMREADBUFFER
#define INCLUDED_STREAMREADBUFFER

#include <cstddef>
#include <functional>
#include <vector>

/*! Read from a source in large chunks, letting another source read
    from the buffer in smaller chunks.

    Prefer chunks of the preferredReadSize; ask for that amount most
    of the time.

    If reader function does not return enough, keep asking.

    This is to appease Qt's audio interface, as multiple small reads
    eats too much CPU. */
class StreamReadBuffer {
public:

  typedef std::function<ssize_t(void*, size_t)> Reader;

  StreamReadBuffer(size_t preferredReadSize, Reader reader);

  ssize_t read(char* data, size_t bytes);

  virtual ~StreamReadBuffer() { }

private:

  size_t sizeAvailable() const;

  /*! Prepare the buffer to be ready to read a certain amount of
      bytes. */
  void prepareToRead(size_t bytes);

  void readChunk();

  size_t m_preferredReadSize;

  Reader m_reader;

  /*! Index of data that has not been given to client. */
  size_t m_bufferPosition;
  /*! How much data we have filled. */
  size_t m_bufferSize;
  std::vector<char> m_buffer;

};

#endif
