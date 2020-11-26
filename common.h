#pragma once

#include <ESP8266WebServer.h>

// For easier type identification
typedef unsigned short int ui8;

extern ui8 wifiIdAddress = 64;

extern ui8 wifiPassAddress = 128;

#include "led.h"

extern ESP8266WebServer server(80);

extern float temperature;
extern String wifiId;
extern String wifiPass;

Led l0 ("LED1", LED0R, LED0G, LED0B);
Led l1 ("LED2", LED1R, LED1G, LED1B);

Led leds[] = {l0, l1};
