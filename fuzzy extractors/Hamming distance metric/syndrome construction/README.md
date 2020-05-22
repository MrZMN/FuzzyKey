# Fuzzy Extractors - syndrome construction

Based on the paper 'FUZZY EXTRACTORS: HOW TO GENERATE STRONG KEYS FROM BIOMETRICS AND OTHER NOISY DATA' by DODIS, etc. (2008 edited version)

### Assumptions
1. We could generate a hundreds-bit long string from the PS. The actual length should be the same as the codeword generated from 128 bit-key. For example, if we use a (131,255) BCH code, we need a 255-bit PS string.

### Protocol Description

- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (depending on string length required).
- Both devices convert the PS analog signal to n-bit strings PS and PS'. These two strings should be very similar. 
- TX choose one kind of ECC. Then calculates SS = syn(PS), which is the ECC syndrome of the PS. Then TX generates a random string s. 
- TX computes ECC(SS) and ECC(s). This is for deleting the channel error.
- TX sends ECC(SS) and ECC(s) to RX over the wireless channel. This channel includes air and human tissue, which might cause error.
- RX received ECC(SS)' and ECC(s)'. RX decodes them to be SS and s.
- RX calculates syn(PS'), then calculates syn(err) = syn(PS') - SS = syn(PS') - syn(PS). err is the difference between PS and PS'.
- RX calculates err based on syn(err). After this, gets PS = PS' xor err.
- RX uses a strong random extractor (a SHA-256 could be used) to generate a uniformly distributed key. PS and s are the inputs of the extractor.

- Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX