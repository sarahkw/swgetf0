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

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>
#include <vector>

#include "../unused/StreamReadBuffer.h"

namespace {

/*! A test source stream */
class TestDataProvider {
public:
  TestDataProvider(int maxBytes)
      : m_maxBytes(maxBytes), m_currentByte(0), m_currentValue(0),
        m_limitSize(0) {}

  size_t read(void* buffer, size_t bytes) {
    m_readSizes.push_back(bytes);

    if (m_limitSize > 0 && bytes > m_limitSize) {
      bytes = m_limitSize;
    }

    size_t actualReadSize = std::min(bytes, m_maxBytes - m_currentByte);
    m_currentByte += actualReadSize;

    for (size_t i = 0; i < actualReadSize; ++i) {
      static_cast<char*>(buffer)[i] = m_currentValue++;
    }
    return actualReadSize;
  }

  void limitSize(size_t limitSize) { m_limitSize = limitSize; }

  const std::vector<size_t>& readSizes() const { return m_readSizes; }
  void clearReadSizes() { m_readSizes.clear(); }

private:

  size_t m_maxBytes;
  size_t m_currentByte;
  int m_currentValue;
  size_t m_limitSize;

  std::vector<size_t> m_readSizes;
};

class TestStreamReadBuffer : public ::testing::Test {
protected:
  TestStreamReadBuffer() : m_tdp(10) {}

  void SetUp() override {
    m_srb = new StreamReadBuffer(3, [this](void* buffer, size_t size) {
	return m_tdp.read(buffer, size);
    });
  }

  void TearDown() override {
    delete m_srb;
  }

  StreamReadBuffer *m_srb;

  TestDataProvider m_tdp;
};

}

using namespace testing;

TEST(TestDataProvider, General) {
  TestDataProvider tdp(10);
  ASSERT_TRUE(tdp.readSizes().empty());

  char buffer[5] = {0};

  ASSERT_EQ(tdp.read(buffer, 2), 2);
  ASSERT_THAT(buffer, ElementsAre(0, 1, 0, 0, 0));
  ASSERT_EQ(tdp.read(buffer, 5), 5);
  ASSERT_THAT(buffer, ElementsAre(2, 3, 4, 5, 6));

  ASSERT_EQ(tdp.read(buffer, 0), 0); // Shouldn't do anything
  ASSERT_THAT(buffer, ElementsAre(2, 3, 4, 5, 6));

  ASSERT_EQ(tdp.read(buffer, 5), 3);
  ASSERT_THAT(buffer, ElementsAre(7, 8, 9, 5, 6));

  ASSERT_THAT(tdp.readSizes(), ElementsAre(2, 5, 0, 5));
}

TEST_F(TestStreamReadBuffer, Run) {
  char buffer[5] = {0};

  ASSERT_EQ(m_srb->read(buffer + 0, 1), 1);
  ASSERT_EQ(m_srb->read(buffer + 1, 1), 1);
  ASSERT_EQ(m_srb->read(buffer + 2, 1), 1);

  ASSERT_THAT(buffer, ElementsAre(0, 1, 2, 0, 0));
  ASSERT_THAT(m_tdp.readSizes(), ElementsAre(3));

  ASSERT_EQ(m_srb->read(buffer + 3, 1), 1);

  ASSERT_THAT(buffer, ElementsAre(0, 1, 2, 3, 0));
  ASSERT_THAT(m_tdp.readSizes(), ElementsAre(3, 3));

}

TEST_F(TestStreamReadBuffer, LimitedSize) {
  char buffer[5] = {0};

  m_tdp.limitSize(2);

  ASSERT_EQ(m_srb->read(buffer + 0, 1), 1);
  ASSERT_EQ(m_srb->read(buffer + 1, 1), 1);
  ASSERT_EQ(m_srb->read(buffer + 2, 1), 1);

  ASSERT_THAT(buffer, ElementsAre(0, 1, 2, 0, 0));
  ASSERT_THAT(m_tdp.readSizes(), ElementsAre(3, 3));
  m_tdp.clearReadSizes();

  ASSERT_EQ(m_srb->read(buffer + 3, 1), 1);
  ASSERT_THAT(m_tdp.readSizes(), ElementsAre());

  ASSERT_EQ(m_srb->read(buffer, 5), 5);
  ASSERT_THAT(buffer, ElementsAre(4, 5, 6, 7, 8));
  ASSERT_THAT(m_tdp.readSizes(), ElementsAre(3, 3, 3));
}
