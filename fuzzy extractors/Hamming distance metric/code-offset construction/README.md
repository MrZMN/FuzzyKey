# Fuzzy Extractors - code-offset construction (based on Hamming distance metric)

Based on the paper 'FUZZY EXTRACTORS: HOW TO GENERATE STRONG KEYS FROM BIOMETRICS AND OTHER NOISY DATA' by DODIS, etc. (2008 edited version)

### Assumptions
We could measure the PS, then generate a random hundred-bit string from the PS measurements.

### Protocol Description

- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (time depends on the ps string length required).
- Suppose we use (n,k) binary BCH code. Both devices convert the PS analog signal to n-bit strings PS and PS'. These two strings should be very similar. 
- TX generates a n-bit random codeword c using the error correction code. This is the same as generating a random k-bit nonce and encoding it to be a n-bit codeword. 
- TX computes δ = c xor PS to conceal PS. Then TX generates a random n-bit string s. 
- TX sends δ and s to RX over the wireless channel.
- RX received δ and s. 
- RX calculates c' = PS' xor δ = PS' xor c xor PS. c' should be different from c because of the inequality between PS and PS'.
- RX decodes c'. If the total error is tolerable, RX gets c' = c.
- RX calculates PS = c xor δ. The PS data should be the same as the one at TX side.
- After sharing PS values and a random string s, both TX/RX compute PS = PS xor s, then use PS as the input of a strong random extractor (a SHA-256 in our implementation), which outputs a uniformly distributed key. We take the first 128-bit of the sha-256 output as the encryption key.


- (Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX)

![Image text](https://github.com/MrZMN/Implementation-of-PS-based-key-distribution-methods/blob/master/images/codeoffsetextractor.png)

### Details

- We can choose different ECC codes, including: 1. different types, like RS or BCH codes. 2. different param settings, including code length, error tolerance, etc. 
- The choice of ECC code should follow these conditions: 1. the error tolerance ability should be related with the PS mismatch. 2. the device ability, such as memory, energy consumption should be concerned. 3. the type of PS mismatches, such as consecutive or random, should be concerned.
- In the above implementation, errors on wireless channel wasn't concerned.

### Security Level

Suppose we use (n,k) ECC, each symbol consists of f bits, and we need b blocks in total. The entropy loss is (n-k)\*f\*b. 
