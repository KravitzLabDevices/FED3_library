## [FED3 library](https://github.com/KravitzLabDevices/FED3_library) documentation

This documentation site will cover the user functions available in the FED3 library, for programming your own FED3 programs.

#Variables 
LeftCount: The total number of pokes on the left nosepoke, initializes to 0 
leftInterval: The duration of the last left nosepoke

#Poke functions
1. logLeftPoke();
This function causes FED3 to increment LeftCount and log the left poke and duration to the SD card.  
