## This is the FED3 library (beta)
The goal of this library is to abstract most of the housekeeping functions of [FED3](https://github.com/KravitzLabDevices/FED3) smart pellet dispensing device behind the scenes and let the user focus on writing the behavioral control. See the [Wiki](https://github.com/KravitzLabDevices/FED3_library/wiki) for documentation on how to use the library.   This library is in development, please report bugs using Issues. 

## Installation instructions
### 1. Install the FED3 library from the Arduino IDE <br>
Search for the FED3 libary in the Arduino library manager and install the latest version.
![FED3 library manager](https://github.com/KravitzLabDevices/FED3/blob/main/photos/FED3librarymanager.png)

### 2. Install dependency libraries in your Arduino libraries folder <br>
Navigate to the FED3 library in your Arduino libraries folder (usually: `Documents\Arduino\libraries\FED3_library\`) and look in the \extras directory for the FED3_support_libraries.zip file. Unzip the contents of this folder.

![FED3 libraries](https://github.com/KravitzLabDevices/FED3/blob/main/photos/FED3_libraries.png)

Copy the extracted libraries to `\Arduino\libraries\`.


### 3. Start coding!
Check out the example FED3 programs by clicking File > Examples > FED3 <br>
![FED3 examples](https://raw.githubusercontent.com/KravitzLabDevices/FED3/main/photos/FED3example2.png)


This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
human readable:https://creativecommons.org/licenses/by-sa/3.0/
legal wording:https://creativecommons.org/licenses/by-sa/3.0/legalcode
