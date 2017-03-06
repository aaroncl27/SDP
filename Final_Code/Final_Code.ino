//DT01
//Heat Therapy Pseudocode

#include <project.h>

//Declare variables for runtime and operating temperature
int SetTempConst = 38,SetTemp, temptest, timeDisp;
unsigned long SetTimeConst=0, SetTime=1, runTime;
//  int SetTemp, SetTime;

void setup() {
  Serial.begin(115200);
  pinMode(Timer, INPUT);
  pinMode(relayPin, OUTPUT);
  pinMode(buzzPin, OUTPUT);
  pinMode(TempPin1, INPUT);
  pinMode(TempPin2, INPUT);
  pinMode(TimePin, INPUT);
  InitTimersSafe(); //initialize all timers except for 0, to save time keeping functions

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C
  display.display();
  delay(250);
  display.clearDisplay();

  sensors.begin();
  IsOn();
  //PulseOff();
  //demonstrateFrequencysEffectOnResolution();
  settingHighResolutionDuty();
}

//Get Operating Temperature and Runtime Settings
void IsOn() {
  //int timeSet=digitalRead(TimePin1);
  if (digitalRead(TimePin) == HIGH) {
   SetTime = 900000; //15 desired minutes in milliseconds
  }
  else {
    SetTime = 1800000; //30 desired minutes in milliseconds
    SetTimeConst = SetTime;
  }
  //delay(1000);

  if (digitalRead(TempPin1) == HIGH) {
    SetTemp = 38; //Sets desired temp to 38C
  }
  else if (digitalRead(TempPin2) == HIGH) {
    SetTemp = 34; //Sets desired temp to 34C
  }
  else {
    SetTemp = 36; //Sets desired temp to 36C
  }
  Serial.print("Device Runtime: ");
  Serial.println(SetTime/60/1000);//Display Desired Runtime
  Serial.print("Device Temperature Setpoint: ");
  Serial.println(SetTemp);//Display Desired Operating Temperature

}

void IsOk() {
  float temp;
  temp =  Temp(); //Read temperature from bandage

  if (temp >= 40) {
    PulseOff();//If the temperature read by the device is over 40C, kill power to the device
  }
}

float Temp() {
  float temp1, temp2, temp3, temp4, temp5, avgTemp;
  int devCount, devCountAdj;
  
  
  sensors.requestTemperatures(); // Send the command to get temperatures
  devCount=sensors.getDeviceCount();
  devCountAdj=devCount;
  float temp[devCount];
  
  for(int j=0;j<devCount;j++){
  temp[j]=sensors.getTempCByIndex(j);
  }
  //temp1 = sensors.getTempCByIndex(0);//Using sensor library read the temperature from the first sensor on the bus
  //temp2 = sensors.getTempCByIndex(1);//Using sensor library read the temperature from the second sensor on the bus
  //temp3 = sensors.getTempCByIndex(2);//Using sensor library read the temperature from the third sensor on the bus
  //temp4 = sensors.getTempCByIndex(3);//Using sensor library read the temperature from the fourth sensor on the bus
  //temp5 = sensors.getTempCByIndex(4);//Using sensor library read the temperature from the fifth sensor on the bus
  /*Serial.print("Temperature 1 is: ");
    Serial.println(temp1);
    Serial.print("Temperature 2 is: ");
    Serial.println(temp2);
    Serial.print("Temperature 3 is: ");
    Serial.println(temp3);
    Serial.print("Temperature 4 is: ");
    Serial.println(temp4);
    Serial.print("Temperature 5 is: ");
    Serial.println(temp5);*/
  for (int j = 0;j<devCount;j++){
    if(temp[j]>0){
    avgTemp += temp[j];}
    else{
      devCountAdj--;
    }
  }
  avgTemp = avgTemp / devCountAdj;
  
  Serial.print("Temperature Average is: ");
  Serial.println(avgTemp);
  
  //Serial.print("Device Count is: ");
  //Serial.println(sensors.getDeviceCount());
  Display(avgTemp);
  return avgTemp;
}

void Display(float tempReadout) {
  display.clearDisplay();
  display.setTextSize(1);//Set Text Size to one for the top line. This allows 2 lines of text to fit. Otherwise text size should be 2
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Operating Temp");
  display.println(SetTemp);
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

void WarningBeep() {
  analogWrite(buzzPin, 1000); //Makes buzzer beep signifying that the device has finished running
  delay(500);
  analogWrite(buzzPin , 0);
  delay(500);
}


void PulseOff() {
while(1){
  digitalWrite(relayPin, LOW);//Brings pin tied to the latch on the relay to low
  delay(50);//waits for 20 ms
  digitalWrite(relayPin, HIGH);//Brings pin tied to the relay to high, flipping the latch on the relay
  delay(1000);
  };
}

/*void demonstrateFrequencysEffectOnResolution()
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
  }*/

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

    //setting the duty to 50% with the highest possible resolution that
    //can be applied to the timer (up to 16 bit). 1/2 of 65536 is 32768.
    //pwmWriteHR(ckt, 32768);
    //Serial.println("High Resolution PWM");
    //delay(1000);
    if (millis() % 1000 == 0) {
      Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
      IsOk();
      if((digitalRead(TimePin)==HIGH&&SetTime!=900000)||((digitalRead(TimePin)==LOW&&SetTime!=1800000))){
      delay(100);
      IsOn();}
      if((digitalRead(TempPin1)==HIGH&&SetTemp!=38)||((digitalRead(TempPin2)==HIGH&&SetTemp!=34))||(((digitalRead(TempPin1)==LOW&&digitalRead(TempPin2)==LOW)&&SetTemp!=36))){
      delay(250);
      IsOn();}
      runTime = millis();
      Serial.print("Runtime in seconds: ");
      Serial.println(runTime / 1000);
      Serial.print("Runtime in ms: ");
      Serial.println(millis()-748);
      Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
      timeDisp = (((SetTime / 1000) / 60) - ((runTime / 1000) / 60));//compare the time in min to the runtime in min.

      
      if (timeDisp <= 0){//when the timer on the device reaches zero
        display.clearDisplay();
        display.setTextSize(3);//Set Text Size to one for the top line. This allows 2 lines of text to fit. Otherwise text size should be 2
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Heating");
        display.print("Complete");
        display.display();
        for (int n = 0; n < 3; n++) {
          //WarningBeep();
        }
        //while (1) {} //remove this when no longer testing
        PulseOff();
       }
    }
  }
}
void loop () {

}

