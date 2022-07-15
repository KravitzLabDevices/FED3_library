/*
  Feeding experimentation device 3 (FED3)
  Random Ratio Script

  Lex Kravitz
  alexxai@wustl.edu
  March, 2022

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2022 Lex Kravitz
*/

////////////////////////////////////////////////////
// Set the FR limits for the random ratio
////////////////////////////////////////////////////
int FR = random(1, 11);                               //Set the min and max for the random ratio.  In this example this is set between 1 and 10.

////////////////////////////////////////////////////
// Start FED3 library and make the fed3 object
////////////////////////////////////////////////////
#include <FED3.h>                                     //Include the FED3 library 
String sketch = "RndRatio";                           //Unique identifier text for each sketch
FED3 fed3 (sketch);                                   //Start the FED3 object

void setup() {
  randomSeed(analogRead(0));
  fed3.begin();                                       //Setup the FED3 hardware
  fed3.FR = FR;                                       //share starting FR ratio with the fed3 library for logging
}

void loop() {
  fed3.run();                                         //Call fed.run at least once per loop

  // If Left poke is triggered
  if (fed3.Left) {
    fed3.Click();                                     //click stimulus
    fed3.logLeftPoke();                               //Log left poke
    if (fed3.LeftCount % FR == 0) {                   //if random ratio is  met
      fed3.ConditionedStimulus();                     //deliver conditioned stimulus (tone and lights)
      fed3.Feed();                                    //deliver pellet
      FR = random(1, 11);                             //randomize the number of pokes required for next pellet
      fed3.FR = FR;                                   //share this new ratio with the fed3 library for logging
    }
  }

  // If Right poke is triggered
  if (fed3.Right) {
    fed3.logRightPoke();                              //Log Right poke
    fed3.Click();                                     //click stimulus
  }
}
