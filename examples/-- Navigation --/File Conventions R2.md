## File Conventions R2

The following conventions are applicable to *library modules* as well as *application modules*.  A *library module* consists of {a .cpp file, a .h file, and optionally an .md file}. An *application* consists of one main.cpp file and optionally of *application modules*, that is, a set of .cpp and .h files.

#### library.h files contain

- ***Defines, typedefs, and structures***. 
- ***Declaration of Classes***. Before the declarations, include any <header.h> files needed for data types and or instantiation of these classes.
- ***Functions prototypes***, aka exported functions. Such functions are defined in the .cpp
- ***Exported class instances, variables, structures***. Export only the ones allocated in the .cpp file
- ***Header files***. Include the *minimum necessary* <header.h> files to properly compile the .h file
- ***.h file organization:***
  1. [#pragma once]()
  2. *HEADERS*
     - <headers.h> needed to compile properly this file
  3. *DECLARATIONS*
     - defines
     - structures and typedefs
     - *class declarations*, template *definitions*. Optionally simple *class definitions*.
  4. *EXPORTS*
     - external variables or structures, defined in .cpp
     - function prototypes, defined in .cpp
     - exported class(es) if allocated by the .cpp file

#### library.cpp files contain

***Include files***. Such includes should be the minimum necessary for the .cpp file to compile properly. The last include must be the associated .h file. It is assumed that the included files include themselves dependencies from other includes. 

- ***Forward references of functions***.
- ***Variables and structure allocations*** to be used externally. Use [static]() for all internal variables or structures
- ***Function implementations*** to be exported. Use [static]() for functions not be to exported
- ***Class definitions***.
- ***Class allocations*** -- optional. If a class is allocated, then the .h file must include the corresponding [extern](), unless the class only used by this module.
- ***No extern definitions should be needed***, since such definitions are in the included .h files

#### Application .cpp files

Applications files in general use library modules.  The <include.h> files should satisfy all use of functions and classes.

If the main application file depends on other sub-ordinate application modules, such modules should follow the same conventions as the library modules.