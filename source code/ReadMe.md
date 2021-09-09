### Source code description


This directory includes two parts:
1. Source code implementations (in plain C) of physiological signal based-fuzzy cryptographic primitives on TI MSP430FR5969 LaunchPad development board. All codes run on TI Code Composer Studio IDE. The fuzzy primitives are divided into Hamming metric and set metric methods. Note each Hamming metric method has two versions as 'BCH based' and 'RS based' depending on the underlying Error Correction Code applied. Each method comprises two parts: TX and RX. There are multiple parameter choices for each method. All feasible parameters have been defined as macros at the top of each 'main.c' file.

2. Codes that test whether a program running on MSP430FR5969 is constant-time.