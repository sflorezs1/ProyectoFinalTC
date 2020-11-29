#pragma once

#include "common.h"

#include "util.h"

/*
   Namespace to include all Server Pages
*/
namespace NMCUServer
{
/*
   Handle Network registration page
*/
void handleRoot()
{
  server.send(200, "text/html", R"rawliteral(
            <!DOCTYPE html>
            <html>
            <head>
                <title> Project RGB Automation Wireless Selector</title>
            </head>
            <body>
                <h1>Wireless Network Selection</h2>
                <br/>
                <p>Please enter the ssid and password of your Wireless Network.</p>
                <p>If the connection is successful, the wireless network you're connected should dissapear, otherwise, it will stay and you should try again. </p>
                <br/>
                <form action="/" method="post">
                <label for="wssid">SSID: </label>
                <input type="text" id="wssid" name="wssid"/>
                <br/>
                <label for="wpass">Password: </label>
                <input type="password" required id="wpass" name="wpass"/>
                <input type="submit" required value="Connect"/>
                </form>
            </body>
            </html>
        )rawliteral");
}

/*
   Handle a get_temperature request to the server,
   Sends a float in plain text
*/
void handleTempResponse()
{
  Serial.println("Temperature requested");
  Serial.print("temperature: ");
  Serial.println(temperature);
  server.send(200, "text/plain", String(temperature));
}

/*
    Handle server response with arguments for WiFi Credentials
*/
void handleResponse()
{
  // Check if request has arguments for wssid parameter
  if (server.hasArg("wssid") && server.arg("wssid").length() > 0)
  {
    // Store wssid as WiFi SSID
    wifiId = server.arg("wssid");
    Serial.print("Wifi ssid aquired successfully = ");
    Serial.println(wifiId);
  }
  // Check if request has arguments for wpass parameter
  if (server.hasArg("wpass") && server.arg("wpass").length() > 0)
  {
    // Store wpass as WiFi Password
    wifiPass = server.arg("wpass");
    Serial.print("Wifi password aquired successfully = ");
    Serial.println(wifiPass);
  }
}

/*
    Handle server response for set_leds,
    Sets LEDs to given colors
*/
void handleColorResponse()
{
  Serial.println("Color given");
  // Check if request has arguments for led_color_1 parameter
  if (server.hasArg("led_color_1") && server.arg("led_color_1").length() > 0)
  {
    // Set LED l0 to given HEX String Color
    l0.setColor(Util::hexToRGB(server.arg("led_color_1")));
    // l0.printColor();
  }
  else
  {
    Serial.println("No Color for led1 given on request");
  }
  // Check if request has arguments for led_color_2 parameter
  if (server.hasArg("led_color_2") && server.arg("led_color_2").length() > 0)
  {
    // Set LED l0 to given HEX String Color
    l1.setColor(Util::hexToRGB(server.arg("led_color_2")));
    // l1.printColor();
  }
  else
  {
    Serial.println("No color for led2 given on request");
  }
  server.send(200, "text/plain", "OK");
}


/*
    Handle server request for get_leds,
    sends JSON with HEX Strings for Current Colors on LEDs
*/
void hanldeColorRequest()
{
  Serial.println("Color requested");
  String response = "{";
  response += "\"color_led_1\": \"" + Util::RGBToHex(l0.getColor());
  response += "\", \"color_led_2\": \"" + Util::RGBToHex(l1.getColor());
  response += "\"}";
  server.send(200, "application/json", response);
}

/*
   Handle server on 404 Not Found
*/
void handleNotFound()
{
  Serial.println("Someone tried to connect to non routed page");
  server.send(404, "text/html", R"rawliteral(
        <!DOCTYPE html>
        <html>
        <head>
            <title>Error: not found</title>
        </head>
        <body>
            <h1>Error 404: not found</h1>
            <p>The requested resource was not found on this server.</p>
        </body>
        </html>
    )rawliteral");
}
}
