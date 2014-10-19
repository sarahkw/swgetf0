cmake_minimum_required(VERSION 2.6)
project(swpitcher)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(GetF0)

option(ENABLE_TESTS "Compile unit tests" OFF)
if(ENABLE_TESTS)
  add_subdirectory(tests)
endif(ENABLE_TESTS)

find_package(SDL REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(OpenGL REQUIRED)
# sw: TODO: Apparently it's not good to use PkgConfig directly
find_package(PkgConfig)
pkg_check_modules(PA_SIMPLE REQUIRED libpulse-simple)

include_directories(${SDL_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${PA_SIMPLE_INCLUDE_DIRS})

################################################################################
# swpitcher.qt

  # TODO um, don't release this
set(CMAKE_PREFIX_PATH "/home/sarah/Qt/5.3/gcc_64/lib/cmake")
find_package(Qt5Widgets REQUIRED)
find_package(Qt5OpenGL REQUIRED)
QT5_WRAP_UI(QTFILES_UI ui/mainwindow.ui)
QT5_WRAP_CPP(QTFILES_CPP mainwindow.h)
add_executable(swpitcher.qt
  swpitcher.qt.cpp
  mainwindow.cpp
  CircularBuffer.cpp
  GetF0StreamImpl.cpp
  ${QTFILES_UI} ${QTFILES_CPP})
qt5_use_modules(swpitcher.qt Widgets OpenGL)
target_link_libraries(swpitcher.qt GetF0 ${PA_SIMPLE_LIBRARIES})

################################################################################
# swpitcher

add_executable(swpitcher
  CircularBuffer.cpp
  GetF0StreamImpl.cpp
  swpitcher.cpp
  viewer/video/Driver.cc
  viewer/video/Text.cc
  viewer/video/Color.cc
  viewer/base/ColorPalette.cc
  viewer/viewer.cc
)
target_link_libraries(swpitcher
  GetF0
  ${SDL_LIBRARY} ${SDLTTF_LIBRARY} ${OPENGL_gl_LIBRARY} ${PA_SIMPLE_LIBRARIES})

################################################################################
# echo

add_executable(echo/echo
  echo/echo.c
)
target_link_libraries(echo/echo ${PA_SIMPLE_LIBRARIES})
