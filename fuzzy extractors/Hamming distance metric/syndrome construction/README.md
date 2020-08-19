# Fuzzy Extractors - syndrome construction (based on Hamming distance metric)

Based on the paper 'FUZZY EXTRACTORS: HOW TO GENERATE STRONG KEYS FROM BIOMETRICS AND OTHER NOISY DATA' by DODIS, etc. (2008 edited version)

### Assumptions
We could measure the PS, then generate a hundred-bit string from the PS measurements.

### Protocol Description

- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (time depends on the ps string length required).
- Both devices convert the PS analog signal to n-bit strings PS and PS'. These two strings should be very similar. 
- TX regards the n-bit PS as one/many codeword(s), and calculates SS = syndrome(PS), which is the ECC syndrome of the PS.
- TX sends SS to RX over the wireless channel.
- RX receives SS.
- RX calculates syndrome(PS') just like what TX did, then calculates syndrome(err) = syndrome(PS') xor syndrome(PS) = syndrome(PS') xor SS. err is the mismatch between PS and PS'. If syndrome(err) is a zero vector, there're no mismatches on PS measurements between TX/RX.
- Otherwise, if syndrome(err) isn't a zero vector, RX corrects PS' based on syndrome(err). The corrected PS data should be the same as the one at TX side.
- After sharing the same PS values, both TX/RX use PS as the input of a strong random extractor (i.e. a secure hash), which outputs a uniformly distributed key. The first 128-bit of the strong extractor output is regarded as the encryption key.

- (Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX)

![Image text](https://github.com/MrZMN/Implementation-of-PS-based-key-distribution-methods/blob/master/images/Syndrome-based%20fuzzy%20extractor.png)

### Details

- We can choose different ECC codes, including: 1. different types, like RS or BCH codes. 2. different param settings, including code length, error tolerance, etc. 
- The choice of ECC code should follow these conditions: 1. the error tolerance ability should be related with the PS mismatch. 2. the device ability, such as memory, energy consumption should be concerned. 3. the type of PS mismatches, such as consecutive or random, should be concerned.
- In the above implementation, errors on wireless channel wasn't concerned.

### Security Level

Suppose we use (n,k) ECC, each symbol consists of f bits, and we need b blocks in total. The entropy loss is (n-k)\*f\*b.
