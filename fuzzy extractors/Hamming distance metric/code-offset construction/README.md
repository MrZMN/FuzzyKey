# Fuzzy Extractors - code-offset construction

Based on the paper 'FUZZY EXTRACTORS: HOW TO GENERATE STRONG KEYS FROM BIOMETRICS AND OTHER NOISY DATA' by DODIS, etc. (2008 edited version)

### Assumptions
1. We could generate a hundreds-bit long string from the PS. The actual length should be the same as the codeword generated from 128 bit-key. For example, if we use a (131,255) BCH code, we need a 255-bit PS string.

### Protocol Description

- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (depending on string length required).
- Both devices convert the PS analog signal to n-bit strings PS and PS'. These two strings should be very similar. 
- TX generates a random codeword c of length n using one kind of ECC. 
- TX computes δ = c xor PS to conceal PS. Then TX generates a random string s. 
- TX computes ECC(δ) and ECC(s). This is for deleting the channel error.
- TX sends ECC(δ) and ECC(s) to RX over the wireless channel. This channel includes air and human tissue, which might cause error. 
- RX received ECC(δ)' and ECC(s)'. RX decodes them to be δ and s.
- RX calculates c' = PS' xor δ = PS' xor c xor PS. c' should be different from c because of the inequality between PS and PS'.
- RX decodes c'. If the total error is tolerable, RX gets c' = c.
- RX calculates PS' = c' xor δ = PS.
- RX uses a strong random extractor (a SHA-256 could be used) to generate a uniformly distributed key. PS and s are the inputs of the extractor.

- Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX