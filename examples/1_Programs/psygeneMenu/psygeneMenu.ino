/*
  Feeding experimentation device 3 (FED3)

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2025 Lex Kravitz

*/

#include <FED3.h>           //Include the FED3 library
String sketch = "PsyGene";  //Unique identifier text for each sketch
FED3 fed3(sketch);          //Start the FED3 object

//variables for PR tasks
int poke_num = 0;                  //number of pokes since last pellet
int pellets_in_current_block = 0;  //pellet number in current block
int pokes_required = 1;            //current FR
unsigned long poketime = 0;        //time of poke
int resetInterval = 1800;          //number of seconds without a poke to reset

void setup() {
  fed3.psygene = true;
  fed3.begin();  //Setup the FED3 hardware
}

void loop() {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 0: Bandit 100-0, Mode 2: Bandit 80-20
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if ((fed3.FEDmode == 0) or (fed3.FEDmode == 2)) {
    fed3.prob_left = 0;     // Initial reward probability of left poke
    fed3.prob_right = 100;  // Initial reward probability of right poke
    int probs[2] = { 100, 0 };

    if (fed3.FEDmode == 0) {
      fed3.sessiontype = "Bandit100";
      fed3.prob_left = 0;     // Initial reward probability of left poke
      fed3.prob_right = 100;  // Initial reward probability of right poke
    }

    else if (fed3.FEDmode == 2) {
      fed3.sessiontype = "Bandit80";
      fed3.prob_left = 20;   // Initial reward probability of left poke
      fed3.prob_right = 80;  // Initial reward probability of right poke
      probs[0] = 80;
      probs[1] = 20;
    }

    int pellet_counter = 0;     //pellet counter variable
    int timeoutIncorrect = 10;  //timeout duration in seconds, set to 0 to remove the timeout
    int new_prob = 0;
    String last_poke = "";
    int random_n = 0;
    fed3.countAllPokes = false;
    fed3.pelletsToSwitch = 20;      // Number of pellets required to finish the block and change reward probabilities
    fed3.allowBlockRepeat = false;  // Whether the same probabilities can be used for two blocks in a row
    randomSeed(12);
    fed3.writeHeader();

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
        new_prob = probs[random(0, 2)];
        if (!fed3.allowBlockRepeat) {
          while (new_prob == fed3.prob_left) {
            new_prob = probs[random(0, 2)];
          }
          fed3.prob_left = new_prob;
          fed3.prob_right = 100 - fed3.prob_left;
        } else {
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
        fed3.logLeftPoke();  //Log left poke
        random_n = random(100);
        if (random(100) < fed3.prob_left) {  //Select a random number between 0-100 and ask if it is between 0-80 (80% of the time).  If so:
          //fed3.BNC(50,3);
          fed3.ConditionedStimulus();  //Deliver conditioned stimulus (tone and lights)
          fed3.Feed();                 //Deliver pellet
          //fed3.BNC(50,6);
          pellet_counter++;  //Increase pellet counter by one
        } else {             //If random number is between 81-100 (20% of the time)
          //fed3.BNC(50,4);
          fed3.Tone(300, 600);  //Play the error tone
          fed3.Timeout(timeoutIncorrect, true, true);
        }
        last_poke = "Left";
      }

      // This is part 3. This is the behavior of the task after a right poke.
      // Notice that in this example the behavior after a right poke is exactly the
      // same as the the behvaior after a left poke.
      if (fed3.Right) {
        fed3.BlockPelletCount = pellet_counter;
        fed3.logRightPoke();                  //Log Right poke
        if (random(100) < fed3.prob_right) {  //Select a random number between 0-100 and ask if it is between 80-100 (20% of the time).  If so:
          fed3.ConditionedStimulus();         //Deliver conditioned stimulus (tone and lights)
          fed3.Feed();                        //Deliver pellet
          pellet_counter++;                   //Increase pellet counter by one
        } else {                              //If random number is between 0-80 (80% of the time)
          fed3.Tone(300, 600);                //Play the error tone
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
    fed3.sessiontype = "FR1";
    fed3.run(); 
    if (fed3.Left) {
      fed3.logLeftPoke();          //Log left poke
      fed3.ConditionedStimulus();  //deliver conditioned stimulus (tone and lights)
      fed3.Feed();                 //deliver pellet
    }
    if (fed3.Right) {  //If right poke is triggered
      fed3.logRightPoke();
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 3: Progressive Ratio
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode == 3) {
    fed3.sessiontype = "PR1";            //The text in "sessiontype" will appear on the screen and in the logfile
    fed3.run();                          //Call fed.run at least once per loop
    checkReset();                        //Check if it's time to reset to FR1
    if (fed3.Left) {                     //If left poke is triggered
      fed3.logLeftPoke();                //log Left poke
      poke_num++;                        //increment poke number.
      poketime = fed3.unixtime;          //update the current time of poke
      if (poke_num == pokes_required) {  //check if current FR has been achieved
        fed3.ConditionedStimulus();      //Deliver conditioned stimulus (tone and lights)
        pellets_in_current_block++;      //increment the pellet number by 1
        fed3.BlockPelletCount = pellets_in_current_block;
        fed3.Feed();               //Deliver pellet
        fed3.BNC(500, 1);          //Send 500ms pulse to the BNC output when pellet is detected (move this line to deliver this pulse elsewhere)
        pokes_required += 1;       //Edit this line to change the PR incremementing formula.  Default is for each pellet add 1 to the pokes required.
        fed3.FR = pokes_required;  //Update the FR requirement in the functions in the FED3 library
        poke_num = 0;              //reset poke_num to 0
      }
    }

    if (fed3.Right) {       //If right poke is triggered
      fed3.logRightPoke();  //log right poke
    }
  }
}

//////////////////////////////////////////////////////////////////////////////////////
//if more than 30 mins has passed since last poke -- reset the block and parameters
//////////////////////////////////////////////////////////////////////////////////////
void checkReset() {
  if (fed3.unixtime - poketime >= resetInterval) {  //if the reset interval has elapsed since last poke
    pellets_in_current_block = 0;
    fed3.BlockPelletCount = pellets_in_current_block;
    poke_num = 0;
    pokes_required = 1;
    fed3.FR = pokes_required;
    Serial.println("          ");
    Serial.println("****");  //print **** on the serial monitor

    fed3.pixelsOn(5, 5, 5, 5);
    delay(200);
    fed3.pixelsOff();
    poketime = fed3.unixtime;  //store the current time of poke
  }
}