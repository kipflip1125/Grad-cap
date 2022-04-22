#include <Adafruit_GFX.h>
#include <FastLED_NeoMatrix.h>
#include <FastLED.h>
#include "penguin.h"
#include "csuf.h"

#include <ArduinoBLE.h>

// defining led matrix data
#define BRIGHTNESS 20
#define DATA_PIN 2
#define mw 16
#define mh 16
#define NUMMATRIX (mw*mh)
CRGB leds[NUMMATRIX];
FastLED_NeoMatrix *matrix = new FastLED_NeoMatrix(leds, mw, mh,
                                               NEO_MATRIX_TOP + NEO_MATRIX_RIGHT
                                               + NEO_MATRIX_ROWS + NEO_MATRIX_ZIGZAG);

// Defining necessary bluetooth data
BLEService ledService("19B10000-E8F2-537E-4F6C-D104768A1214");

BLEByteCharacteristic switchCharacteristic("led", BLERead |BLEWrite);

#define CSUF_BLUE 0x0129
#define CSUF_ORANGE 0x63E0

void display_picture(const unsigned short picture[])
{
  matrix->drawRGBBitmap(0, 0, picture, 16, 16);
  matrix->show();
}

void display_scrollText(String message1, String message2) {
    uint8_t size = max(int(mw/8), 1);
    matrix->clear();
    matrix->setTextWrap(false);  // we don't wrap text so it scrolls nicely
    matrix->setTextSize(1);
    matrix->setRotation(0);
    for (int8_t x=7; x>-50; x--)
    {
        yield();
        matrix->clear();
        matrix->setCursor(x,0);
        matrix->setTextColor(CSUF_BLUE);
        matrix->print(message1);
        if (mh>11)
        {
            matrix->setCursor(-20-x,mh-7);
            matrix->setTextColor(CSUF_ORANGE);
            matrix->print(message2);
        }
        matrix->show();
             delay(50);
    }
}

void setup()
{
  delay(1000);
  Serial.begin(9600);
  
  FastLED.addLeds<WS2812, DATA_PIN, RGB>(leds, NUMMATRIX);
  FastLED.setBrightness(BRIGHTNESS);
  
  if(!BLE.begin())
  {
    Serial.println("starting Bluetooth Low Energy module failed!");
    while(1);
  }

  BLE.setLocalName("Nano LED");
  BLE.setAdvertisedService(ledService);

  ledService.addCharacteristic(switchCharacteristic);

  BLE.addService(ledService);

  BLE.advertise();

  Serial.println("BLE LED Peripheral");
  Serial.print("Init on pin: ");
  Serial.println(DATA_PIN);
  Serial.print("Matrix Size: ");
  Serial.print(mw);
  Serial.print(" ");
  Serial.print(mh);
  Serial.print(" ");
  Serial.println(NUMMATRIX);
  Serial.println("Waiting for connections...");
}

void blackout()
{
  for(int i = 0; i < mw*mh; i++)
  {
    leds[i] = CRGB(0,0,0);
  }
  matrix->show();
}

void loop()
{
  BLEDevice central = BLE.central();
  if(central)
  {
    Serial.print("Connected to central: ");
    Serial.println(central.address());

    while(central.connected())
    {
      if(switchCharacteristic.written())
      {
        if(switchCharacteristic.value())
        {     
          Serial.println(char(switchCharacteristic.value()));
          switch(switchCharacteristic.value())
          {
            case 01:
              Serial.println("led Blackout");
              blackout();
              break;
            case 02:
              Serial.println("printing penguin");
              display_picture(penguin);
              break;
            case 03:
              Serial.println("printing csuf logo");
              display_picture(csuf);
              break;
            case 04:
              Serial.println("Printing congrats grad");
              display_scrollText("Congrats", "Grad");
              break;
          }
        }
      }
    }
  }
  
}
