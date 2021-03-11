/*
  Feeding experimentation device 3 (FED3)
  Probabalistic reversal task

  Starts with the left poke resulting in a pellet N% of the time and the right poke resulting in a pellet
  100-N% of the time, with N defined by the variable "probability".  For instance, if N = 80,
  a left poke delivers a pellet 80% of the time and a right poke causes a pellet 20%  of the time

  After a defined number of pokes the probabilities on the right and left pokes switch.  This is set
  by the variable "trialsToSwitch".

  Code written by alexxai@wustl.edu and meaghan.creed@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "Prob_Rev";                             //Unique identifier text for each sketch, change string only
FED3 fed3 (sketch);                                     //Start the FED3 object - don't change

int probability = 80;
int trialsToSwitch = 3;
int counter = 0;
int trialTimeout = 2;

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware, all pinmode screen etc, initialize SD card
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop

  //Determine which poke is active based on the trial number
  if (counter == (trialsToSwitch * 2) - 1) counter = 0;
  if (counter < trialsToSwitch) fed3.activePoke = 1;
  if (counter >= trialsToSwitch) fed3.activePoke = 0;

  ////////////////////////////////////////////////
  // There are 4 cases we need to account for:
  ////////////////////////////////////////////////

  // Case 1: The *left* poke is the high probability poke and animal pokes *left*
  if (fed3.activePoke == 1 and fed3.Left) {
    fed3.logLeftPoke();                                 //Log left poke
    if (random(100) < probability) {                    //Select a random number between 0-100 and ask if it is between 0-80 (80% of the time).  If so:
      fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
      fed3.Feed();                                        //Deliver pellet
    }
    else {                                              //If random number is between 81-100 (20% of the time)
      fed3.Tone(4000, 200);                               //Play the error tone
    }
    counter ++;                                         //Regardless of the probabalistic outcome increment the trial counter
    fed3.Timeout(trialTimeout);                         //Time-out between trials
  }

  // Case 2: The *left* poke is the high probability poke and animal pokes *right*
  if (fed3.activePoke == 1 and fed3.Right) {
    fed3.logRightPoke();                                //Log Right poke
    if (random(100) > probability) {                    //Select a random number between 0-100 and ask if it is between 80-100 (20% of the time).  If so:
      fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
      fed3.Feed();                                        //Deliver pellet
    }
    else {                                              //If random number is between 0-80 (80% of the time)
      fed3.Tone(4000, 200);                               //Play the error tone
    }
    counter ++;                                         //increment the trial counter
    fed3.Timeout(trialTimeout);                         //Time-out between trials
  }

  // Case 3: The *right* poke is the high probability poke and animal pokes *left*
  if (fed3.activePoke == 0 and fed3.Left) {
    fed3.logLeftPoke();                                 //Log left poke
    if (random(100) > probability) {                    //Select a random number between 0-100 and ask if it is between 80-100 (20% of the time).  If so:
      fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
      fed3.Feed();                                        //Deliver pellet
    }
    else {                                              //If random number is between 0-80 (80% of the time)
      fed3.Tone(4000, 200);                               //Play the error tone
    }
    counter ++;                                         //increment the trial counter
    fed3.Timeout(trialTimeout);                         //Time-out between trials
  }

  // Case 4: The *right* poke is the high probability poke and animal pokes *right*
  if (fed3.activePoke == 0 and fed3.Right) {
    fed3.logRightPoke();                                //Log Right poke
    if (random(100) < probability) {                    //Select a random number between 0-100 and ask if it is between 0-80 (80% of the time).  If so:
      fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
      fed3.Feed();                                        //Deliver pellet
    }
    else {                                              //If random number is between 80-100 (20% of the time)
      fed3.Tone(4000, 200);                               //Play the error tone
    }
    counter ++;                                         //increment the trial counter
    fed3.Timeout(trialTimeout);                         //Time-out between trials
  }
}
