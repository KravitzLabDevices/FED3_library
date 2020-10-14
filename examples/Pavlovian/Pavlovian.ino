/*
  Feeding experimentation device 3 (FED3)
  Pavlovian Pellet Drop
  This script will delivery a conditioned stimulus, wait 5 seconds, and then drop a pellet
  After the pellet has been taken, it will wait 30 seconds and repeat from the beginning

  alexxai@wustl.edu
  August, 2020
*/

#include <FED3.h>                //Include the FED3 library 
String sketch = "Pavlov";        //Unique identifier text for each sketch      
FED3 fed3 (sketch);              //Start the FED3 object
Pellet_delay = 2;                //How long to wait between conditioned stimulus and pellet

void setup() {
  fed3.begin();                  //Setup the FED3 hardware
  fed3.FEDmode = 0;              //Customize the display options to FEDmode 0 for an free pellet session
  fed3.EnableSleep = false;      //Set to false to inhibit sleeping to use the Serial port; Set to true to reduce battery power
}

void loop() {
  fed3.run();                    //Call fed.run at least once per loop

  ////////////////////////////////////////////////////
  // Write your behavioral program below this line  //
  ////////////////////////////////////////////////////

  fed3.ConditionedStimulus();     //Play conditioned stimulus (light and tones)
  delay (Pellet_delay*1000);       
  fed3.Feed();                    //Drop pellet
  
  while (digitalRead (PELLET_WELL) == LOW) {  // Wait here while there's a pellet in the well
  }
  
  serialOutput();                 //Print data to Serial Monitor
  delay (30000);                  //Wait 30 seconds after pellet is removed before starting over again
}

////////////////////////////////////////////////////
// Display data via the serial monitor (click Tools > Serial Monitor)
////////////////////////////////////////////////////
void serialOutput() {
  Serial.println(" ");
  Serial.println ("Pavlovian Session");
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
