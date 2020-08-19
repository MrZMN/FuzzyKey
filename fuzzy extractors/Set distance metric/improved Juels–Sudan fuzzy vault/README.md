# Fuzzy Extractors - Improved Juels–Sudan (fuzzy vault) construction (based on set distance metric)

Based on the paper 'FUZZY EXTRACTORS: HOW TO GENERATE STRONG KEYS FROM BIOMETRICS AND OTHER NOISY DATA' by DODIS, etc. (2008 edited version)

### Assumptions
We could measure the PS values and generate a set based on them. Each set element should be different from each other.

### Protocol Description

- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (time depends on the set size required).
- TX/RX each generates a set based on PS measurements (suppose the set size is s) as PS and PS'. These two sets should be very similar. 
- TX calculates secure sketch ss based on PS. The length of ss depends on the error tolerance t. 
- TX sends ss to RX over the wireless channel.
- RX received ss. 
- RX recovers PS' = PS based on ss received, using Berlecamp-Welch algorithm.
- The elements in PS' should be the same as the ones in PS, however, the order of the PS elements at two sides might be different. To solve this, both TX/RX sort the n-elements in PS and PS' in ascending order.
- After sharing the same PS values, both TX/RX use PS as the input of a strong random extractor (i.e. a secure hash), which outputs a uniformly distributed key. The first 128-bit of the strong extractor output is regarded as the encryption key.


- (Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX)

![Image text](https://github.com/MrZMN/Implementation-of-PS-based-key-distribution-methods/blob/master/images/Improved%20J-S%20fuzzy%20extractor.png)

### Details

- The error tolerance t means the max tolerable symmetric difference size between PS and PS'. For instance, the symmetric difference size between {1,2,3} and {2,3,4} is 2.
- In the above implementation, error on wireless channel wasn't concerned.

### Security Level

Suppose the universe size is n, the maximum error tolerance (symmetric difference size) is t, then the entropy loss is t\*log(n) with base 2.
