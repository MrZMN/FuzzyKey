# Fuzzy Vault

Based on the paper 'A Fuzzy Vault Scheme' by Juels & Sudan

### Assumptions
1. We could measure physiological siganls, such as Internal Pulse Intervals (IPI). Different from schemes like fuzzy commitment, we don't need to convert the PS into bit strings. 
2. The devices using this protocol are able to send/receive quite much information through wireless channel.

### Protocol Description
- TX and RX synchronise with each other, and measure the same PS for a period of time simultaneously.
- Both devices get a set containing a series of PS values, such as IPI times. Assume TX gets set A, RX gets set B. They should be really similar. The number of PS measurements in each set should be more than 8.
- TX generates a random 128 bit key, then devides the key into 8 pieces of 16 bits, regarding them as coefficients of a 7-degree polynomial p(x).
- TX projects the PS values in set A onto the polynomial p(x), resulting a series of (more than 8) legit points.
- TX generates a large number of chaff points to conceal the legit points. The chaff points (x,y) should meet: 1. x is different from PS values in set A. 2. y is not on the polynomial (y != p(x)). 
- TX mixes legit points and chaff points together randomly. 
- TX sends all the points to RX through a human body channel (air and tissue).
- RX receives all the points. 
- RX compares the PS values in set B with the x values of all received points. If matched, RX takes this point as a legit point.
- Whenever RX collects 8 legit points, it stops comparing and uses Lagrange Interpolation to reconstruct the polynomial. If there're less than 8 points matched, the key distribution fails.
- RX converts the polynomial coefficients into a 128 bit key by concatenating them together.

- Provement of agreement: send a hash from TX->RX, or send a MAC from RX->TX

### Adjustments Needed for Embedded Systems

According to the current codes, for each point (x,y), x is an int value (4 bytes in C) and y is a double value (8 bytes). This means each point corresponds to 12 bytes. Considering there'll be thousands of points to be sent/received for security concern, it's impossible for a 2KB memory embedded system to generate/send the points in one time. It's feasible to do this in batches, namely, TX generates and sends hundreds of points at a time for several times, and the same for RX.






