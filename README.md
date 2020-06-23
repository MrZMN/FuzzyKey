# Implementation-of-PS-based-key-distribution-methods-for-body-area-network

Comparison of the well-accepted body-area key distribution methods based on PS (physiological signals). All methods have been implemented in C, which are targeted for embedded systems.

### Background & Research Purpose

Realising secure key distribution in implanted medical devices (IMD) scenarios, namely, between an IMD inside the body and a programmer outside the body, is significant. The PS-based key distribution method is one of the best choices, which is the field we're focusing on. 

The main idea is: by measuring the patient's PS information (for example, heart rate, although proven to be insecure) simultaneously for a period of time, both the IMD and programmer could distribute a secure key using the measured PS as a secret. The conditions of these methods include: 1. The PS can only be measured by an IMD or a legit programmer which touches the patient's skin, namely, can't be obtained by a remote attacker. 2. The PS itself is reliable, i.e. contains certain amount of entropy, easy to measure and could be measured similarly by both devices, etc. 

We choose the PS-based key distribution methods for several reasons, for instance, they're user friendly, simple to use and useful in emergency circumstances. Besides, it substaintially reduces the risks of RF signal leakage, because even though a remote attacker eavesdrops the RF signal successfully, he can't get any useful information without knowing the PS measurements. Apart from the IMD scenarios, these methods also work on **body-area networks**, i.e. used to authenticate sensors. 

Therefore, this project will collect, compare well-known PS-based key distribution methods and implement them in a simulated board (MSP430 is chosen as an equivalent of IMD) and a simulated human body environment. Our purpose is to provide a framework for the real-world developers who implement these methods for security. Although there're researches that have implemented one or two of these methods, we haven't found papers that implement and compare all of them in the same simulated environment. Besides, few researches focus on the the energy consumption, which is vital in embedded systems. We believe it's useful to give people an idea of - are these methods really practical for IMD which has hardware/energy constrains? Especially, how much energy/computation/time are required? Which might be the best choices for medical device/body-area scenarios? 

### Assumptions and Conditions

Since our work is mainly benchmark, we need to restrict several assumptions for our implementations:

#### For device
1. We only focus on key distribution on two devices, namely, all the methods work on a TX and a RX. By our implementation, we will also give a clear conclusion about: TX or RX is more suitable for the resouce-restricted device, depending on the resource consumptions we measure, such as energy consumption. Actually, this is also one of the targets of this research, because we haven't seen papers giving clear info like, the IMD should be TX when fuzzy vault is used because TX consumes less energy than RX.
2. We will not measure PS for real. Instead, we assume the devices could already easily measure different PSs, for instance, some may measure IPIs (time difference between two ECG pulses) while some may measure some other PS. We also assume the devices have the abilities such as generating random numbers, converting the analog signals into digital bit strings, etc.

#### For algorithms
1. The target of each method is the same: distribute a secure 128-bit key on both devices.
2. All methods chosen must be secure in IMD&programmer scenrio and well accepted by security communities. And the methods implemented should provide similar level of security.
3. All these methods could work whenever the PS measurements on both devices are 'similar enough'. However, different PSs may result in different error rates (different measurements match rates). In order to show the performances of these methods on different PSs, we will apply different error rates on the PS measurements, so that the results could tell us which method works better on which PS. For example, fuzzy commitment works great when error rate is less than 5%, but works bad when error rate increases. (Apart from measurement mismatch, transmission error could be concerned as well.)  

### Methods Chosen

Till now, these methods have been well accepted by the security community:
1. Based on PS in binary format
- fuzzy commitment
- fuzzy extractor: code-offset construction 
- fuzzy extractor: syndrome construction
2. Based on PS in set format (for example, use IPI time as set element directly)
- fuzzy vault
- fuzzy extractor: improved fuzzy vault
- fuzzy extractor: PinSketch

Most of these methods are based on ECC (Error-correction Codes). The ECC used:
1. binary BCH code
2. Reed-Solomon code

Note: Because many methods rely on ECC codes, it'll also be a research area to test which ECC settings perform better. For example, different ECC types may have different effects, such as: RS works well in burst error naturally. Besides, for block codes, larger block size may result in more memory consumption, but may require less energy consumption. The choice of different block sizes may affect. 







