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

#include "StreamReadBuffer.h"

#include <cstring>
#include <QDebug>

StreamReadBuffer::StreamReadBuffer(size_t preferredReadSize, Reader reader)
    : m_preferredReadSize(preferredReadSize), m_reader(reader),
      m_bufferPosition(0), m_bufferSize(0) {
  m_buffer.resize(preferredReadSize);
}

ssize_t StreamReadBuffer::read(char *data, size_t bytes) {
  // Don't have enough data in our buffer to service the read?
  if (sizeAvailable() < bytes) {
    prepareToRead(bytes);
    do {
      readChunk();

      // TODO Some sanity check to prevent infinite looping.
    } while (sizeAvailable() < bytes);
  }

  memcpy(data, m_buffer.data() + m_bufferPosition, bytes);
  m_bufferPosition += bytes;
  return bytes;
}

size_t StreamReadBuffer::sizeAvailable() const {
  return m_bufferSize - m_bufferPosition;
}

/*! Prepare the buffer to be ready to read a certain amount of
    bytes. */
void StreamReadBuffer::prepareToRead(size_t bytes) {

  if (m_bufferPosition + bytes > m_buffer.size()) {
    memmove(m_buffer.data(), m_buffer.data() + m_bufferPosition,
            sizeAvailable());
    m_bufferSize = sizeAvailable();
    m_bufferPosition = 0;
  }

  if (bytes > m_buffer.size()) {
    m_buffer.resize(bytes + m_preferredReadSize);

    // qDebug() << "Resized to" << m_buffer.size();
  }
}

void StreamReadBuffer::readChunk() {
  ssize_t sizeRead =
      m_reader(m_buffer.data() + m_bufferSize, m_preferredReadSize);

  // TODO Handle errors better than asserting
  Q_ASSERT(sizeRead >= 0);

  m_bufferSize += sizeRead;
}
