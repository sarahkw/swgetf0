cmake_minimum_required(VERSION 2.6)
project(swgetf0)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(GetF0)

option(ENABLE_TESTS "Compile unit tests" OFF)
if(ENABLE_TESTS)
  add_subdirectory(tests)
endif(ENABLE_TESTS)

# sw: TODO: Apparently it's not good to use PkgConfig directly
find_package(PkgConfig)
pkg_check_modules(PA_SIMPLE REQUIRED libpulse-simple)

include_directories(${PA_SIMPLE_INCLUDE_DIRS})

################################################################################
# swgetf0

  # TODO um, don't release this
set(CMAKE_PREFIX_PATH "/home/sarah/Qt/5.3/gcc_64/lib/cmake")
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
find_package(Qt5Multimedia REQUIRED)
QT5_WRAP_UI(QTFILES_UI mainwindow.ui about.ui)
QT5_WRAP_CPP(QTFILES_CPP mainwindow.h about.h)
add_executable(swgetf0
  swgetf0.cpp
  mainwindow.cpp
  CircularBuffer.cpp
  GetF0StreamImpl.cpp
  about.cpp
  ${QTFILES_UI} ${QTFILES_CPP})
qt5_use_modules(swgetf0 Widgets OpenGL Multimedia)
target_link_libraries(swgetf0 GetF0 ${PA_SIMPLE_LIBRARIES})

################################################################################
# echo

add_executable(echo/echo
  echo/echo.c
)
target_link_libraries(echo/echo ${PA_SIMPLE_LIBRARIES})
