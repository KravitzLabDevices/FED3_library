## [FED3 library](https://github.com/KravitzLabDevices/FED3_library) documentation

This documentation site will cover the user functions available in the FED3 library, for programming your own FED3 programs.

### Variables 
- LeftCount: The total number of pokes on the left nosepoke, initializes to 0 
- leftInterval: The duration of the last left nosepoke in ms
- RightCount: The total number of pokes on the right nosepoke, initializes to 0 
- rightInterval: The duration of the last riht nosepoke in ms
- PelletCount: The total number of pellets dispensed, initializes to 0
- retInterval: How long the pellet remained in the well before it was taken in ms. Maximum value is 60000.
- pellet: Event 

### Poke functions
- logLeftPoke(): Causes FED3 to increment LeftCount and log the left poke and duration to the SD card
- logRightPoke(): Causes FED3 to increment RightCount and log the right poke and duration to the SD card

### Feeding functions
- Feed(): Causes FED3 to drop a pellet. FED3 will continue attempting to drop pellet until it is detected in the pellet well, at which point it increments PelletCount and waits for up to 60 seconds for the pellet to be removed.  

### Audio and Neopixel functions
- pixelsOff(): Turn all Neopixels off
