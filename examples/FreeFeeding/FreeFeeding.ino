/*
  Feeding experimentation device 3 (FED3)
  Free Feeding
  
  alexxai@wustl.edu
  December, 2020
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "FreeFeed";                                  //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
  fed3.FEDmode = 0;                                     //Customize the display options to FEDmode 1 for an operant session
  fed3.EnableSleep = true;                             //Set to false to inhibit sleeping to use the Serial port; Set to true to reduce battery power
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop
  fed3.Feed();                                          //Drop pellet!
  serialOutput();
}

////////////////////////////////////////////////////
// Display data via the serial monitor (click Tools > Serial Monitor)
////////////////////////////////////////////////////
void serialOutput() {
  Serial.println(" ");
  Serial.println ("FreeFeeding");
  Serial.print ("Unixtime is: ");
  Serial.println(fed3.unixtime);
  Serial.println("Pellets");
  Serial.print("   ");
  Serial.println(fed3.PelletCount);
  Serial.println("**********************************");
}
