# Fuzzy Extractors - Improved Juels–Sudan (fuzzy vault) construction (based on Set distance metric)

Based on the paper 'FUZZY EXTRACTORS: HOW TO GENERATE STRONG KEYS FROM BIOMETRICS AND OTHER NOISY DATA' by DODIS, etc. (2008 edited version)

### Assumptions
1. We could measure the PS values. (Assume each PS measurement is one byte) Each collected PS measurements should be different from each other.

### Protocol Description

- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (time depends on the number of ps measurements required).
- Both TX/RX should measure a set of PS values (suppose we collect n PS measurements at both TX/RX sides) as PS and PS'. These two sets should be very similar. 
- TX calculates set (secure sketch) ss based on PS. The size of ss is the error tolerance t. Then TX generates a random n-element set s. 
- TX sends ss and s to RX over the wireless channel.
- RX received ss and s. 
- RX recovers PS' = PS based on ss received, using Berlecamp-Welch algorithm.
- The elements in PS' should be the same as the ones in PS, however, the order of the PS elements at two sides might be different. To solve this, both TX/RX sort the n-elements in PS and PS' in ascending order.
- After sharing PS values and a random set s, both TX/RX compute PS = PS xor s, then use PS as the input of a strong random extractor (a SHA-256 in our implementation), which outputs a uniformly distributed key. We take the first 128-bit of the sha-256 output as the encryption key.


- (Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX)

![Image text](https://github.com/MrZMN/Implementation-of-PS-based-key-distribution-methods/blob/master/images/improvedJSextractor.png)

### Details

- The error tolerance t means the max tolerable symmetric difference between PS and PS'. For instance, the symmetric difference between {1,2,3} and {2,3,4} is {1,4}.
- In the above implementation, errors on wireless channel wasn't concerned.

### Security Level

Suppose the universe size is n, the maximum error tolerance (symmetric difference size) is t, then the entropy loss is t\*log(n) with base 2.
