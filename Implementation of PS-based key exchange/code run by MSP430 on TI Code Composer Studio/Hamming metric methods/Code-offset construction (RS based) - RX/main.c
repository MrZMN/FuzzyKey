#include <msp430fr5969.h>
#include <stdint.h>
#include "aes256.h"

#define iterationtime 100   //We test the energy consumption of 100 consecutive executions, then take the average

//RS code configurations//All settings in RS_n_k_t format, where t is error tolerance in symbol. The maximum tolerable mismatch rate = t/n

//error tolerance = 2%
#define RS_31_29_1

//error tolerance = 5%
#define RS_15_13_1
#define RS_31_27_2

//error tolerance = 10%
#define RS_63_49_7


#ifdef RS_31_29_1

#define exectime 1  //the number of executions needed for this construction
#define n  31       //n = 2^m-1, length of codeword (symbols)
#define k  29       //k = n-2*t, length of data (symbols)
#define t  1
//GF tables
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
int8_t index_of[n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};

#elif defined RS_15_13_1

#define exectime 3  //the number of executions needed for this construction
#define n  15   //n = 2^m-1, length of codeword (symbols)
#define k  13   //k = n-2*t, length of data (symbols)
#define t  1
//GF tables
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 3, 6, 12, 11, 5, 10, 7, 14, 15, 13, 9, 0};
int8_t index_of[n+1] = {-1, 0, 1, 4, 2, 8, 5, 10, 3, 14, 9, 7, 6, 13, 11, 12};

#elif defined RS_31_27_2

#define exectime 1  //the number of executions needed for this construction
#define n  31   //n = 2^m-1, length of codeword (symbols)
#define k  27   //k = n-2*t, length of data (symbols)
#define t  2
//GF tables
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
int8_t index_of[n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};

#elif defined RS_63_49_7

#define exectime 1  //the number of executions needed for this construction
#define n  63   //n = 2^m-1, length of codeword (symbols)
#define k  49   //k = n-2*t, length of data (symbols)
#define t  7
//GF tables
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24, 48, 35, 5, 10, 20, 40, 19, 38, 15, 30, 60, 59, 53, 41, 17, 34, 7, 14, 28, 56, 51, 37, 9, 18, 36, 11, 22, 44, 27, 54, 47, 29, 58, 55, 45, 25, 50, 39, 13, 26, 52, 43, 21, 42, 23, 46, 31, 62, 63, 61, 57, 49, 33, 0};
int8_t index_of[n+1] = {-1, 0, 1, 6, 2, 12, 7, 26, 3, 32, 13, 35, 8, 48, 27, 18, 4, 24, 33, 16, 14, 52, 36, 54, 9, 45, 49, 38, 28, 41, 19, 56, 5, 62, 25, 11, 34, 31, 17, 47, 15, 23, 53, 51, 37, 44, 55, 40, 10, 61, 46, 30, 50, 22, 39, 43, 29, 60, 42, 21, 20, 59, 57, 58};

#endif

//put modular operation into RAM
#pragma CODE_SECTION(mod, ".run_from_ram_mod")
uint16_t mod(uint16_t num1, uint16_t num2){
    return num1%num2;
}

//RS decoding (Simon Rockliff's implementation of Berlekamp's algorithm)
void decode_rs(int8_t codeword[n]){
    uint8_t i, j, u, q, count=0, syn_error=0;
    int8_t elp[n-k+2][n-k], d[n-k+2], l[n-k+2], u_lu[n-k+2], s[n-k+1];
    int8_t root[t], loc[t], z[t+1], err[n], reg[t+1];

    i = 0;
    while(i != n){
        codeword[i] = index_of[(uint8_t)(codeword[i])];
        i++;
    }

    i = 1;
    while(i != n-k+1){
        s[i] = 0;
        j = 0;
        while(j != n){
            s[i] ^= alpha_to[mod(codeword[j]+i*j, n)] & (-(1 ^ ((unsigned int)codeword[j]) >> (sizeof(unsigned int) * 8 - 1)));
            j++;
        }
        if(s[i] != 0){
            syn_error = 1;
        }
        s[i] = index_of[(uint8_t)(s[i])];
        i++;
    }

    if(syn_error){
        d[0] = 0;
        d[1] = s[1];
        elp[0][0] = 0;
        elp[1][0] = 1;
        i = 1;
        while(i != n-k){
            elp[0][i] = -1;
            elp[1][i] = 0;
            i++;
        }
        l[0] = 0;
        l[1] = 0;
        u_lu[0] = -1;
        u_lu[1] = 0;
        u = 0;

        do{
            u++;
            if (d[u] == -1){
                l[(uint8_t)(u+1)] = l[u];

                i = 0;
                while(i != l[u]+1){
                    elp[(uint8_t)((u+1))][i] = elp[u][i];
                    elp[u][i] = index_of[(uint8_t)(elp[u][i])];
                    i++;
                }
            }else{
                q = u-1;
                while ((d[q]==-1) && (q>0)){
                    q--;
                }
                if (q>0){
                    j = q;
                    do{
                        j--;
                        if((d[j] != -1) && (u_lu[q] < u_lu[j])){
                            q = j;
                        }
                    }while(j>0);
                }

                if (l[u] > l[q]+u-q){
                    l[(uint8_t)(u+1)] = l[u];
                }
                else{
                    l[(uint8_t)(u+1)] = l[q]+u-q;
                }

                i = 0;
                while(i != n-k){
                    elp[(uint8_t)(u+1)][i] = 0;
                    i++;
                }

                i = 0;
                while(i != l[q]+1){
                    if (elp[q][i] != -1){
                        elp[(uint8_t)(u+1)][(uint8_t)(i+u-q)] = alpha_to[mod(d[u]+n-d[q]+elp[q][i], n)];
                    }
                    i++;
                }

                i = 0;
                while(i != l[u]+1){
                    elp[(uint8_t)(u+1)][i] ^= elp[u][i];
                    elp[u][i] = index_of[(uint8_t)(elp[u][i])];
                    i++;
                }
            }

            u_lu[(uint8_t)(u+1)] = u-l[(uint8_t)(u+1)];

            if(u<n-k){
                if(s[(uint8_t)(u+1)] != -1){
                    d[(uint8_t)(u+1)] = alpha_to[(uint8_t)(s[(uint8_t)(u+1)])];
                }else{
                    d[(uint8_t)(u+1)] = 0;
                }

                i = 1;
                while(i != l[(uint8_t)(u+1)]+1){
                    if ((s[(uint8_t)(u+1-i)]!=-1) && (elp[(uint8_t)(u+1)][i]!=0)){
                        d[(uint8_t)(u+1)] ^= alpha_to[mod(s[(uint8_t)(u+1-i)]+index_of[(uint8_t)(elp[(uint8_t)(u+1)][i])], n)];
                    }
                    i++;
                }
                d[(uint8_t)(u+1)] = index_of[(uint8_t)(d[(uint8_t)(u+1)])];
            }

        }while ((u<n-k) && (l[(uint8_t)(u+1)] <= t));

        u++;
        if(l[u]<=t){
            i = 0;
            while(i != l[u]+1){
                elp[u][i] = index_of[(uint8_t)(elp[u][i])];
                i++;
            }

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
                        reg[j] = mod(reg[j]+j, n);
                        q ^= alpha_to[(uint8_t)(reg[j])];
                    }
                    j++;
                }
                if(!q){
                    root[count] = i;
                    loc[count] = n-i;
                    count++;
                }
                i++;
            }

            if (count==l[u]){
                i = 1;
                while(i != l[u]+1){
                    if ((s[i]!=-1) && (elp[u][i]!=-1)){
                        z[i] = alpha_to[(uint8_t)(s[i])] ^ alpha_to[(uint8_t)(elp[u][i])];
                    }else if ((s[i]!=-1) && (elp[u][i] == -1)){
                        z[i] = alpha_to[(uint8_t)(s[i])];
                    }else if ((s[i]==-1) && (elp[u][i] != -1)){
                        z[i] = alpha_to[(uint8_t)(elp[u][i])];
                    }else{
                        z[i] = 0;
                    }

                    j = 1;
                    while(j != i){
                        if((s[j]!=-1) && (elp[u][(uint8_t)(i-j)]!=-1)){
                            z[i] ^= alpha_to[mod(elp[u][(uint8_t)(i-j)] + s[j], n)];
                        }
                        j++;
                    }
                    z[i] = index_of[(uint8_t)(z[i])];
                    i++;
                }

                i = 0;
                while(i != n){
                    err[i] = 0;
                    codeword[i] = alpha_to[(uint8_t)(codeword[i])] & (-(1 ^ ((unsigned int)codeword[j]) >> (sizeof(unsigned int) * 8 - 1)));
                    i++;
                }

                i = 0;
                while(i != l[u]){
                    err[(uint8_t)(loc[i])] = 1;

                    j = 1;
                    while(j != l[u]+1){
                        if (z[j]!=-1){
                            err[(uint8_t)(loc[i])] ^= alpha_to[mod(z[j]+j*root[i], n)];
                        }
                        j++;
                    }
                    if(err[(uint8_t)(loc[i])]!=0){
                        err[(uint8_t)(loc[i])] = index_of[(uint8_t)(err[(uint8_t)(loc[i])])];
                        q = 0;

                        j = 0;
                        while(j != l[u]){
                            if(j!=i){
                                q += index_of[(uint8_t)(1^alpha_to[mod(loc[j]+root[i], n)])];
                            }
                            j++;
                        }
                        q = mod(q, n);
                        err[(uint8_t)(loc[i])] = alpha_to[mod(err[(uint8_t)(loc[i])]-q+n, n)];
                        codeword[(uint8_t)(loc[i])] ^= err[(uint8_t)(loc[i])];
                    }
                    i++;
                }
            }else{
                i = 0;
                while(i != n){
                    codeword[i] = alpha_to[(uint8_t)(codeword[i])] & (-(1 ^ ((unsigned int)codeword[j]) >> (sizeof(unsigned int) * 8 - 1)));
                    i++;
                }
            }
        }else{
            i = 0;
            while(i != n){
                codeword[i] = alpha_to[(uint8_t)(codeword[i])] & (-(1 ^ ((unsigned int)codeword[j]) >> (sizeof(unsigned int) * 8 - 1)));
                i++;
            }
        }
    }else{
        i = 0;
        while(i != n){
            codeword[i] = alpha_to[(uint8_t)(codeword[i])] & (-(1 ^ ((unsigned int)codeword[j]) >> (sizeof(unsigned int) * 8 - 1)));
            i++;
        }
    }
}

void hirose(uint8_t hash[32], int8_t data[], uint8_t datalength){

    uint8_t i, j;
    uint8_t H[16], G[16], key[32], AEScipher[16];

    i = 0;
    while(i != 16){
        H[i] = 0;
        G[i] = 0;
        i++;
    }

    i = 0;
    while(i != 2){      //number of rounds. The input length of each round is 16 bytes
        //update the 256-bit key
        j = 0;
        while(j != 16){
            key[j] = H[j];
            if(j+i*16 < datalength){
                key[(uint8_t)(j+16)] = data[(uint8_t)(j+i*16)];
            }else{
                key[(uint8_t)(j+16)] = 0;
            }
            j++;
        }

        AES256_setCipherKey(AES256_BASE, key, AES256_KEYLENGTH_256BIT);

        //Encrypt
        j = 0;
        while(j != 16){
            H[j] = G[j] ^ 0xFF;
            j++;
        }
        AES256_encryptData(AES256_BASE, H, AEScipher);
        //update H
        j = 0;
        while(j != 16){
            H[j] ^= AEScipher[j];
            j++;
        }

        //Encrypt
        AES256_encryptData(AES256_BASE, G, AEScipher);
        //update G
        j = 0;
        while(j != 16){
            G[j] ^= AEScipher[j];
            j++;
        }
        i++;
    }

    //conbine the arrays to get the result
    i = 0;
    while(i != 16){
        hash[i] = H[i];
        hash[(uint8_t)(i+16)] = G[i];
        i++;
    }
}

//RX of fuzzy commitment
int main(){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    // Configure all port pins as output low to save power
    PAOUT = 0; PBOUT = 0; PJOUT = 0;
    PADIR = 0xFFFF; PBDIR = 0xFFFF; PJDIR = 0xFF;
    PAIFG = 0; PBIFG = 0;
    PM5CTL0 &= ~LOCKLPM5;       // Disable GPIO power-on default high-impedance mode to activate previously configured port settings


    uint16_t ite = 0;
    uint8_t i;

    //Extracted PS bit string. Assume it's composed of all 1s. (please select your PS type and extraction method)
    int8_t ps[n];                  //extracted PS bit string
    i = 0;
    while(i != n){
        ps[i] = n;
        i++;
    }

    //add some mismatches to the PS bit string (for test only)
    i = 0;
    while(i != t){
        ps[i] ^= 1;
        i++;
    }

    __no_operation();                         // SET A BREAKPOINT HERE
    while(ite!= iterationtime * exectime){

        //Received fuzzy commitment from TX
        int8_t securesketch[n];

        //reveal the (maybe malformed) codeword
        i = 0;
        while(i != n){
            ps[i] ^= securesketch[i];
            i++;
        }

        //Decode the RS codeword
        decode_rs(ps);

        //reveal the ps
        i = 0;
        while(i != n){
            ps[i] ^= securesketch[i];
            i++;
        }

        //Strong extractor
        uint8_t hash[32];           //output of the hash

        //for RS code based methods, we directly use the ps as the input of hash
        //hash
        hirose(hash, ps, n);

        __no_operation();                         // SET A BREAKPOINT HERE
        ite++;
    }
}
