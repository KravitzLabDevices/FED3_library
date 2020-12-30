/*
  Feeding experimentation device 3 (FED3)
  Closed economy Progressive Ratio (this is a progressive ratio program that resets if no pokes are made within a set time)

  alexxai@wustl.edu and kbarclay@wustl.edu
  December, 2020
*/

#include <FED3.h>                                      //Include the FED3 library 
int poke_num = 0;                                      // this variable is the number of pokes since last pellet
int pellets_in_current_block = 0;                      // variable stores pellets number that the mouse is trying to receive. 1 is the first pellet. 2 is the second pellet
int pokes_required = 1;                                //increase the number of pokes required each time a pellet is received using an exponential equation
unsigned long poketime = 0;                            //time of poke
int resetInterval = 1800;                              //number of seconds without a poke until reset

String sketch = "ClsdEcon";                            //Unique identifier text for each sketch
FED3 fed3 (sketch);                                    //Start the FED3 object

void setup() {
  fed3.begin();                                        //Setup the FED3 hardware
}

void loop() {
  fed3.run();                                          //Call fed.run at least once per loop
  ////////////////////////////////////////////////////
  // Write your behavioral program below this line  //
  ////////////////////////////////////////////////////

  reset();

  if (fed3.Left) {                                     //If left poke is triggered and pellet is not in the well
    fed3.logLeftPoke();                                //Log left poke
    poke_num++;                                        //store this new poke number as current poke number.
    poketime = fed3.unixtime;                          //store the current time of poke
    serialoutput();                                    //print this on the serial monitor -- if you want
    if (poke_num == pokes_required) {                  //check to see if the mouse has acheived the correct number of pokes in order to receive the pellet
      fed3.ConditionedStimulus();                      //Deliver conditioned stimulus (tone and lights)
      pellets_in_current_block++;                      //increase the pellet number by 1 after the mouse receives the pellet
      fed3.BlockPelletCount = pellets_in_current_block;
      fed3.Feed();                                     //Deliver pellet
      
      //increment the number of pokes required according to the progressive ratio:
      pokes_required = round((5 * exp((pellets_in_current_block + 1) * 0.2)) - 5);      
      
      fed3.FR = pokes_required;
      poke_num = 0;                                    //reset the number of pokes back to 0, for the next trial
      fed3.Left = false;
    }
  }

  if (fed3.Right) {                                    //If right poke is triggered and pellet is not in the well
    fed3.logRightPoke();
  }
}

//This extra function controls when to reset the PR ratio if enough time has elapsed without a poke
void reset() {
  if (fed3.unixtime - poketime >= resetInterval) {     //if more than 30 mins has passed since last poke -- reset the block and parameters
    pellets_in_current_block = 0;
    fed3.BlockPelletCount = pellets_in_current_block;
    poke_num = 0;
    pokes_required = 1;
    fed3.FR = pokes_required;
    Serial.println("          ");
    Serial.println("****");                            //if more than 30 mins has passed since last poke --print **** on the serial monitor

    fed3.pixelsOn((5, 5, 5));
    delay(200);
    fed3.pixelsOff();
    poketime = fed3.unixtime;                          //store the current time of poke
  }
}

//This function activates the serial monitor.  Enable_Sleep must be set to false for this to work.
void serialoutput() {
  ////////////////////////////////////////////////////
  // Use Serial.print statements for debugging
  ////////////////////////////////////////////////////
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
