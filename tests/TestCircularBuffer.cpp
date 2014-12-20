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

#include "../CircularBuffer.h"

using namespace testing;

namespace {

class TestCircularBuffer : public ::testing::Test {
protected:
  TestCircularBuffer() : m_cb3(3), m_cb0(0) {}

  void SetUp() override {}

  void TearDown() override {}

  CircularBuffer<int> m_cb3;

  CircularBuffer<int> m_cb0;
};
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

  ASSERT_THAT(m_cb3, ElementsAre(1, 2, 3));

  ASSERT_EQ(m_cb3.size(), 3);
}

TEST_F(TestCircularBuffer, ReadLoop)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);
  m_cb3.push_back(4);
  m_cb3.push_back(5);

  ASSERT_THAT(m_cb3, ElementsAre(3, 4, 5));

  ASSERT_EQ(m_cb3.size(), 3);
}

TEST_F(TestCircularBuffer, ReadLoopTwiceWithIteratorWrite)
{
  const int values[] = {1, 2, 3, 4, 5, 6, 7};
  for (auto value : values) {
    m_cb3.push_back(value);
  }

  ASSERT_THAT(m_cb3, ElementsAre(5, 6, 7));

  ASSERT_EQ(m_cb3.size(), 3);
}

TEST_F(TestCircularBuffer, ZeroWorkingSet)
{
  ASSERT_TRUE(m_cb0.begin() == m_cb0.end());
  m_cb0.push_back(0);
  ASSERT_TRUE(m_cb0.begin() == m_cb0.end());
}
