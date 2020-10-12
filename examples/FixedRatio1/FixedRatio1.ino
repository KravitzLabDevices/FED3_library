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
  fed3.FEDmode = 1;                                     //Customize the display options to FEDmode 1 for an operant session
  fed3.EnableSleep = false;                             //Set to false to inhibit sleeping to use the Serial port; Set to true to reduce battery power
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop

  ////////////////////////////////////////////////////
  // Write your behavioral program below this line  //
  ////////////////////////////////////////////////////
  if (fed3.Left and fed3.PelletAvailable == false) {    //If left poke is triggered and pellet is not in the well
    fed3.CheckPokes();                                  //Run CheckPokes to log the pokes
    fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
    serialOutput();                                     //Print data to Serial Monitor
    fed3.Feed();                                        //Deliver pellet
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
