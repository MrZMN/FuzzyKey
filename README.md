# Implementation-of-PS-based-key-distribution-methods

This is a repo storing the analysis of chosen key distribution methods based on PS (physiological signal), as well as codes implemented in C, which are targeted for embedded systems.

### Background & Research Purpose

The aim is to find and compare methods realising secure key distribution in implanted medical devices (IMD) scenarios, namely, between an IMD inside the body and a programmer outside the body. The PS-based key distribution method is one of the choices. The main idea is: by measuring the patient's PS information (for example, heart rate, although proven to be insecure) simultaneously for a period of time, both the IMD and programmer could regard the measured PS as a secret and use it to distribute a key. The conditions of these methods include: 1. The PS can only be measured by an IMD or a legit programmer which touches the patient's skin, namely, can't be obtained by a remote attacker. 2. The PS itself is reliable, i.e. contains certain amount of entropy, easy to measure and could be measured similarly by both devices, etc. We choose the PS-based key distribution methods for several reasons, for instance, they're user friendly, simple to use and useful in emergency circumstances. Besides, it substaintially reduces the risks of RF signal leakage, because even though a remote attacker eavesdrops the RF signal successfully, he can't get any useful information without knowing the PS value.

Therefore, this project will collect, compare proper PS-based key distribution methods and implement them in a simulated board (MSP430 is chosen as an equivalent of IMD) and a simulated human body environment. Our purpose is to provide a framework for the real-world developers who implement these methods for security. We believe it's useful to give people an idea of - are these methods really practical for IMD which has hardware/energy constrains? How much energy/computation/time are required? Which might be the best choices for medical device scenarios? Our research could also be extended to more general areas, such as security in body-area networks.

### Assumptions and Conditions

Since our work is mainly benchmark, we need to restrict several assumptions for our implementations:
1. All methods chosen must be secure in IMD&programmer scenrio and well accepted by security communities. And the methods implemented should provide similar level of security.
2. (For now) We assume we use IPI (time difference between two ECG pulses) as the PS. Although it's been proved to be insecure, we want to make use of its properties, such as: easy to measure, each IPI measurement could generate 4-bit randomness, etc. Based on this, we assume both the IMD and programmer could measure IPIs (time), and also be able to generate bit strings based on them. Later we may use a similar and secure PS to replace it.
3. All the methods implemented should provide similar level of performance, i.e. FAR and FRR. 
4. We want to share a 128-bit key.
5. All the methods work on a TX and a RX. The TX could be IMD or programmer, depending on actual constrains and performance, such as energy consumption.
6. We should simulate the same error influence (both PS signal difference and transmission channel error) happening on different methods.

### Methods Chosen

Till now, these methods have been chosen to be implemented:
1. Based on PS in binary format
- fuzzy commitment
- fuzzy extractor: code-offset construction 
- fuzzy extractor: syndrome construction
2. Based on PS in set format (use IPI time directly)?
- fuzzy vault
- fuzzy extractor: improved fuzzy vault
- fuzzy extractor: PinSketch

Most of these methods are based on ECC (Error-correction Codes). The ECC used:
1. BCH code
2. Reed-Solomon code










