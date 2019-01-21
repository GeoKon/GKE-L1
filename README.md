Guide to GKESP-L1 Library
-------------------------

This library includes the files and classes shown below. More details
can be found in the `\src` subdirectory and the associated `.md` files.


| Files          | Class(es)       | ..\examples    | .                        Functionality                              .  |
|------          |:---------:      |--------------- |---------                                            |
|cliClass.cpp    |  CLI, EXE       | cliClass.ino   | Asynchronous serial I/O for CLI. Dispatch class to CMDTABLE handlers | 
|cpuClass.cpp    |  CPU, BUF, COUT | cpuClass.ino   | Baseline NodeMCU i/o functions. Efficient String class replacement by Buf. Console Out pipe |  
|eepClass.cpp    |  EEP            | eepClass.ino   | Access to EEPROM for WiFi and User parameters |
|oledClass.cpp   |  OLED           | oledClass.ino  | Connects to 128x32, 128x64 (0.91" and 0.96") OLEDs |
|timeClass.cpp   |  TIME ModuloX   | timeClass.ino  | Time base (free running). ModuloTic, ModuleSec, ModuleHrs utility classes |
|webSupport.cpp  | .               | .              | To be deprecated |

### Dependencies

In general, all L2 classes depend on L1 classes. More specifically,

| Class     | Depends Class(es)     |
|-------:   |:---------------------:|
| BUF       | none                  |
| CLI       | CPU                   |
| CPU		| none      	        |
| EEP       | CPU   				|
| EXE       | CPU   				|
| ModuloTic | none   				| 
| ModuloSec | TIME   				|
| ModuloHrs | TIME   				| 
| OLED      | CPU   				|
| TIME      | CPU   				|

See the following for a graphical representation
![](https://i.imgur.com/WJ9iVsr.gif)

