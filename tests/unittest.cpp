#include "gtest/gtest.h"

#include "../CircularBuffer.h"

namespace {

class CircularBufferTest : public ::testing::Test {
protected:
  CircularBufferTest() : m_cb3(3) { }

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
  bool hasReadAnything = false;
  for (auto item : m_cb3) {
    FAIL() << "Item was returned.";
  }
}

TEST_F(CircularBufferTest, ReadWithoutLoop)
{
  m_cb3.push_back(1);
  m_cb3.push_back(2);
  m_cb3.push_back(3);

  auto iter = m_cb3.begin();
  ASSERT_NE(iter, m_cb3.end());
  ASSERT_EQ(*iter, 1);
  ++iter;

  ASSERT_NE(iter, m_cb3.end());
  ASSERT_EQ(*iter, 2);
  ++iter;

  ASSERT_NE(iter, m_cb3.end());
  ASSERT_EQ(*iter, 3);
  ++iter;

  ASSERT_EQ(iter, m_cb3.end());
}
