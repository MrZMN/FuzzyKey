# Fuzzy Extractors - BCH-based Pinsketch construction (based on Set distance metric)

Based on the paper 'FUZZY EXTRACTORS: HOW TO GENERATE STRONG KEYS FROM BIOMETRICS AND OTHER NOISY DATA' by DODIS, etc. (2008 edited version)

### Assumptions
We could measure the PS values and generate a set based on them. Each set element should be different from each other.

### Protocol Description

- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (time depends on the set size required).
- TX/RX each generates a set based on PS measurements (suppose the set size is s) as PS and PS'. These two sets should be very similar.
- TX calculates secure sketch ssTX based on PS. The length of ssTX depends on the error tolerance t.
- TX sends ssTX to RX over the wireless channel.
- RX received ssTX. 
- RX calculates ssRX based on PS', the process is the same as the one at TX side. Then RX calculates ssRX = ssRX xor ssTX.
- Mismatch between PS and PS' exists if ssRX is not a zero vector. If so, RX calculates the set setdiff, which is the set difference between PS and PS', based on ssRX.
- RX calculates PS' = set difference between PS' and setdiff. 
- The elements in PS' should be the same as the ones in PS, however, the order of the PS elements at two sides might be different. To solve this, both TX/RX sort the n-elements in PS and PS' in ascending order.
- After sharing the same PS values, both TX/RX use PS as the input of a strong random extractor (i.e. a secure hash), which outputs a uniformly distributed key. The first 128-bit of the strong extractor output is regarded as the encryption key.


- (Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX)

![Image text](https://github.com/MrZMN/Implementation-of-PS-based-key-distribution-methods/blob/master/images/BCH-based%20Pinsketch%20.png)

### Details

- - The error tolerance t means the max tolerable symmetric difference size between PS and PS'. For instance, the symmetric difference size between {1,2,3} and {2,3,4} is 2.
- This construction is based on binary BCH code only. The choice of BCH algorithm is based on: The computation of BCH code should follow the PS element characteristic, i.e. codeword size n of BCH code should be 2^8-1 when each PS element consists of 8 bits. 
- In the above implementation, error on wireless channel wasn't concerned.

### Security Level

Suppose the universe size is n, the maximum error tolerance (symmetric difference size) is t, then the entropy loss is t\*log(n+1) with base 2.
