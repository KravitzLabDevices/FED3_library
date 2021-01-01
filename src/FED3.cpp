/*
  Feeding experimentation device 3 (FED3) library
  Code by Lex Kravitz, adapted to Arduino library format by Eric Lin
  alexxai@wustl.edu
  erclin@ucdavis.edu
  December 2020 

  FED was originally developed by Nguyen at al and published in 2016:
  https://www.ncbi.nlm.nih.gov/pubmed/27060385

  This device includes hardware and code from:
  *** Adafruit, who made the hardware breakout boards and associated code we used in FED ***

  Cavemoa's excellent examples of datalogging with the Adalogger:
  https://github.com/cavemoa/Feather-M0-Adalogger

  Arduino Time library http://playground.arduino.cc/code/time
  Maintained by Paul Stoffregen https://github.com/PaulStoffregen/Time

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2019, 2020 Lex Kravitz
*/


/****************************************************************************************************************
                                                                                                    Startup stuff
****************************************************************************************************************/
#include "Arduino.h"
#include "FED3.h"

//  Start FED3 and RTC objects
FED3 *pointerToFED3;
RTC_PCF8523 rtc; 

//  Interrupt handlers
static void outsidePelletTriggerHandler(void) {
  pointerToFED3->pelletTrigger();
}

static void outsideLeftTriggerHandler(void) {
  pointerToFED3->leftTrigger();
}

static void outsideRightTriggerHandler(void) {
  pointerToFED3->rightTrigger();
}

/**************************************************************************************************************************************************
                                                                                                        Main loop
**************************************************************************************************************************************************/
void FED3::run() {
  //This should be called at least once per loop.  It updates the time, updates display, and goes to sleep if sleep is enabled
  if (digitalRead(PELLET_WELL) == HIGH) {  //check for pellet
    PelletAvailable = false;
  }
  DateTime now = rtc.now();
  unixtime  = now.unixtime();
  //If in classic mode, run classic display update, otherwise run regular display update
  if (ClassicFED3) {
    ClassicUpdateDisplay();
  }
  else {
    UpdateDisplay();
  }
  goToSleep();
}

/**************************************************************************************************************************************************
                                                                                                Poke functions
**************************************************************************************************************************************************/
//log left poke
void FED3::logLeftPoke(){
    leftInterval = 0.0;
    leftPokeTime = millis();
    display.fillCircle(25, 59, 5, BLACK);
    display.refresh();
    while (digitalRead (LEFT_POKE) == LOW) {  //After pellet is detected, hang here to detect when it is removed
        leftInterval = (millis()-leftPokeTime);
    }
    LeftCount ++;
    if (ClassicFED3) {
      ClassicUpdateDisplay();
    }
    else {
      UpdateDisplay();
    }
    DisplayLeftInt();
    Event  = "Left";
    logdata();
    Left = false;
}

//log right poke
void FED3::logRightPoke(){
    rightInterval = 0.0;
    rightPokeTime = millis();
    display.fillCircle(25, 79, 5, BLACK);
    display.refresh();
    while (digitalRead (RIGHT_POKE) == LOW) {  //After pellet is detected, hang here to detect when it is removed
        rightInterval = (millis()-rightPokeTime);
    }
    RightCount ++;
    if (ClassicFED3) {
      ClassicUpdateDisplay();
    }
    else {
      UpdateDisplay();
    }
    DisplayRightInt();
    Event  = "Right";
    logdata();
    Right = false;
}

/**************************************************************************************************************************************************
                                                                                                Feeding functions
**************************************************************************************************************************************************/
void FED3::Feed() {
   while (PelletAvailable == false) {
      digitalWrite (MOTOR_ENABLE, HIGH);  //Enable motor driver
      for (int i = 1; i < 10; i++) {
        if (digitalRead (PELLET_WELL) == HIGH) {
          stepper.step(-30);
        }
      }
      
      //If pellet is detected
      if (digitalRead (PELLET_WELL) == LOW) {
        pelletTime = millis();
        display.fillCircle(25, 99, 5, BLACK);
        display.refresh();
        while (digitalRead (PELLET_WELL) == LOW and retInterval < 60000) {  //After pellet is detected, hang here for up to 1 minute to detect when it is removed
            retInterval = (millis()-pelletTime);
            DisplayRetrievalInt();
        }
        while (digitalRead (PELLET_WELL) == LOW){ //if pellet is not taken after 60 seconds, wait here and go to sleep
            run();
        }
        BNC(500, 1);
        PelletCount++;
        numMotorTurns = 0; //reset numMotorTurns
        Event = "Pellet";
        logdata();
        retInterval = 0;
        ratio = ratio + round ((5 * exp (0.2 * PelletCount)) - 5); // this is a formula from Richardson and Roberts (1996) https://www.ncbi.nlm.nih.gov/pubmed/8794935
        PelletAvailable = true;
        if (ClassicFED3) {
          ClassicUpdateDisplay();
        }
        else {
          UpdateDisplay();
        }
        if (timeout > 0) Timeout(timeout); //timeout after each pellet is dropped (you can edit this number)
      }

      pixelsOff();      //turn pixels off 
      digitalWrite (MOTOR_ENABLE, LOW);  //Disable motor driver
      dispenseTimer();  //delay between pellets that also checks pellet well
      numMotorTurns++;

      //Jam clearing movements
      if (numMotorTurns % 5 == 0){
        MinorJam();
      }
      if (numMotorTurns % 10 == 0 and numMotorTurns % 20 != 0) {
        VibrateJam();
      }
      if (numMotorTurns % 20 == 0) {
        ClearJam();
      }
   }
}

//minor movement to clear jam
void FED3::MinorJam(){
    digitalWrite (MOTOR_ENABLE, HIGH);  //Enable motor driver
    stepper.step(100); //minor adjustment to try to dislodget pellet
    ReleaseMotor ();
}

//vibration movement to clear jam
void FED3::VibrateJam() {
    DisplayJamClear();
    if (digitalRead (PELLET_WELL) == HIGH) {
      delay (250); //simple debounce to ensure pellet is out for at least 250ms
      if (digitalRead (PELLET_WELL) == HIGH) {
        digitalWrite (MOTOR_ENABLE, HIGH);  //Enable motor driver
        for (int j = 0; j < 30; j++) {
          if (digitalRead (PELLET_WELL) == HIGH) {
            stepper.step(120);
            if (digitalRead (PELLET_WELL) == HIGH) {
              stepper.step(-60);
            }
          }
        }
        ReleaseMotor ();
      }
    }
  display.fillRect (5, 15, 120, 15, WHITE);  //erase the "Jam clear" text without clearing the entire screen by pasting a white box over it
}

//full rotation to clear jam
void FED3::ClearJam() {
    DisplayJamClear();
    if (digitalRead (PELLET_WELL) == HIGH) {
      delay (250); //simple debounce to ensure pellet is out for at least 250ms
      if (digitalRead (PELLET_WELL) == HIGH) {
        digitalWrite (MOTOR_ENABLE, HIGH);  //Enable motor driver
        for (int i = 0; i < 21 + random(0, 20); i++) {
          if (digitalRead (PELLET_WELL) == HIGH) {
            stepper.step(-i * 4);
          }
        }
      }
      ReleaseMotor ();
    }
    if (digitalRead (PELLET_WELL) == HIGH) {
      delay (250); //simple debounce
      if (digitalRead (PELLET_WELL) == HIGH) {
        digitalWrite (MOTOR_ENABLE, HIGH);  //Enable motor driver
        for (int i = 0; i < 21 + random(0, 20); i++) {
          if (digitalRead (PELLET_WELL) == HIGH) {
            stepper.step(i * 4);
          }
        }
        ReleaseMotor ();
      }
    }
    display.fillRect (5, 15, 120, 15, WHITE);  //erase the "Jam clear" text without clearing the entire screen by pasting a white box over it
    numMotorTurns = 0;
}

//Function for delaying between motor movements, but also ending this delay if a pellet is detected
void FED3::dispenseTimer() {
  for (int i = 1; i < 300; i++) {
    if (digitalRead (PELLET_WELL) == HIGH && PelletAvailable == false) {
      delay(5);
    }
  }
}

//Timeout function
void FED3::Timeout(int seconds) {
    for (int k = 0; k <= seconds; k++) {
      delay (1000);
      display.fillRect (5, 20, 200, 25,WHITE);  //erase the data on screen without clearing the entire screen by pasting a white box over it
      display.setCursor(6, 36);
      display.print("Timeout: ");
      display.print(seconds - k);
      display.refresh();
    }
    display.fillRect (5, 20, 100, 25, WHITE);  //erase the data on screen without clearing the entire screen by pasting a white box over it
    UpdateDisplay();
}

/**************************************************************************************************************************************************
                                                                                       Audio and neopixel stimuli
**************************************************************************************************************************************************/
void FED3::ConditionedStimulus() {
  pixelsOn((0, 2, 2)); 
  tone (BUZZER, 4000, 200);
}

void FED3::Click() {
  tone (BUZZER, 800, 8);
}

void FED3::Noise() {
  // Random noise to signal errors
  for (int i = 0; i < 50; i++) {
    tone (BUZZER, random(10, 250), 10);
    delay(10);
  }
}

//Turn all pixels on to a specific color
void FED3::pixelsOn(uint32_t c) {
  digitalWrite (MOTOR_ENABLE, HIGH);  //ENABLE motor driver
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
  }
  digitalWrite (MOTOR_ENABLE, LOW);  //DISABLE motor driver
}

//Turn all pixels off
void FED3::pixelsOff() {
  digitalWrite (MOTOR_ENABLE, HIGH);  //ENABLE motor driver
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, (0,0,0));
    strip.show();
  }
  digitalWrite (MOTOR_ENABLE, LOW);  //DISABLE motor driver
}

//colorWipe does a color wipe from left to right
void FED3::colorWipe(uint32_t c, uint8_t wait) {
  digitalWrite (MOTOR_ENABLE, HIGH);  //ENABLE motor driver
  for (uint16_t i = 0; i < strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
    strip.show();
    delay(wait);
  }
  digitalWrite (MOTOR_ENABLE, LOW);  //DISABLE motor driver
}

//Short helper function for blinking LEDs and BNC out port
void FED3::Blink(byte PIN, byte DELAY_MS, byte loops) {
  for (byte i = 0; i < loops; i++)  {
    digitalWrite(PIN, HIGH);
    delay(DELAY_MS);
    digitalWrite(PIN, LOW);
    delay(DELAY_MS);
  }
}

//Short helper function for controlling the BNC port
void FED3::BNC(byte DELAY_MS, byte loops) {
  for (byte i = 0; i < loops; i++)  {
    digitalWrite(BNC_OUT, HIGH);
    digitalWrite(GREEN_LED, HIGH);
    delay(DELAY_MS);
    digitalWrite(BNC_OUT, LOW);
    digitalWrite(GREEN_LED, LOW);
    delay(DELAY_MS);
  }
}

/**************************************************************************************************************************************************
                                                                                               Display functions
**************************************************************************************************************************************************/
void FED3::UpdateDisplay() {
  display.setCursor(5, 15);
  display.print("FED:");
  display.println(FED);
  display.setCursor(6, 15);  // this doubling is a way to do bold type
  display.print("FED:");
  display.fillRect (6, 20, 200, 22, WHITE);  //erase the data on screen without clearing the entire screen by pasting a white box over it
  display.fillRect (35, 46, 130, 80, WHITE);  //erase the pellet data on screen without clearing the entire screen by pasting a white box over it
  display.setCursor(5, 36); //display which sketch is running
  display.print(sessiontype);

  if (DisplayPokes == 1) {
    display.setCursor(35, 65);
    display.print("Left: ");
    display.setCursor(95, 65);
    display.print(LeftCount);
    display.setCursor(35, 85);
    display.print("Right:  ");
    display.setCursor(95, 85);
    display.print(RightCount);
  }

  display.setCursor(35, 105);
  display.print("Pellets:");
  display.setCursor(95, 105);
  display.print(PelletCount);

  //Box around data area of screen
  display.drawRect (5, 45, 158, 70, BLACK);
  DisplayBattery();
  DisplayDateTime();
  DisplayIndicators();
  display.refresh();
}

void FED3::DisplayDateTime(){
  // Print date and time at bottom of the screen
  DateTime now = rtc.now();
  display.setCursor(10, 135);
  display.fillRect (0, 123, 200, 60, WHITE);
  display.print(now.month());
  display.print("/");
  display.print(now.day());
  display.print("/");
  display.print(now.year());
  display.print("    ");
  if (now.hour() < 10)
    display.print('0');      // Trick to add leading zero for formatting
  display.print(now.hour());
  display.print(":");
  if (now.minute() < 10)
    display.print('0');      // Trick to add leading zero for formatting
  display.print(now.minute());
}

void FED3::DisplayIndicators(){
  // Pellet circle
  display.fillCircle(25, 99, 5, WHITE); //pellet
  display.drawCircle(25, 99, 5, BLACK);

  //Poke indicators
  if (DisplayPokes == 1) { //only make circles for pokes if DisplayPokes==1
    //draw circles for  pokes
    display.fillCircle(25, 59, 5, WHITE); //left poke
    display.fillCircle(25, 79, 5, WHITE); //right poke
    display.drawCircle(25, 59, 5, BLACK);
    display.drawCircle(25, 79, 5, BLACK);
 
    //Active poke indicator triangles
    if (activePoke == 0) {
      display.fillTriangle (12, 55, 18, 59, 12, 63, WHITE);
      display.fillTriangle (12, 75, 18, 79, 12, 83, BLACK);
    }
    if (activePoke == 1) {
      display.fillTriangle (12, 75, 18, 79, 12, 83, WHITE);
      display.fillTriangle (12, 55, 18, 59, 12, 63, BLACK);
    }
  }
}

void FED3::DisplayBattery(){
  //  Battery graphic showing bars indicating voltage levels
  if (numMotorTurns == 0) {
    display.fillRect (117, 2, 40, 16, WHITE);
    display.drawRect (116, 1, 42, 18, BLACK);
    display.drawRect (157, 6, 6, 8, BLACK);
  }
  //4 bars
  if (measuredvbat > 3.85 & numMotorTurns == 0) {
    display.fillRect (120, 4, 7, 12, BLACK);
    display.fillRect (129, 4, 7, 12, BLACK);
    display.fillRect (138, 4, 7, 12, BLACK);
    display.fillRect (147, 4, 7, 12, BLACK);
  }

  //3 bars
  else if (measuredvbat > 3.7 & numMotorTurns == 0) {
    display.fillRect (119, 3, 26, 13, WHITE);
    display.fillRect (120, 4, 7, 12, BLACK);
    display.fillRect (129, 4, 7, 12, BLACK);
    display.fillRect (138, 4, 7, 12, BLACK);
  }

  //2 bars
  else if (measuredvbat > 3.55 & numMotorTurns == 0) {
    display.fillRect (119, 3, 26, 13, WHITE);
    display.fillRect (120, 4, 7, 12, BLACK);
    display.fillRect (129, 4, 7, 12, BLACK);
  }

  //1 bar
  else if (numMotorTurns == 0) {
    display.fillRect (119, 3, 26, 13, WHITE);
    display.fillRect (120, 4, 7, 12, BLACK);
  }
}

//Display "Check SD Card!" if there is a card error
void FED3::DisplaySDError() {
  display.clearDisplay();
  display.setCursor(20, 40);
  display.println("   Check");
  display.setCursor(10, 60);
  display.println("  SD Card!");
  display.refresh();
}

//Display text when FED is clearing a jam
void FED3::DisplayJamClear() {
  display.fillRect (6, 20, 200, 22, WHITE);  //erase the data on screen without clearing the entire screen by pasting a white box over it
  display.setCursor(6, 36);
  display.print("Clearing jam");
  display.refresh();
}

//Display pellet retrieval interval
void FED3::DisplayRetrievalInt() {
    display.fillRect (91, 22, 70, 15, WHITE); 
    display.setCursor(92, 36);
    display.print (retInterval);
    display.print ("ms");
    display.refresh();
}

//Display left poke duration
void FED3::DisplayLeftInt() {
    display.fillRect (91, 22, 70, 15, WHITE);  
    display.setCursor(92, 36);
    display.print (leftInterval);
    display.print ("ms");
    display.refresh();
}

//Display right poke duration
void FED3::DisplayRightInt() {
    display.fillRect (91, 22, 70, 15, WHITE);  
    display.setCursor(92, 36);
    display.print (rightInterval);
    display.print ("ms");
    display.refresh();
}

/**************************************************************************************************************************************************
                                                                                               SD Logging functions
**************************************************************************************************************************************************/
// Create new files on uSD for FED3 settings
void FED3::CreateFile() {
  // see if the card is present and can be initialized:
  if (!SD.begin(cardSelect, SD_SCK_MHZ(4))) {
    error(2);
  }

  // create files if they dont exist and grab device name and ratio
  configfile = SD.open("DeviceNumber.csv", FILE_WRITE);
  configfile = SD.open("DeviceNumber.csv", FILE_READ);
  FED = configfile.parseInt();
  configfile.close();

  ratiofile = SD.open("FEDmode.csv", FILE_WRITE);
  ratiofile = SD.open("FEDmode.csv", FILE_READ);
  FEDmode = ratiofile.parseInt();
  ratiofile.close();

  startfile = SD.open("start.csv", FILE_WRITE);
  startfile = SD.open("start.csv", FILE_READ);
  timedStart = startfile.parseInt();
  startfile.close();

  stopfile = SD.open("stop.csv", FILE_WRITE);
  stopfile = SD.open("stop.csv", FILE_READ);
  timedEnd = stopfile.parseInt();
  stopfile.close();

  // Name filename in format F###_MMDDYYNN, where MM is month, DD is day, YY is year, and NN is an incrementing number for the number of files initialized each day
  strcpy(filename, "FED_____________.CSV");  // placeholder filename
  getFilename(filename);
}

//Create a new datafile
void FED3::CreateDataFile () {
  getFilename(filename);
  logfile = SD.open(filename, FILE_WRITE);
  if ( ! logfile ) {
    error(3);
  }
}

//Write the header to the datafile
void FED3::writeHeader() {
  // Write data header to file of uSD.
  logfile.println("MM:DD:YYYY hh:mm:ss,LibaryVersion_Sketch,Device_Number,Battery_Voltage,Motor_Turns,Trial_Info,FR,Event,Active_Poke,Left_Poke_Count,Right_Poke_Count,Pellet_Count,Block_Pellet_Count,Retrieval_Time,Poke_Time");
}

//write a configfile (this contains the FED device number)
void FED3::writeConfigFile() {
  configfile = SD.open("DeviceNumber.csv", FILE_WRITE);
  configfile.rewind();
  configfile.println(FED);
  configfile.flush();
  configfile.close();
}

//Write to SD card
void FED3::logdata() {
  /////////////////////////////////
  // Log data and time 
  /////////////////////////////////
  DateTime now = rtc.now();
  logfile.print(now.month());
  logfile.print("/");
  logfile.print(now.day());
  logfile.print("/");
  logfile.print(now.year());
  logfile.print(" ");
  logfile.print(now.hour());
  logfile.print(":");
  if (now.minute() < 10)
    logfile.print('0');      // Trick to add leading zero for formatting
  logfile.print(now.minute());
  logfile.print(":");
  if (now.second() < 10)
    logfile.print('0');      // Trick to add leading zero for formatting
  logfile.print(now.second());
  logfile.print(",");

  /////////////////////////////////
  // Log library version and Sketch identifier text
  /////////////////////////////////
  logfile.print(VER); // Print library version
  logfile.print("_");
  logfile.print(sessiontype);  //print Sketch identifier
  logfile.print(",");

  /////////////////////////////////
  // Log FED device number
  /////////////////////////////////
  logfile.print(FED); // 
  logfile.print(",");

  /////////////////////////////////
  // Log battery voltage
  /////////////////////////////////
  logfile.print(measuredvbat); // 
  logfile.print(",");

  /////////////////////////////////
  // Log motor turns
  /////////////////////////////////
  logfile.print(numMotorTurns); // Print the number of attempts to dispense a pellet
  logfile.print(",");

  /////////////////////////////////
  // Log Trial Info
  /////////////////////////////////
  if (DisplayPokes == 0) {
    logfile.print("Free"); // Print trial type
    logfile.print(",");
  }

  if (DisplayPokes == 1) {
    logfile.print("FR"); // Print FR 
    logfile.print(FR); // Print FR ratio
    logfile.print(",");
  }

  /////////////////////////////////
  // Log FR ratio
  /////////////////////////////////
  logfile.print(FR);
  logfile.print(",");

  /////////////////////////////////
  // Log event type (pellet, right, left)
  /////////////////////////////////
  logfile.print(Event); 
  logfile.print(",");

  /////////////////////////////////
  // Log Active poke side (left, right)
  /////////////////////////////////
  if (activePoke == 0)  logfile.print("Right"); //
  if (activePoke == 1)  logfile.print("Left"); //
  logfile.print(",");

  /////////////////////////////////
  // Log data (leftCount, RightCount, Pellets
  /////////////////////////////////
  logfile.print(LeftCount); // Print Left poke count
  logfile.print(",");
    
  logfile.print(RightCount); // Print Right poke count
  logfile.print(",");

  logfile.print(PelletCount); // print Pellet counts
  logfile.print(",");

  logfile.print(BlockPelletCount); // print Block Pellet counts
  logfile.print(",");

  /////////////////////////////////
  // Log pellet retrieval interval
  /////////////////////////////////
  if (Event != "Pellet"){
    logfile.print(sqrt (-1)); // print NaN if it's not a pellet Event
  }

  else if (retInterval < 60000 ) {  // only log retrieval intervals below 1 minute
    logfile.print(retInterval/1000.000); // print interval between pellet dispensing and being taken
  }

  else if (retInterval >= 60000) {
    logfile.print("Timed_out"); // print "Timed_out" if retreival interval is >60s
  }

  else {
    logfile.print("Error"); // print error if value is < 0 (this shouldn't ever happen)
  }
  logfile.print(",");
  
  /////////////////////////////////
  // Log poke duration
  /////////////////////////////////
  if (Event == "Pellet"){
    logfile.println(sqrt (-1)); // print NaN 
  }

  else if (Left) {  // 
    logfile.println(leftInterval/1000.000); // print left poke timing
  }

  else if (Right) {
    logfile.println(rightInterval/1000.000); // print left poke timing
  }

  /////////////////////////////////
  // logfile.flush write to the SD card
  /////////////////////////////////
  Blink(GREEN_LED, 100, 2);
  logfile.flush();
}

// If any errors are detected with the SD card upon boot this function
// will blink both LEDs on the Feather M0, turn the NeoPixel into red wipe pattern,
// and display "Check SD Card" on the screen
void FED3::error(uint8_t errno) {
  DisplaySDError();
  while (1) {
    uint8_t i;
    for (i = 0; i < errno; i++) {
      Blink(GREEN_LED, 25, 2);
      colorWipe(strip.Color(5, 0, 0), 25); // RED
    }
    for (i = errno; i < 10; i++) {
      colorWipe(strip.Color(0, 0, 0), 25); // clear
    }
  }
}


// This function creates a unique filename for each file that
// starts with "FED", then the date in MMDDYY,
// then an incrementing number for each new file created on the same date
void FED3::getFilename(char *filename) {
  DateTime now = rtc.now();

  filename[3] = FED / 100 + '0';
  filename[4] = FED / 10 + '0';
  filename[5] = FED % 10 + '0';
  filename[7] = now.month() / 10 + '0';
  filename[8] = now.month() % 10 + '0';
  filename[9] = now.day() / 10 + '0';
  filename[10] = now.day() % 10 + '0';
  filename[11] = (now.year() - 2000) / 10 + '0';
  filename[12] = (now.year() - 2000) % 10 + '0';
  filename[16] = '.';
  filename[17] = 'C';
  filename[18] = 'S';
  filename[19] = 'V';
  for (uint8_t i = 0; i < 100; i++) {
    filename[14] = '0' + i / 10;
    filename[15] = '0' + i % 10;

    if (! SD.exists(filename)) {
      break;
    }
  }
  return;
}

/**************************************************************************************************************************************************
                                                                                               Change device number
**************************************************************************************************************************************************/
// Change device number
void FED3::SetDeviceNumber() {
  // This code is activated when both pokes are pressed simultaneously from the 
  //start screen, allowing the user to set the device # of the FED on the device
  while (SetFED == true) {
    //adjust FED device number
    display.fillRect (0, 0, 200, 80, WHITE);
    display.setCursor(5, 46);
    display.println("Set Device Number");
    display.fillRect (36, 122, 180, 28, WHITE);
    delay (100);
    display.refresh();

    display.setCursor(35, 138);
    if (FED < 100 & FED >= 10) {
      display.print ("0");
    }
    if (FED < 10) {
      display.print ("00");
    }
    display.print (FED);

    delay (100);
    display.refresh();

    if (digitalRead(RIGHT_POKE) == LOW) {
      FED += 1;
      EndTime = millis();
      if (FED > 700) {
        FED = 700;
      }
    }

    if (digitalRead(LEFT_POKE) == LOW) {
      FED -= 1;
      EndTime = millis();
      if (FED < 1) {
        FED = 0;
      }
    }
    if (millis() - EndTime > 3000) {  // if 3 seconds passes confirm device #
      SetFED = false;
      display.setCursor(5, 70);
      display.println("...Set!");
      delay (500);
      display.refresh();
      EndTime = millis();

      while (setTimed == true) {
        // set timed feeding start and stop
        display.fillRect (5, 56, 120, 18, WHITE);
        delay (200);
        display.refresh();

        display.fillRect (0, 0, 200, 80, WHITE);
        display.setCursor(5, 46);
        display.println("Set Timed Feeding");
        display.setCursor(15, 70);
        display.print(timedStart);
        display.print(":00 - ");
        display.print(timedEnd);
        display.print(":00");
        delay (50);
        display.refresh();

        if (digitalRead(LEFT_POKE) == LOW) {
          timedStart += 1;
          EndTime = millis();
          if (timedStart > 24) {
            timedStart = 0;
          }
          if (timedStart > timedEnd) {
            timedEnd = timedStart + 1;
          }
        }

        if (digitalRead(RIGHT_POKE) == LOW) {
          timedEnd += 1;
          EndTime = millis();
          if (timedEnd > 24) {
            timedEnd = 0;
          }
          if (timedStart > timedEnd) {
            timedStart = timedEnd - 1;
          }
        }
        if (millis() - EndTime > 3000) {  // if 3 seconds passes confirm time settings
          setTimed = false;
          display.setCursor(5, 95);
          display.println("...Timing set!");
          delay (1000);
          display.refresh();
        }
      }
      writeFEDmode();
      writeConfigFile();
      NVIC_SystemReset();      // processor software reset
    }
  }
}

//Read battery level
void FED3::ReadBatteryLevel() {
  analogReadResolution(10);
  measuredvbat = analogRead(VBATPIN);
  measuredvbat *= 2;    // we divided by 2, so multiply back
  measuredvbat *= 3.3;  // Multiply by 3.3V, our reference voltage
  measuredvbat /= 1024; // convert to voltage
}

//What happens when pellet is detected
void FED3::pelletTrigger() {
  if (Left == false and Right == false){
    if (digitalRead(PELLET_WELL) == HIGH) {
      PelletAvailable = false;
    }
  }
}

//What happens when left poke is poked
void FED3::leftTrigger() {
  if (PelletAvailable == false){
    if (digitalRead(LEFT_POKE) == LOW ) {
      Left = true;
    }
  }
}

//What happens when right poke is poked
void FED3::rightTrigger() {
  if (PelletAvailable == false){
    if (digitalRead(RIGHT_POKE) == LOW ) {
      Right = true;
    }
  }
}

/**********************************************
  Functions to control processor sleeping
**********************************************/
void FED3::goToSleep() {
  ReleaseMotor();
  if (EnableSleep==true){
    LowPower.sleep(3000); 
  }
  pelletTrigger();       //Every 3 seconds check pellet well to make sure it's not stuck thinking there's a pellet when there's not
}

//Pull all motor pins low to de-energize stepper and save power, also disable motor driver with the EN pin
void FED3::ReleaseMotor () {
  digitalWrite(A2, LOW);
  digitalWrite(A3, LOW);
  digitalWrite(A4, LOW);
  digitalWrite(A5, LOW);
  digitalWrite(MOTOR_ENABLE, LOW);  //Disable motor driver
}

/********************************************************
  initialize FED3 object
********************************************************/
FED3::FED3(String sketch) {
  sessiontype = sketch;
}

/********************************************************
  Startup settings, to be run in Arduino setup()
********************************************************/
//  dateTime function
void dateTime(uint16_t* date, uint16_t* time) {
  DateTime now = rtc.now();
  // return date using FAT_DATE macro to format fields
  *date = FAT_DATE(now.year(), now.month(), now.day());

  // return time using FAT_TIME macro to format fields
  *time = FAT_TIME(now.hour(), now.minute(), now.second());
}

void FED3::StartScreen(){
  display.setTextSize(3);
  display.setTextColor(BLACK);
  display.clearDisplay();
  display.setCursor(15, 55);
  display.print("FED3");
  
  //print filename on screen
  display.setTextSize(1);
  display.setCursor(2, 138);
  display.print(filename);

  //Display FED verison number at startup
  display.setCursor(2, 119);
  display.print("v: ");
  display.print(VER);
  display.print("_");
  display.print(sessiontype);
  display.refresh();
    
  //Draw animated mouse...
  for (int i = -50; i < 200; i += 15) {
    display.fillRoundRect (i + 25, 77, 15, 10, 6, BLACK);    //head
    display.fillRoundRect (i + 22, 75, 8, 5, 3, BLACK);      //ear
    display.fillRoundRect (i + 30, 79, 1, 1, 1, WHITE);      //eye

    //movement of the mouse
    if ((i / 10) % 2 == 0) {
      display.fillRoundRect (i, 79, 32, 17, 10, BLACK);      //body
      display.drawFastHLine(i - 8, 80, 18, BLACK);           //tail
      display.drawFastHLine(i - 8, 81, 18, BLACK);
      display.drawFastHLine(i - 14, 79, 8, BLACK);
      display.drawFastHLine(i - 14, 80, 8, BLACK);

      display.fillRoundRect (i + 22, 94, 8, 4, 3, BLACK);    //front foot
      display.fillRoundRect (i , 92, 8, 6, 3, BLACK);        //back foot
    }
    else {
      display.fillRoundRect (i + 2, 77, 30, 17, 10, BLACK);  //body
      display.drawFastHLine(i - 6, 86, 18, BLACK);           //tail
      display.drawFastHLine(i - 6, 85, 18, BLACK);
      display.drawFastHLine(i - 12, 87, 8, BLACK);
      display.drawFastHLine(i - 12, 86, 8, BLACK);

      display.fillRoundRect (i + 15, 94, 8, 4, 3, BLACK);    //foot
      display.fillRoundRect (i + 8, 92, 8, 6, 3, BLACK);     //back foot
    }

    display.refresh();
    delay (80);
    display.fillRect (i - 25, 75, 80, 32, WHITE);
    
      // Push both pokes to edit device number
    if ((digitalRead(LEFT_POKE) == LOW) && (digitalRead(RIGHT_POKE) == LOW)) {
        tone (BUZZER, 1000, 200);
        delay(400);
        tone (BUZZER, 1000, 500);
        delay(200);
        tone (BUZZER, 3000, 600);
        colorWipe(strip.Color(2, 2, 2), 40); // Color wipe
        colorWipe(strip.Color(0, 0, 0), 20); // OFF
        EndTime = millis();
        SetFED = true;
        SetDeviceNumber();
    }
  }
}

void FED3::begin() {
  Serial.begin(9600);
  
  // Initialize pins
  pinMode(PELLET_WELL, INPUT);
  pinMode(LEFT_POKE, INPUT);
  pinMode(RIGHT_POKE, INPUT);
  pinMode(MOTOR_ENABLE, OUTPUT);
  pinMode(GREEN_LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(A2, OUTPUT);
  pinMode(A3, OUTPUT);
  pinMode(A4, OUTPUT);
  pinMode(A5, OUTPUT);
  pinMode(BNC_OUT, OUTPUT);

  // Initialize RTC
  rtc.begin();

  // Initialize Neopixels
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'

  // Initialize stepper
  stepper.setSpeed(12);

  // Initialize display
  display.begin();
  const int minorHalfSize = min(display.width(), display.height()) / 2;
  display.setFont(&FreeSans9pt7b);
  display.setRotation(3);
  display.setTextColor(BLACK);
  display.setTextSize(1);
  display.refresh();
 
  // Initialize SD card and create the datafile
  SdFile::dateTimeCallback(dateTime);
  CreateFile();

  // Initialize interrupts
  pointerToFED3 = this;
  LowPower.attachInterruptWakeup(digitalPinToInterrupt(PELLET_WELL), outsidePelletTriggerHandler, CHANGE);
  LowPower.attachInterruptWakeup(digitalPinToInterrupt(LEFT_POKE), outsideLeftTriggerHandler, CHANGE);
  LowPower.attachInterruptWakeup(digitalPinToInterrupt(RIGHT_POKE), outsideRightTriggerHandler, CHANGE);

  // Create data file for current session
  CreateDataFile();
  writeHeader();
  EndTime = 0;
  
  //read battery level
  ReadBatteryLevel();
  
  // Startup display uses StartScreen() unless CLassicFED3==true, then use ClassicMenu()
  if (ClassicFED3 == true){
    ClassicMenu();
  }
  else {
    StartScreen();
  }
  display.clearDisplay();
  display.refresh();
}

/******************************************************************************************************************************************************
                                                                                           Classic FED3 functions
******************************************************************************************************************************************************/

/********************************************************
  Classic menu display
********************************************************/
void FED3::ClassicMenu () {
  //  0 Free feeding
  //  1 FR1
  //  2 FR3
  //  3 FR5
  //  4 Progressive Ratio
  //  5 Extinction
  //  6 Light tracking FR1 task
  //  7 FR1 (reversed)
  //  8 PR (reversed)
  //  9 self-stim
  //  10 self-stim (reversed)
  //  11 time feeding

  // Set FR based on FEDmode
  if (FEDmode == 0) FR = 0;  // free feeding
  if (FEDmode == 1) FR = 1;  // FR1 spatial tracking task
  if (FEDmode == 2) FR = 3;  // FR3
  if (FEDmode == 3) FR = 5; // FR5
  if (FEDmode == 4) FR = 99;  // Progressive Ratio
  if (FEDmode == 5) { // Extinction
    FR = 1;
    ReleaseMotor ();
    digitalWrite (MOTOR_ENABLE, LOW);  //Disable motor driver
  }
  if (FEDmode == 6) FR = 1;  // Light tracking
  if (FEDmode == 7) FR = 1; // FR1 (reversed)
  if (FEDmode == 8) FR = 1; // PR (reversed)
  if (FEDmode == 9) FR = 1; // self-stim
  if (FEDmode == 10) FR = 1; // self-stim (reversed)

  display.clearDisplay();
  display.setCursor(1, 135);
  display.print(filename);

  display.setFont(&FreeSans9pt7b);
  display.setCursor(10, 20); display.println("Classic FED3");
  display.setCursor(11, 20); display.println("Classic FED3");
  display.fillRect(0, 30, 160, 80, WHITE);
  display.setCursor(10, 40);
  display.print("Select Program:");
  
  display.setCursor(10, 60);
  //Text to display selected FR ratio
  if (FEDmode == 0) display.print("Free feeding");
  if (FEDmode == 1) display.print("Fixed Ratio 1");
  if (FEDmode == 2) display.print("Fixed Ratio 3");
  if (FEDmode == 3) display.print("Fixed Ratio 5");
  if (FEDmode == 4) display.print("Progressive Ratio");
  if (FEDmode == 5) display.print("Extinction");
  if (FEDmode == 6) display.print("Light tracking");
  if (FEDmode == 7) display.print("FR1");
  if (FEDmode == 8) display.print("Progressive Ratio");
  if (FEDmode == 9) display.print("Self-Stimulation");
  if (FEDmode == 10) display.print("Self-Stimulation");
  if (FEDmode == 8 || FEDmode == 7 || FEDmode == 10) display.setCursor(10, 75);
  if (FEDmode == 8 || FEDmode == 7 || FEDmode == 10) display.print("(reversed)");
  if (FEDmode == 11) display.print("Timed feeding");
  
  //Draw animated mouse...
  for (int i = -50; i < 200; i += 15) {
    display.fillRoundRect (i + 25, 77, 15, 10, 6, BLACK);    //head
    display.fillRoundRect (i + 22, 75, 8, 5, 3, BLACK);      //ear
    display.fillRoundRect (i + 30, 79, 1, 1, 1, WHITE);      //eye

    //movement of the mouse
    if ((i / 10) % 2 == 0) {
      display.fillRoundRect (i, 79, 32, 17, 10, BLACK);      //body
      display.drawFastHLine(i - 8, 80, 18, BLACK);           //tail
      display.drawFastHLine(i - 8, 81, 18, BLACK);
      display.drawFastHLine(i - 14, 79, 8, BLACK);
      display.drawFastHLine(i - 14, 80, 8, BLACK);

      display.fillRoundRect (i + 22, 94, 8, 4, 3, BLACK);    //front foot
      display.fillRoundRect (i , 92, 8, 6, 3, BLACK);        //back foot
    }
    else {
      display.fillRoundRect (i + 2, 77, 30, 17, 10, BLACK);  //body
      display.drawFastHLine(i - 6, 86, 18, BLACK);           //tail
      display.drawFastHLine(i - 6, 85, 18, BLACK);
      display.drawFastHLine(i - 12, 87, 8, BLACK);
      display.drawFastHLine(i - 12, 86, 8, BLACK);

      display.fillRoundRect (i + 15, 94, 8, 4, 3, BLACK);    //foot
      display.fillRoundRect (i + 8, 92, 8, 6, 3, BLACK);     //back foot
    }

    display.refresh();
    delay (80);
    display.fillRect (i-25, 73, 90, 35, WHITE);
    previousFEDmode = FEDmode;
    previousFED = FED;
    if (digitalRead (LEFT_POKE) == LOW | digitalRead (RIGHT_POKE) == LOW) SelectMode();
  }
  ReadBatteryLevel();
  display.clearDisplay();
  display.refresh();
}

// Set FEDMode
void FED3::SelectMode() {
  // Mode select on startup screen
  //If both pokes are activated
  if ((digitalRead(LEFT_POKE) == LOW) && (digitalRead(RIGHT_POKE) == LOW)) {
    tone (BUZZER, 3000, 500);
    colorWipe(strip.Color(2, 2, 2), 40); // Color wipe
    colorWipe(strip.Color(0, 0, 0), 20); // OFF
    EndTime = millis();
    SetFED = true;
    setTimed = true;
    SetDeviceNumber();
  }

  //If Left Poke is activated
  else if (digitalRead(LEFT_POKE) == LOW) {
    EndTime = millis();
    FEDmode -= 1;
    tone (BUZZER, 2500, 200);
    colorWipe(strip.Color(2, 0, 2), 40); // Color wipe
    colorWipe(strip.Color(0, 0, 0), 20); // OFF
    if (FEDmode == -1) FEDmode = 11;
  }

  //If Right Poke is activated
  else if (digitalRead(RIGHT_POKE) == LOW) {
    EndTime = millis();
    FEDmode += 1;
    tone (BUZZER, 2500, 200);
    colorWipe(strip.Color(2, 2, 0), 40); // Color wipe
    colorWipe(strip.Color(0, 0, 0), 20); // OFF
    if (FEDmode == 12) FEDmode = 0;
  }

  if (FEDmode < 0) FEDmode = 0;
  if (FEDmode > 11) FEDmode = 11;

  display.fillRect (10, 48, 200, 50, WHITE);  //erase the selected program text
  display.setCursor(10, 60);  //Display selected program
  if (FEDmode == 0) display.print("Free feeding");
  if (FEDmode == 1) display.print("Fixed Ratio 1");
  if (FEDmode == 2) display.print("Fixed Ratio 3");
  if (FEDmode == 3) display.print("Fixed Ratio 5");
  if (FEDmode == 4) display.print("Progressive Ratio");
  if (FEDmode == 5) display.print("Extinction");
  if (FEDmode == 6) display.print("Light tracking");
  if (FEDmode == 7) display.print("FR1");
  if (FEDmode == 8) display.print("Progressive Ratio");
  if (FEDmode == 9 || FEDmode == 10) display.print("Self-Stimulation");
  if (FEDmode == 8 || FEDmode == 7 || FEDmode == 10) display.setCursor(10, 75);
  if (FEDmode == 8 || FEDmode == 7 || FEDmode == 10) display.print("(reversed)");
  if (FEDmode == 11) display.print("Timed feeding");
  display.refresh();
  
  while (millis() - EndTime < 1500) {
    SelectMode();
  }
  display.setCursor(10, 100);
  display.println("...Selected!");
  display.refresh();
  delay (500);
  writeFEDmode();
  delay (200);
  NVIC_SystemReset();      // processor software reset
}

//Display update for Classic FED3 variant
void FED3::ClassicUpdateDisplay() {
  display.setCursor(5, 15);
  display.print("FED:");
  display.println(FED);
  display.setCursor(6, 15);  // this doubling is a way to do bold type
  display.print("FED:");
  display.fillRect (6, 20, 200, 22, WHITE);  //erase the data on screen without clearing the entire screen by pasting a white box over it

  if (FEDmode == 0) {
    display.fillRect (35, 24, 200, 50, WHITE);  //erase the data on screen without clearing the entire screen by pasting a white box over it
    display.setCursor(22, 64);
    display.print("Free Feeding");
  }

  if (FEDmode == 11) {
    display.fillRect (35, 24, 200, 50, WHITE);  //erase the data on screen without clearing the entire screen by pasting a white box over it
    display.setCursor(22, 64);
    display.println("Timed Feeding");
    display.setCursor(22, 80);
    display.print(timedStart);
    display.print(" to ");
    display.print(timedEnd);
  }

  if (FEDmode < 4 & FEDmode != 0 & FEDmode != 11) {
    display.setCursor(5, 36);
    display.print("FR: ");
    display.setCursor(6, 36);
    display.print("FR: ");
    display.print(FR);
  }

  if (FEDmode == 4 & LeftCount == 0) { //Prog ratio, first poke
    display.setCursor(5, 36);
    display.print("PR: ");
    display.setCursor(6, 36);
    display.print("PR: ");
    display.fillRect (35, 24, 200, 55, WHITE);  //erase the data on screen without clearing the entire screen by pasting a white box over it
    display.print(1);
  }

  if (FEDmode == 4 & LeftCount != 0) { // Prog ratio, NOT first poke
    display.setCursor(5, 36);
    display.print("PR: ");
    display.setCursor(6, 36);
    display.print("PR: ");
    display.fillRect (35, 24, 200, 55, WHITE);  //erase the data on screen without clearing the entire screen by pasting a white box over it
    display.print(FR);
  }

  if (FEDmode == 5) {
    display.setCursor(5, 36);
    display.print("Extinction");
  }

  if (FEDmode == 6) {
    display.setCursor(5, 36);
    display.print("Light tracking");
  }

  if (FEDmode == 7) {
    display.setCursor(5, 36);
    display.print("FR1 (reversed)");
  }

  if (FEDmode == 8 & RightCount == 0) { //Prog ratio, first poke
    display.setCursor(5, 36);
    display.print("PR(R): ");
    display.setCursor(6, 36);
    display.print("PR(R): ");
    display.fillRect (55, 24, 200, 55, WHITE);  //erase the data on screen without clearing the entire screen by pasting a white box over it
    display.print(1);
  }

  if (FEDmode == 8 & RightCount != 0) { // Prog ratio, NOT first poke
    display.setCursor(5, 36);
    display.print("PR(R): ");
    display.setCursor(6, 36);
    display.print("PR(R): ");
    display.fillRect (55, 24, 200, 55, WHITE);  //erase the data on screen without clearing the entire screen by pasting a white box over it
    display.print(ratio - RightCount);
  }

  if (FEDmode == 9) {
    display.setCursor(5, 36);
    display.print("Stim");
  }

  if (FEDmode == 10) {
    display.setCursor(5, 36);
    display.print("Stim (R)");
  }

  if (FEDmode != 11 & FEDmode != 0) { // don't erase this if it's a free or timed feeding session
    display.fillRect (35, 42, 130, 80, WHITE);  //erase the pellet data on screen without clearing the entire screen by pasting a white box over it
  }

  display.fillRect (93, 90, 70, 20, WHITE);  //erase the pellet data on screen without clearing the entire screen by pasting a white box over it

  if (FEDmode > 0 & FEDmode != 11) {
    display.setCursor(35, 65);
    display.print("Left: ");
    display.setCursor(95, 65);
    display.print(LeftCount);

    display.setCursor(35, 85);
    display.print("Right:  ");
    display.setCursor(95, 85);
    display.print(RightCount);
  }

  if (FEDmode != 5 && FEDmode != 9 && FEDmode != 10) {  //don't show pellets if extinction or opto session
    display.setCursor(35, 105);
    display.print("Pellets:");
    display.setCursor(95, 105);
    display.print(PelletCount);
  }

  //  Battery graphic showing bars indicating voltage levels
  //Clear battery area and draw outline of battery, only do this when numMotorTurns = 0 so it doesn't flicker;
  if (numMotorTurns == 0) {
    display.fillRect (117, 2, 40, 16, WHITE);
    display.drawRect (116, 1, 42, 18, BLACK);
    display.drawRect (157, 6, 6, 8, BLACK);
  }
  //4 bars
  if (measuredvbat > 3.85 & numMotorTurns == 0) {
    display.fillRect (120, 4, 7, 12, BLACK);
    display.fillRect (129, 4, 7, 12, BLACK);
    display.fillRect (138, 4, 7, 12, BLACK);
    display.fillRect (147, 4, 7, 12, BLACK);
  }

  //3 bars
  else if (measuredvbat > 3.7 & numMotorTurns == 0) {
    display.fillRect (119, 3, 26, 13, WHITE);
    display.fillRect (120, 4, 7, 12, BLACK);
    display.fillRect (129, 4, 7, 12, BLACK);
    display.fillRect (138, 4, 7, 12, BLACK);
  }

  //2 bars
  else if (measuredvbat > 3.55 & numMotorTurns == 0) {
    display.fillRect (119, 3, 26, 13, WHITE);
    display.fillRect (120, 4, 7, 12, BLACK);
    display.fillRect (129, 4, 7, 12, BLACK);
  }

  //1 bar
  else if (numMotorTurns == 0) {
    display.fillRect (119, 3, 26, 13, WHITE);
    display.fillRect (120, 4, 7, 12, BLACK);
  }


  //Box around data area of screen
  display.drawRect (5, 45, 158, 70, BLACK);

  // Print date and time at bottom of the screen
  DateTime now = rtc.now();
  display.setCursor(10, 135);
  display.fillRect (0, 123, 200, 60, WHITE);
  display.print(now.month());
  display.print("/");
  display.print(now.day());
  display.print("/");
  display.print(now.year());
  display.print("    ");
  if (now.hour() < 10)
    display.print('0');      // Trick to add leading zero for formatting
  display.print(now.hour());
  display.print(":");
  if (now.minute() < 10)
    display.print('0');      // Trick to add leading zero for formatting
  display.print(now.minute());

  // Poke and pellet indicator graphics
  if (FEDmode > 0 & FEDmode != 11) {
    //poke indicators
    if (digitalRead(RIGHT_POKE) == HIGH) {
      display.fillCircle(25, 79, 5, WHITE);
      display.drawCircle(25, 79, 5, BLACK);
    }

    if (digitalRead(LEFT_POKE) == HIGH) {
      display.fillCircle(25, 59, 5, WHITE);
      display.drawCircle(25, 59, 5, BLACK);
    }

    //indicate which poke is active with a filled triangle beside it
    if (FEDmode == 7 || FEDmode == 8 || FEDmode == 10) {
      activePoke = 0;
    }

    if (activePoke == 0) {
      display.fillTriangle (12, 55, 18, 59, 12, 63, WHITE);
      display.fillTriangle (12, 75, 18, 79, 12, 83, BLACK);
    }

    if (activePoke == 1) {
      display.fillTriangle (12, 75, 18, 79, 12, 83, WHITE);
      display.fillTriangle (12, 55, 18, 59, 12, 63, BLACK);
    }
  }

  if (FEDmode != 5 && FEDmode != 9 && FEDmode != 10) { //no need to show pellets if extinction or opto stim sessions
    if (digitalRead(PELLET_WELL) == HIGH) {
      display.fillCircle(25, 99, 5, WHITE);
      display.drawCircle(25, 99, 5, BLACK);
    }

    if (digitalRead(PELLET_WELL) == LOW) {
      display.fillCircle(25, 99, 5, BLACK);
    }
  }
  display.refresh();
}



void FED3::Classiclogdata() {
  /////////////////////////////////
  // Log data and time 
  /////////////////////////////////
  DateTime now = rtc.now();
  logfile.print(now.month());
  logfile.print("/");
  logfile.print(now.day());
  logfile.print("/");
  logfile.print(now.year());
  logfile.print(" ");
  logfile.print(now.hour());
  logfile.print(":");
  if (now.minute() < 10)
    logfile.print('0');      // Trick to add leading zero for formatting
  logfile.print(now.minute());
  logfile.print(":");
  if (now.second() < 10)
    logfile.print('0');      // Trick to add leading zero for formatting
  logfile.print(now.second());
  logfile.print(",");

  /////////////////////////////////
  // Log library version and Sketch identifier text
  /////////////////////////////////
  logfile.print(VER); // Print library version
  logfile.print("_");
  logfile.print(sessiontype);  //print Sketch identifier
  logfile.print(",");

  /////////////////////////////////
  // Log FED device number
  /////////////////////////////////
  logfile.print(FED); // 
  logfile.print(",");

  /////////////////////////////////
  // Log battery voltage
  /////////////////////////////////
  logfile.print(measuredvbat); // 
  logfile.print(",");

  /////////////////////////////////
  // Log motor turns
  /////////////////////////////////
  logfile.print(numMotorTurns); // Print the number of attempts to dispense a pellet
  logfile.print(",");

  /////////////////////////////////
  // Log Trial Info
  /////////////////////////////////
  if (FEDmode == 4) {
    logfile.print("PR");
    logfile.print(round((5 * exp (0.2 * PelletCount)) - 5)); // Print current PR ratio
    logfile.print(",");
  }

  else if (FEDmode == 8) {
    logfile.print("PR_reversed");
    logfile.print(round((5 * exp (0.2 * PelletCount)) - 5)); // Print current PR ratio
    logfile.print(",");
  }

  else if (FEDmode == 0) {
    logfile.print("FED"); // Print trial type
    logfile.print(",");
  }

  else if (FEDmode == 5) {
    logfile.print("Extinction"); // Print trial type
    logfile.print(",");
  }

  else if (FEDmode == 6) {
    logfile.print("FR1_Light_tracking"); // Print trial type
    logfile.print(",");
  }

  else if (FEDmode == 7) {
    logfile.print("FR1_reversed"); // Print trial type
    logfile.print(",");
  }

  else if (FEDmode == 9) {
    logfile.print("Self_stim"); // Print trial type
    logfile.print(",");
  }

  else if (FEDmode == 10) {
    logfile.print("Self_stim_reversed"); // Print trial type
    logfile.print(",");
  }

  else if (FEDmode == 11) {
    logfile.print("Timed_"); // Print trial type
    logfile.print(timedStart); // Print trial type
    logfile.print("to"); // Print trial type
    logfile.print(timedEnd); // Print trial type
    logfile.print(",");
  }

  else {
    logfile.print("FR"); // Print FR 
    logfile.print(FR); // Print FR ratio
    logfile.print(",");
  }

  /////////////////////////////////
  // Log FR ratio
  /////////////////////////////////
  logfile.print(FR);
  logfile.print(",");

  /////////////////////////////////
  // Log event type (pellet, right, left)
  /////////////////////////////////
  logfile.print(Event); 
  logfile.print(",");

  /////////////////////////////////
  // Log Active poke side (left, right)
  /////////////////////////////////
  if (activePoke == 0)  logfile.print("Right"); //
  if (activePoke == 1)  logfile.print("Left"); //
  logfile.print(",");

  /////////////////////////////////
  // Log data (leftCount, RightCount, Pellets
  /////////////////////////////////
  logfile.print(LeftCount); // Print Left poke count
  logfile.print(",");
    
  logfile.print(RightCount); // Print Right poke count
  logfile.print(",");

  logfile.print(PelletCount); // print Pellet counts
  logfile.print(",");

  logfile.print(BlockPelletCount); // print Block Pellet counts
  logfile.print(",");

  /////////////////////////////////
  // Log pellet retrieval interval
  /////////////////////////////////
//   if (pellet == false ) {
    if (Event == "Pellet"){
    logfile.print(sqrt (-1)); // print NaN if it's not a pellet line!
  }

  else if (retInterval < 60000 ) {  // only log retrieval intervals below 2 minutes
    logfile.print(retInterval/1000.000); // print interval between pellet dispensing and being taken
  }

  else if (retInterval >= 60000) {
    logfile.print("Timed_out"); // print "Timed_out" if retreival interval is >60s
  }

  else {
    logfile.print("Error"); // print error if value is < 0 (this shouldn't ever happen)
  }
  logfile.print(",");
  
  /////////////////////////////////
  // Log poke duration
  /////////////////////////////////
//   if (pellet == true ) {
  if (Event == "Pellet"){
    logfile.println(sqrt (-1)); // print NaN 
  }

  else if (Left) {  // 
    logfile.println(leftInterval/1000.000); // print left poke timing
  }

  else if (Right) {
    logfile.println(rightInterval/1000.000); // print left poke timing
  }

  /////////////////////////////////
  // logfile.flush write to the SD card
  /////////////////////////////////
  Blink(GREEN_LED, 100, 2);
  logfile.flush();
}

//write a FEDmode file (this contains the last used FEDmode)
void FED3::writeFEDmode() {
  ratiofile = SD.open("FEDmode.csv", FILE_WRITE);
  ratiofile.rewind();
  ratiofile.println(FEDmode);
  ratiofile.flush();
  ratiofile.close();

  startfile = SD.open("start.csv", FILE_WRITE);
  startfile.rewind();
  startfile.println(timedStart);
  startfile.flush();
  startfile.close();

  stopfile = SD.open("stop.csv", FILE_WRITE);
  stopfile.rewind();
  stopfile.println(timedEnd);
  stopfile.flush();
  stopfile.close();
}