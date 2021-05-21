/*
  Feeding experimentation device 3 (FED3)
  Pavlovian Pellet Drop
  This script will delivery a conditioned stimulus, wait 5 seconds, and then drop a pellet
  After the pellet has been taken, it will complete a timeout of N seconds and repeat from the beginning

  alexxai@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "Pavlov";                               //Unique identifier text for each sketch (this will show up on the screen and in log file)
FED3 fed3 (sketch);                                     //Start the FED3 object
int Pellet_delay = 5;                                   //How long to wait between conditioned stimulus and pellet (in s)

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
  fed3.DisplayPokes = false;                            //Customize the DisplayPokes option to 'false' to not display the poke indicators
  fed3.timeout = 10;                                    //Set a timeout period (in seconds) to wait before starting a new trial after each pellet is taken
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop
  fed3.ConditionedStimulus();                           //Conditioned stimulus (light and tones)
  fed3.Timeout(Pellet_delay);                           //delay between tone and pellet
  fed3.Feed();                                          //Drop pellet
  
  while (digitalRead (PELLET_WELL) == LOW) {            //Wait here while there's a pellet in the well
    delay (10);
  }
}
