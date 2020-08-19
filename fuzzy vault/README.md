# Fuzzy Vault

Based on the paper 'A Fuzzy Vault Scheme' by Juels & Sudan

### Assumptions
1. We could measure the PS values and generate a set based on them. Each set element should be different from each other.
2. The devices using this protocol are able to send/receive quite much information through wireless channel.

### Protocol Description
- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously (time depends on the set size required).
- TX/RX generate a set based PS measurements (suppose the set size is s) as PS and PS'. These two sets should be very similar. 
- TX generates a purely random 128-bit key, then devides the key into k equal-size sections, regarding them as coefficients of a (k-1)-degree polynomial p(x). 
- TX projects the s set elements onto p(x), resulting s legit points.
- TX generates a large number (>>s) of chaff points to conceal the legit points. The chaff points (x,y) should meet two conditions: 1. x is different from elements in PS. 2. y is not on the polynomial (y != p(x)). 
- TX mixes legit points and chaff points together randomly. 
- TX sends all the points to RX through the wireless channel.
- RX receives all the points. 
- RX compares the elements in PS' with all the x values of received points. If matched, RX regards this point as a legit point.
- After collecting all legit points, the key polynomial p(x) could be recovered using Berlecamp-Welch algorithm, if the mismatch rate between PS and PS' is tolerable.
- RX converts the polynomial coefficients into a 128-bit key by concatenating them together.

- Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX

![Image text](https://github.com/MrZMN/Implementation-of-PS-based-key-distribution-methods/blob/master/images/fuzzyvault.png)

### Details

- The error tolerance t means the max tolerable symmetric difference size between PS and PS'. For instance, the symmetric difference size between {1,2,3} and {2,3,4} is 2.
- In the above implementation, errors on wireless channel wasn't concerned.

### Adjustments Needed for Embedded Systems

According to the current codes, for each point (x,y), both x and y consist of 1 byte. This means each point corresponds to 2 bytes. Considering there'll be thousands of points to be sent/received for security concern, it's impossible for a 2KB memory embedded system to generate/send the points in one time. It's feasible to do this in batches, namely, TX generates and sends hundreds of points at a time for several times, and the same for RX.

### Security Level

Suppose the number of legit points is l, the number of chaff points is c, the number of key polynomial coefficients is p (polynomial degree is p-1). According to Berlekamp-Welch algorithm, the key polynomial could be successfully recovered if RX recognises at least (l+p)/2 legit points from all the points transmitted from TX.  

Suppose we use (a, b) to represent the number of combinations when choosing b elements randomly from a elements without puting back. Then the upper bound of the adversary's attempt should be: (l+c, (l+p)/2) / (l, (l+p)/2). For instance, we generate a 7-degree key polynomial, take 10 legit points (allow at most 10% mismatch rate on PS measurement) and 2000 chaff points at TX side, the security level should be at most: (2010, 9)/(10, 9), which is roughly about 2^77. The best way to increase security is to increase the number of chaff points.
