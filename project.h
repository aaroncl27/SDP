#ifndef PROJECT__INCLUDED
#define PROJECT_H_INCLUDED

#include <OneWire.h>
#include <DallasTemperature.h>
#include <PWM.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define Timer 22
#define TempSet1 23
#define TempSet2 24
#define relayPin 13
#define buzzPin 3
#define Temp1 A0
#define Temp2 A2
#define Temp3 A4
#define Temp4 A6
#define Temp5 A8
#define ONE_WIRE_BUS 2
#define ckt 11
#define TempPin1 12 // assign pin 12 to High Temp
#define TempPin2 10// assign pin 13 to Medium Temp
#define TimePin1  5 // assign pin 5 to 15 Minutes

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);

#endif // PROJECT_H_INCLUDED
