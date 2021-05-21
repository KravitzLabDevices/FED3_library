/*
  Feeding experimentation device 3 (FED3)
  Progressive Poke Time

  This program progressively increases the required poke duration to trigger a pellet, 
  incrementing by a user-settable duration after a user-settable number of pellets has been obtained.

  
  alexxai@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "ProgPkTim";                            //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
  fed3.minPokeTime = 100;                               //Initial minimum duration poke need to be broken to count (in ms)?
}

//Set these variables
int pokesToIncrement = 3;                               //How many sucessful pokes before minPokeTime is incremented?
int increment = 100;                                    //How much to increment (in ms)?

void loop() {
  fed3.run();                                           //Call fed.run at the top of the void loop
  fed3.DisplayMinPoke();                                //Display minPokeTime on screen

  if (fed3.Left) {                                      //If left poke is triggered
    fed3.logLeftPoke();                                 //Log left poke
    if (fed3.leftInterval < fed3.minPokeTime) {
      fed3.Click();                                     //If poke is not held long enough play Click
    }
    else {
      fed3.ConditionedStimulus();                       //Deliver conditioned stimulus (tone and lights)
      fed3.Feed();                                      //Deliver pellet
      if (fed3.PelletCount % pokesToIncrement == 0) {   //If the correct number of pellets been dispensed...
        fed3.minPokeTime += increment;                  //Add to the poke duration
      }
    }
  }

  if (fed3.Right) {                                     //If right poke is triggered
    fed3.logRightPoke();                                //Log right poke
  }
}
