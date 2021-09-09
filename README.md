# FuzzyKey: Comparing Fuzzy Cryptographic Primitives on Resource-Constrained Devices

### Background

There are various kinds of entropy sources in the nature, for example, a human's physiological signal (such as heart rate), PUF, biometrics, etc. Intuitively, a cryptographic key can be shared between two devices that can independently measure such an entropy source. However, this is not viable because the measurements are always not identical but at best rather similar due to inherent noise introduced by the measuring process. Fuzzy primitives, including the fuzzy commitment, fuzzy vault and fuzzy extractor, allow two parties to agree a cryptographic key based on a noisy entropy source.

Although the fuzzy cryptographic primitives have been frequently analysed and widely applied in multiple scenarios, little effort has been devoted into studying the feasibility of fuzzy primitives in resource-constrained systems as well as how to conﬁgure them to optimise performance. We address this gap by implementing and optimising six fuzzy cryptographic primitives on a TI MSP430 low power microcontroller. The implementations are in plain C and mainly rely on standard C libraries, thus can be easily ported on other platforms. We evaluate and compare the resource consumption of each construction under various parameter settings both at the transmitter and receiver. We demonstrate that fuzzy primitives are feasible o resource-constrained embedded devices. Additionally, we also implement countermeasures against timing attacks for the most eﬃcient constructions. Our work can be regarded as a reference for real-world designs. More details can be viewed in: (paper waiting for official publication)

### PS (physiological signal) as an entropy source

We specifically focus on the application scenario when PS is used as a shared entropy source. This is because this scenario involves the most enery sensitive devices, e.g., IMDs (implantable medical devices) and wearables. These devices (such as a pacemaker and an external device) are able to share a key by independently and synchronously taking a measurement of a given user’s PS (can only be measured by making physical contact with the user). Therefore, the fuzzy primitives are also implemented in this context. However, we note that our work also serves as a reference for other use cases such as biometrics or PUFs.

### Implemented fuzzy primitives

The implemented fuzzy primitives are divided into two categories: based on the Hamming metric space or the set metric space.
1. Based on Hamming metric
- Fuzzy commitment
- Fuzzy extractor: code-offset construction 
- Fuzzy extractor: syndrome construction
2. Based on set metric
- Fuzzy vault
- Fuzzy extractor: improved Juels–Sudan construction
- Fuzzy extractor: PinSketch construction

Most of these methods are based on ECC (Error-correction Code) including:
1. Binary Bose-Chaudhuri-Hocquenghem (BCH) code
2. Reed-Solomon (RS) code

### Directory description

This repository includes two directories:
1. FuzzyKey/Code run by MSP430 on TI Code Composer Studio/: Source code implementations (in plain C) of PS-based fuzzy cryptographic primitives on TI MSP430 LaunchPad development board. All codes run on TI Code Composer Studio IDE. Note that each Hamming metric method has two versions as 'BCH based' and 'RS based' depending on the underlying applied ECC. Each method comprises two parts: TX and RX. There are multiple parameter choices for each method. All feasible parameters have been defined as macros at the top of each 'main.c' file.

2. FuzzyKey/Timing-leakage test/: Codes that test whether a program running on MSP430FR5969 is constant-time.



