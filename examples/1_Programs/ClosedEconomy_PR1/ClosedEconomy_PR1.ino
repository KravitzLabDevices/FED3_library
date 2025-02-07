/*
  Feeding experimentation device 3 (FED3)

  This is a resetting progressive ratio task meant to be used in a closed economy setting as in Mourra et al., 2020: https://pubmed.ncbi.nlm.nih.gov/31809732/
  The task starts with left pokes active on an FR1, and incrememnts the FR +2 for each earned pellet. ie: FR1, FR3, FR5, etc.
  If 30 minutes pass without a left or right poke the FR ratio resets to FR1

  Code by: Kia Barclay and Lex Kravitz 
  alexxai@wustl.edu, kbarclay@wustl.edu
  January, 2021

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
*/

#include <FED3.h>                                      //Include the FED3 library 

int poke_num = 0;                                      //number of pokes since last pellet
int pellets_in_current_block = 0;                      //pellet number in current block
int pokes_required = 1;                                //current FR
unsigned long poketime = 0;                            //time of poke
int resetInterval = 1800;                              //number of seconds without a poke to reset

String sketch = "ClosedEcon_PR1";                      //Unique identifier text for each sketch - only the first 8 characters will show on the screen
FED3 fed3 (sketch);                                    //Start the FED3 object

void setup() {
  fed3.begin();                                        //Setup the FED3 hardware
  fed3.FEDmode = 1;                                    //Customize the display options to FEDmode 1 for an operant session
  fed3.EnableSleep = true;                             //Set to false to inhibit sleeping to use the Serial port; Set to true to reduce battery power
  fed3.FR = pokes_required;
}

void loop() {
  fed3.run();                                          //Call fed.run at least once per loop
  checkReset();                                        //Check if it's time to reset to FR1
  if (fed3.Left) {                                     //If left poke is triggered
    fed3.logLeftPoke();                                //log Left poke
    poke_num++;                                        //increment poke number.
    poketime = fed3.unixtime;                          //update the current time of poke
    serialoutput();                                    //print data to the Serial monitor - EnableSleep must be false to use Serial monitor
    if (poke_num == pokes_required) {                  //check if current FR has been achieved
      fed3.ConditionedStimulus();                      //Deliver conditioned stimulus (tone and lights)
      pellets_in_current_block++;                      //increment the pellet number by 1
      fed3.BlockPelletCount = pellets_in_current_block;
      fed3.Feed();                                     //Deliver pellet
      fed3.BNC(500, 1);                                //Send 500ms pulse to the BNC output when pellet is detected (move this line to deliver this pulse elsewhere)
      pokes_required += 1;                             //Edit this line to change the PR incremementing formula.  Default is for each pellet add 1 to the pokes required.
      fed3.FR = pokes_required;                        //Update the FR requirement in the functions in the FED3 library
      poke_num = 0;                                    //reset poke_num to 0
    }
  }

  if (fed3.Right) {                                    //If right poke is triggered
    fed3.logRightPoke();                               //log right poke
  }
}

//////////////////////////////////////////////////////////////////////////////////////
//if more than 30 mins has passed since last poke -- reset the block and parameters
//////////////////////////////////////////////////////////////////////////////////////
void checkReset() {
  if (fed3.unixtime - poketime >= resetInterval) {   //if the reset interval has elapsed since last poke
    pellets_in_current_block = 0;
    fed3.BlockPelletCount = pellets_in_current_block;
    poke_num = 0;
    pokes_required = 1;
    fed3.FR = pokes_required;
    Serial.println("          ");
    Serial.println("****");                           //print **** on the serial monitor

    fed3.pixelsOn(5, 5, 5, 5);
    delay(200);
    fed3.pixelsOff();
    poketime = fed3.unixtime;                        //store the current time of poke
  }
}

//////////////////////////////////////////////////////////////////////////////////////
// Use Serial.print statements for debugging
//////////////////////////////////////////////////////////////////////////////////////
void serialoutput() {
  Serial.print("Unixtime: ");
  Serial.println(fed3.unixtime);
  Serial.println("Pellets   RightPokes   LeftPokes   Poke_Num  Pel  Pokes_Required  PokeTime   Reset  FR");
  Serial.print("   ");
  Serial.print(fed3.PelletCount);
  Serial.print("          ");
  Serial.print(fed3.RightCount);
  Serial.print("          ");
  Serial.print(fed3.LeftCount);
  Serial.print("          ");
  Serial.print(poke_num);
  Serial.print("          ");
  Serial.print(pellets_in_current_block);
  Serial.print("          ");
  Serial.print(pokes_required);
  Serial.print("       ");
  Serial.print(poketime);
  Serial.print("          ");
  Serial.print(fed3.FR);
  Serial.println(" ");
  Serial.println(" ");
}
