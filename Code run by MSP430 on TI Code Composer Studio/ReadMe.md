### Codes run by MSP430 on TI Code Composer Studio

Source code implementations (in plain C) of PS-based fuzzy cryptographic primitives on TI MSP430 LaunchPad development board. All codes run on TI Code Composer Studio IDE. Note that each Hamming metric method has two versions as 'BCH based' and 'RS based' depending on the underlying applied ECC. Each method comprises two parts: TX and RX. There are multiple parameter choices for each method. All feasible parameters have been defined as macros at the top of each 'main.c' file.

PS: We used *TI Ultra Low Power Advisor tool* to minimise power consumption - this also makes part of the codes look 'wired', e.g., the *IF ELSE* structures are generally replaced by *WHILE* structures.  
