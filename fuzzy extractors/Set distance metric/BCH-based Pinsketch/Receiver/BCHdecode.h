#define m 8         //m should be between 2 and 20
#define n 255       //n = 2**m -1
#define length 255  //length should follow: 2**(m-1) - 1 < length <= n
#define t 4         //error correcting capability
#define nummeasure 16

#include "stdint.h"

uint8_t	k, d, count;
uint8_t	p[m+1];		
int	alpha_to[n+1], index_of[n+1] , g[length];
uint8_t	recd[length], data[length], bb[length];
uint8_t PS[nummeasure], SSTX[t], SSRX[t], loc[t];
int s[2*t+1];

void read_p();
void generate_gf();
// void gen_poly();
void decode_bch();

