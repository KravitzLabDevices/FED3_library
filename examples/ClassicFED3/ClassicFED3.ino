/*
  Feeding experimentation device 3 (FED3)
  Classic FED3 script
  This script mimicks the classic FED3 menuing system for selecting among the following programs

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
*/

#include <FED3.h>                5//Include the FED3 library 
String sketch = "Classic";       //Unique identifier text for each sketch
FED3 fed3 (sketch);              //Start the FED3 object

//variables for use in PR tasks
int poke_num = 0;                                      // this variable is the number of pokes since last pellet
int pokes_required = 1;                                //increase the number of pokes required each time a pellet is received using an exponential equation

void setup() {
  fed3.ClassicFED3 = true;
  fed3.begin();                  //Setup the FED3 hardware
}

void loop() {
  fed3.run();                    //Call fed.run at least once per loop

  ///////////////////////////////////////////////////////
  // Mode 0: Free feeding
  ///////////////////////////////////////////////////////
  if (fed3.FEDmode == 0) {
    fed3.DisplayPokes = false;   //Turn off poke indicators for free feeding mode
    fed3.Feed();
  }

  ///////////////////////////////////////////////////////
  // Modes 1-3: Fixed Ratio Programs FR1, FR3, FR5
  ///////////////////////////////////////////////////////
  if ((fed3.FEDmode == 1) or (fed3.FEDmode == 2) or (fed3.FEDmode == 3)) {
    if (fed3.Left) {
      fed3.logLeftPoke();                               //Log left poke
      if (fed3.LeftCount % fed3.FR == 0) {                 //if fixed ratio is  met
        fed3.ConditionedStimulus();                   //deliver conditioned stimulus (tone and lights)
        fed3.Feed();                                  //deliver pellet
      }
    }
  }
  if (fed3.Right) {                                     //If right poke is triggered
    fed3.logRightPoke();
  }

  ///////////////////////////////////////////////////////
  // Mode 4: Progressive Ratio
  ///////////////////////////////////////////////////////
  if (fed3.Left) {                                     //If left poke is triggered and pellet is not in the well
    fed3.logLeftPoke();                                //Log left poke
    poke_num++;                                        //store this new poke number as current poke number.
    if (poke_num == pokes_required) {                  //check to see if the mouse has acheived the correct number of pokes in order to receive the pellet
      fed3.ConditionedStimulus();                      //Deliver conditioned stimulus (tone and lights)
      fed3.Feed();                                     //Deliver pellet
      //increment the number of pokes required according to the progressive ratio:
      pokes_required = round((5 * exp((fed3.PelletCount + 1) * 0.2)) - 5);
      fed3.FR = pokes_required;
      poke_num = 0;                                    //reset the number of pokes back to 0, for the next trial
      fed3.Left = false;
    }
    else {
      fed3.Click();                                    //If not enough pokes, just do a Click
    }
  }
  if (fed3.Right) {                                    //If right poke is triggered and pellet is not in the well
    fed3.logRightPoke();
  }

  ///////////////////////////////////////////////////////
  // Mode 5: Extinction
  ///////////////////////////////////////////////////////
  if (fed3.FEDmode == 5) {
    if (fed3.Left) {
      fed3.logLeftPoke();                               //Log left poke
      fed3.ConditionedStimulus();                   //deliver conditioned stimulus (tone and lights)
    }

    if (fed3.Right) {                                     //If right poke is triggered
      fed3.logRightPoke();
    }
  }
 
}
