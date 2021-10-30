#include <msp430fr5969.h>
#include <stdint.h>
#include "aes256.h"

#define iterationtime 100   //We test the energy consumption of 100 consecutive executions, then take the average

#define constant_time_RS_31_29_1    //constant-time protection on RS(31,29,1) variant
#define constant_time_RS_15_13_1    //constant-time protection on RS(15,13,1) variant
#define constant_time_RS_31_27_2    //constant-time protection on RS(31,27,2) variant
#define constant_time_RS_63_49_7    //constant-time protection on RS(63,49,7) variant

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


#if defined(constant_time_RS_31_29_1) || defined(constant_time_RS_31_27_2) 

//Barrett Reduction for constant-time modular 31
uint8_t mod31(int16_t divisor){
    int16_t as = divisor >> 5;
    divisor -= as * 31;
    divisor -= 31 & (-(1 ^ ((uint16_t)(divisor-31)) >> 15));
    return divisor;
}

#elif defined constant_time_RS_15_13_1

//Barrett Reduction for constant-time modular 15
uint8_t mod15(int16_t divisor){
  int16_t as = divisor >> 4;
  divisor -= as * 15;
  divisor -= 15 & (-(1 ^ ((uint16_t)(divisor-15)) >> 15));
  return divisor;
}

#elif defined constant_time_RS_63_49_7

//Barrett Reduction for constant-time modular 63
uint8_t mod63(int16_t divisor){
    int16_t as = divisor >> 6;
    divisor -= as * 63;
    divisor -= 63 & (-(1 ^ ((uint16_t)(divisor-63)) >> 15));
    return divisor;
}

#endif


//calculate the secure sketch
void getsyndrome(uint8_t syndrome[], uint8_t codeword[]){

    uint8_t i, j;
    int8_t temp[n];

    i = 0;
    while(i != n){
        temp[i] = index_of[codeword[i]];
        i++;
    }

    syndrome[0] = 0;

    i = 1;
    while(i != n-k+1){
        syndrome[i] = 0;
        j = 0;
        while(j != n){

            //constant-time protection
            #if defined(constant_time_RS_31_29_1) || defined(constant_time_RS_31_27_2) 
            syndrome[i] ^= alpha_to[mod31((temp[j]+i*j) & (-(1^(uint16_t)temp[j] >> 15)))] & (-(1 ^ ((uint16_t)temp[j]) >> 15));

            #elif defined constant_time_RS_15_13_1
            syndrome[i] ^= alpha_to[mod15((temp[j]+i*j) & (-(1^(uint16_t)temp[j] >> 15)))] & (-(1 ^ ((uint16_t)temp[j]) >> 15));  

            #elif defined constant_time_RS_63_49_7
            syndrome[i] ^= alpha_to[mod63((temp[j]+i*j) & (-(1^(uint16_t)temp[j] >> 15)))] & (-(1 ^ ((uint16_t)temp[j]) >> 15));   

            //without constant-time protection
            #else
            
            if(temp[j] != -1){
                syndrome[i] ^= alpha_to[mod(temp[j]+i*j, n)];
            }

            #endif

            j++;
        }
        i++;
    }
}

//Hirose
void hirose(uint8_t hash[32], uint8_t data[], uint8_t datalength){

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

int main(){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    // Configure all port pins as output low to save power
    PAOUT = 0; PBOUT = 0; PJOUT = 0;
    PADIR = 0xFFFF; PBDIR = 0xFFFF; PJDIR = 0xFF;
    PAIFG = 0; PBIFG = 0;
    PM5CTL0 &= ~LOCKLPM5;       // Disable GPIO power-on default high-impedance mode to activate previously configured port settings

    
    uint16_t ite = 0;
    uint8_t i;

    //Extracted PS bit string (uniformly distributed random). Assume it's composed of all 1s.
    uint8_t ps[n];
    i = 0;
    while(i != n){
        ps[i] = n;
        i++;
    }

    __no_operation();                         // SET A BREAKPOINT HERE
    while(ite != iterationtime * exectime){

        //public information sent in air
        uint8_t securesketch[2*t+1];

        //calculate the syndrome
        getsyndrome(securesketch, ps);


        //Strong extractor
        //hash result is 256-bit
        uint8_t hash[32];

        hirose(hash, ps, n);

        ite++;
    }
}
