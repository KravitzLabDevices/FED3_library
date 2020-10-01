/*
  Feeding experimentation device 3 (FED3)
  Customizable Fixed Ratio Example

  Lex Kravitz
  alexxai@wustl.edu
  October, 2020
*/

////////////////////////////////////////////////////
// Customize the fixed ratio session
////////////////////////////////////////////////////
int FR = 5;
bool LeftActive = true;                                 //Set to false to make right poke active

////////////////////////////////////////////////////
// Start FED3 library and make the fed3 object
////////////////////////////////////////////////////
#include <FED3.h>                                       //Include the FED3 library 
int sketch = 3;                                         //Name the sketch with a unique integer
FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
  fed3.FEDmode = 1;                                     //Customize the display options to FEDmode 1 for an operant session
  fed3.FR = FR;                                         //Share the FR ratio with the fed3 library so it is logged on the SD card and displayed on the screen
  fed3.EnableSleep = false;                             //Set to false to inhibit sleeping to use the Serial port; Set to true to reduce battery power
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop

  ////////////////////////////////////////////////////
  // Write your behavioral program below this line
  ////////////////////////////////////////////////////


  ////////////////////////////////////////////////////
  // If Left poke is active
  ////////////////////////////////////////////////////
  if (LeftActive) {
    fed3.activePoke = 1;                                  //update activepoke variable in the FED3 library for accurate logging and display
    //if left poke is triggered, leftpoke is ready, and pellet is not in the well
    if (fed3.Left and fed3.LeftReady and fed3.PelletAvailable == false) {
      fed3.CheckPokes();                                  //run CheckPokes to log the pokes
      if (fed3.LeftCount % FR != 0) {                     //if fixed ratio is not met
        fed3.Click();                                     //click stimulus
      }

      else {                                              //if fixed ratio is met
        fed3.ConditionedStimulus();                       //deliver conditioned stimulus (tone and lights)
        fed3.Feed();                                      //deliver pellet
      }
      serialOutput();
    }
  }

  ////////////////////////////////////////////////////
  // If Right poke is active
  ////////////////////////////////////////////////////
  else {
    fed3.activePoke = 0;
    //if right poke is triggered, rightpoke is ready, and pellet is not in the well
    if (fed3.Right and fed3.RightReady and fed3.PelletAvailable == false) {
      fed3.CheckPokes();                                  //run CheckPokes to log the pokes
      if (fed3.RightCount % FR != 0) {                    //if fixed ratio is not met
        fed3.Click();                                     //click stimulus
      }

      else {                                              //if fixed ratio is met
        fed3.ConditionedStimulus();                       //deliver conditioned stimulus (tone and lights)
        fed3.Feed();                                      //deliver pellet
      }
      serialOutput();
    }
  }
}

////////////////////////////////////////////////////
// Display data via the serial monitor (click Tools > Serial Monitor)
////////////////////////////////////////////////////
void serialOutput() {
  Serial.println(" ");
  Serial.print ("Fixed Ratio is: ");
  Serial.println(FR);
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
