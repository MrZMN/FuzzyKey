#include <stdio.h>
#include <stdint.h>

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

//RS decoding: codeword[n] (might be malformed) as input. This function will correct it. In each byte of codeword[], only the last m-bits are valid
void decode_rs(int8_t codeword[n]){
  uint8_t i, j, u, q, count=0, syn_error=0;
  int8_t elp[n-k+2][n-k], d[n-k+2], l[n-k+2], u_lu[n-k+2], s[n-k+1] ;
  int8_t root[t], loc[t], z[t+1], err[n], reg[t+1] ;

  /* put codeword[i] into index form */
  i = 0;
  while(i != n){
    codeword[i] = index_of[(uint8_t)codeword[i]] ;
    i++;
  }

  /* first form the syndromes */
  i = 1;
  while(i != n-k+1){
    s[i] = 0;
    j = 0;
    while(j != n){
      if(codeword[j] != -1){
        s[i] ^= alpha_to[(codeword[j]+i*j)%n];      /* codeword[j] in index form */
      }
      j++;
    }
    if(s[i] != 0){
      syn_error = 1;        /* set flag if non-zero syndrome => error */
    }
    s[i] = index_of[(uint8_t)s[i]];
    i++;
  }

  // if there're errors in the codeword
  if(syn_error){
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

//used for decoding when m = 4
void decode_m_4(uint8_t codeword[n]){

  int8_t codeword1[n], codeword2[n];

  uint8_t i;

  i = 0;
  while(i != n){
    codeword1[i] = (int8_t)(codeword[i] & 0x0f);
    codeword2[i] = (int8_t)((codeword[i] >> 4) & 0x0f);
    i++;
  }

  decode_rs(codeword1);
  decode_rs(codeword2);

  i = 0;
  while(i != n){
    codeword[i] = (uint8_t)((codeword2[i] << 4) | codeword1[i]);
    i++;
  }

}

// This is RX of a fuzzy commitment constructed by a (15,11) RS-code
// RX receives 1 array from TX, which is 'commitment[]' below 
int main(){

  uint8_t i, iteration;
  uint8_t key[8];

  //In order to share 128-bit key, there will be 'iterationnum' times of operations needed
  iteration = 0;
  while(iteration != iterationnum){

    uint8_t ps[n] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //assume we get n ps measurements (uniformly distributed random) and each measurement is one byte. Should be different at each iteration
    //add some errors
    // ps[0] ^= 1;
    ps[n/2] ^= 1;
    ps[n-1] ^= 1;

    uint8_t commitment[n] = {18, 131, 107, 233, 16, 168, 49, 194, 248, 150, 113, 249, 13, 14, 15}; //received from TX. Should be different at each iteration

    //codeword = commitment xor ps. If ps values at TX/RX sides mismatch, there'll be errors in codeword
    i = 0;
    while(i != n){
      commitment[i] ^= ps[i];
      i++;
    }

    //decode the codeword
    decode_m_4(commitment);

    //take the key from the codeword
    i = 0;
    while(i != 8){
      key[i] = commitment[i+n-k];
      i++;
    }

    printf("Key:\n");
    for(i = 0; i < 8; i++){
      printf("%d, ", key[i]);
    }
    printf("\n");

    iteration++;
  }
  

}

