# Fuzzy Extractors - code-offset construction (based on Hamming distance metric)

Based on the paper 'FUZZY EXTRACTORS: HOW TO GENERATE STRONG KEYS FROM BIOMETRICS AND OTHER NOISY DATA' by DODIS, etc. (2008 edited version)

### Assumptions
1. We could not only measure the PS values, but could generate a hundreds-bit long string from the PS measurements. The string length should be the same as the overall codeword (encoded key using ECC) length. For example, if we use (255,131) BCH code, we'll need a 255-bit PS string.

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

### Details

- We can choose different ECC codes, including: 1. different types, like RS or BCH codes. 2. different param settings, including code length, error tolerance, etc. The choice of ECC code should follow these conditions: The error tolerance ability should be related with the PS inequality; the device ability, such as memory, energy consumption should be concerned; the type of PS mismatches, such as consecutive or random, should be concerned.
- In the above implementation, errors on wireless channel wasn't concerned.

### Security Level
If the extracted PS string is uniformly distributed random, the transmitted fuzzy commitment could be regarded as one time pad, which is completely secure. The security level isn't relevant to the parameter settings of ECC.
