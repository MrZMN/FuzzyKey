# Fuzzy Commitment

Based on the paper 'A Fuzzy Commitment Scheme' by Juels & Wattenberg

### Assumptions
We could measure the PS, then generate a uniformly distirbuted random hundred-bit string from the PS measurements. 

### Protocol Description
- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (time depends on the ps string length required).
- Both devices convert the PS analog signal to n-bit uniformly distirbuted random strings PS and PS'. These two strings should be very similar. 
- TX generates a purely random 128-bit key (may add some zero paddings), then encodes it to be one/many codewords c using error correction code. The whole length of the codeword(s) should be n-bit. 
- TX computes δ = c xor PS.
- TX sends δ to RX over the wireless channel. 
- RX received δ.
- RX calculates codeword c' = PS' xor δ = PS' xor c xor PS. c' should be different from c because of the mismatch between PS and PS'. 
- RX decodes c'. If the PS measurement mismatch is tolerable, RX gets c' = c. Then RX could extract the 128-bit key from c easily. 

- (Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX)

![Image text](https://github.com/MrZMN/Implementation-of-PS-based-key-distribution-methods/blob/master/images/fuzzycommitment.png)

### Details
- We can choose different ECC codes, including: 1. different types, like RS or BCH codes. 2. different param settings, including code length, error tolerance, etc. - The choice of ECC code should follow these conditions: 1. the error tolerance ability should be related with the PS mismatch. 2. the device ability, such as memory, energy consumption should be concerned. 3. the type of PS mismatches, such as consecutive or random, should be concerned.
- In the above implementation, errors on wireless channel wasn't concerned. 

### Security level
If the extracted PS string is uniformly distributed random, the transmitted fuzzy commitment could be regarded as one time pad, which is completely secure. The security level isn't relevant to the parameter settings of ECC.







