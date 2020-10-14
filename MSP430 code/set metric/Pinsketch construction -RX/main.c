#include <msp430fr5969.h>
#include <stdint.h>
#include "aes256.h"

#define iterationtime 100

//////mismatch rate = 2%

//////mismatch rate = 2%
//#define exectime 1      //the number of executions needed for this construction
////parameter settings of a (n,k) BCH-code.
//#define s 50    //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
//#define t 2     //error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number
//#define n 255   //universe size
////The max tolerable mismatch rate = t/2s
////The initial entropy is log(n,s) with base 2, where (n,s) stands for the number of combinations of taking random s elements from n elements without putting back
//
////////mismatch rate = 5%
//#define exectime 2      //the number of executions needed for this construction
////parameter settings of a (n,k) BCH-code.
//#define s 20    //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
//#define t 2     //error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number
//#define n 255   //universe size
////The max tolerable mismatch rate = t/2s
////The initial entropy is log(n,s) with base 2, where (n,s) stands for the number of combinations of taking random s elements from n elements without putting back

//////mismatch rate = 10%

//parameter settings of a (n,k) BCH-code.
#define exectime 4      //the number of executions needed for this construction
#define s 10    //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
#define t 2     //error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number
#define n 255   //universe size
//The max tolerable mismatch rate = t/2s
//The initial entropy is log(n,s) with base 2, where (n,s) stands for the number of combinations of taking random s elements from n elements without putting back

//GF(2^8) tables
//polynomial form
int16_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 0};
//index form
int16_t index_of[n+1] = {-1, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};


//put modular operation into RAM
#pragma CODE_SECTION(mod, ".run_from_ram_mod")
uint16_t mod(uint16_t num1, uint16_t num2){
    return num1%num2;
}

//put division operation into RAM
#pragma CODE_SECTION(divi, ".run_from_ram_div")
uint16_t divi(uint16_t num1, uint16_t num2){
    return num1/num2;
}

//Sort the array in ascending order
void bubble_sort(uint8_t array[], uint8_t len){
    uint8_t i, j, temp;
    i = 0;
    while(i != len-1){
        j = 0;
        while(j != len-1-i){
            if(array[j] > array[(uint8_t)(j+1)]){
                temp = array[j];
                array[j] = array[(uint8_t)(j+1)];
                array[(uint8_t)(j+1)] = temp;
            }
            j++;
        }
        i++;
    }
}

//Calculate the secure sketch of RX
void securesketch(uint8_t ps[], int16_t ss[]){
    uint8_t i, j, degree;

    i = 0;
    while(i != t){
        ss[i] = 0;
        degree = 2*i+1;         //degree is 1, 3, 5... we only need the odd-indexes because for binary BCH code, each even-index value could be generated from the odd-index values
        j = 0;
        while(j != s){
            ss[i] ^= alpha_to[mod(index_of[ps[j]]*degree, n)];
            j++;
        }
        i++;
    }
}

//calculate the whole syndrome of binary BCH code
void recoversyn(int16_t syndrome[], int16_t ss[]){

    uint8_t i;

    syndrome[0] = 0;
    i = 1;
    while(i != 2*t+1){
        syndrome[i] = 0;
        if(mod(i, 2) != 0){
            syndrome[i] = ss[divi(i-1,2)];
        }else{
            if(syndrome[divi(i,2)] == -1){
                syndrome[i] = -1;
            }else{
                syndrome[i] = mod(syndrome[divi(i,2)]*2, n);
            }
        }
        i++;
    }
}

//calculate arr1[] = setdifference (arr1[], arr2[])
void dosetdiff(uint8_t arr1[], uint8_t arr1size, int16_t arr2[], uint8_t arr2size){
    uint8_t i, j;
    int8_t temp1[s], temp2[t];

    i = 0;
    while(i != arr1size){
        j = 0;
        while(j != arr2size){
            if(arr1[i] == arr2[j]){
                temp1[i] = -1;
                temp2[j] = -1;
            }
            j++;
        }
        i++;
    }

    i = 0;
    while(i != arr2size){
        if(temp2[i] != -1 && arr2[i] != -1){
           j = 0;
           while(j != arr1size){
               if(temp1[j] == -1){
                  arr1[j] = arr2[i];
                  temp1[j] = 0;
                  break;
              }
              j++;
           }
       }
       i++;
    }
}

//Simon Rockliff'ss implementation of Berlekamp'ss algorithm.
void getsetdiff(int16_t ss[], int16_t setdiff[]){

    uint16_t    i, j, u, q, count;
    int16_t     elp[2*t+2][t+1], d[2*t], l[2*t], u_l[2*t],reg[2*t];

    //Step 1: calculate the error location poly
    d[0] = 0;
    d[1] = ss[1];
    elp[0][0] = 0;
    elp[1][0] = 1;

    i = 1;
    while(i != 2*t){
        elp[0][i] = -1;
        elp[1][i] = 0;
        i++;
    }

    l[0] = 0;
    l[1] = 0;
    u_l[0] = -1;
    u_l[1] = 0;

    u = 0;

    do {
        u++;
        if(d[u] == -1) {
            l[u + 1] = l[u];
            i = 0;
            while(i != l[u]+1){
                elp[u + 1][i] = elp[u][i];
                elp[u][i] = index_of[(uint8_t)(elp[u][i])];
                i++;
            }
        }else

        {
            q = u - 1;
            while ((d[q] == -1) && (q > 0))
                q--;

            if(q > 0) {
                j = q;
                do{
                    j--;
                    if ((d[j] != -1) && (u_l[q] < u_l[j]))
                        q = j;
                }while (j > 0);
            }

            if(l[u] > l[q] + u - q){
                l[u + 1] = l[u];
            }
            else{
                l[u + 1] = l[q] + u - q;
            }

            i = 0;
            while(i != 2*t){
                elp[u + 1][i] = 0;
                i++;
            }
            i = 0;
            while(i != l[q]+1){
                if (elp[q][i] != -1){
                    elp[u + 1][i + u - q] = alpha_to[mod(d[u] + n - d[q] + elp[q][i], n)];
                }
                i++;
            }
            i = 0;
            while(i != l[u]+1){
                elp[u + 1][i] ^= elp[u][i];
                elp[u][i] = index_of[elp[u][i]];
                i++;
            }
        }
        u_l[u + 1] = u - l[u + 1];

        if (u < 2*t) {
            d[u + 1] = alpha_to[(uint8_t)(ss[u + 1])] & (-(1 ^ ((unsigned int)ss[u + 1]) >> (sizeof(unsigned int) * 8 - 1)));
            i = 1;
            while(i != l[u + 1]+1){
                if ((1 ^ ((unsigned int)ss[u + 1 - i]) >> (sizeof(unsigned int) * 8 - 1)) && (elp[u + 1][i] != 0)){
                    d[u + 1] ^= alpha_to[mod(ss[u + 1 - i] + index_of[(uint8_t)(elp[u + 1][i])], n)];
                }
                i++;
            }

            d[u + 1] = index_of[(uint8_t)(d[u + 1])];
        }
    }while ((u < 2*t) && (l[u + 1] <= t));

    u++;

    if(l[u] <= t){
        i = 0;
        while(i != l[u]+1){
            elp[u][i] = index_of[(uint8_t)(elp[u][i])];
            i++;
        }


        //Step 2: Chien search - find roots of the error location polynomial
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
                if (reg[j] != -1) {
                    reg[j] = mod(reg[j] + j, n);
                    q ^= alpha_to[(uint8_t)(reg[j])];
                }
                j++;
            }
            if(!q){
                setdiff[count] = n - i; //error location
                if(setdiff[count] != -1){
                    setdiff[count] = alpha_to[(uint8_t)(setdiff[count])];
                }
                count++;
            }
            i++;
        }
    }
}

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
    while(i != 1){      //round number
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

//This is RX of a BCH-based Pinsketch
int main(){

    WDTCTL = WDTPW | WDTHOLD;   // stop watchdog timer
    // Configure all port pins as output low to save power
    PAOUT = 0; PBOUT = 0; PJOUT = 0;
    PADIR = 0xFFFF; PBDIR = 0xFFFF; PJDIR = 0xFF;
    PAIFG = 0; PBIFG = 0;
    PM5CTL0 &= ~LOCKLPM5;       // Disable GPIO power-on default high-impedance mode to activate previously configured port settings

    __no_operation();                         // SET A BREAKPOINT HERE
    uint16_t ite = 0;
    uint8_t i;
    while(ite!= iterationtime * exectime){

        uint8_t errflag = 0;

        //RX's set generated from PS measurements. Each element should be different from each other. We use 1~s to simulate it.
        uint8_t ps[s];
        i = 0;
        while(i != s){
            ps[i] = i+1;
            i++;
        }

        //add some errors (for test only)
        i = 0;
        while(i != t/2){
            ps[i] += 100;
            i++;
        }

        int16_t ssTX[t] = {11, 3};      //stores secure sketch at TX side - received from TX

        int16_t ssRX[t];                //stores secure sketch at RX side
        //Calculate the secure sketch of RX
        securesketch(ps, ssRX);

        //Calculate syndrome of error = SSTX xor SSRX
        i = 0;
        while(i != t){
            ssRX[i] = index_of[(uint8_t)(ssRX[i] ^ ssTX[i])];

            if(ssRX[i] != -1){  //when the syndrome of error isn't a zero vector, there's mismatch
                errflag = 1;
            }
            i++;
        }

        //if there're mismatches, find the set difference between PS measurements at TX/RX sides
        if(errflag){

            int16_t syndrome[2*t+1];        //the whole syndrome, generated using ssRX[] xor ssTX[]
            //calculate the whole syndrome (ssTX[] and ssRX[] only contain the odd-indexes values)
            recoversyn(syndrome, ssRX);

            //Decode the syndrome
            int16_t setdiff[t];     //stores the set difference between ps measurements at TX/RX sides
            i = 0;
            while(i != t){
                setdiff[i] = -1;
                i++;
            }
            getsetdiff(syndrome, setdiff);


            //Correct the set - only concern the mismatch situation, i.e. TX/RX have same num of measurements and some of them mismatch
            dosetdiff(ps, s, setdiff, t);
        }

        //Generate the 128-bit key
        //sort the array (sort because RX could only recover all the PS elements, but doesn't ensure the PS values are in the same order)
        bubble_sort(ps, s);

        //Strong Extractor
        //hash result is 256-bit
        uint8_t hash[32];
        hirose(hash, ps, s);

        __no_operation();                         // SET A BREAKPOINT HERE
        ite++;
    }
}

