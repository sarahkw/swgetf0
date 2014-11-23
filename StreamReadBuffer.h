/*
  Copyright 2014 Sarah Wong
*/

#ifndef INCLUDED_STREAMREADBUFFER
#define INCLUDED_STREAMREADBUFFER

#include <cstddef>
#include <functional>
#include <vector>
#include <cstring>

/*! Read from a source in large chunks, letting another source read
    from the buffer in smaller chunks.

    Prefer chunks of the preferredReadSize; ask for that amount most
    of the time.

    If reader function does not return enough, keep asking.

    This is to appease Qt's audio interface, as multiple small reads
    eats too much CPU. */
class StreamReadBuffer {
public:

  typedef std::function<size_t(void*, size_t)> Reader;

  StreamReadBuffer(size_t preferredReadSize, Reader reader)
      : m_preferredReadSize(preferredReadSize),
        m_reader(reader),
        m_bufferPosition(0),
	m_bufferSize(0)
  {
    m_buffer.resize(preferredReadSize);
  }

  size_t read(char* data, size_t bytes)
  {
    // Don't have enough data in our buffer to service the read?
    if (sizeAvailable() < bytes) {
      prepareToRead(bytes);
      do {
	readChunk();
      } while (sizeAvailable() < bytes);
    }

    memcpy(data, m_buffer.data() + m_bufferPosition, bytes);
    m_bufferPosition += bytes;
  }

  virtual ~StreamReadBuffer() { }

private:

  size_t sizeAvailable() const { return m_bufferSize - m_bufferPosition; }

  /*! Prepare the buffer to be ready to read a certain amount of
      bytes. */
  void prepareToRead(size_t bytes) {

    if (m_bufferPosition + bytes > m_buffer.capacity()) {
      memmove(m_buffer.data(), m_buffer.data() + m_bufferPosition,
              sizeAvailable());
      m_bufferSize = sizeAvailable();
      m_bufferPosition = 0;
    }

    if (bytes > m_buffer.capacity()) {
      m_buffer.resize(bytes + m_preferredReadSize);
    }
  }

  void readChunk() {
    size_t sizeRead =
        m_reader(m_buffer.data() + m_bufferPosition, m_preferredReadSize);

    m_bufferPosition += sizeRead;
  }

  size_t m_preferredReadSize;

  Reader m_reader;

  /*! Index of data that has not been given to client. */
  size_t m_bufferPosition;
  /*! How much data we have filled. */
  size_t m_bufferSize;
  std::vector<char> m_buffer;

};

#endif
