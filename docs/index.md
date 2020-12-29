## [FED3 library](https://github.com/KravitzLabDevices/FED3_library) documentation

This documentation site will cover the user functions available in the FED3 library, for programming your own FED3 programs.

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
