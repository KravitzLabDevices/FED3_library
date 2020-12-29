/*
  Feeding experimentation device 3 (FED3)
  Fixed Ratio 1

  alexxai@wustl.edu
  October, 2020
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "FR1";                                  //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
  fed3.FEDmode = 1;                                     //Customize the display options to FEDmode 1 for an operant session, FED 0 for a non-operant
  fed3.EnableSleep = true;                              //Set to false to inhibit sleeping to use the Serial port; Set to true to reduce battery power
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop

  ////////////////////////////////////////////////////
  // Write your behavioral program below this line  //
  ////////////////////////////////////////////////////

  if (fed3.Left) {                                      //If left poke is triggered
    fed3.logLeftPoke();                                 //Log left poke
    fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
    fed3.Feed();                                        //Deliver pellet
    serialOutput();                                     //Print data to Serial Monitor
  }
  if (fed3.Right) {                                     //If right poke is triggered
    fed3.logRightPoke();                                //Log right poke
  }
}

////////////////////////////////////////////////////
// Display data via the serial monitor (click Tools > Serial Monitor)
////////////////////////////////////////////////////
void serialOutput() {
  Serial.println(" ");
  Serial.println ("Fixed Ratio 1");
  Serial.print ("Unixtime is: ");
  Serial.println(fed3.unixtime);
  Serial.println("Pellets   RightPokes   LeftPokes");
  Serial.print("   ");
  Serial.print(fed3.PelletCount);
  Serial.print("          ");
  Serial.print(fed3.RightCount);
  Serial.print("          ");
  Serial.println(fed3.LeftCount);
  Serial.println(" ");
  Serial.println("**********************************");
}
