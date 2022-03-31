# FuzzyKey: Comparing Fuzzy Cryptographic Primitives on Resource-Constrained Devices

### Background

There are various kinds of entropy sources in the nature, for example, a human's physiological signal (such as heart rate), PUF, biometrics, etc. Intuitively, a cryptographic key can be shared between two devices that can independently measure such an entropy source. However, this is not viable because the measurements are always not identical but at best rather similar due to inherent noise introduced by the measuring process. Fuzzy primitives, including the fuzzy commitment, fuzzy vault and fuzzy extractor, allow two parties to agree on a cryptographic key based on a noisy entropy source.

Although the fuzzy cryptographic primitives have been frequently analysed and widely applied in multiple scenarios, little effort has been devoted into studying the feasibility of fuzzy primitives in resource-constrained systems as well as how to conﬁgure them to optimise performance. We address this gap by implementing and optimising six fuzzy cryptographic primitives on a TI MSP430 low power microcontroller. The implementations are in plain C and mainly rely on standard C libraries, thus can be easily ported to different platforms. We evaluate and compare the resource consumption of each construction under various parameter settings both at the transmitter and receiver. We demonstrate that fuzzy primitives are feasible on resource-constrained embedded devices. Additionally, we also implement countermeasures against timing attacks for the most eﬃcient constructions. Our work can be regarded as a reference for real-world designs. We recommend you to view more details in: hhttps://link.springer.com/chapter/10.1007/978-3-030-97348-3_16

### PS (physiological signal) as an entropy source

We specifically focus on the application scenario when PS is used as a shared entropy source. This is because this scenario involves the most enery sensitive devices, e.g., IMDs (implantable medical devices) and wearables. These devices (such as a pacemaker and an external device) are able to share a key by independently and synchronously taking a measurement of a given user’s PS (can only be measured by making physical contact with the user). Therefore, the fuzzy primitives are also implemented in this context. However, we note that our work also serves as a reference for other use cases such as biometrics or PUFs.

### Implemented fuzzy primitives

The implemented fuzzy primitives are divided into two categories: based on the Hamming metric space or the set metric space.
1. Based on Hamming metric
- Fuzzy commitment
- Fuzzy extractor: Code-offset construction 
- Fuzzy extractor: Syndrome construction
2. Based on set metric
- Fuzzy vault
- Fuzzy extractor: Improved Juels–Sudan construction
- Fuzzy extractor: PinSketch construction

Many algorithm are based on ECC (Error-correction Code) including:
1. Binary Bose-Chaudhuri-Hocquenghem (BCH) code
2. Reed-Solomon (RS) code

### Acknowledgement

We partially refer to Simon Rockliff's implementation of RS code (cf. http://www.eccpage.com/rs.c, 1989) for our decoding methods. We thank Simon for his work, and acknowledge him in the comments of relevant programs as required.

