# Fuzzy Extractors

Based on the paper 'FUZZY EXTRACTORS: HOW TO GENERATE STRONG KEYS FROM BIOMETRICS AND OTHER NOISY DATA' by DODIS, etc. (2008 edited version)

### Protocol Description

Different from fuzzy commitment and fuzzy vault, fuzzy extractors don't aim to conceal and transmit the key itself in the transmission procedure. Indeed, its target is only to correct the mismatches on PS measurements at TX/RX, which is called 'secure sketch' in their work. Afterwards, both sides use the same PS as the input of a strong random extractor, which outputs a uniformly distributed key. Besides, fuzzy extractor contains different constructions, which are based on different metrics of the PS.

The main protocol could be described briefly:
- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously. A bit string or set will be generated.
- TX does some operations to generate a message, which is called secure sketch, based on its measurement PS.
- TX transmits the secure sketch to RX over the wireless channel. 
- RX receives the secure sketch. It provides its measurement PS' and does some operations to recover PS' = PS.
- Both TX/RX use the same strong random extractor (i.e. a secure hash method) to generate a uniformly distributed key. The agreed PS is the input of the extractor.

- Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX

The 'operations' mentioned above depend on the exact constructions.

![Image text](https://github.com/MrZMN/Implementation-of-PS-based-key-distribution-methods/blob/master/images/Fuzzy%20extractor.png)

### Strong Extractor

A strong extractor is used to extract uniformly distributed randomness from arbitrary-length input. In our experiments, we use a secure AES-based hashing method as the strong extractor. If the input of the hash (agreed PS) has at least 128-bit entropy and the hash itself is secure, then it's secure to use the first 128-bit of the hash output as an encryption key.

![Image text](https://github.com/MrZMN/Implementation-of-PS-based-key-distribution-methods/blob/master/images/Entropy%20loss%20structure.png)

### Security Level

Different from fuzzy commitment and fuzzy vault, fuzzy extractors don't use physiological signals to conceal the secret key. Instead, the key is extracted from the PS itself. The main idea is to recover the mismatch on the PS measurements at TX/RX, then exact uniformly distributed random key from the agreed PS.

Therefore, the security is ensured if the original entropy of the measured PS at both sides is high enough. Precisely, the remaining entropy of agreed PS should be at least 128-bit, considering the entropy loss due to transmission/storage of public information.

For the Hamming-distance based fuzzy extractors, the original PS entropy is the same as the length of the extracted PS bit string. For the set-distance based fuzzy extractors, suppose the universe size is n, the set size is s, then the original entropy is log(n,s) with base 2, where (n,s) stands for the number of combinations when taking random s elements from n elements without putting back.
