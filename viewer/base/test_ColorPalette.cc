#include <iostream>
#include <cstdlib>

#include "ColorPalette.h"

using namespace viewer;

int main(int argc, char *argv[])
{
  int cols = atol(argv[1]);

  base::ColorPalette cp(cols);

  for (int i = 0; i < cols; ++i) {
    std::cout << cp.color(i) << std::endl;
  }

  return 0;
}
