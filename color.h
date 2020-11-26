#pragma once

#include "common.h"

struct Color
{
  ui8 r, g, b;
  Color(ui8 r = 0, ui8 g = 0, ui8 b = 0) : r(r), g(g), b(b) {};
};
