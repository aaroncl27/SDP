#ifndef PROJECT__INCLUDED
#define PROJECT_H_INCLUDED

#include <OneWire.h>
#include <DallasTemperature.h>
#include <PWM.h>

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define ONE_WIRE_BUS 2
#define BuzzPin 3
#define OLED_RESET 4
#define ckt 11
#define Timer 22
#define TimePin  24 // assign pin 5 to 15 Minutes
#define TempPin2 28// assign pin 13 to Medium Temp
#define TempPin1 32 // assign pin 12 to High Temp
#define RelayPin 31
#define Button 51

Adafruit_SSD1306 display(OLED_RESET);

// Setup a oneWire instance to communicate with any OneWire devices
// (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);


//Declare variables for runtime and operating temperature
int SetTempConst = 38,SetTemp, temptest, timeDisp;
unsigned long SetTimeConst=0, SetTime=1, runTime;
//  int SetTemp, SetTime;
// Variables will change:
int ButtonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 200;    // the debounce time; increase if the output






#endif // PROJECT_H_INCLUDED
