//DT01
//Heat Therapy Pseudocode

#include <project.h>

//Declare variables for runtime and operating temperature
int SetTemp = 38, temptest, timeDisp, i = 0;
unsigned long SetTime=300000, runTime;
  int SetTemp, SetTime;
  
void setup() {
  Serial.begin(115200);
  pinMode(Timer, INPUT);
  //pinMode(SetTemp1, INPUT);
  //pinMode(SetTemp2, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);
  pinMode(Temp1, INPUT);
  pinMode(Temp2, INPUT);
  pinMode(Temp3, INPUT);
  pinMode(Temp4, INPUT);
  pinMode(Temp5, INPUT);
  pinMode(TempPin1, INPUT);
  pinMode(TempPin2, INPUT);
  pinMode(TimePin1, INPUT);
  InitTimersSafe(); //initialize all timers except for 0, to save time keeping functions

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3D (for the 128x64)
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(250);

  // Clear the buffer.
  display.clearDisplay();

  sensors.begin();
  IsOn();
  //demonstrateFrequencysEffectOnResolution();
  settingHighResolutionDuty();
}

//Get Operating Temperature and Runtime Settings
void IsOn() {
   if (digitalRead(TimePin1) == HIGH){
  //Serial.println("15");
    SetTime= 15;
  }else {
    //Serial.println("30");
    SetTime= 30;}
    delay(1000);

  if (digitalRead(TempPin1) == HIGH){
 //Serial.println("H");
    SetTemp= 38;
  }else if (digitalRead(TempPin2) == HIGH){
    //Serial.println("L");
    SetTemp= 34;
  } else{
    //Serial.println("M");
    SetTemp= 36;
  }
  Serial.println(SetTime);
  Serial.println(SetTemp);

}

void IsOk() {
  float temp;
  temp =  Temp(); //Read temperature from bandage

  if (temp >= 40) {
    PulseOff();
  }
}

float Temp() {
  float temp1, temp2, temp3, temp4, temp5, avgTemp;
  // call sensors.requestTemperatures() to issue a global temperature
  // request to all devices on the bus
  //Serial.print(" Requesting temperatures...");
  sensors.requestTemperatures(); // Send the command to get temperatures
  //Serial.println("DONE");
  temp1 = sensors.getTempCByIndex(0);
  temp2 = sensors.getTempCByIndex(1);
  temp3 = sensors.getTempCByIndex(2);
  temp4 = sensors.getTempCByIndex(3);
  temp5 = sensors.getTempCByIndex(4);
  /*Serial.print("Temperature 1 is: ");
  Serial.println(temp1); // Why "byIndex"?
  Serial.print("Temperature 2 is: ");
  Serial.println(temp2); // Why "byIndex"?
  Serial.print("Temperature 3 is: ");
  Serial.println(temp3); // Why "byIndex"?
  Serial.print("Temperature 4 is: ");
  Serial.println(temp4); // Why "byIndex"?
  Serial.print("Temperature 5 is: ");
  Serial.println(temp5); // Why "byIndex"?*/
  avgTemp = temp1 + temp2 + temp3 + temp4 + temp5;
  avgTemp = avgTemp / 5;
  Serial.print("Temperature Average is: ");
  Serial.println(avgTemp); // Why "byIndex"?
  Serial.print("Device Count is: ");
  Serial.println(sensors.getDeviceCount());
  Display(avgTemp);
  return avgTemp;
}

void Display(float tempReadout) {
  display.clearDisplay();
  display.setTextSize(1);//Set Text Size to one for the top line. This allows 2 lines of text to fit. Otherwise text size should be 2
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Running Average");
  display.println(" C");
  display.setTextSize(2);
  display.println("Temp ");
  //Serial.println(Temp());
  Serial.print("Thermistor Avg: ");
  Serial.println(tempReadout);
  display.print(tempReadout);
  display.println(" C");
  display.print(timeDisp);
  display.println(" Min Rem");
  display.display();
  //delay(250);

}

void SetFreq() {
  //Determine desired MOSFET switching frequency based on desired temperature
  //Use PWM to set MOSFET switching frequency
}

void WarningBeep() {
  analogWrite(buzzPin, 1000); //Set off warning buzzer if temperature too high
  delay(500); //Delaying
  analogWrite(buzzPin , 0); //Used to make buzzer pulse
  delay(500); //Delaying
}


void PulseOff() {
  digitalWrite(relayPin, LOW);
  delay(20);
  digitalWrite(relayPin, HIGH);
  delay(1000);
  while(1){};
}

void demonstrateFrequencysEffectOnResolution()
{
  Serial.println("As frequency increases, resolution will decrease...");
  for (int i = 1; i < 10000; i += 10)
  {
    SetPinFrequency(ckt, i);  //setting the frequency

    uint16_t frequency = Timer1_GetFrequency();
    uint16_t decimalResolution = Timer1_GetTop() + 1;
    uint16_t binaryResolution = GetPinResolution(ckt); //this number will be inaccurately low because the float is being truncated to a int

    char strOut[75];
    sprintf(strOut, "Frequency: %u Hz\r\n Number of Possible Duties: %u\r\n Resolution: %u bit\r\n", frequency, decimalResolution, binaryResolution );

    Serial.println(strOut);
  }

  Serial.println("...Finished");
}

void settingHighResolutionDuty()
{
  int k = 0;
  SetPinFrequency(ckt, 10); //setting the frequency to 10 Hz
  //Serial.println("\r\npwmWrite() and pwmWriteHR() are identical except for the valid range of inputs.\r\nThe following loop calls both functions to produce the same result on the \r\nLED pin. The pin should to run 10Hz at 50% duty regardless of the function called.\r\n");

  //(10Hz 50% duty) for 1 second before calling
  //the other function. This demonstrates the use of pwmWriteHR() and how its
  //use is nearly identical to pwmWrite()
  while (true)
  {
    //setting the duty to 50% with 8 bit pwm. 128 is 1/2 of 256
    pwmWrite(ckt, 128);
    //Serial.println("8-Bit PWM");
    //delay(1000);
    i++;
    //Serial.println(i);
    //setting the duty to 50% with the highest possible resolution that
    //can be applied to the timer (up to 16 bit). 1/2 of 65536 is 32768.
    //pwmWriteHR(ckt, 32768);
    //Serial.println("High Resolution PWM");
    //delay(1000);
    if (millis()%1000 == 0) {
      IsOk();
      runTime = millis()-853;
      //Serial.println(runTime%1000);
      Serial.println(runTime/1000);
      i = 0;
      //k++;
      //Serial.println(k);
      Serial.println(millis()-853);
      timeDisp = (((SetTime/1000)/60) - ((runTime/1000) / 60));
      if (timeDisp == 0) {
        display.clearDisplay();
        display.setTextSize(3);//Set Text Size to one for the top line. This allows 2 lines of text to fit. Otherwise text size should be 2
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("AvgTemp");
        display.print(" C");
        display.display();
        for (int n = 0; n < 3; n++) {
          WarningBeep();
        }
        while (1) {} //remove this when no longer testing
        PulseOff();
      }
    }
  }

}

void loop () {
  /*

    Serial.println(i);
    timeDisp = (SetTime - ((i) / 60));
    if (timeDisp == 0) {
      cumAvg = cumAvg / i;
      display.clearDisplay();
      display.setTextSize(3);//Set Text Size to one for the top line. This allows 2 lines of text to fit. Otherwise text size should be 2
      display.setTextColor(WHITE);
      display.setCursor(0, 0);
      display.println("AvgTemp");
      display.print(cumAvg);
      display.print(" C");
      display.display();
      for (int n = 0; n < 3; n++) {
        WarningBeep();
      }
      while (1) {} //remove this when no longer testing
      PulseOff();
    }
    IsOk();
    //Set resonating Frequency for MOSFET switching
    SetFreq();


    delay(630);*/
}

