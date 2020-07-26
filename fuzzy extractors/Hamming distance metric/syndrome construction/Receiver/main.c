#include <stdio.h>
#include <stdint.h>
#include <string.h>

//parameter settings of a (n,k) RS-code. n, k means num of symbols
#define m  4    // RS code over GF(2^m), each sysmbol contains m bits 
#define n  15   // n = 2^m-1, length of codeword (symbols) 
#define k  11   // k = n-2*t, length of data (symbols) 
#define t  2   // number of symbol errors that can be corrected - error rate = t/n
#define iterationnum 128/(2*m*k) + 1

//GF(2^m)
//poly form
uint8_t alpha_to [n+1] = {1, 2, 4, 8, 3, 6, 12, 11, 5, 10, 7, 14, 15, 13, 9, 0};
//index form
int8_t index_of [n+1] = {-1, 0, 1, 4, 2, 8, 5, 10, 3, 14, 9, 7, 6, 13, 11, 12};

//define the operations for sha-256  
#define SHFR(x, times) (((x) >> (times)))
#define ROTR(x, times) (((x) >> (times)) | ((x) << ((sizeof(x) << 3) - (times))))
#define ROTL(x, times) (((x) << (times)) | ((x) >> ((sizeof(x) << 3) - (times))))
#define CHX(x, y, z) (((x) &  (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) &  (y)) ^ ( (x) & (z)) ^ ((y) & (z)))
#define BSIG0(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define BSIG1(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SSIG0(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SSIG1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))
#define SHA256_BLOCK_SIZE (512/8)
#define SHA256_COVER_SIZE (SHA256_BLOCK_SIZE*2)
//define the operations for sha-256 
static uint32_t inisett[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

//calculate the syndrome of a codeword[n]
void getsyndrome(int8_t syndrome[], int8_t codeword[]){
    
    uint8_t i, j; 

    i = 0;
    while(i != n){
        codeword[i] = index_of[codeword[i]]; 
        i++;
    }

    syndrome[0] = 0;

    i = 1;
    while(i != n-k+1){
        syndrome[i] = 0;
        j = 0;
        while(j != n){
            if(codeword[j] != -1){
                syndrome[i] ^= alpha_to[(codeword[j]+i*j)%n];  
            }
            j++;
        }
        i++;
    }

}

//input: s[] = syndrome(TX) xor syndrome(RX), (maybe malformed) codeword[n]. This function will correct the codeword
void correcterrors(int8_t s[], int8_t codeword[], uint8_t errorflag){

   uint8_t i,j,u,q ;
   int8_t elp[n-k+2][n-k], d[n-k+2], l[n-k+2], u_lu[n-k+2];
   int8_t count=0, root[t], loc[t], z[t+1], err[n], reg[t+1] ;

    // if there're errors in the codeword
    if(errorflag){

      i = 1;
      while(i != n-k+1){
        s[i] = index_of[s[i]];
        i++;
      }

      d[0] = 0 ;           /* index form */
      d[1] = s[1] ;        /* index form */
      elp[0][0] = 0 ;      /* index form */
      elp[1][0] = 1 ;      /* polynomial form */
      i = 1;
      while(i != n-k){
        elp[0][i] = -1 ;   /* index form */
        elp[1][i] = 0 ;   /* polynomial form */
        i++;
      }
      l[0] = 0 ;
      l[1] = 0 ;
      u_lu[0] = -1 ;
      u_lu[1] = 0 ;
      u = 0 ;

      do{
        u++ ;
        if (d[u] == -1){
          l[(uint8_t)(u+1)] = l[u] ;

          i = 0;
          while(i != l[u]+1){
            elp[(uint8_t)(u+1)][i] = elp[u][i];
            elp[u][i] = index_of[(uint8_t)elp[u][i]];
            i++;
          }
        }else{
            /* search for words with greatest u_lu[q] for which d[q]!=0 */
          q = u-1 ;
          while ((d[q]==-1) && (q>0)){
            q--;
          }
          /* have found first non-zero d[q]  */
          if (q>0){ 
            j = q;
            do{ 
              j--;
              if((d[j] != -1) && (u_lu[q] < u_lu[j])){
                q = j;
              }
            }while(j>0);
          }

          /* have now found q such that d[u]!=0 and u_lu[q] is maximum */
          /* store degree of new elp polynomial */
          if (l[u] > l[q]+u-q){
            l[(uint8_t)(u+1)] = l[u] ;
          }
          else{
            l[(uint8_t)(u+1)] = l[q]+u-q ;
          }

          i = 0;
          while(i != n-k){
            elp[(uint8_t)(u+1)][i] = 0;
            i++;
          }

          i = 0;
          while(i != l[q]+1){
            if (elp[q][i] != -1){
              elp[(uint8_t)(u+1)][(uint8_t)(i+u-q)] = alpha_to[(d[u]+n-d[q]+elp[q][i])%n] ;
            }
            i++;
          }

          i = 0;
          while(i != l[u]+1){
            elp[(uint8_t)(u+1)][i] ^= elp[u][i];
            elp[u][i] = index_of[(uint8_t)elp[u][i]];  /*convert old elp value to index*/
            i++;
          }
        }
            
        u_lu[(uint8_t)(u+1)] = u-l[(uint8_t)(u+1)];

        /* form (u+1)th discrepancy */
        if(u<n-k){    /* no discrepancy computed on last iteration */
          if(s[(uint8_t)(u+1)] != -1){
            d[(uint8_t)(u+1)] = alpha_to[(uint8_t)s[(uint8_t)(u+1)]];
          }else{
             d[(uint8_t)(u+1)] = 0;
          }

          i = 1;
          while(i != l[(uint8_t)(u+1)]+1){
            if ((s[(uint8_t)(u+1-i)]!=-1) && (elp[(uint8_t)(u+1)][i]!=0)){
              d[(uint8_t)(u+1)] ^= alpha_to[(s[(uint8_t)(u+1-i)]+index_of[(uint8_t)elp[(uint8_t)(u+1)][i]])%n];
            }
            i++;
          }
          d[(uint8_t)(u+1)] = index_of[(uint8_t)d[(uint8_t)(u+1)]];    /* put d[u+1] into index form */
        }

      }while ((u<n-k) && (l[(uint8_t)(u+1)] <= t));

      u++;
      if(l[u]<=t){         /* can correct error */
        /* put elp into index form */
        i = 0;
        while(i != l[u]+1){
          elp[u][i] = index_of[(uint8_t)elp[u][i]];
          i++;
        }

        /* find roots of the error location polynomial */
        i = 1;
        while(i != l[u]+1){
          reg[i] = elp[u][i];
          i++;
        }
        count = 0;

        i = 1;
        while(i != n+1){
          q = 1;
          j = 1;
          while(j != l[u]+1){
            if (reg[j]!=-1){
              reg[j] = (reg[j]+j)%n ;
              q ^= alpha_to[(uint8_t)reg[j]];
            }
            j++;
          }
          if(!q){        /* store root and error location number indices */
            root[count] = i;
            loc[count] = n-i;
            count++;
          }
          i++;
        }

        if (count==l[u]){    /* no. roots = degree of elp hence <= t errors */        
        /* form polynomial z(x) */
          i = 1;
          while(i != l[u]+1){
            if ((s[i]!=-1) && (elp[u][i]!=-1)){
              z[i] = alpha_to[(uint8_t)s[i]] ^ alpha_to[(uint8_t)elp[u][i]] ;
            }else if ((s[i]!=-1) && (elp[u][i] == -1)){
              z[i] = alpha_to[(uint8_t)s[i]] ;
            }else if ((s[i]==-1) && (elp[u][i] != -1)){
              z[i] = alpha_to[(uint8_t)elp[u][i]] ;
            }else{
              z[i] = 0 ;
            }

            j = 1;
            while(j != i){
              if((s[j]!=-1) && (elp[u][(uint8_t)(i-j)]!=-1)){
                z[i] ^= alpha_to[(elp[u][(uint8_t)(i-j)] + s[j])%n] ;
              }
              j++;
            }
            z[i] = index_of[(uint8_t)z[i]] ;         /* put into index form */
            i++;
          }

        /* evaluate errors at locations given by error location numbers loc[i] */

          i = 0;
          while(i != n){
            err[i] = 0;
            if(codeword[i]!=-1){        /* convert codeword[] to polynomial form */
              codeword[i] = alpha_to[(uint8_t)codeword[i]] ;
            }else{
              codeword[i] = 0 ;
            }
            i++;
          }

          i = 0;
          while(i != l[u]){
            err[(uint8_t)loc[i]] = 1;       /* accounts for z[0] */

            j = 1;
            while(j != l[u]+1){
              if (z[j]!=-1){
                err[(uint8_t)loc[i]] ^= alpha_to[(z[j]+j*root[i])%n] ;
              }
              j++;
            }
            if(err[(uint8_t)loc[i]]!=0){
              err[(uint8_t)loc[i]] = index_of[(uint8_t)err[(uint8_t)loc[i]]] ;
              q = 0 ;     /* form denominator of error term */

              j = 0;
              while(j != l[u]){
                if(j!=i){
                  q += index_of[1^alpha_to[(loc[j]+root[i])%n]] ;
                }
                j++;
              }
              q = q % n ;
              err[(uint8_t)loc[i]] = alpha_to[(err[(uint8_t)loc[i]]-q+n)%n] ;
              codeword[(uint8_t)loc[i]] ^= err[(uint8_t)loc[i]] ;  /*codeword[i] must be in polynomial form */
            }
            i++;
          }
        }else{  /* no. roots != degree of elp => >t errors and cannot solve */
          i = 0;
          while(i != n){
            if(codeword[i]!=-1){
              codeword[i] = alpha_to[(uint8_t)codeword[i]]; /* convert codeword[] to polynomial form */
            }else{
              codeword[i] = 0; /* just output received codeword as is */
            }
            i++;
          }
        }
      }else{         /* elp has degree has degree >t hence cannot solve */
          i = 0;
          while(i != n){
            if(codeword[i]!=-1){
              codeword[i] = alpha_to[(uint8_t)codeword[i]]; /* convert codeword[] to polynomial form */
            }else{
              codeword[i] = 0; /* just output received codeword as is */
            }
            i++;
          }
      }
    }else{       /* no non-zero syndromes => no errors: output received codeword */
      i = 0;
      while(i != n){
        if(codeword[i]!=-1){
          codeword[i] = alpha_to[(uint8_t)codeword[i]]; /* convert codeword[] to polynomial form */
        }else{
          codeword[i] = 0; /* just output received codeword as is */
        }
        i++;
      }
    }
 }

 void transform(const uint8_t *msg, uint32_t *h){

    uint32_t w[64];
    uint32_t a0, b1, c2, d3, e4, f5, g6, h7;
    uint32_t t1, t2;

    uint8_t i, j = 0;

    i = 0;
    while(i != 16){
        w[i] = (uint32_t)msg[j]<<24 | (uint32_t)msg[(uint8_t)(j+1)]<<16 | (uint32_t)msg[(uint8_t)(j+2)]<<8 | msg[(uint8_t)(j+3)];
        j += 4;
        i++;
    }

    i = 16;
    while(i != 64){
        w[i] = SSIG1(w[(uint8_t)(i-2)])+w[(uint8_t)(i-7)]+SSIG0(w[(uint8_t)(i-15)])+w[(uint8_t)(i-16)];
        i++;
    }

    a0 = h[0];
    b1 = h[1];
    c2 = h[2];
    d3 = h[3];
    e4 = h[4];
    f5 = h[5];
    g6 = h[6];
    h7 = h[7];

    i = 0;
    while(i != 64){
        t1 = h7 + BSIG1(e4) + CHX(e4, f5, g6) + inisett[i] + w[i];
        t2 = BSIG0(a0) + MAJ(a0, b1, c2);
        h7 = g6;
        g6 = f5;
        f5 = e4;
        e4 = d3 + t1;
        d3 = c2;
        c2 = b1;
        b1 = a0;
        a0 = t1 + t2;
        i++;
    }


    h[0] += a0;
    h[1] += b1;
    h[2] += c2;
    h[3] += d3;
    h[4] += e4;
    h[5] += f5;
    h[6] += g6;
    h[7] += h7;
}

void sha256(const uint8_t *message, uint32_t len, uint32_t *sha)
{
    uint8_t *tmp = (uint8_t*)message;
    uint8_t  cover_data[SHA256_COVER_SIZE];
    uint32_t cover_size = 0;

    uint32_t i = 0;
    uint32_t blocknum = 0;
    uint32_t padlen = 0;
    uint32_t h[8];

    h[0] = 0x6a09e667;
    h[1] = 0xbb67ae85;
    h[2] = 0x3c6ef372;
    h[3] = 0xa54ff53a;
    h[4] = 0x510e527f;
    h[5] = 0x9b05688c;
    h[6] = 0x1f83d9ab;
    h[7] = 0x5be0cd19;

    memset(cover_data, 0x00, sizeof(uint8_t)*SHA256_COVER_SIZE);

    blocknum = len/SHA256_BLOCK_SIZE;
    padlen = len % SHA256_BLOCK_SIZE;

    if (padlen < 56 ) {
        cover_size = SHA256_BLOCK_SIZE;
    }else {
        cover_size = SHA256_BLOCK_SIZE*2;
    }

    if (padlen != 0) {
        memcpy(cover_data, tmp + (blocknum * SHA256_BLOCK_SIZE), padlen);
    }
    cover_data[padlen] = 0x80;
    cover_data[cover_size-4]  = ((len*8)&0xff000000) >> 24;
    cover_data[cover_size-3]  = ((len*8)&0x00ff0000) >> 16;
    cover_data[cover_size-2]  = ((len*8)&0x0000ff00) >> 8;
    cover_data[cover_size-1]  = ((len*8)&0x000000ff);

    i = 0;
    while(i != blocknum){
        transform(tmp, h);
        tmp += SHA256_BLOCK_SIZE;
        i++;
    }

    tmp = cover_data;
    blocknum = cover_size/SHA256_BLOCK_SIZE;

    i = 0;
    while(i != blocknum){
        transform(tmp, h);
        tmp += SHA256_BLOCK_SIZE;
        i++;
    }

    memcpy(sha, h, sizeof(uint32_t)*8);
}

//used for encoding when m = 4
void getsyndrome_m_4(uint8_t codeword[n], uint8_t syndrome[2*t+1]){

    int8_t codeword1[n], codeword2[n];
    int8_t syndrome1[2*t+1], syndrome2[2*t+1];

    uint8_t i;

    i = 0;
    while(i != n){
        codeword1[i] = (int8_t)(codeword[i] & 0x0f);
        codeword2[i] = (int8_t)((codeword[i] >> 4) & 0x0f);
        i++;
    }

    // getsyndrome(uint8_t syndrome[], int8_t codeword[])
    getsyndrome(syndrome1, codeword1);
    getsyndrome(syndrome2, codeword2);

    i = 0;
    while(i != 2*t+1){
        syndrome[i] = (uint8_t)((syndrome2[i] << 4) | syndrome1[i]);
        i++;
    }

}

//used for encoding when m = 4
void correcterr_m_4(uint8_t syndrome[2*t+1], uint8_t codeword[n]){

    int8_t codeword1[n], codeword2[n];
    int8_t syndrome1[2*t+1], syndrome2[2*t+1];
    uint8_t errflag1 = 0, errflag2 = 0;

    uint8_t i;

    i = 0;
    while(i != n){
        codeword1[i] = (int8_t)(codeword[i] & 0x0f);
        codeword2[i] = (int8_t)((codeword[i] >> 4) & 0x0f);
        codeword1[i] = index_of[codeword1[i]];   
        codeword2[i] = index_of[codeword2[i]];  
        i++;
    }

    i = 0;
    while(i != 2*t+1){
        syndrome1[i] = (int8_t)(syndrome[i] & 0x0f);
        syndrome2[i] = (int8_t)((syndrome[i] >> 4) & 0x0f);
        if(syndrome1[i] != 0){  //if all elements in syndrome[] are 0, it means there're no mismatches
          errflag1 = 1;
        }
        if(syndrome2[i] != 0){
          errflag2 = 1;
        }
        i++;
    }

    correcterrors(syndrome1, codeword1, errflag1);
    correcterrors(syndrome2, codeword2, errflag2);

    i = 0;
    while(i != n){
        codeword[i] = (uint8_t)((codeword2[i] << 4) | codeword1[i]);
        i++;
    }

}

// This is RX of a syndrome-based fuzzy extractor constructed by a (15,11) RS-code
// RX receives 2 strings from TX, which are 'ss[]' and 'randStr[]' below
int main(){

  uint8_t i;
  uint8_t ps[n] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //assume we get n ps measurements (uniformly distributed random) and each measurement is one byte. Should be different at each iteration
  //add some errors. The error tolerance depends on the ECC code used. 
  ps[0] ^= 0x11;
  ps[1] ^= 0x11;
  // ps[2] ^= 1;
  uint8_t randStr[n] = {89, 199, 163, 120, 43, 247, 16, 81, 250, 207, 227, 61, 123, 7, 102};
  int8_t ss[2*t+1] = {0, 2, 14, 11, 3};
  uint32_t key[8];               //the final key
  uint8_t syndrome[2*t+1];           //stores the syndrome of a codeword                                                                                                            

  //Get the syndrome of the codeword (regard the ps values as a codeword)
  getsyndrome_m_4(ps, syndrome);

  //calculate syn(pv) XOR syn(pv'). syn(pv) is the secure sketch ss[] sent from TX
  i = 0;
  while(i != 2*t+1){
    syndrome[i] ^= ss[i]; 
    i++;
  }

  //correct the mismatches on PS measurements at RX side
  correcterr_m_4(syndrome, ps);

  //Generate the 128-bit key
  //ps values xor random string - to ensure the key is totally random
  i = 0;
  while(i != n){
      ps[i] ^= randStr[i];
      i++;
  }

  //generate the uniformly distributed key using sha-256
  sha256(ps, n, key);

  //take the first 128 bits of sha-256 output as the key
  printf("The final 128-bit key:\n");
  for(i = 0; i < 4; i++){
      printf("%x", key[i]);
  }
  printf("\n");

}
