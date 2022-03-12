/*
  Feeding experimentation device 3 (FED3)
  Optogenetic self-stimulation example (FR1)
  alexxai@wustl.edu
  March, 2022

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "OptoStim";                                  //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop
  if (fed3.Left) {                                      //If left poke is triggered
    fed3.logLeftPoke();                                 //Log left poke
    fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights for 200ms)
    fed3.Event = "20HzStim";                            //Label the event "20HzStim" - Event is a text string and can be set to anything
    fed3.logdata();                                     //log data without incrementing left or right pokes
    fed3.pulseGenerator(10, 20, 20);                    //Deliver a traing of 10ms pulses at 20Hz for 1 sec
  }

  if (fed3.Right) {                                     //If right poke is triggered
    fed3.logRightPoke();                                //Log right poke
  }
}
