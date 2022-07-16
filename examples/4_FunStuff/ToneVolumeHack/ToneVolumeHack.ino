/*
  This is a hack to get FED3 to change the volume of the FED3 tones.
  Unclear how useful this will be, the code is just to show it is possible.
  Lex Kravitz, July 2022
  */

#include <FED3.h>                                       //Include the FED3 library 
String sketch = "Tone_volume";                          //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
  pinMode(0, OUTPUT);                                   //Set the buzzer as an output
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop
  if (fed3.Left) {                                      //If left poke is triggered
    fed3.logLeftPoke();                                 //Log left poke
    for (int i = 0; i < 500; i++) {                     //play soft tone by calling a 0.1% duty cycle at 1kHz
      digitalWrite (0, HIGH);
      delayMicroseconds (1);
      digitalWrite (0, LOW);
      delayMicroseconds (999);
    }
  }
  if (fed3.Right) {                                     //If right poke is triggered
    fed3.logRightPoke();                                //Log right poke
    for (int i = 0; i < 500; i++){                      //play loud tone by calling a 50% duty cycle at 1kHz
      digitalWrite (0, HIGH);
      delayMicroseconds (500);
      digitalWrite (0, LOW);
      delayMicroseconds (500);
    }
  }
}
