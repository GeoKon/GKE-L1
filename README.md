Guide to GKESP-L1 Library
-------------------------

This library includes the files and classes shown below. More details
can be found in the `\src` subdirectory and the associated `.md` files.


| Files          | Class(es)       | ..\examples    | .                        Functionality                              .  |
|------          |:---------:      |--------------- |---------                                            |
|macros.h		| none			| none			| PR(), PRN(), PF(), PFN(), CRLF() Macros |
|bufClass.cpp	| BUF			| bufClass.ino	| Convenient buffer classes, either in the Heap or Stack |
|cpuClass.cpp	| CPU, COUT		| cpuClass.inoi	| Baseline NodeMCU i/o functions. Console Out pipe |  
|cliClass.cpp	| CLI, EXE		| cliClass.ino	| Asynchronous serial I/O for CLI. Dispatch class to CMDTABLE handlers | 
|eepClass.cpp	| EEP			| eepClass.ino	| Access to EEPROM for WiFi and User parameters |
|ticClass.cpp	| TICms, PROFc	| ticClass.ino	| Tic routines for loop(). Also a timing profile class |
|gkeL1ios.h		| nonec			| none			| Composit include of all above .h files |
|ads15Class.cpp	| ADS15			| ads15Class.ino| ADS1115 (16-bit A/D converter) driver class |
|oledClass.cpp	| OLED			| oledClass.ino	| Connects to 128x32, 128x64 (0.91" and 0.96") OLEDs |
|ds18Class.cpp   |  DS18, FLT      | ds18Class.ino  | Dallas Temp OneWire replacement library optimized for size. Requires OneWire.cpp, .h |
|timeClass.cpp   |  TIME ModuloX   | timeClass.ino  | Time base (free running). ModuloTic, ModuleSec, ModuleHrs utility classes |
|webSupport.cpp  | .               | .              | To be deprecated |

### Dependencies

In general, all L1 are independent of each other with `bufClass` as the only exception. All L2 classes depend on L1 classes. More specifically,

| Class     | Depends Class(es)     |
|-------:   |:---------------------:|
| BUF       | none                  |
| CPU		| none      	        |
| CLI       | BUF                   |
| EXE       | BUF   				|
| EEP       | BUF   				|
| TICms     | none   				|
| ModuloTic | none   				| 
| ModuloSec | TIME   				|
| ModuloHrs | TIME   				| 
| OLED      | CPU   				|
| TIME      | CPU   				|

See the following for a graphical representation
![](https://i.imgur.com/WJ9iVsr.gif)

