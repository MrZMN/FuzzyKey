#include <msp430fr5969.h>
#include <stdint.h>
#include <stdlib.h>
#include "rng.h"

#define iterationtime 10

//Fuzzy Vault Configurations//the settings are in FV_n_s_t format. The error tolerance is t/2s while t = s-keypolydeg-1

//error tolerance = 2%
#define FV_255_50_2

//error tolerance = 5%
#define FV_255_20_2

//error tolerance = 10%
#define FV_255_10_2


#ifdef FV_255_50_2

#define exectime 1      //the number of executions needed for this construction
#define keypolydeg 47   //the degree of key polynomial. key length = (keypolydeg+1)*log(n+1) (bit)
#define s 50            //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
#define chaff 205       //number of chaff points added for security
#define n 255
//The max tolerable mismatch rate = t/2s
//t = s-keypolydeg-1

#elif defined FV_255_20_2

#define exectime 1      //the number of executions needed for this construction
#define keypolydeg 17   //the degree of key polynomial. key length = (keypolydeg+1)*log(n+1) (bit)
#define s 20            //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
#define chaff 235       //number of chaff points added for security
#define n 255
//The max tolerable mismatch rate = t/2s
//t = s-keypolydeg-1

#elif defined FV_255_10_2

#define exectime 2      //the number of executions needed for this construction
#define keypolydeg 7    //the degree of key polynomial. key length = (keypolydeg+1)*log(n+1) (bit)
#define s 10            //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
#define chaff 245       //number of chaff points added for security
#define n 255
//The max tolerable mismatch rate = t/2s
//t = s-keypolydeg-1

#endif

//GF(2^8) tables
//polynomial form
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 0};
//index form
uint8_t index_of[n+1] = {255, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};

//put modular operation into RAM
#pragma CODE_SECTION(mod, ".run_from_ram_mod")
uint16_t mod(uint16_t num1, uint16_t num2){
    return num1%num2;
}

//check if 'value' is in set[]
uint8_t isinset(uint8_t value, uint8_t set[], uint8_t setsize){
    uint8_t i;
    uint8_t flag = 0;
    i = 0;
    while(i != setsize){
        if(value == set[i]){
            flag = 1;
        }
        i++;
    }
    return flag;
}

//project x -> y on the poly[]
uint8_t projectpoints(uint8_t x, uint8_t poly[], uint8_t degree){
    uint8_t i;
    uint8_t y = n;

    if(x != n){
        i = 0;
        while(i != degree +1){
            if(y == n){
                y = mod(poly[i] + (x * (degree-i)), n);
            }else{
                y = index_of[(uint8_t)(alpha_to[y] ^ alpha_to[mod(poly[i] + (x * (degree-i)), n)])];
            }
            i++;
        }
    }
    return y;
}

//check if point (x,y) is on the poly
uint8_t isonpoly(uint8_t x, uint8_t y, uint8_t poly[], uint8_t degree){
    uint8_t flag = 0;
    uint8_t px = projectpoints(x, poly, degree);

    if(y == px){
        flag = 1;
    }
    return flag;
}

//TX of Fuzzy Vault
int main(){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    // Configure all port pins as output low to save power
    PAOUT = 0; PBOUT = 0; PJOUT = 0;
    PADIR = 0xFFFF; PBDIR = 0xFFFF; PJDIR = 0xFF;
    PAIFG = 0; PBIFG = 0;
    PM5CTL0 &= ~LOCKLPM5;       // Disable GPIO power-on default high-impedance mode to activate previously configured port settings

    // Configure MPU with INFO memory as RW
    MPUCTL0 = MPUPW;
    MPUSAM |= MPUSEGIRE | MPUSEGIWE;
    MPUCTL0 = MPUPW | MPUENA;

    
    uint16_t ite = 0;
    uint8_t i;

    //TX's set generated from PS measurements. Each element should be different from each other. We use 1~s to simulate it.
    uint8_t ps[s];
    i = 0;
    while(i != s){
        ps[i] = i+1;
        i++;
    }

    __no_operation();                         // SET A BREAKPOINT HERE
    while(ite!= iterationtime * exectime){

        uint8_t randval[16];                //RNG of MS430
        rng_generateBytes(randval, 16);

        //Randomly generate (part of) a key. Assume it's all 1s.
        uint8_t key[keypolydeg+1];
        i = 0;
        while(i != 8){
            key[i] = randval[i];
            i++;
        }

        //public information sent in air
        uint8_t points[s+chaff][2];

        //Generate all the points (valid + chaff)
        uint8_t randseed = 0;
        i = 0;
        while(i != s+chaff){
            points[i][0] = i;
            if(isinset(i, ps, s)){  //valid point
                points[i][1] = projectpoints(i, key, keypolydeg);
            }else{                  //chaff point
                rng_generateBytes(randval, 16);
                randseed = randval[0];
                while(isonpoly(i, randseed, key, keypolydeg)){
                    rng_generateBytes(randval, 16);
                    randseed = randval[0];
                }
                points[i][1] = randseed;
            }
            i++;
        }

        ite++;
    }
}
