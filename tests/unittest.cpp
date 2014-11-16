/*
    Copyright (C) 2014  Sarah Wong

    This file is part of swgetf0.

    swgetf0 is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "gtest/gtest.h"

#include "../CircularBuffer.h"

namespace {

class CircularBufferTest : public ::testing::Test {
protected:
  CircularBufferTest() : m_cb3(3) {}

  void SetUp() override {}

  void TearDown() override {}

  CircularBuffer<int> m_cb3;
};
}

TEST(CircularBufferZeroTest, TestThrow)
{
  EXPECT_THROW(CircularBuffer<int>(0), std::invalid_argument);
}

TEST_F(CircularBufferTest, ReadEmptyBuffer)
{
  ASSERT_TRUE(m_cb3.begin() == m_cb3.end());

  ASSERT_EQ(m_cb3.size(), 0);
}

TEST_F(CircularBufferTest, ReadWithoutLoop)
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

TEST_F(CircularBufferTest, ReadLoop)
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

TEST_F(CircularBufferTest, ReadLoopTwiceWithIteratorWrite)
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
