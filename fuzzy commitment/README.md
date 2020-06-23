# Fuzzy Commitment

Based on the paper 'A Fuzzy Commitment Scheme' by Juels & Wattenberg

### Assumptions
1. We could generate a hundreds-bit long string from the PS measurements. The string length should be the same as the overall codeword (ECC encoded key) length. For example, if we use (255,131) BCH code, we'll get one 255-bit codeword, so we need a 255-bit PS string.

### Protocol Description
- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (time depends on string length required).
- Both devices convert the PS analog signal to n-bit strings PS and PS'. These two strings should be similar. 
- TX generates a purely random 128 bit string, then encodes it to be a codeword/several codewords c using one kind of ECC. The sum of the codewords' lengthes is n.
- TX computes δ = c xor PS.
- TX sends δ to RX over the wireless channel. 
- RX received δ.
- RX calculates c' = PS' xor δ = PS' xor c xor PS. c' should be different from c because of the inequality between PS and PS'. 
- RX decodes c'. If the PS measurement mismatch is tolerable, RX gets c' = c. Then RX could extract the 128-bit key from c easily. 

- Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX

### Details
- The choices of ECC usage include: 1. different types, like RS or BCH codes. 2. different param settings, including code length, error tolerance, etc.
- In the above implementation, errors on wireless channel wasn't concerned. 

### Security level
The security depends on two parts:








