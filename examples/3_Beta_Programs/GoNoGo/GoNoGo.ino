/*
  Feeding experimentation device 3 (FED3)
  Go No-go session.   When mouse pokes on Active poke, either a high or low frequency tone occurs.
  If it is a high tone the mouse must remove his nose within 500ms to get the pellet.
  If it is low tone the mouse must hold the poke for at least 1000ms before removing to get the pellet
  Incorrect responses are met with a time-out (10 seconds by default).
  This may require extensive training to get mice to do this well, possibly with incrementing difficulty.

  alexxai@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz
*/

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "GoNoGo";                               //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object
int highOrLow = random(0, 2);                           //Set a random variable called highOrLow. 0 means low tone, 1 means high tone
int gotime = 200;                                       //This is the duration in ms which the mouse must remove from the poke on a go trial to get pellet
int nogotime = 200;                                    //This is the duration in ms which the mouse must remain in the well on a nogo trial to get pellet

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop

  if (fed3.Left) {                                      //If left poke is triggered
    if (highOrLow == 1) {                               //If it is a high tone trial
      fed3.Tone(2500, 500);                             //Pley high tone
      fed3.logLeftPoke();                               //Log left poke
      if (fed3.leftInterval < gotime) {                 //If poke duration is less than the gotime 
        fed3.Feed();                                    //Feed
      }
      else {                                            //Otherwise
        fed3.Click(); delay(50); fed3.Click(); 
        delay(50); fed3.Click() ;delay(50);             //Play some clicks
        fed3.Timeout(10);                               //Timeout
      }
    }
    else if (highOrLow == 0) {                          //If it is a low tone trial
      fed3.Tone(50, 500);                               //Play low tone
      fed3.logLeftPoke();                               //Log left poke
      if (fed3.leftInterval > nogotime) {               //If poke duration is more than the nogo time
        fed3.Feed();                                    //Feed
      } 
      else {                                            //Otherwise
        fed3.Click(); delay(50); fed3.Click(); 
        delay(50); fed3.Click() ;delay(50);             //Play some clicks
        fed3.Timeout(10);                               //Timeout
      }
    }
    highOrLow = random(0, 2);                           //re-randomize highOrLow
  }

  if (fed3.Right) {                                     //If right poke is triggered
    fed3.logRightPoke();                                //Log right poke
  }
}
