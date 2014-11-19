/*
  Copyright 2014 Sarah Wong
*/

#include "gtest/gtest.h"

#include <iostream>

#include "../StreamReadBuffer.h"

namespace {

class TestStreamReadBuffer : public ::testing::Test {
protected:
  TestStreamReadBuffer() {}

  void SetUp() override {
    m_srb = new StreamReadBuffer(5, [](void* buffer, size_t size) {
	std::cout << "Stream read, of size " << size << std::endl;
	return size;
    });
  }

  void TearDown() override {}

  StreamReadBuffer *m_srb;
};

}

TEST_F(TestStreamReadBuffer, Run) {
  // NOP, just run fixture
}
