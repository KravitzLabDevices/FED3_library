## [FED3 library](https://github.com/KravitzLabDevices/FED3_library) documentation

This documentation site will cover the user variables and functions available for manipulation in the FED3 library

### Variables 
- **LeftCount**: Int, total number of pokes on the left nosepoke, initializes to 0 
- **leftInterval**: Int, duration of the last left nosepoke in ms
- **RightCount**: Int, number of pokes on the right nosepoke, initializes to 0 
- **rightInterval**: Int, duration of the last riht nosepoke in ms
- **PelletCount**: Int, total number of pellets dispensed, initializes to 0
- **retInterval**: Int, how long the pellet remained in the well before it was taken in ms. Maximum value is 60000.
- **Event**: String, variable containing which type of event triggered the datalogging, options are Left, Right, or Pellet

### Poke functions
- **logLeftPoke()**: Causes FED3 to increment **LeftCount** and log the left poke and duration to the SD card
- **logRightPoke()**: Causes FED3 to increment **RightCount** and log the right poke and duration to the SD card

### Feeding functions
- **Feed()**: Causes FED3 to drop a pellet. FED3 will continue attempting to drop pellet until it is detected in the pellet well, and will initiate jam clearing operations automatically if it fails to dispense on the following schedule: Every 5th attempt (**MinorJam**); Every 10th attempt (**VibrateJam**); Every 10th attempt (**ClearJam**). Once it detects a pellet it increments **PelletCount** and waits for up to 60 seconds for the pellet to be removed so it can calculate **retInterval**.  
- **MinorJam()**: Causes FED3 pellet disk to make a small backwards movement 
- **VibrateJam()**: Causes FED3 pellet disk to make a vibrating movement for ~10 seconds, stopping this movement if a pellet is detected
- **ClearJam()**: Causes FED3 pellet disk to make a full rotation backwards and forwards, stopping this movement if a pellet is detected

### Audio and Neopixel functions
- **pixelsOff()**: Turn all Neopixels off
- **ConditionedStimulus()**: Turn all pixels on a green/blue color and a 4000Hz tone for 200ms
- **Click()**: A short "click" (800Hz tone for 8ms)
- **Noise()**: A ~500ms random noise stimulus


## Examples
### Here is an FR1 session where a left poke delivers the conditioned stimulus and a pellet, and the right poke does nothing 

```
#include <FED3.h>                                       //Include the FED3 library 
String sketch = "FR1";                                  //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
  fed3.FEDmode = 1;                                     //Customize the display options to FEDmode 1 for an operant session, FED 0 for a non-operant
  fed3.EnableSleep = true;                              //Set to false to inhibit sleeping to use the Serial port; Set to true to reduce battery power
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop

  if (fed3.Left) {                                      //If left poke is triggered
    fed3.logLeftPoke();                                 //Log left poke
    fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
    fed3.Feed();                                        //Deliver pellet
  }
  
  if (fed3.Right) {                                     //If right poke is triggered
    fed3.logRightPoke();                                //Log Right poke
  }
}
```
