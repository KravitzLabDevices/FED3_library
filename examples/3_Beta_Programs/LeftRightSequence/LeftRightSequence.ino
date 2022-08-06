/*
  Feeding experimentation device 3 (FED3)
  Left Right Sequence task
  alexxai@wustl.edu
  April 2022

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "LeftRight";                            //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object
bool leftTriggered = false;
unsigned long poketime = 0;                             //time of poke
byte resetInterval = 60;                                //How long do they have to poke right (in seconds)?

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
  fed3.disableSleep();
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop
  leftReset();

  if (fed3.PelletCount == 60)  resetInterval = 30;       // after 40 pellets make left-right reset interval = 9
  if (fed3.PelletCount == 120) resetInterval = 10;       // after 40 pellets make left-right reset interval = 8
  if (fed3.PelletCount == 160) resetInterval = 8;       // after 80 pellets make left-right reset interval = 7
  if (fed3.PelletCount == 200) resetInterval = 6;      // after 120 pellets make left-right reset interval = 6
  if (fed3.PelletCount == 240) resetInterval = 5;      // after 120 pellets make left-right reset interval = 6
  if (fed3.PelletCount == 280) resetInterval = 4;      // after 120 pellets make left-right reset interval = 6

  //if left poke is triggered
  if (fed3.Left) {                                      //If left poke is triggered
    Serial.println ("Left");
    fed3.BlockPelletCount = millis();
    fed3.logLeftPoke();                                 //Log left poke
    leftTriggered = true;
    poketime = fed3.unixtime;                          //update the current time of poke
  }

  if (fed3.Right and leftTriggered == true) {                                     //If right poke is triggered
    Serial.println ("Right_after_left");
    fed3.BlockPelletCount = millis();
    fed3.logRightPoke();                                //Log right poke
    fed3.pixelsOn(0, 0, 10, 0);
    fed3.Feed();                                        //Deliver pellet
    fed3.BNC(500, 1);
    leftTriggered = false;
  }

  if (fed3.Right and leftTriggered == false) {                                     //If right poke is triggered
    Serial.println ("Right_no_left");
    fed3.BlockPelletCount = millis();
    fed3.logRightPoke();                                //Log right poke
  }
}

void leftReset() {
  if ((fed3.unixtime - poketime >= resetInterval) and leftTriggered == true) {   //if the reset interval has elapsed since last poke
    Serial.print (resetInterval);
    Serial.println ("s, LeftTriggered RESET");
    leftTriggered = false;
    fed3.BlockPelletCount = millis();
    fed3.Event = "RESET";
    fed3.logdata();
  }
}
