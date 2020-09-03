#include <stdio.h>
#include <stdint.h>

// #define m  5     //number of bits per symbol
#define n  31       //n = 2^m-1, length of codeword (symbols) 
#define k  29       //k = n-2*t, length of data (symbols) 
#define t  1        //number of symbol errors that can be corrected
//The max tolerable mismatch rate = t/n

//GF(2^5)
//polynomial form
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
//index form
int8_t index_of[n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};

//calculate the securesketch
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
            if(temp[j] != -1){
                syndrome[i] ^= alpha_to[(temp[j]+i*j)%n];
            }
            j++;
        }
        i++;
    }
}

//TX of syndrome-based construction of fuzzy extractor
int main(){

    uint8_t i;

    //Extracted PS bit string (uniformly distributed random). Assume it's composed of all 1s.
    uint8_t ps[n] = {31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31};
    //sent in air
    uint8_t syndrome[2*t+1]; 

    //calculate the secure sketch of TX
    getsyndrome(syndrome, ps);


    //Print the secure sketch
    for(i = 1; i < 2*t+1; i++){
        printf("%d, ", syndrome[i]);
    }
    printf("\n");
}
