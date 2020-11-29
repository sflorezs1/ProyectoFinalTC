#pragma once

#include "common.h"

/*
   Struct to store a RGB Value
   unsigned short int r, g, b
*/
struct Color
{
  ui8 r, g, b;
  
  /*
     Trivial Constructor
  */
  Color(ui8 r = 0, ui8 g = 0, ui8 b = 0) : r(r), g(g), b(b) {};
};
