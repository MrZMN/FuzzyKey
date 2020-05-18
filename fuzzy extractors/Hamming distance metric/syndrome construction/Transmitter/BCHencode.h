#define m 8         //m should be between 2 and 20
#define n 255        //n = 2**m -1
#define length 255   //length should follow: 2**(m-1) - 1 < length <= n
#define t 18         //error correcting capability
#define k 131

#include "stdint.h"

int d;
int p[m+1], alpha_to[n+1], index_of[n+1], g[length];
// int recd[length], data[k], bb[length-k];

void read_p();
void generate_gf();
void gen_poly();
void encode_bch(int recd[n], int data[k], int bb[n-k]);
void initBCH();
