//DT01
//Heat Therapy Pseudocode

#include <project.h>
#define LedPin 41

void setup() {
  delay(200);
  Serial.begin(115200);
  pinMode(Timer, INPUT);
  pinMode(RelayPin, OUTPUT);
  pinMode(BuzzPin, OUTPUT);
  pinMode(LedPin, OUTPUT);
  pinMode(TempPin1, INPUT);
  pinMode(TempPin2, INPUT);
  pinMode(TimePin, INPUT);
  pinMode(Button, INPUT);
  pinMode(CoilRelayPin, OUTPUT);
  digitalWrite(Button, LOW);
  digitalWrite(RelayPin, LOW);
  digitalWrite(CoilRelayPin, LOW);
  InitTimersSafe(); //initialize all timers except for 0, to save time keeping functions
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // initialize with the I2C addr 0x3C
  display.display();
  delay(250);
  display.clearDisplay();
  sensors.begin();
  Init();

}

void (* resetFunc)(void) = 0;
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
  Serial.println(SetTime / 60 / 1000); //Display Desired Runtime
  Serial.print("Device Temperature Setpoint: ");
  Serial.println(SetTemp);//Display Desired Operating Temperature

}

void tempCheck() {
  float temp;
  temp =  Temp(); //Read temperature from bandage
  if (temp >= 40) {
    pulseOff();//If the temperature read by the device is over 40C, kill power to the device
  }

  if ((temp < SetTemp - 0.5) && (RelayState == FALSE)) {
    digitalWrite(CoilRelayPin, HIGH);
    delay(20);
    digitalWrite(CoilRelayPin, LOW);
    delay(1000);
    RelayState = TRUE;
  }

  if ((temp >= SetTemp + 0.5) && (RelayState == TRUE)) {
    digitalWrite(CoilRelayPin, HIGH);
    delay(20);
    digitalWrite(CoilRelayPin, LOW);
    delay(1000);
    RelayState = FALSE;
  }

}

float Temp() {
  float temp1, temp2, temp3, temp4, temp5, avgTemp, compAvgTemp;
  int devCount, devCountAdj;

  sensors.requestTemperatures(); // Send the command to get temperatures
  devCount = sensors.getDeviceCount();
  devCountAdj = devCount;
  float temp[devCount];
  compAvgTemp = sensors.getTempCByIndex(2);
  for (int j = 0; j < devCount; j++) {
    temp[j] = sensors.getTempCByIndex(j);
  }
  for (int j = 0; j < devCount; j++) {
    if ((temp[j] < compAvgTemp - 2)) {
      temp[j] = -127;
      Serial.println(temp[j]);
    }
  }
  for (int j = 0; j < devCount; j++) {
    if (temp[j] > 0) {
      avgTemp += temp[j];
    }
    else {
      devCountAdj--;
    }
  }
  avgTemp = avgTemp / devCountAdj;
  compAvgTemp = avgTemp;
  Serial.print("Desired Temp: ");
  Serial.println(SetTemp);
  Serial.print("Average Temp: ");
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
  display.print(tempReadout);
  display.println(" C");
  display.print(timeDisp);
  display.println(" Min Rem");
  display.display();

}

void warningBeep() {
  analogWrite(BuzzPin, 1000); //Makes buzzer beep signifying that the device has finished running
  delay(500);
  analogWrite(BuzzPin , 0);
  delay(500);
}

bool startButton() {
  ButtonState = LOW;
  // read the state of the switch into a local variable:
  int reading = digitalRead(Button);

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
  digitalWrite(LedPin, LOW);
  digitalWrite(RelayPin, LOW);//Brings pin tied to the latch on the relay to low
  delay(50);//waits for 20 ms
  digitalWrite(RelayPin, HIGH);//Brings pin tied to the relay to high, flipping the latch on the relay
  delay(5000);
  resetFunc();

}

void Main()
{
  int k = 0;
  bool estop;
  while (true)
  {
    estop = startButton();
    if (millis() % 1000 == 0) {
      Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
      tempCheck();
      if ((digitalRead(TimePin) == HIGH && SetTime != 900000) || ((digitalRead(TimePin) == LOW && SetTime != 1800000))) {
        delay(100);
        Init();
      }
      if ((digitalRead(TempPin1) == HIGH && SetTemp != 38) || ((digitalRead(TempPin2) == HIGH && SetTemp != 34)) || (((digitalRead(TempPin1) == LOW && digitalRead(TempPin2) == LOW) && SetTemp != 36))) {
        delay(250);
        Init();
      }
      runTime = millis() - StandbyTime;
      Serial.print("Runtime in seconds: ");
      Serial.println(runTime / 1000);
      //Serial.print("Runtime in ms: ");
      //Serial.println(millis() - 748);
      Serial.println(startButton());
      //Serial.println(sensors.getTempCByIndex(0));
      //Serial.println(sensors.getTempCByIndex(1));
      //Serial.println(sensors.getTempCByIndex(2));
      //Serial.println(sensors.getTempCByIndex(3));
      //Serial.println(sensors.getTempCByIndex(4));
      Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
      timeDisp = (((SetTime / 1000) / 60) - ((runTime / 1000) / 60));//compare the time in min to the runtime in min.

      if (timeDisp <= 0 || estop == HIGH) { //when the timer on the device reaches zero
        display.clearDisplay();
        display.setTextSize(2);//Set Text Size to one for the top line. This allows 2 lines of text to fit. Otherwise text size should be 2
        display.setTextColor(WHITE);
        display.setCursor(0, 0);
        display.println();
        display.println("    Done");
        display.print("  Heating");
        display.display();
        for (int n = 0; n < 3; n++) {
          //uwarningBeep();
        }
        //while (1) {} //remove this when no longer testing
        delay(500);
        pulseOff();
      }
    }
  }
}

void initDisplay() {
  display.clearDisplay();
  display.setTextSize(2);//Set Text Size to one for the top line
  display.setTextColor(WHITE);//Set the text color to white
  display.setCursor(0, 0);//Set cursor on first line of the screen to start printing
  display.println();//Leaves top line blank
  display.println("  Initial");
  display.println("   Setup");
  display.println("    .");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);//Set Text Size to one for the top line
  display.setTextColor(WHITE);//Set the text color to white
  display.setCursor(0, 0);//Set cursor on first line of the screen to start printing
  display.println();//Leaves top line blank
  display.println("  Initial");
  display.println("   Setup");
  display.println("    ..");
  display.display();
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);//Set Text Size to one for the top line
  display.setTextColor(WHITE);//Set the text color to white
  display.setCursor(0, 0);//Set cursor on first line of the screen to start printing
  display.println();//Leaves top line blank
  display.println("  Initial");
  display.println("   Setup");
  display.println("    ...");
  display.display();
}

void loop () {
  display.clearDisplay();
  display.setTextSize(3);//Set Text Size to one for the top line
  display.setTextColor(WHITE);//Set the text color to white
  display.setCursor(0, 0);//Set cursor on first line of the screen to start printing
  display.println();//Leaves top line blank
    display.display();
  StandbyTime = millis();//Keep track of how long the device waits for the user to press the button. This is used for calculating the runtime of the device.
  if (startButton() == HIGH) {//Loop until the user presses the Start button. The user must press the button to start the program. Otherwise it'll loop forever.
    digitalWrite(LedPin, HIGH);//Turn on the light in the button
    initDisplay();//Function to show a "loading screen" of sorts on the screen.

    digitalWrite(RelayPin, LOW);//Brings pin tied to the latch on the relay to low
    delay(20);//waits for 20 ms
    digitalWrite(RelayPin, HIGH);//Brings pin tied to the relay to high, flipping the latch on the relay

    //delay(1500);
    Main();//Goes to main function of device
  }
}
