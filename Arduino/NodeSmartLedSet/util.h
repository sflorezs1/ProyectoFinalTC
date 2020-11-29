#pragma once

#include "color.h"

/*
   Namespace to include all Utilities
*/
namespace Util
{
/*
   Transform HEX Strinv Color value into Color Struct
*/
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

/*
   convert Integer value into two digit HEX String
*/
String hexify(ui8 a)
{
  return a > 15 ? String(a, HEX) : "0" + String(a, HEX);
}

/*
   Transform Color struct into HEX String Color value
*/
String RGBToHex(Color c)
{
  return "#" + hexify(c.r) + hexify(c.g) + hexify(c.b);
}
}
