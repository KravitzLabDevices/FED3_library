/*
  Feeding experimentation device 3 (FED3)
  Left Right sequence - Stop signal task - first train mice on the Left-Right Sequence task
  alexxai@wustl.edu
  April 2022

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "StopSig";                            //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object

//Poke variables
bool leftTriggered = false;
unsigned long poketime = 0;                            //time of left poke
byte resetInterval = 4;

//stop sig variables
bool stopTrial = false;
int stopSigTime = 0;
byte stopProb = 30;
byte consecutiveRegulars = 0;

void setup() {
  randomSeed(analogRead(0));
  fed3.begin();                                         //Setup the FED3 hardware
  fed3.disableSleep();                                  //Disable sleep so we can use millis() and Serial.print statements
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop

  /////////////////////////////////
  //Right poke without left
  /////////////////////////////////
  if (fed3.Right and leftTriggered == false) {                                     //If right poke is triggered
    fed3.RightCount ++;
    fed3.Click();
    Serial.println ("Right_no_left");
    fed3.Event = "Right_no_left";
    fed3.logdata();
    fed3.BlockPelletCount = millis();
    fed3.Right = false;
  }

  /////////////////////////////////
  //if left poke is triggered
  /////////////////////////////////
  if (fed3.Left and leftTriggered == false) {                                      //If left poke is triggered
    fed3.LeftCount ++;
    Serial.println ("Left");
    fed3.Click();
    fed3.BlockPelletCount = millis();
    leftTriggered = true;
    fed3.run();
    poketime = fed3.unixtime;                          //update the current time of poke

    // Is it a stop trial?
    if ((random (0, 10) < (stopProb / 10)) or consecutiveRegulars > 4) {
      Serial.println ("Stop signal!");
      stopSigTime = fed3.unixtime;
      delay (500);
      fed3.Tone (6000, 3000);
      stopTrial = true;
      fed3.Event = ">Left_Stop_trial";
      consecutiveRegulars = 0;
    }

    //if it is NOT a stop trial
    else {
      Serial.println ("Regular_trial");
      stopTrial = false;
      fed3.Event = ">Left_Regular_trial";
      consecutiveRegulars ++;
    }

    Serial.print ("Consecutive regular trials: ");
    Serial.println(consecutiveRegulars);
    fed3.logdata();
    fed3.Left = false;
  }

  /////////////////////////////////
  //no stop signal - right poke delivers pellet
  /////////////////////////////////
  if (fed3.Right and leftTriggered == true and stopTrial == false) {                                    //If right poke is triggered
    fed3.RightCount ++;
    fed3.Click();
    fed3.BlockPelletCount = millis();
    Serial.println ("Right_Regular_(correct)");
    fed3.Event = "Right_Regular_(correct)";
    fed3.logdata();
    fed3.Feed();                                        //Deliver pellet
    //fed3.BNC(500, 1);
    leftTriggered = false;
    fed3.Right = false;
  }

  /////////////////////////////////
  //stop signal trial - right poke enters timeout
  /////////////////////////////////
  if (fed3.Right and leftTriggered == true and stopTrial == true) {                                    //If right poke is triggered
    fed3.RightCount ++;
    fed3.Click();
    Serial.println ("Right_STOP_SIGNAL");
    fed3.BlockPelletCount = millis();
    fed3.Event = "Right_STOP_(incorrect)";
    fed3.logdata();
    fed3.Noise(2000);
    fed3.pixelsOn(2, 2, 2, 2);
    fed3.Timeout(30);
    fed3.pixelsOff();
    leftTriggered = false;
    fed3.Right = false;
  }

  /////////////////////////////////
  //stop signal trial - withholding right poke delivers pellet!
  /////////////////////////////////
  if ((fed3.unixtime - poketime >= resetInterval) and leftTriggered == true and stopTrial == true) {                                    //If right poke is triggered
    Serial.println ("Withheld poking, get pellet!");
    fed3.Event = "NoPoke_STOP_(correct)";
    fed3.BlockPelletCount = millis();
    fed3.logdata();
    fed3.Feed();                                        //Deliver pellet
    //fed3.BNC(500, 1);
    leftTriggered = false;
  }

  /////////////////////////////////
  // If it is a regular trial but he does NOT poke on right, reset and give timeout
  /////////////////////////////////
  if ((fed3.unixtime - poketime >= resetInterval) and leftTriggered == true and stopTrial == false) {   //if the reset interval has elapsed since last poke
    Serial.println ("No_poke, left poke reset");
    leftTriggered = false;
    fed3.BlockPelletCount = millis();
    fed3.Event = "NoPoke_Regular_(incorrect)";
    fed3.logdata();
    fed3.Left = false;
    fed3.Right = false;
    fed3.Timeout(30);
  }
}
