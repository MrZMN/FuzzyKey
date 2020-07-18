# Fuzzy Extractors - BCH-based Pinsketch construction (based on Set distance metric)

Based on the paper 'FUZZY EXTRACTORS: HOW TO GENERATE STRONG KEYS FROM BIOMETRICS AND OTHER NOISY DATA' by DODIS, etc. (2008 edited version)

### Assumptions
1. We could measure the PS values. (Assume each PS measurement is one byte) Each collected PS measurements should be different from each other.

### Protocol Description

- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (time depends on the number of ps measurements required).
- Both TX/RX should measure a set of PS values (suppose we collect n PS measurements at both TX/RX sides) as PS and PS'. These two sets should be very similar. 
- TX calculates set (secure sketch) ssTX based on PS. The size of ssTX is the error tolerance t. Then TX generates a random n-element set s. 
- TX sends ssTX and s to RX over the wireless channel.
- RX received ssTX and s. 
- RX calculates t-element ssRX based on PS', the process is the same as the one at TX side. Then RX calculates ssRX = ssRX xor ssTX by xoring the t values at the same indexes respectively.
- Mismatch between PS and PS' exists if ssRX is not a zero vector. If so, RX calculates the set setdiff, which is the set difference between PS and PS', based on ssRX.
- RX calculates PS' = set difference between PS' and setdiff. 
- The elements in PS' should be the same as the ones in PS, however, the order of the PS elements at two sides might be different. To solve this, both TX/RX sort the n-elements in PS and PS' in ascending order.
- After sharing PS values and a random set s, both TX/RX compute PS = PS xor s, then use PS as the input of a strong random extractor (a SHA-256 in our implementation), which outputs a uniformly distributed key. We take the first 128-bit of the sha-256 output as the encryption key.


- (Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX)

### Details

- The error tolerance t means the max tolerable set difference between PS and PS'. For instance, the set difference between {1,2,3} and {2,3,4} is 2.
- This construction is based on binary BCH code only. The choice of BCH algorithm is based on: The computation of BCH code should follow the PS element characteristic, i.e. codeword size n of BCH code should be 2^8-1 when each PS element consists of 8 bits. 
- In the above implementation, errors on wireless channel wasn't concerned.

### Security Level
