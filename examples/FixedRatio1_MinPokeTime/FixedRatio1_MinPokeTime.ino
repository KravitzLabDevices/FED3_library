/*
  Feeding experimentation device 3 (FED3)
  Fixed Ratio 1 with a minimum poke time
  alexxai@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "FR1_Min";                              //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object
float minPokeTime = 1000;                               //How long does poke need to be broken to count (in ms)?

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
}

void loop() {
  fed3.run();                                           //Call fed.run at the top of the void loop

  if (fed3.Left) {                                      //If left poke is triggered
    fed3.logLeftPoke();                                 //Log left poke
    if (fed3.leftInterval < (minPokeTime)) {
      fed3.Click();
    }
    else {
      fed3.ConditionedStimulus();                       //Deliver conditioned stimulus (tone and lights)
      fed3.Feed();                                      //Deliver pellet
    }
  }

  if (fed3.Right) {                                     //If right poke is triggered
    fed3.logRightPoke();
  }
}
