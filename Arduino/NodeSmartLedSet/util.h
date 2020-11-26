#pragma once

#include "color.h"

namespace Util
{
Color hexToRGB(String hexValue)
{
  char *ptr;
  hexValue = hexValue.substring(1, hexValue.length());
  char hex[sizeof(hexValue)];
  hexValue.toCharArray(hex, sizeof(hex));
  unsigned long int hexint = strtoul(hex, &ptr, 16);
  return {
    .r = (ui8)((hexint >> 16) & 0xFF),
    .g = (ui8)((hexint >> 8) & 0xFF),
    .b = (ui8)((hexint) & 0xFF)};
}

String hexify(ui8 a)
{
  return a > 15 ? String(a, HEX) : "0" + String(a, HEX);
}

String RGBToHex(Color c)
{
  return "#" + hexify(c.r) + hexify(c.g) + hexify(c.b);
}
}
