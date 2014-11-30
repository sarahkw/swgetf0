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

#ifndef INCLUDED_STREAMREADBUFFER
#define INCLUDED_STREAMREADBUFFER

#include <cstddef>
#include <functional>
#include <vector>

/*! Read from a source in large chunks, letting another source read
    from the buffer in smaller chunks.

    Prefer chunks of the preferredReadSize; ask for that amount most
    of the time.

    If reader function does not return enough, keep asking. */
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
