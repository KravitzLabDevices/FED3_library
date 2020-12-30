/*
  Feeding experimentation device 3 (FED3)
  Customizable Fixed Ratio Example

  Lex Kravitz
  alexxai@wustl.edu
  December, 2020
*/

////////////////////////////////////////////////////
// Customize the FR number and which poke is active
////////////////////////////////////////////////////
int FR = 5;
bool LeftActive = true;                               //Set to false to make right poke active

////////////////////////////////////////////////////
// Start FED3 library and make the fed3 object
////////////////////////////////////////////////////
#include <FED3.h>                                     //Include the FED3 library 
String sketch = "FRCustom";                           //Unique identifier text for each sketch
FED3 fed3 (sketch);                                   //Start the FED3 object

void setup() {
  fed3.begin();                                       //Setup the FED3 hardware
  fed3.FR = FR;                                       //Share the FR ratio with the fed3 library so it is logged on the SD card and displayed on the screen
  if (LeftActive == false) {
    fed3.activePoke = 0;                              //update the activepoke variable in the FED3 library for logging and display. This defaults to 1, so only set to 0 if LeftActive == false
  }
}
void loop() {
  fed3.run();                                         //Call fed.run at least once per loop

  // If Left poke is triggered
  if (fed3.Left) {
    fed3.logLeftPoke();                               //Log left poke
    if (LeftActive == true) {
      if (fed3.LeftCount % FR == 0) {                 //if fixed ratio is  met
        fed3.ConditionedStimulus();                   //deliver conditioned stimulus (tone and lights)
        fed3.Feed();                                  //deliver pellet
      }
      else {                                          //if fixed ratio is not met
        fed3.Click();                                 //click stimulus
      }
    }
  }

  // If Right poke is triggered
  if (fed3.Right) {
    fed3.logRightPoke();                              //Log Right poke
    if (LeftActive == false) {
      if (fed3.RightCount % FR == 0) {                 //if fixed ratio is  met
        fed3.ConditionedStimulus();                   //deliver conditioned stimulus (tone and lights)
        fed3.Feed();                                  //deliver pellet
      }
      else {                                          //if fixed ratio is not met
        fed3.Click();                                 //click stimulus
      }
    }
  }
}
