set(GTEST_PATH "" CACHE STRING "Path to GTEST installation")

include_directories("${GTEST_PATH}/include")

add_executable(unittest unittest.cpp)
target_link_libraries(unittest
  pthread
  "${GTEST_PATH}/make/gtest.a" "${GTEST_PATH}/make/gtest_main.a")
