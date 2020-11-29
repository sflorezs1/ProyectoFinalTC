#include <EEPROM.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <DHTesp.h>
#include <ESP8266mDNS.h>

/*
  Include OWN headers
*/
#include "common.h"
#include "page.h"
#include "color.h"
#include "led.h"

// Clear pin
#define CLRPin D1

// DHT Pin
#define DHTPin D0

// Access Point Credentials
#define AP_SSID "Project_RGB_Automation"
#define AP_PASS "conmutacion20202*"

// Timeout for connection
#define TIMEOUT 10000

// DHT Sensor Object
DHTesp dht;
// Multicast DNS Object
MDNSResponder mdns;

bool isConnectionSuccessful = false;

// Character to end Strings in EEPROM
const char ENDCHAR = 3;

// Time counters
unsigned long int startTime, startTimeLoop = 0;

// Store the themperature
float temperature = 0.0;

// Wireless Network Credentials
String wifiId = "", wifiPass = "";


/*
  Setup Wifi Access Point for network configuration
*/
void setAP()
{
  // Print Access Point Credentials
  Serial.print("SSID: ");
  Serial.println(AP_SSID);
  Serial.print("Password: ");
  Serial.println(AP_PASS);

  // Create network objects
  IPAddress apLocalIp(192, 168, 1, 1);
  IPAddress apGateway(192, 168, 1, 254);
  IPAddress apSubnet(255, 255, 255, 0);

  // Start Access Point
  WiFi.softAP(AP_SSID, AP_PASS);
  WiFi.hostname("Project_RGB_Remote_NMCU");
  WiFi.softAPConfig(apLocalIp, apGateway, apSubnet);
  delay(100);
  Serial.print("IP address: ");
  Serial.println(WiFi.softAPIP());
}

/*
  Setup webserver
*/
void setServer()
{
  Serial.println("Launching server");
  Serial.println("Server was launched on ");

  // Create routes on server
  // Handle root page, Wireless Network registration
  server.on("/",                HTTP_GET,   NMCUServer::handleRoot);
  // Handle response on root page, try connection and store credentials
  server.on("/",                HTTP_POST,  NMCUServer::handleResponse);
  // Handle led color request, return a JSON with all led colors
  server.on("/get_leds",        HTTP_GET,   NMCUServer::hanldeColorRequest);
  // Handle led color change request, decode HEX String to RGB Struct
  server.on("/set_leds",        HTTP_GET,   NMCUServer::handleColorResponse);
  // Handle temperature request, return float in plain text
  server.on("/get_temperature", HTTP_GET,   NMCUServer::handleTempResponse);
  // Handle 404 not found
  server.onNotFound(                        NMCUServer::handleNotFound);

  // Start server
  server.begin();
}

/*
  Try to recover old credentials from the EEPROM
*/
bool isConnectionStored()
{
  String id;  // Temporal wifi ssid
  String pass;  // Temporal wifi pass

  // Try to retrieve wifi ssid from EEPROM
  for (unsigned short int i = wifiIdAddress; i <= wifiPassAddress; i++)
  {
    if (i == wifiPassAddress)
    {
      return false;
    }
    else
    {
      char r = EEPROM.read(i);
      if (r == ENDCHAR)
      {
        break;
      }
      id += r;
    }
  }
  Serial.print("\tFound Wifi SSID = ");
  Serial.println(id);

  // Try to retrieve wifi pass from EEPROM
  for (unsigned short int i = wifiIdAddress; i <= wifiPassAddress + 64; i++)
  {
    if (i == wifiPassAddress + 64)
    {
      return false;
    }
    else
    {
      char r = EEPROM.read(i);
      if (r == ENDCHAR)
      {
        break;
      }
      pass += r;
    }
  }

  Serial.print("\tFound Wifi Password = ");
  Serial.println(pass);
  return true;
}

void setup()
{
  // Init Serial Communication
  Serial.begin(9600);
  // CLR
  pinMode(CLRPin, INPUT_PULLUP);
  bool resetRom = digitalRead(CLRPin);
  Serial.println(resetRom ? "YES" : "NO");
  // Init EEPROM with 512 bytes
  EEPROM.begin(512);
  // Erase all 512 bytes from EEPROM
  if (resetRom)
  {
    resetEEPROM();
  }
  Serial.println("Starting booting sequence...");
  // Set default leds
  Serial.println("Enumerating LEDs...");

  // Recover last colors from the LEDs
  l0.recoverColor();
  l1.recoverColor();

  // Setup DHT Temperature Sensor
  dht.setup(DHTPin, DHTesp::DHT22);
  Serial.println("Testing wireless connection...");
  Serial.println("\tChecking stored connections...");
  setServer();
  // Try to recover last used connection
  if (isConnectionStored())
  {
    Serial.println("Using stored connection");
    // Try to connect using retreived credentials
    if (attemptConnection())
    {
      Serial.println("Connection was successful!");
      isConnectionSuccessful = true;
    }
    else
    {
      Serial.println("Stored connection is not available");
      isConnectionSuccessful = false;
    }
  }
  else
  {
    Serial.println("No stored connections were found,");
    isConnectionSuccessful = false;
  }
  // Expose WiFi Access Point if not connected
  if (!isConnectionSuccessful)
  {
    setAP();
  }
}

void loop()
{
  // Wait for the DHT minimum sampling period
  if ((millis() - startTimeLoop) >= (unsigned long long int) dht.getMinimumSamplingPeriod())
  {
    startTimeLoop = millis();
    float tmp = dht.getTemperature();
    temperature = isnan(tmp) ? temperature : tmp;
  }
  // Manage Multicast DNS
  mdns.update();

  // Manage Web Server
  server.handleClient();
  //Try Connection
  if (wifiId != "" && wifiPass != "" && !isConnectionSuccessful)
  {
    isConnectionSuccessful = attemptConnection();
  }
}

/*
   Write Wifi Credentials to EEPROM
*/
void saveWifiCredentials()
{
  // Save SSID
  unsigned int c = 0;
  for (unsigned short int i = wifiIdAddress; i < wifiIdAddress + wifiId.length(); i++)
  {
    EEPROM.write(i, wifiId.charAt(c));
    c++;
  }
  EEPROM.write(wifiIdAddress + wifiId.length(), ENDCHAR);
  // Save PASS
  c = 0;
  for (unsigned short int i = wifiPassAddress; i < wifiPassAddress + wifiPass.length(); i++)
  {
    EEPROM.write(i, wifiPass.charAt(c));
    c++;
  }
  EEPROM.write(wifiPassAddress + wifiPass.length(), ENDCHAR);
  EEPROM.commit();
}

/*
   Try to make a connection, if it times out, the config page will run again
*/
bool attemptConnection()
{
  startTime = millis();
  WiFi.begin(wifiId, wifiPass);
  while (WiFi.status() != WL_CONNECTED)
  {
    Serial.print(".");
    delay(100);
    Serial.println(millis() - startTime);
    if ((millis() - startTime) >= TIMEOUT)
    {
      Serial.println("Connection timed out, awaiting for new credentials");
      WiFi.disconnect();
      wifiId = "";
      wifiPass = "";
      return false;
    }
  }
  Serial.println("Connection was Successful");
  // Stop WebServer
  // server.stop();
  // Remove Access Point as connections will now be done over wifi
  WiFi.softAPdisconnect(true);
  // Save the valid credentials to EEPROM
  saveWifiCredentials();
  Serial.print("Local IP Addr: ");
  Serial.println(WiFi.localIP());
  if (mdns.begin("nodemcu", WiFi.localIP()))
  {
    Serial.println("mDNS started!");
  }
  MDNS.addService("http", "tcp", 80);
  return true;
}

/*
   Erase all data from EEPROM
*/
void resetEEPROM()
{
  Serial.print("Clearing EEPROM ");
  // Go byte to byte reseting to 0
  for (unsigned int i = 0; i < 512; i++)
  {
    EEPROM.write(i, 0);
    if (i % 9 == 0)
    {
      Serial.print(".");
    }
  }
  Serial.println();
  EEPROM.commit();
  Serial.println("EEPROM cleared!");
}
