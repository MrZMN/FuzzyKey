#define m 8         //m should be between 2 and 20
#define n 255       //n = 2**m -1
#define length 255  //length should follow: 2**(m-1) - 1 < length <= n
#define t 4         //error correcting capability
#define nummeasure 16

#include "stdint.h"

uint8_t	p[m+1], alpha_to[n+1], index_of[n+1];
uint8_t PS[nummeasure], SS[t];

void read_p();
void generate_gf();


