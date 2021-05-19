## This is the FED3 library (beta)
The goal of this library is to abstract most of the housekeeping functions of [FED3](https://github.com/KravitzLabDevices/FED3) smart pellet dispensing device behind the scenes and let the user focus on writing the behavioral control. See the [Wiki](https://github.com/KravitzLabDevices/FED3_library/wiki) for documentation on how to use the library.   This library is in development, please report bugs using Issues. 

## Installation instructions
### 1. Install the [Arduino](www.arduino.cc) IDE.

### 2. Install the FED3 library from the Arduino IDE <br>
Search for the FED3 libary in the Arduino library manager and install the latest version.
![FED3 library manager](https://github.com/KravitzLabDevices/FED3/blob/main/photos/FED3librarymanager.png)

### 3. Install dependency libraries in your Arduino libraries folder <br>
Navigate to the FED3 library in your Arduino libraries folder (usually: `Documents\Arduino\libraries\FED3_library\`) and look in the \extras directory for the FED3_support_libraries.zip file. Unzip the contents of this folder.

![FED3 libraries](https://github.com/KravitzLabDevices/FED3/blob/main/photos/FED3_libraries.png)

Copy the extracted libraries to `\Arduino\libraries\`.

### 4. Set the real time clock (RTC) on the FED3. 
Open the "SetClock" example sketch in the Arduino IDE and flash this code to the FED3. You should see a message on the screen with the correct local time, indicating that the RTC was set correctly.<br>
![FED3 RTC](https://raw.githubusercontent.com/KravitzLabDevices/FED3/main/photos/RTCscreen.jpg)

### 5. Flash an example (ClassicFED3 is a good one to start with)
Check out the example FED3 programs by clicking File > Examples > FED3 <br>
![FED3 examples](https://raw.githubusercontent.com/KravitzLabDevices/FED3/main/photos/FED3example2.png)

### 6. Start coding!
Check out the [functions](https://github.com/KravitzLabDevices/FED3_library/wiki/3.-Functions) and [variables](https://github.com/KravitzLabDevices/FED3_library/wiki/2.-Variables) of the FED3 library to start making custom programs.
