/*
  Copyright 2014 Sarah Wong
*/

#include "gtest/gtest.h"
#include "gmock/gmock.h"

#include <iostream>
#include <vector>

#include "../StreamReadBuffer.h"

namespace {

/*! A test source stream */
class TestDataProvider {
public:
  TestDataProvider(int maxBytes)
      : m_maxBytes(maxBytes), m_currentByte(0), m_currentValue(0)
  {
  }

  size_t read(void* buffer, size_t bytes) {
    m_readSizes.push_back(bytes);

    size_t actualReadSize = std::min(bytes, m_maxBytes - m_currentByte);
    m_currentByte += actualReadSize;

    for (size_t i = 0; i < actualReadSize; ++i) {
      static_cast<char*>(buffer)[i] = m_currentValue++;
    }
    return actualReadSize;
  }

  const std::vector<size_t>& readSizes() const { return m_readSizes; }
  void clearReadSizes() { m_readSizes.clear(); }

private:

  size_t m_maxBytes;
  size_t m_currentByte;
  int m_currentValue;

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

  ASSERT_EQ(m_srb->read(buffer, 1), 1);
  ASSERT_EQ(m_srb->read(buffer + 1, 1), 1);
  ASSERT_EQ(m_srb->read(buffer + 2, 1), 1);

  ASSERT_THAT(buffer, ElementsAre(0, 1, 2, 0, 0));


  ASSERT_THAT(m_tdp.readSizes(), ElementsAre(3));
}
