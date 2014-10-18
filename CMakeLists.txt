cmake_minimum_required(VERSION 2.6)
project(swpitcher)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(GetF0)
set(LIBS ${LIBS} GetF0)

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

find_package(SDL REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(OpenGL REQUIRED)
# sw: TODO: Apparently it's not good to use PkgConfig directly
find_package(PkgConfig)
pkg_check_modules(PA_SIMPLE REQUIRED libpulse-simple)

include_directories(${SDL_INCLUDE_DIR} ${SDLTTF_INCLUDE_DIR} ${OPENGL_INCLUDE_DIR} ${PA_SIMPLE_INCLUDE_DIRS})
set(LIBS ${LIBS} ${SDL_LIBRARY} ${SDLTTF_LIBRARY} ${OPENGL_gl_LIBRARY} ${PA_SIMPLE_LIBRARIES})


target_link_libraries(swpitcher ${LIBS})

