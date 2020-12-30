/*
  Feeding experimentation device 3 (FED3)
  Free Feeding mode

  alexxai@wustl.edu
  December, 2020
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "FreeFeed";                             //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
  fed3.DisplayPokes = false;                            //Customize the DisplayPokes option to 'false' to not display the poke indicators
  fed3.timeout = 10;                                    //Set a timeout period (in seconds) after each pellet is taken
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop
  fed3.Feed();                                          //Drop pellet
}
