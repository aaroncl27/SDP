//DT01
//Heat Therapy Pseudocode

#include <project.h>

void setup() {
  delay(200);
  Serial.begin(115200);
  pinMode(Timer, INPUT);
  pinMode(RelayPin, OUTPUT);
  pinMode(BuzzPin, OUTPUT);
  pinMode(TempPin1, INPUT);
  pinMode(TempPin2, INPUT);
  pinMode(TimePin, INPUT);
  pinMode(Button, INPUT);
  digitalWrite(Button, LOW);
  digitalWrite(RelayPin, LOW);
  InitTimersSafe(); //initialize all timers except for 0, to save time keeping functions

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C
  display.display();
  delay(250);
  display.clearDisplay();

  sensors.begin();
  Init();
  //setFreq();
}

void (* resetFunc)(void)=0;
//Get Operating Temperature and Runtime Settings

void Init() {
  if (digitalRead(TimePin) == HIGH) {
   SetTime = 900000; //15 desired minutes in milliseconds
  }
  else {
    SetTime = 1800000; //30 desired minutes in milliseconds
    SetTimeConst = SetTime;
  }

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

void tempCheck() {
  float temp;
  temp =  Temp(); //Read temperature from bandage
  if (temp >= 40) {
    pulseOff();//If the temperature read by the device is over 40C, kill power to the device
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

void warningBeep() {
  analogWrite(BuzzPin, 1000); //Makes buzzer beep signifying that the device has finished running
  delay(500);
  analogWrite(BuzzPin , 0);
  delay(500);
}

bool startButton(){
  ButtonState = LOW;
    // read the state of the switch into a local variable:
  int reading = digitalRead(Button);

  // check to see if you just pressed the Button
  // (i.e. the input went from LOW to HIGH),  and you've waited
  // long enough since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    // whatever the reading is at, it's been there for longer
    // than the debounce delay, so take it as the actual current state:

    // if the Button state has changed:
    if (reading != ButtonState) {
      ButtonState = reading;
    }
  }
    lastButtonState = reading;
  return ButtonState;
}

void pulseOff() {
//tie an IO pin with a 1k resistor to the reset pin
//when pin is low it isnt reset
//when pin is high the arduino resets

  digitalWrite(RelayPin, LOW);//Brings pin tied to the latch on the relay to low
  delay(50);//waits for 20 ms
  digitalWrite(RelayPin, HIGH);//Brings pin tied to the relay to high, flipping the latch on the relay
  delay(3000);
    resetFunc();

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

void setFreq()
{
  int k = 0;
  bool estop;
  SetPinFrequency(ckt, 10); //setting the frequency to 10 Hz
  //Serial.println("\r\npwmWrite() and pwmWriteHR() are identical except for the valid range of inputs.\r\nThe following loop calls both functions to produce the same result on the \r\nLED pin. The pin should to run 10Hz at 50% duty regardless of the function called.\r\n");
  //(10Hz 50% duty) for 1 second before calling
  //the other function. This demonstrates the use of pwmWriteHR() and how its
  //use is nearly identical to pwmWrite()
  while (true)
  {
    //if(startButton() == HIGH){
    //  pulseOff();
    //}
    //setting the duty to 50% with 8 bit pwm. 128 is 1/2 of 256
    estop = startButton();
    pwmWrite(ckt, 128);
    //Serial.println("8-Bit PWM");

    //setting the duty to 50% with the highest possible resolution that
    //can be applied to the timer (up to 16 bit). 1/2 of 65536 is 32768.
    //pwmWriteHR(ckt, 32768);
    //Serial.println("High Resolution PWM");
    //delay(1000);
    if (millis() % 1000 == 0) {
      Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
      tempCheck();
      if((digitalRead(TimePin)==HIGH&&SetTime!=900000)||((digitalRead(TimePin)==LOW&&SetTime!=1800000))){
      delay(100);
      Init();}
      if((digitalRead(TempPin1)==HIGH&&SetTemp!=38)||((digitalRead(TempPin2)==HIGH&&SetTemp!=34))||(((digitalRead(TempPin1)==LOW&&digitalRead(TempPin2)==LOW)&&SetTemp!=36))){
      delay(250);
      Init();}
      runTime = millis();
      Serial.print("Runtime in seconds: ");
      Serial.println(runTime / 1000);
      Serial.print("Runtime in ms: ");
      Serial.println(millis()-748);
      Serial.println(startButton());
      Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
      timeDisp = (((SetTime / 1000) / 60) - ((runTime / 1000) / 60));//compare the time in min to the runtime in min.

      if (timeDisp <= 0||estop==HIGH){//when the timer on the device reaches zero
        display.clearDisplay();
        display.setTextSize(3);//Set Text Size to one for the top line. This allows 2 lines of text to fit. Otherwise text size should be 2
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Heating");
        display.print("Complete");
        display.display();
        for (int n = 0; n < 3; n++) {
          //warningBeep();
        }
        //while (1) {} //remove this when no longer testing
        delay(500);
        pulseOff();
       }
    }
  }
}
void loop () {  
      // only toggle the LED if the new Button state is HIGH
      if (startButton() == HIGH) {
        display.clearDisplay();
        display.setTextSize(2);//Set Text Size to one for the top line. This allows 2 lines of text to fit. Otherwise text size should be 2
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println("Initial");
        display.println("Setup");
        display.display();
        digitalWrite(RelayPin, LOW);//Brings pin tied to the latch on the relay to low
          delay(50);//waits for 20 ms
          digitalWrite(RelayPin, HIGH);//Brings pin tied to the relay to high, flipping the latch on the relay

          delay(4000);
        setFreq();
      }

  // save the reading.  Next time through the loop,
  // it'll be the lastButtonState:

}

