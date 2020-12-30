Check out the [FED3 Wiki](https://github.com/KravitzLabDevices/FED3_library/wiki) for complete documentation and examples, quick overview below:

### Anatomy of a FED3 script. 
This is an annotation of the [Fixed-Ratio 1 Example](https://github.com/KravitzLabDevices/FED3_library/blob/main/docs/index.md#examples) at the bottom of this page to describe how to write a new FED3 script. Start with this small block of code to import the FED3 library, name your sketch (here it is "FR1"), and start the FED3 object.  The only thing you'll edit here is the text "FR1", which should be set to a unique String of text to identify your sketch.  This text will show up in the logfile in the column "LibaryVersion_Sketch". 
```c
#include <FED3.h>                                       //Include the FED3 library 
String sketch = "FR1";                                  //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object
```

In "void setup()" the function fed3.begin() initializes all of the FED3 hardware and gets it ready to use. Don't edit this part.
```c
void setup() {
  fed3.begin();                                         //FED3 hardware setup commands
}
```

Finally, "void loop()" is where you will write your behavioral program.  Make sure to include the function fed3.run() at the top of the loop.  fed3.run() updates the display and handles processor sleep modes. Define what happens when the mouse pokes left (fed3.Left), or right (fed3.Right) under the if statements for each.  Variables and functions that FED3 can use to make programs are listed below. 

```c
void loop() {
  fed3.run();                                           //Call fed.run at least once per loop

  if (fed3.Left) {                                      //If left poke is triggered
    fed3.logLeftPoke();                                 //Log left poke
    fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
    fed3.Feed();                                        //Deliver pellet
  }
  
  if (fed3.Right) {                                     //If right poke is triggered
    fed3.logRightPoke();                                //Log right poke
  }
}
```

### Variables.  These can be set or recalled in your sketch with 'fed3.Variable'
- **LeftCount**: Int, total number of pokes on the left nosepoke, initializes to 0 
- **leftInterval**: Int, duration of the last left nosepoke in ms
- **RightCount**: Int, number of pokes on the right nosepoke, initializes to 0 
- **rightInterval**: Int, duration of the last riht nosepoke in ms
- **PelletCount**: Int, total number of pellets dispensed, initializes to 0
- **retInterval**: Int, how long the pellet remained in the well before it was taken in ms. Times out at 60000ms
- **Event**: String, variable containing which type of event triggered the datalogging, options are Left, Right, or Pellet
- **EnableSleep**: Boolean, defaults to "true". Set to "false" to disable sleep functionality. This will drain the battery ~5x faster but can be useful when troubleshooting new programs.

---
  
### Functions. These can be called in your sketch with 'fed3.Function()'
> Poke functions.  
- **logLeftPoke()**: Causes FED3 to increment **LeftCount** and log the left poke and duration to the SD card
- **logRightPoke()**: Causes FED3 to increment **RightCount** and log the right poke and duration to the SD card

> Feeding functions
- **Feed()**: Causes FED3 to drop a pellet. FED3 will continue attempting to drop pellet until it is detected in the pellet well, and will initiate jam clearing operations automatically if it fails to dispense on the following schedule: Every 5th attempt (**MinorJam**); Every 10th attempt (**VibrateJam**); Every 10th attempt (**ClearJam**). Once it detects a pellet it increments **PelletCount** and waits for up to 60 seconds for the pellet to be removed so it can calculate **retInterval**.  
- **MinorJam()**: Causes FED3 pellet disk to make a small backwards movement 
- **VibrateJam()**: Causes FED3 pellet disk to make a vibrating movement for ~10 seconds, stopping this movement if a pellet is detected
- **ClearJam()**: Causes FED3 pellet disk to make a full rotation backwards and forwards, stopping this movement if a pellet is detected
- **Timeout(seconds)**: Starts a timeout period, length controlled by **seconds**.  Duration of timeout counts down on FED3 screen. 
```c
Example: Timeout(10) will make FED unresponsive for 10 seconds when called.
```

> Audio and Neopixel functions
- **pixelsOff()**: Turn all Neopixels off
- **ConditionedStimulus()**: Turn all pixels on a green/blue color and a 4000Hz tone for 200ms
- **Click()**: Short "click" (800Hz tone for 8ms)
- **Noise()**: ~500ms random noise stimulus

> FED3 output port
- **BNC(delay, number)**: Send **number** of pulses of length **delay** from the BNC output port. 
```c
Example: BNC(20, 5) will send five 20ms pulses from the port. 
```

> Display functions
- **UpdateDisplay()**: Update all values on FED3 display

> SDcard logging functions
- **logdata()**: Log current data to the SD card. This will print one line to the data file containing the following fields:


``` 
MM:DD:YYYY hh:mm:ss, LibaryVersion_Sketch, Device_Number, Battery_Voltage, Motor_Turns, Trial_Info, FR, Event,
Active_Poke, Left_Poke_Count, Right_Poke_Count, Pellet_Count, Block_Pellet_Count, Retrieval_Time, Poke_Time
```

---

### Examples
> Fixed-Ratio 1: The left poke is logged, results in a conditioned stimulus and a pellet, while the right poke is logged but has no other consequence.

```c
#include <FED3.h>                                       //Include the FED3 library 
String sketch = "FR1";                                  //Unique identifier text for each sketch
FED3 fed3 (sketch);                                     //Start the FED3 object

void setup() {
  fed3.begin();                                         //Setup the FED3 hardware
}

void loop() {
  fed3.run();                                           //Call fed.run at least once per loop

  if (fed3.Left) {                                      //If left poke is triggered
    fed3.logLeftPoke();                                 //Log left poke
    fed3.ConditionedStimulus();                         //Deliver conditioned stimulus (tone and lights)
    fed3.Feed();                                        //Deliver pellet
  }

  if (fed3.Right) {                                     //If right poke is triggered
    fed3.logRightPoke();
  }
}
```
