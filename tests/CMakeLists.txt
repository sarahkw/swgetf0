set(GTEST_PATH "" CACHE STRING "Path to GTEST installation")
set(GMOCK_PATH "" CACHE STRING "Path to GMOCK installation")

include_directories("${GTEST_PATH}/include")
include_directories("${GMOCK_PATH}/include")

add_executable(unittest
  TestCircularBuffer.cpp
  TestScheme.cpp
  ../schemeinterface.cpp
  ../unused/StreamReadBuffer.cpp
  ${QTFILES_RCC}  # from parent CMakeLists.txt
)
qt5_use_modules(unittest Widgets)
target_link_libraries(unittest
  pthread
  tinyscheme
  "${GTEST_PATH}/libgtest.a" "${GMOCK_PATH}/libgmock_main.a")
