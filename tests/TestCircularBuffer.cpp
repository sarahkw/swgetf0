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

TEST_F(TestCircularBuffer, ExpandBlank)
{
  m_cb3.expand(3);
  ASSERT_THAT(m_cb3, ElementsAre());
}

TEST_F(TestCircularBuffer, ExpandNotFull)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.expand(3);
  ASSERT_THAT(m_cb3, ElementsAre(1, 2));
}

TEST_F(TestCircularBuffer, ExpandFull)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);
  m_cb3.expand(3);
  ASSERT_THAT(m_cb3, ElementsAre(0, 0, 0, 1, 2, 3));
}

TEST_F(TestCircularBuffer, ExpandOverflow1)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);
  m_cb3.push_back(4);
  m_cb3.expand(3);
  ASSERT_THAT(m_cb3, ElementsAre(0, 0, 0, 2, 3, 4));
}

TEST_F(TestCircularBuffer, ExpandOverflow2)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);
  m_cb3.push_back(4);
  m_cb3.push_back(5);
  m_cb3.push_back(6);
  m_cb3.expand(3);
  ASSERT_THAT(m_cb3, ElementsAre(0, 0, 0, 4, 5, 6));

  // Make sure it still works afterwards

  m_cb3.push_back(7);
  m_cb3.push_back(8);
  m_cb3.push_back(9);
  ASSERT_THAT(m_cb3, ElementsAre(4, 5, 6, 7, 8, 9));

  m_cb3.push_back(10);
  ASSERT_THAT(m_cb3, ElementsAre(5, 6, 7, 8, 9, 10));
}

TEST_F(TestCircularBuffer, ShrinkAllEmpty)
{
  m_cb3.shrink(3);
  ASSERT_THAT(m_cb3, ElementsAre());
  ASSERT_EQ(m_cb3.size(), 0);

  // Use after
  m_cb3.push_back(0);
  ASSERT_THAT(m_cb3, ElementsAre());
  ASSERT_EQ(m_cb3.size(), 0);
}

TEST_F(TestCircularBuffer, ShrinkAllFull)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);
  m_cb3.shrink(3);
  ASSERT_EQ(m_cb3.size(), 0);
  ASSERT_THAT(m_cb3, ElementsAre());

  m_cb3.push_back(3);
  ASSERT_EQ(m_cb3.size(), 0);
  ASSERT_THAT(m_cb3, ElementsAre());
}

TEST_F(TestCircularBuffer, ShrinkAllOverflow)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);
  m_cb3.push_back(4);
  m_cb3.shrink(3);
  ASSERT_EQ(m_cb3.size(), 0);
  ASSERT_THAT(m_cb3, ElementsAre());

  m_cb3.push_back(3);
  ASSERT_EQ(m_cb3.size(), 0);
  ASSERT_THAT(m_cb3, ElementsAre());
}

TEST_F(TestCircularBuffer, ShrinkAllPartial)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.shrink(3);
  ASSERT_EQ(m_cb3.size(), 0);
  ASSERT_THAT(m_cb3, ElementsAre());

  m_cb3.push_back(3);
  ASSERT_EQ(m_cb3.size(), 0);
  ASSERT_THAT(m_cb3, ElementsAre());
}

TEST_F(TestCircularBuffer, ShrinkPartialEmpty)
{
  m_cb3.shrink(2);
  ASSERT_THAT(m_cb3, ElementsAre());
  ASSERT_EQ(m_cb3.size(), 0);

  // Use after
  m_cb3.push_back(0);
  ASSERT_THAT(m_cb3, ElementsAre(0));
  ASSERT_EQ(m_cb3.size(), 1);
}

TEST_F(TestCircularBuffer, ShrinkPartialNotFullNoLoss)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.shrink(1);
  ASSERT_THAT(m_cb3, ElementsAre(1, 2));
  ASSERT_EQ(m_cb3.size(), 2);

  // Use after
  m_cb3.push_back(3);
  ASSERT_THAT(m_cb3, ElementsAre(2, 3));
  ASSERT_EQ(m_cb3.size(), 2);
  m_cb3.push_back(4);
  ASSERT_THAT(m_cb3, ElementsAre(3, 4));
  ASSERT_EQ(m_cb3.size(), 2);
}

TEST_F(TestCircularBuffer, ShrinkPartialNotFullWithLoss)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.shrink(2);
  ASSERT_THAT(m_cb3, ElementsAre(2));
  ASSERT_EQ(m_cb3.size(), 1);

  // Use after
  m_cb3.push_back(3);
  m_cb3.push_back(4);
  m_cb3.push_back(5);
  ASSERT_THAT(m_cb3, ElementsAre(5));
  ASSERT_EQ(m_cb3.size(), 1);
  m_cb3.push_back(6);
  m_cb3.push_back(7);
  ASSERT_THAT(m_cb3, ElementsAre(7));
  ASSERT_EQ(m_cb3.size(), 1);
}

TEST_F(TestCircularBuffer, ShrinkPartialFull)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);
  m_cb3.shrink(2);
  ASSERT_THAT(m_cb3, ElementsAre(3));
  ASSERT_EQ(m_cb3.size(), 1);

  // Use after
  m_cb3.push_back(4);
  m_cb3.push_back(5);
  ASSERT_THAT(m_cb3, ElementsAre(5));
  ASSERT_EQ(m_cb3.size(), 1);
}

TEST_F(TestCircularBuffer, ShrinkPartialOverflow)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);
  m_cb3.push_back(4);
  m_cb3.shrink(1);
  ASSERT_EQ(m_cb3.size(), 2);
  ASSERT_THAT(m_cb3, ElementsAre(3, 4));

  m_cb3.push_back(5);
  ASSERT_EQ(m_cb3.size(), 2);
  ASSERT_THAT(m_cb3, ElementsAre(4, 5));
  m_cb3.push_back(6);
  ASSERT_EQ(m_cb3.size(), 2);
  ASSERT_THAT(m_cb3, ElementsAre(5, 6));
}

TEST_F(TestCircularBuffer, ShrinkPartialOverflow2)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);
  m_cb3.push_back(4);
  m_cb3.push_back(5);
  m_cb3.shrink(2);
  ASSERT_EQ(m_cb3.size(), 1);
  ASSERT_THAT(m_cb3, ElementsAre(5));

  m_cb3.push_back(7);
  ASSERT_EQ(m_cb3.size(), 1);
  ASSERT_THAT(m_cb3, ElementsAre(7));
  m_cb3.push_back(8);
  ASSERT_EQ(m_cb3.size(), 1);
  ASSERT_THAT(m_cb3, ElementsAre(8));
}

TEST_F(TestCircularBuffer, ShrinkExpandFull)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);

  m_cb3.shrink(2);
  m_cb3.expand(1);

  ASSERT_THAT(m_cb3, ElementsAre(0, 3));
  ASSERT_EQ(m_cb3.size(), 2);
}

TEST_F(TestCircularBuffer, ShrinkExpandFullWrap)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);
  m_cb3.push_back(4);
  m_cb3.push_back(5);

  m_cb3.shrink(2);
  m_cb3.expand(2);

  ASSERT_EQ(m_cb3.size(), 3);
  ASSERT_THAT(m_cb3, ElementsAre(0, 0, 5));
}

TEST_F(TestCircularBuffer, ShrinkExpandNotFull)
{
  m_cb3.push_back(1);

  m_cb3.shrink(1);
  m_cb3.expand(1);

  ASSERT_THAT(m_cb3, ElementsAre(1));
  ASSERT_EQ(m_cb3.size(), 1);
}

TEST_F(TestCircularBuffer, ShrinkExpandNotFullBecomeFull)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);

  m_cb3.shrink(1);
  m_cb3.expand(1);

  ASSERT_THAT(m_cb3, ElementsAre(0, 1, 2));
  ASSERT_EQ(m_cb3.size(), 3);
}
