/*
  Basic Pellet Dispenser
  FED3 will dispense a pellet when it detects a HIGH pulse on the BNC port.  It will not dispense another until that pellet is removed.  Timestamps of pellet removal are logged.
  alexxai@wustl.edu
  January, 2021

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020, 2021 Lex Kravitz
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "Dispenser";                            //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  fed3.DisplayPokes = false;                            //turn off poke indicators on screen
  fed3.disableSleep();                                  //disable sleep for this program as the BNC port is not currently set up as an interrupt
  fed3.begin();                                         //Setup the FED3 hardware
}

void loop() {
  fed3.run();                                           //Call fed.run at the top of the void loop
  fed3.ReadBNC(true);                                   //This sets the BNC to be input and reads it.  The boolean argument refers to whether you want the green LED on the Adalogger board insied of FED3 to blink when it detects an input (true) or not (false)
  if (fed3.BNCinput) {                                  //If HIGH is detected on BNC input
    fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
    fed3.Feed();                                        //Deliver pellet
  }
}
