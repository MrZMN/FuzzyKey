# Fuzzy Extractors - syndrome construction (based on Hamming distance metric)

Based on the paper 'FUZZY EXTRACTORS: HOW TO GENERATE STRONG KEYS FROM BIOMETRICS AND OTHER NOISY DATA' by DODIS, etc. (2008 edited version)

### Assumptions
1. We could not only measure the PS values, but could generate a hundreds-bit long string from the PS measurements. The string length should be the same as the overall codeword (encoded key using ECC) length. For example, if we use (255,131) BCH code, we'll get one 255-bit codeword, so we need a 255-bit PS string.

### Protocol Description

- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (time depends on the ps string length required).
- Suppose we use (n,k) binary BCH code. Both devices convert the PS analog signal to n-bit strings PS and PS'. These two strings should be very similar. 
- TX regards the n-bit PS as a codeword (although it's not), and calculates SS = syndrome(PS), which is the ECC syndrome of the PS. Then TX generates a random n-bit string s.
- TX sends SS and s to RX over the wireless channel.
- RX received SS and s.
- RX calculates syndrome(PS') just like what TX did, then calculates syndrome(err) = syndrome(PS') xor syndrome(PS) = syndrome(PS') xor SS. err is the mismatch between PS and PS'. If syndrome(err) is a zero vector, there're no mismatches on PS measurements between TX/RX.
- If syndrome(err) isn't a zero vector, RX corrects PS' based on syndrome(err). The corrected PS data should be the same as the one at TX side.
- After sharing PS values and a random string s, both TX/RX compute PS = PS xor s, then use PS as the input of a strong random extractor (a SHA-256 in our implementation), which outputs a uniformly distributed key. We take the first 128-bit of the sha-256 output as the encryption key.

- (Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX)

![Image text](https://github.com/MrZMN/Implementation-of-PS-based-key-distribution-methods/blob/master/images/syndromeextractor.png)

### Details

- We can choose different ECC codes, including: 1. different types, like RS or BCH codes. 2. different param settings, including code length, error tolerance, etc. The choice of ECC code should follow these conditions: The error tolerance ability should be related with the PS inequality; the device ability, such as memory, energy consumption should be concerned; the type of PS mismatches, such as consecutive or random, should be concerned.
- In the above implementation, errors on wireless channel wasn't concerned.

### Security Level

The transmitted syndrome SS is calculated based on the extracted PS string (regarded as a codeword). For instance, based on (n,k) BCH code and n-bit PS string, we regard the PS string as a n-1 degree polynomial r(x) (each bit corresponds to one coefficient of the polynomial), then each element of the syndrome is calculated as: SS[i] = r(a^i), i from 1 to 2t, where a^i belongs to GF(2^m) and n = 2^m-1.  

Therefore, the security of this method is based on the length of the randomly distributed PS string. For a (n,k) BCH code, the security level should be at most 2^n; for a (n,k) RS code, the security level should be at most 2^(m\*n), where n = 2^m-1. 
