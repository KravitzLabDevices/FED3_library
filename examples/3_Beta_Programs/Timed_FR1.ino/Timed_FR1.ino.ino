/*
  Feeding experimentation device 3 (FED3)
  Fixed Ratio 1 with a user-settable Timed Feeding option
  Device starts in FR1
  Hold both pokes at startup to enter a small edit menu where you can change the device number and the timed-feeding hours
  Device will only dispense pellets during these hours, although it will log pokes at all times
 
  alexxai@wustl.edu
  October, 2021

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include <FED3.h>                                                                      //Include the FED3 library 
String sketch = "FR1_timed";                                                           //Unique identifier text for each sketch
FED3 fed3 (sketch);                                                                    //Start the FED3 object

void setup() {
  fed3.setTimed = true;                                                                //Set a flag to ask FED3 to edit "Timed Feeding" times when it enters the "Set Device Number" edit menu (this defaults to false)
  fed3.begin();                                                                        //Setup the FED3 hardware

  delay (2000);                                                                        //Delay 2s to allow user time to poke both pokes
  if (fed3.Left && fed3.Right) {                                                      //If both pokes are activated during the mouse running screen
    fed3.SetDeviceNumber();                                                            //Enter "Set Device Number" edit menu to set device number and timed feeding hours
  }
}

void loop() {
  fed3.run();                                                                          //Call fed.run at least once per loop
  if (fed3.Left) {
    if (fed3.currentHour >= fed3.timedStart && fed3.currentHour < fed3.timedEnd) {     //If left poke is triggered and it's between the specified times
      fed3.logLeftPoke();                                                              //Log left poke
      fed3.ConditionedStimulus();                                                      //Deliver conditioned stimulus (tone and lights for 200ms)
      fed3.Feed();                                                                     //Deliver pellet
    }
    else {                                                                             //If it's not between the specified times
      fed3.logLeftPoke();                                                              //Log left poke
    }
  }

  if (fed3.Right) {                                                                    //If right poke is triggered
    fed3.logRightPoke();                                                               //Log right poke
  }
}
