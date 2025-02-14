/*
  Feeding experimentation device 3 (FED3)
  Classic FED3 script
  This script mimics the classic FED3 menuing system for selecting among the following programs

  // FEDmodes:
  // 0 Free feeding
  // 1 FR1
  // 2 FR3
  // 3 FR5
  // 4 Progressive Ratio
  // 5 Extinction
  // 6 Light tracking FR1 task
  // 7 FR1 (reversed)
  // 8 PR (reversed)
  // 9 Optogenetic stimulation
  // 10 Optogenetic stimulation (reversed)
  // 11 Timed free feeding

  alexxai@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz

*/

#include <FED3.h>                //Include the FED3 library 
String sketch = "Classic";       //Unique identifier text for each sketch
FED3 fed3 (sketch);              //Start the FED3 object

//variables for PR tasks
int poke_num = 0;                                      // this variable is the number of pokes since last pellet
int pokes_required = 1;                                // increase the number of pokes required each time a pellet is received using an exponential equation

void setup() {
  fed3.psygene = true;
  fed3.begin();                                        //Setup the FED3 hardware
}

void loop() {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 0: Bandit
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 0) {
    int pellet_counter = 0;                               //pellet counter variable
    int timeoutIncorrect = 10;                            //timeout duration in seconds, set to 0 to remove the timeout
    int probs[2] = {100, 0};                               //Reward probability options
    int new_prob = 0;                
    String last_poke = "";
    int random_n = 0;
    fed3.sessiontype = "Bandit";                     //The text in "sessiontype" will appear on the screen and in the logfile
    fed3.countAllPokes = false;
    fed3.pelletsToSwitch = 20;                          // Number of pellets required to finish the block and change reward probabilities
    fed3.prob_left = 0;                                // Initial reward probability of left poke
    fed3.prob_right = 100;                               // Initial reward probability of right poke
    fed3.allowBlockRepeat = false;                      // Whether the same probabilities can be used for two blocks in a row
    fed3.begin();                                       // Setup the FED3 hardware, all pinmode screen etc, initialize SD card
    randomSeed(12);

    while (true) {
      //////////////////////////////////////////////////////////////////////////////////
      //  This is the main bandit task. In general it will be composed of three parts:
      //  1. Set up conditions to trigger a change in reward probabilities
      //  2. Set up behavior upon a left poke
      //  3. Set up behavior upon a right poke
      //////////////////////////////////////////////////////////////////////////////////
      
      fed3.run();
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
        //fed3.BNC(50,1);    
        fed3.BlockPelletCount = pellet_counter;
        fed3.logLeftPoke();                                   //Log left poke
        random_n = random(100);
        if (random(100) < fed3.prob_left) {                        //Select a random number between 0-100 and ask if it is between 0-80 (80% of the time).  If so:
          //fed3.BNC(50,3);
          fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
          fed3.Feed();                                        //Deliver pellet
          //fed3.BNC(50,6);
          pellet_counter ++;                                  //Increase pellet counter by one
        }
        else {                                                //If random number is between 81-100 (20% of the time)
          //fed3.BNC(50,4);
          fed3.Tone(300, 600);                               //Play the error tone
          fed3.Timeout(timeoutIncorrect, true, true);
        } 
        last_poke = "Left";
      }

      // This is part 3. This is the behavior of the task after a right poke.
      // Notice that in this example the behavior after a right poke is exactly the
      // same as the the behvaior after a left poke.
      if (fed3.Right) {
        //fed3.BNC(50,2);
        fed3.BlockPelletCount = pellet_counter;
        fed3.logRightPoke();                                  //Log Right poke
        if (random(100) < fed3.prob_right) {                       //Select a random number between 0-100 and ask if it is between 80-100 (20% of the time).  If so:
          fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
          fed3.Feed();                                        //Deliver pellet
          pellet_counter ++;                                  //Increase pellet counter by one
        }
        else {                                                //If random number is between 0-80 (80% of the time)
          fed3.Tone(300, 600);                               //Play the error tone
          fed3.Timeout(timeoutIncorrect, true, true);
        }
        last_poke = "Right";
      }
    }

 }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 1: FR1
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (fed3.FEDmode == 1) {
    if (fed3.Left) {
      fed3.logLeftPoke();                               //Log left poke
      fed3.ConditionedStimulus();                     //deliver conditioned stimulus (tone and lights)
      fed3.Feed();                                    //deliver pellet
    }
    if (fed3.Right) {                                    //If right poke is triggered
      fed3.logRightPoke();
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 2: Progressive Ratio
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 2) {
    fed3.sessiontype = "ProgRatio";                      //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.Left) {                                     //If left poke is triggered and pellet is not in the well
      fed3.logLeftPoke();                                //Log left poke
      fed3.Click();                                      //Click
      poke_num++;                                        //store this new poke number as current poke number.
      if (poke_num == pokes_required) {                  //check to see if the mouse has acheived the correct number of pokes in order to receive the pellet
        fed3.ConditionedStimulus();                      //Deliver conditioned stimulus (tone and lights)
        fed3.Feed();                                     //Deliver pellet
        pokes_required = round((5 * exp((fed3.PelletCount + 1) * 0.2)) - 5);  //increment the number of pokes required according to the progressive ratio:
        fed3.FR = pokes_required;
        poke_num = 0;                                    //reset poke_num back to 0 for the next trial
      }
    }
    if (fed3.Right) {                                    //If right poke is triggered and pellet is not in the well
      fed3.logRightPoke();
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 3: Extinction
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 3) {
    fed3.sessiontype = "Extinct";                        //The text in "sessiontype" will appear on the screen and in the logfile
    if (fed3.Left) {
      fed3.logLeftPoke();                                //Log left poke
      fed3.ConditionedStimulus();                        //deliver conditioned stimulus (tone and lights)
    }

    if (fed3.Right) {                                    //If right poke is triggered
      fed3.logRightPoke();
    }
  }

  fed3.run();
}
