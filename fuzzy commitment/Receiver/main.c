#include <time.h>
#include <stdio.h>
#include <stdlib.h>

//parameter settings of a (n,k) RS-code
#define m  5		// RS code over GF(2^m), each sysmbol contains m bits 
#define n  31		// n = 2^m-1, length of codeword (symbols) 
#define t  2 		// number of symbol errors that can be corrected
#define k  27		// k = n-2*t, length of data (symbols) 
#define pslength m*n/8+1	//20
#define keylength m*k/8+1	//17

//GF(2^m)
int alpha_to [n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
int index_of [n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};

void decode_rs(int []);

// This is a fuzzy commitment constructed by a (31,27) RS-code
// RX receives 1 string from TX, which is 20 bytes 'XORstring' below 
int main(){

	srand((unsigned)time(NULL));	//for random number generation

	register int i, j;
	int bytepos, bitpos;
	int idx = 0;
				
	int key[keylength];			//stores the random encryption key			
	int data[k], codeword[n];	//For ECC coding usage
	int ps[pslength] = {209, 13, 81, 222, 10, 247, 222, 236, 110, 103, 71, 208, 153, 193, 164, 125, 192, 74, 241, 160};	//stores the ps measurements
	int ecc[pslength] = {9, 35, 204, 123, 24, 47, 198, 46, 241, 7, 201, 222, 129, 169, 165, 5, 230, 166, 123, 64};		//stores the tansmitted info from TX->RX

	//errors. The error tolerance depends on the ECC code used. For (31,27)RS code, at most 2 consecutive 5-bit error (93.5% ps matching)
	ps[0] ^= 1;
	ps[10] ^= 1;

	//PS(mismatch with the one in TX side) xor codeword
	for(i = 0; i < pslength; i++){
		ecc[i] ^= ps[i];
	}

	//For ECC decoding only. Convert the codeword from 20 bytes(8 bit each) -> 31 symbols(5 bits each)
	for(i = 0; i < n; i ++){
		codeword[i] = 0;
		for(j = 0; j < m; j++){
			idx = m*i + j;
			bytepos = idx / 8;
			bitpos = idx % 8;
			codeword[i] <<= 1;
			codeword[i] ^= ((ecc[bytepos] & (0x80 >> bitpos)) >> (7 - bitpos));
		}
	}

	//Decode the RS code
	decode_rs(codeword);

	//Convert the 31 symbols(5 bits each) -> 20 bytes(8 bits each)
	for(i = 0; i < pslength; i++){
		ecc[i] = 0;
		for(j = 0; j < 8; j++){
			idx = i*8 + j;
			bytepos = idx / m;
			bitpos = idx % m;
			ecc[i] <<= 1;
			if(bytepos < n){
				ecc[i] ^= (codeword[bytepos] & (0x10 >> bitpos)) >> (4 - bitpos); 
			}
		}
	}

	//Get the 128-bit key
	for(i = 0; i < 16; i++){
		key[i] = ((ecc[t*m/4 + i] & 0xf) << 4) | ((ecc[t*m/4 + i + 1] & 0xf0) >> 4);
		// if(key[i] <= 0xf){
		// 	printf("0%x", key[i]);
		// }else{
		// 	printf("%x", key[i]);
		// }
		printf("%d, ", key[i]);
	}

}

void decode_rs(int recd[n]){
   register int i,j,u,q ;
   int elp[n-k+2][n-k], d[n-k+2], l[n-k+2], u_lu[n-k+2], s[n-k+1] ;
   int count=0, syn_error=0, root[t], loc[t], z[t+1], err[n], reg[t+1] ;

     for (i=0; i<n; i++)
     recd[i] = index_of[recd[i]] ;          /* put recd[i] into index form */

/* first form the syndromes */
   for (i=1; i<=n-k; i++)
    { s[i] = 0 ;
      for (j=0; j<n; j++)
        if (recd[j]!=-1)
          s[i] ^= alpha_to[(recd[j]+i*j)%n] ;      /* recd[j] in index form */
/* convert syndrome from polynomial form to index form  */
      if (s[i]!=0)  syn_error=1 ;        /* set flag if non-zero syndrome => error */
      s[i] = index_of[s[i]] ;
    } ;

   if (syn_error){
      d[0] = 0 ;           /* index form */
      d[1] = s[1] ;        /* index form */
      elp[0][0] = 0 ;      /* index form */
      elp[1][0] = 1 ;      /* polynomial form */
      for (i=1; i<n-k; i++)
        { elp[0][i] = -1 ;   /* index form */
          elp[1][i] = 0 ;   /* polynomial form */
        }
      l[0] = 0 ;
      l[1] = 0 ;
      u_lu[0] = -1 ;
      u_lu[1] = 0 ;
      u = 0 ;

      do
      {
        u++ ;
        if (d[u]==-1)
          { l[u+1] = l[u] ;
            for (i=0; i<=l[u]; i++)
             {  elp[u+1][i] = elp[u][i] ;
                elp[u][i] = index_of[elp[u][i]] ;
             }
          }
        else
/* search for words with greatest u_lu[q] for which d[q]!=0 */
          { q = u-1 ;
            while ((d[q]==-1) && (q>0)) q-- ;
/* have found first non-zero d[q]  */
            if (q>0)
             { j=q ;
               do
               { j-- ;
                 if ((d[j]!=-1) && (u_lu[q]<u_lu[j]))
                   q = j ;
               }while (j>0) ;
             } ;

/* have now found q such that d[u]!=0 and u_lu[q] is maximum */
/* store degree of new elp polynomial */
            if (l[u]>l[q]+u-q)  l[u+1] = l[u] ;
            else  l[u+1] = l[q]+u-q ;

/* form new elp(x) */
            for (i=0; i<n-k; i++)    elp[u+1][i] = 0 ;
            for (i=0; i<=l[q]; i++)
              if (elp[q][i]!=-1)
                elp[u+1][i+u-q] = alpha_to[(d[u]+n-d[q]+elp[q][i])%n] ;
            for (i=0; i<=l[u]; i++)
              { elp[u+1][i] ^= elp[u][i] ;
                elp[u][i] = index_of[elp[u][i]] ;  /*convert old elp value to index*/
              }
          }
        u_lu[u+1] = u-l[u+1] ;

/* form (u+1)th discrepancy */
        if (u<n-k)    /* no discrepancy computed on last iteration */
          {
            if (s[u+1]!=-1)
                   d[u+1] = alpha_to[s[u+1]] ;
            else
              d[u+1] = 0 ;
            for (i=1; i<=l[u+1]; i++)
              if ((s[u+1-i]!=-1) && (elp[u+1][i]!=0))
                d[u+1] ^= alpha_to[(s[u+1-i]+index_of[elp[u+1][i]])%n] ;
            d[u+1] = index_of[d[u+1]] ;    /* put d[u+1] into index form */
          }
      } while ((u<n-k) && (l[u+1]<=t)) ;

      u++ ;
      if (l[u]<=t)         /* can correct error */
       {
/* put elp into index form */
         for (i=0; i<=l[u]; i++)   elp[u][i] = index_of[elp[u][i]] ;

/* find roots of the error location polynomial */
         for (i=1; i<=l[u]; i++)
           reg[i] = elp[u][i] ;
         count = 0 ;
         for (i=1; i<=n; i++)
          {  q = 1 ;
             for (j=1; j<=l[u]; j++)
              if (reg[j]!=-1)
                { reg[j] = (reg[j]+j)%n ;
                  q ^= alpha_to[reg[j]] ;
                } ;
             if (!q)        /* store root and error location number indices */
              { root[count] = i;
                loc[count] = n-i ;
                count++ ;
              };
          } ;
         if (count==l[u])    /* no. roots = degree of elp hence <= t errors */
          {
/* form polynomial z(x) */
           for (i=1; i<=l[u]; i++)        /* Z[0] = 1 always - do not need */
            { if ((s[i]!=-1) && (elp[u][i]!=-1))
                 z[i] = alpha_to[s[i]] ^ alpha_to[elp[u][i]] ;
              else if ((s[i]!=-1) && (elp[u][i]==-1))
                      z[i] = alpha_to[s[i]] ;
                   else if ((s[i]==-1) && (elp[u][i]!=-1))
                          z[i] = alpha_to[elp[u][i]] ;
                        else
                          z[i] = 0 ;
              for (j=1; j<i; j++)
                if ((s[j]!=-1) && (elp[u][i-j]!=-1))
                   z[i] ^= alpha_to[(elp[u][i-j] + s[j])%n] ;
              z[i] = index_of[z[i]] ;         /* put into index form */
            } ;

  /* evaluate errors at locations given by error location numbers loc[i] */
           for (i=0; i<n; i++)
             { err[i] = 0 ;
               if (recd[i]!=-1)        /* convert recd[] to polynomial form */
                 recd[i] = alpha_to[recd[i]] ;
               else  recd[i] = 0 ;
             }
           for (i=0; i<l[u]; i++)    /* compute numerator of error term first */
            { err[loc[i]] = 1;       /* accounts for z[0] */
              for (j=1; j<=l[u]; j++)
                if (z[j]!=-1)
                  err[loc[i]] ^= alpha_to[(z[j]+j*root[i])%n] ;
              if (err[loc[i]]!=0)
               { err[loc[i]] = index_of[err[loc[i]]] ;
                 q = 0 ;     /* form denominator of error term */
                 for (j=0; j<l[u]; j++)
                   if (j!=i)
                     q += index_of[1^alpha_to[(loc[j]+root[i])%n]] ;
                 q = q % n ;
                 err[loc[i]] = alpha_to[(err[loc[i]]-q+n)%n] ;
                 recd[loc[i]] ^= err[loc[i]] ;  /*recd[i] must be in polynomial form */
               }
            }
          }
         else    /* no. roots != degree of elp => >t errors and cannot solve */
           for (i=0; i<n; i++)        /* could return error flag if desired */
               if (recd[i]!=-1)        /* convert recd[] to polynomial form */
                 recd[i] = alpha_to[recd[i]] ;
               else  recd[i] = 0 ;     /* just output received codeword as is */
       }
     else         /* elp has degree has degree >t hence cannot solve */
       for (i=0; i<n; i++)       /* could return error flag if desired */
          if (recd[i]!=-1)        /* convert recd[] to polynomial form */
            recd[i] = alpha_to[recd[i]] ;
          else  recd[i] = 0 ;     /* just output received codeword as is */
    }
   else       /* no non-zero syndromes => no errors: output received codeword */
    for (i=0; i<n; i++)
       if (recd[i]!=-1)        /* convert recd[] to polynomial form */
         recd[i] = alpha_to[recd[i]] ;
       else  recd[i] = 0 ;
 }

