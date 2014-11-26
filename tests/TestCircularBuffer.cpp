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

#include "../CircularBuffer.h"

namespace {

class TestCircularBuffer : public ::testing::Test {
protected:
  TestCircularBuffer() : m_cb3(3) {}

  void SetUp() override {}

  void TearDown() override {}

  CircularBuffer<int> m_cb3;
};
}

TEST(CircularBufferZeroTest, TestThrow)
{
  EXPECT_THROW(CircularBuffer<int>(0), std::invalid_argument);
}

TEST_F(TestCircularBuffer, ReadEmptyBuffer)
{
  ASSERT_TRUE(m_cb3.begin() == m_cb3.end());

  ASSERT_EQ(m_cb3.size(), 0);
}

TEST_F(TestCircularBuffer, ReadWithoutLoop)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);

  auto iter = m_cb3.begin();
  ASSERT_TRUE(iter != m_cb3.end());
  ASSERT_EQ(*iter, 1);
  ++iter;

  ASSERT_TRUE(iter != m_cb3.end());
  ASSERT_EQ(*iter, 2);
  ++iter;

  ASSERT_TRUE(iter != m_cb3.end());
  ASSERT_EQ(*iter, 3);
  ++iter;

  ASSERT_TRUE(iter == m_cb3.end())
      << "Make sure after we read the whole buffer, we stop reading.";

  ASSERT_EQ(m_cb3.size(), 3);
}

TEST_F(TestCircularBuffer, ReadLoop)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);
  m_cb3.push_back(4);
  m_cb3.push_back(5);

  auto iter = m_cb3.begin();
  ASSERT_TRUE(iter != m_cb3.end());
  ASSERT_EQ(*iter, 3);
  ++iter;

  ASSERT_TRUE(iter != m_cb3.end());
  ASSERT_EQ(*iter, 4);
  ++iter;

  ASSERT_TRUE(iter != m_cb3.end());
  ASSERT_EQ(*iter, 5);
  ++iter;

  ASSERT_TRUE(iter == m_cb3.end())
      << "Make sure after we read the whole buffer, we stop reading.";

  ASSERT_EQ(m_cb3.size(), 3);
}

TEST_F(TestCircularBuffer, ReadLoopTwiceWithIteratorWrite)
{
  const int values[] = {1, 2, 3, 4, 5, 6, 7};
  for (auto value : values) {
    m_cb3.push_back(value);
  }

  auto iter = m_cb3.begin();
  ASSERT_TRUE(iter != m_cb3.end());
  ASSERT_EQ(*iter, 5);
  ++iter;

  ASSERT_TRUE(iter != m_cb3.end());
  ASSERT_EQ(*iter, 6);
  ++iter;

  ASSERT_TRUE(iter != m_cb3.end());
  ASSERT_EQ(*iter, 7);
  ++iter;

  ASSERT_TRUE(iter == m_cb3.end())
      << "Make sure after we read the whole buffer, we stop reading.";

  ASSERT_EQ(m_cb3.size(), 3);
}
