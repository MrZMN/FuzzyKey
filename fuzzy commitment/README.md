# Fuzzy Commitment

Based on the paper 'A Fuzzy Commitment Scheme' by Juels & Wattenberg

### Assumptions
1. We could generate a hundreds-bit long string from the PS. The actual length should be the same as the codeword generated from 128 bit-key. For example, if we use a (131,255) BCH code, we need a 255-bit PS string.

### Protocol Description
- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (depending on string length required).
- Both devices convert the PS analog signal to n-bit strings PS and PS'. These two strings should be very similar. 
- TX generates a random 128 bit key, then encodes it to be a codeword c of length n (paddings needed maybe) using one kind of ECC.
- TX computes δ = c xor PS
- TX sends δ to RX over the wireless channel. This channel includes air and human tissue, which might cause error e. 
- RX received δ', we assume δ' = c xor PS xor e
- RX calculates c' = PS' xor δ' = PS' xor c xor PS xor e. c' should be different from c because: 1.the inequality between PS and PS' 2. channel noise
- RX decodes c'. If the total error is tolerable, RX gets c' = c. The first 128 bit message is the key.

- Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX








