#pragma once

#include "common.h"
#include "color.h"

// Predefined pins
#define LED0R D2
#define LED0G D3
#define LED0B D5
#define LED1R D6
#define LED1G D7
#define LED1B D8

typedef unsigned short int ui8;

ui8 currentAddress = 0;

/*
   Return the next assignable address in the EEPROM
*/
unsigned int assignAddress()
{
  if (currentAddress >= wifiIdAddress)
  {
    Serial.println("Ran out of addresses, the program may not work as intended!");
  }
  return currentAddress++;
}

/*
   Basic LED Class to manage a RGB Led
   Color c: Current Color the LED is representing
   unsigned short int rPin, gPin, bPin, rAddr, gAddr, bAddr: Physical pins and EEPROM addresses
   String identifier: String identifier for the LED
*/
class Led
{
  public:
    /*
       Constructor
    */
    Led(String identifier, ui8 rPin, ui8 gPin, ui8 bPin)
    {
      pinMode(rPin, OUTPUT);
      pinMode(gPin, OUTPUT);
      pinMode(bPin, OUTPUT);
      this->identifier = identifier;
      this->rPin = rPin;
      this->gPin = gPin;
      this->bPin = bPin;
      this->rAddr = assignAddress();
      this->gAddr = assignAddress();
      this->bAddr = assignAddress();
    }

    /*
       Set a new Color
       Color c: New color to represent
    */
    void setColor(Color c)
    {
      // Common anode LED
      analogWrite(this->rPin, 1023 - (int)(c.r * 24.9));
      analogWrite(this->gPin, 1023 - (int)(c.g * 24.9));
      analogWrite(this->bPin, 1023 - (int)(c.b * 24.9));
      this->c = c;
      this->printColor();
      this->saveCurrentColor();
    }

    /*
       Print the current Color Struct Stored on EEPROM
    */
    void printColorFromEEPROM() {
      Serial.println(EEPROM.read(this->rAddr));
      Serial.println(EEPROM.read(this->gAddr));
      Serial.println(EEPROM.read(this->bAddr));

    }

    /*
       Print the current Color Struct Stored on Object
    */
    void printColor()
    {
      Serial.print(this->identifier);
      Serial.print(" set to color(");
      Serial.print(this->c.r);
      Serial.print(", ");
      Serial.print(this->c.g);
      Serial.print(", ");
      Serial.print(this->c.b);
      Serial.println(")");
      Serial.println(this->rAddr);
      Serial.println(this->gAddr);
      Serial.println(this->bAddr);
    }

    /*
       Return the current Color Struct Stored in Object
    */
    Color getColor()
    {
      return this->c;
    }

    /*
       Return LED String identifier
    */
    String getIdentifier()
    {
      return this->identifier;
    }

    /*
       Set LED to Color Struct Stored on EEPROM
       Warning: Must be used after setup, EEPROM.begin()
    */
    void recoverColor() {
      // Recover last color used
      // If none led will turn off
      this->setColor({
        .r = (ui8) EEPROM.read(this->rAddr),
        .g = (ui8) EEPROM.read(this->gAddr),
        .b = (ui8) EEPROM.read(this->bAddr)
      });
    }

    /*
       Save current Color Struct stored in Object to EEPROM
    */
    void saveCurrentColor()
    {
      EEPROM.write(this->rAddr, this->c.r);
      EEPROM.write(this->gAddr, this->c.g);
      EEPROM.write(this->bAddr, this->c.b);
      EEPROM.commit();
    }

  private:
    ui8 rPin, gPin, bPin, rAddr, gAddr, bAddr;
    Color c;
    String identifier;
};
