add_definitions(-DUSE_DL=0 -DUSE_MATH=1 -DUSE_ASCII_NAMES=0 -DSTANDALONE=0)

add_library(tinyscheme
  scheme.c)
