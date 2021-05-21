/*
  Feeding experimentation device 3 (FED3)
  Progressive Ratio Script.
  The number of pellets required increments by the forumla in Richardson and Roberts (1996) https://www.ncbi.nlm.nih.gov/pubmed/8794935

  alexxai@wustl.edu and kbarclay@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include <FED3.h>                                      //Include the FED3 library 
int poke_num = 0;                                      // this variable is the number of pokes since last pellet
int pokes_required = 1;                                //increase the number of pokes required each time a pellet is received using an exponential equation
String sketch = "ProgRat";                            //Unique identifier text for each sketch
FED3 fed3 (sketch);                                    //Start the FED3 object

void setup() {
  fed3.begin();                                        //Setup the FED3 hardware
}

void loop() {
  fed3.run();                                          //Call fed.run at least once per loop
  if (fed3.Left) {                                     //If left poke is triggered and pellet is not in the well
    fed3.logLeftPoke();                                //Log left poke
    poke_num++;                                        //store this new poke number as current poke number.
    if (poke_num == pokes_required) {                  //check to see if the mouse has acheived the correct number of pokes in order to receive the pellet
      fed3.ConditionedStimulus();                      //Deliver conditioned stimulus (tone and lights)
      fed3.Feed();                                     //Deliver pellet
      
      //increment the number of pokes required according to the progressive ratio:
      pokes_required = round((5 * exp((fed3.PelletCount + 1) * 0.2)) - 5);  //this is an exponential ratio, change this line to edit ratio formula
      
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
}
