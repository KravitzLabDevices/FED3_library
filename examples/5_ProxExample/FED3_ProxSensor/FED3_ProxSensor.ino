/*
  Feeding experimentation device 3 (FED3)
  Custom code to log VL6180x proximity sensor for mouse position.
  With each call to logdata() the code will also log the values from the prox sensor for 18s at 20Hz.
  alexxai@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "FR1_Prox";                             //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object
float minPokeTime = 200;                                //How long does poke need to be broken to count (in ms)?

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
 // fed3.disableSleep();
}

void loop() {
  fed3.run();                                           //Call fed.run at the top of the void loop

  if (fed3.Left) {                                      //If left poke is triggered
    unsigned long leftPokeTime = millis();
    while (digitalRead (6) == LOW) {}  //Hang here until poke is clear
    fed3.leftInterval = (millis() - leftPokeTime);
    if (fed3.leftInterval < (minPokeTime)) {
      fed3.Click();
      fed3.Left = false;
    }
    else {
      fed3.ConditionedStimulus();                       //Deliver conditioned stimulus (tone and lights)
      fed3.BNC(500, 1);            //open door
      fed3.logLeftPoke();                               //Log left poke
    }
  }

  if (fed3.Right) {                                      //If right poke is triggered
    unsigned long rightPokeTime = millis();
    while (digitalRead (5) == LOW) {}  //Hang here until poke is clear
    fed3.rightInterval = (millis() - rightPokeTime);
    if (fed3.rightInterval < (minPokeTime)) {
      fed3.Click();
      fed3.Right = false;
     }
    else {
      fed3.logRightPoke();
    }
  }
}
