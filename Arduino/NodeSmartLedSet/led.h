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

unsigned int assignAddress()
{
  if (currentAddress >= wifiIdAddress)
  {
    Serial.println("Ran out of addresses, the program may not work as intended!");
  }
  return currentAddress++;
}

class Led
{
  public:
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

    void printColorFromEEPROM() {
      Serial.println(EEPROM.read(this->rAddr));
      Serial.println(EEPROM.read(this->gAddr));
      Serial.println(EEPROM.read(this->bAddr));

    }

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

    Color getColor()
    {
      return this->c;
    }

    String getIdentifier()
    {
      return this->identifier;
    }

    // After setup
    void recoverColor() {
      // Recover last color used
      // If none led will turn off
      this->setColor({
        .r = (ui8) EEPROM.read(this->rAddr),
        .g = (ui8) EEPROM.read(this->gAddr),
        .b = (ui8) EEPROM.read(this->bAddr)
      });
    }

    void saveCurrentColor()
    {
      EEPROM.write(this->rAddr, this->c.r);
      EEPROM.write(this->gAddr, this->c.g);
      EEPROM.write(this->bAddr, this->c.b);
      EEPROM.commit();
    }

  private:
    unsigned short int rPin, gPin, bPin, rAddr, gAddr, bAddr;
    Color c;
    String identifier;
};
