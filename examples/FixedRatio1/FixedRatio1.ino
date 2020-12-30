/*
  Feeding experimentation device 3 (FED3)
  Fixed Ratio 1
  alexxai@wustl.edu
  December, 2020
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "FR1";                                  //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
}

void loop() {
  fed3.run();                                           //Call fed.run at the top of the void loop

  if (fed3.Left) {                                      //If left poke is triggered
    fed3.logLeftPoke();                                 //Log left poke
    fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
    fed3.Feed();                                        //Deliver pellet
  }

  if (fed3.Right) {                                     //If right poke is triggered
    fed3.logRightPoke();
  }
}
