#include <stdio.h>
#include <stdint.h>

// #define m  5		//number of bits per symbol
#define n  31		//n = 2^m-1, length of codeword (symbols) 
#define k  29		//k = n-2*t, length of data (symbols) 
// #define t  1 	//number of symbol errors that can be corrected
//The max tolerable mismatch rate = t/n

//GF(2^5)
//polynomial form
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
//index form
int8_t index_of[n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};
//generator polynomial
int8_t g[n-k+1] = {3, 19, 0};

//RS encoding
void encode_rs(uint8_t codeword[n], uint8_t data[k]){
	uint8_t i, j;
	int8_t temp;
	uint8_t b[n-k];

  	i = 0;
  	while(i != n-k){
  		b[i] = 0;
  		i++;
  	}
     
  	i = k;
  	do{
  		i--;
  		temp = index_of[data[i]^b[n-k-1]];
    	if(temp != -1){   
      		for(j = n-k-1; j > 0; j--){
        		if(g[j] != -1){
          			b[j] = b[j-1] ^ alpha_to[(g[j]+temp)%n];
        		}
        		else{
          			b[j] = b[j-1];
        		}
      		}
      	b[0] = alpha_to[(g[0]+temp)%n];
    	}else{
      		for(j = n-k-1; j > 0; j--){
        		b[j] = b[j-1];
      		}
      		b[0] = 0;
    	}
  	}while(i != 0);

  	i = 0;
  	while(i != n-k){
  		codeword[i] = b[i];
  		i++;
  	}
  	i = 0;
  	while(i != k){
  		codeword[i+n-k] = data[i];
  		i++;
  	}
} 
 

//TX of fuzzy commitment
int main(){

	uint8_t i;

    //Randomly generate 128-bit key. Assume it's composed of all 1s. The rest are paddings of 0s.
    uint8_t key[k] = {31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 28, 0, 0, 0};
    //Extracted PS bit string (uniformly distributed random). Assume it's composed of all 1s.
    uint8_t ps[n] = {31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31};
    //Sent in air
    uint8_t fuzzycommitment[n];

    //Encode the key to be a RS codeword
    encode_rs(fuzzycommitment, key);

    for(i = 0; i < n; i++){
    	fuzzycommitment[i] ^= ps[i];
    }

	//Print the key
	printf("Key:\n");
	for (i = 0; i < k; i++) {
		printf("%d, ", key[i]);
	}
	printf("\n");
	//Print the fuzzy commitment
	printf("Fuzzy commitment (sent to Receiver):\n");
	for (i = 0; i < n; i++) {
		printf("%d, ", fuzzycommitment[i]);
	}
	printf("\n");
}