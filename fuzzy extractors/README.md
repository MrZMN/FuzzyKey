# Fuzzy Extractors

Based on the paper 'FUZZY EXTRACTORS: HOW TO GENERATE STRONG KEYS FROM BIOMETRICS AND OTHER NOISY DATA' by DODIS, etc. (2008 edited version)

### Protocol Description

Different from fuzzy commitment and fuzzy vault, fuzzy extractors don't aim to conceal and transmit the key itself in the transmission procedure. Indeed, its target is only to ensure both sides have the same PS, which is called 'secure sketch' in their work. Afterwards, both sides use the equal PS as the input of a strong random extractor, which outputs a uniformly distributed key. Besides, fuzzy extractor contains different constructions, which are based on different metrics of the PS.

The main protocol could be described briefly:
- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously. Some fuzzy extractors need PS in bit string form, while others just need the set format.
- TX does some operations to conceal the PS values (some methods like ECC needed).
- TX transmits the concealed PS and a random string to RX over the wireless channel. 
- RX receives the info. It provides PS' and does some operations to recover PS' = PS.
- RX uses a strong random extractor (a SHA-256 could be used) to generate a uniformly distributed key. PS and random string are the inputs of the extractor.

- Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX

The 'operations' mentioned above depend on the exact constructions.

### Security Level

Different from fuzzy commitment and fuzzy vault, fuzzy extractors don't use physiological signals to conceal the secret key. Instead, the key comes from the PS itself. The main idea is to recover the mismatch on the PS measurements at TX/RX, then exact uniformly distributed random key from the agreed PS.

Therefore, the security is ensured if the original entropy of PS measured by both sides is high enough. Precisely, the remaining entropy of agreed PS should be at least 128-bit, considering the entropy loss due to transmission/storage of public information.

For the Hamming-distance based fuzzy extractors, the original PS entropy is the same as the length of the extracted PS bit string. For the set-distance based fuzzy extractors, suppose the universe size is n, the set size is s, then the original entropy is log(n,s) with base 2, where (n,s) stands for the number of combinations when taking random s elements from n elements without putting them back.
