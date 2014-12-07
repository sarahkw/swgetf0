cmake_minimum_required(VERSION 2.6)
project(swgetf0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(GetF0)

# sw: TODO: Maybe try to contain tinyscheme to not spill this
add_definitions(
  -DUSE_DL=0
  -DUSE_MATH=1
  -DUSE_ASCII_NAMES=0
  -DSTANDALONE=0
  -DUSE_INTERFACE=1)
add_subdirectory(tinyscheme)

# sw: TODO: Apparently it's not good to use PkgConfig directly
find_package(PkgConfig)
pkg_check_modules(PORTAUDIOCPP REQUIRED portaudiocpp)

include_directories(${PORTAUDIOCPP_INCLUDE_DIRS})

################################################################################
# swgetf0

find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
QT5_ADD_RESOURCES(QTFILES_RCC resources.rcc)
QT5_WRAP_UI(QTFILES_UI mainwindow.ui about.ui configuration.ui)
QT5_WRAP_CPP(QTFILES_CPP mainwindow.h about.h configuration.h)
add_executable(swgetf0
  swgetf0.cpp
  mainwindow.cpp
  CircularBuffer.cpp
  about.cpp
  configuration.cpp
  schemeconfig.cpp
  ${QTFILES_UI} ${QTFILES_CPP}
  ${QTFILES_RCC})
qt5_use_modules(swgetf0 Widgets OpenGL)
target_link_libraries(swgetf0 GetF0 tinyscheme
  ${PORTAUDIOCPP_LIBRARIES})

################################################################################
# unit tests

option(ENABLE_TESTS "Compile unit tests" OFF)
if(ENABLE_TESTS)
  add_subdirectory(tests)
endif(ENABLE_TESTS)
