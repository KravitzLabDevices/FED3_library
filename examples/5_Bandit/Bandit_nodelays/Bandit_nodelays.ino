/*
  Feeding experimentation device 3 (FED3)
  Bandit task

  This example shows a simple 2-armed bandit task. Here, the reward probabilities of left and right 
  always add 100, and change simultaneously. Thus, this is a special case of the 2-armed bandit task 
  that is equivalent to a probabilistic reversal task. 

  Code written by alegariamacal@wustl.edu
  Feb, 2024

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include <FED3.h>                                      //Include the FED3 library 
String sketch = "Bandit";                             //Unique identifier text for each sketch, change string only. 
FED3 fed3 (sketch);                                   //Start the FED3 object - don't change

int pellet_counter = 0;                               //pellet counter variable
int timeoutIncorrect = 10;                            //timeout duration in seconds, set to 0 to remove the timeout
int probs[2] = {100,0};                               //Reward probability options
int new_prob = 0;                
String last_poke = "";
int random_n = 0;

void setup() {
  fed3.countAllPokes = false;
  fed3.pelletsToSwitch = 30;                          // Number of pellets required to finish the block and change reward probabilities
  fed3.prob_left = 100;                                // Initial reward probability of left poke
  fed3.prob_right =  0;                               // Initial reward probability of right poke
  fed3.allowBlockRepeat = false;                      // Whether the same probabilities can be used for two blocks in a row
  fed3.begin();                                       // Setup the FED3 hardware, all pinmode screen etc, initialize SD card
  randomSeed(12);
}

void loop() {
  //////////////////////////////////////////////////////////////////////////////////
  //  This is the main bandit task. In general it will be composed of three parts:
  //  1. Set up conditions to trigger a change in reward probabilities
  //  2. Set up behavior upon a left poke
  //  3. Set up behavior upon a right poke
  //////////////////////////////////////////////////////////////////////////////////
  fed3.run();                                           //Call fed.run at least once per loop

  // This is part 1. In this example, reward probabilities will be switched when 30 rewards
  // (value of fed3.pelletsToswitch assigned in line 35) are obtained. Notice that in this example
  // the reward probability of left + reward probability of right always add to 100. Additionally
  // in this example, the reward probabilities in the new block are not allowed to be the same to
  // the reward probability of the previous block.
  if (pellet_counter == fed3.pelletsToSwitch) {
    pellet_counter = 0;
    new_prob = probs[random(0,2)];
    if (! fed3.allowBlockRepeat) {
      while (new_prob == fed3.prob_left) {
        new_prob = probs[random(0,2)];
      }
      fed3.prob_left = new_prob;
      fed3.prob_right = 100 - fed3.prob_left;
    }
    else {
      fed3.prob_left = new_prob;
      fed3.prob_right = 100 - fed3.prob_left;
    }
  }
  
  // This is part 2. This is the behavior of the task after a left poke.
  // Notice that in this example pellet_counter only increases if a pellet
  // was actually delivered (fed.Feed() is called). Additionally, the timeout
  // resets if the mouse pokes during timeout, and also white noise is present
  // through the whole timeout
  if (fed3.Left) {
    fed3.BlockPelletCount = pellet_counter;
    fed3.logLeftPoke();                                   //Log left poke
    random_n = random(100);
    if (random(100) < fed3.prob_left) {                        //Select a random number between 0-100 and ask if it is between 0-80 (80% of the time).  If so:
      fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
      fed3.Feed();                                        //Deliver pellet
      pellet_counter ++;                                  //Increase pellet counter by one
    }
    else {                                                //If random number is between 81-100 (20% of the time)
      fed3.Timeout(timeoutIncorrect, true, true);
    } 
    last_poke = "Left";
  }

  // This is part 3. This is the behavior of the task after a right poke.
  // Notice that in this example the behavior after a right poke is exactly the
  // same as the the behvaior after a left poke.
  if (fed3.Right) {
    fed3.BlockPelletCount = pellet_counter;
    fed3.logRightPoke();                                  //Log Right poke
    if (random(100) < fed3.prob_right) {                       //Select a random number between 0-100 and ask if it is between 80-100 (20% of the time).  If so:
      fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
      fed3.Feed();                                        //Deliver pellet
      pellet_counter ++;                                  //Increase pellet counter by one
    }
    else {                                                //If random number is between 0-80 (80% of the time)
      fed3.Timeout(timeoutIncorrect, true, true);
    }
    last_poke = "Right";
  }
}
