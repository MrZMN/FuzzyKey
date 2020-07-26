#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>

//parameter settings of a (n,k) RS-code. n, k means num of symbols
#define m  4		// RS code over GF(2^m), each sysmbol contains m bits 
#define n  15		// n = 2^m-1, length of codeword (symbols) 
#define k  11		// k = n-2*t, length of data (symbols) 
// #define t  2 	// number of symbol errors that can be corrected - error rate = t/n
#define iterationnum 128/(2*m*k) + 1

//GF(2^m)
//poly form
uint8_t alpha_to [n+1] = {1, 2, 4, 8, 3, 6, 12, 11, 5, 10, 7, 14, 15, 13, 9, 0};
//index form
int8_t index_of [n+1] = {-1, 0, 1, 4, 2, 8, 5, 10, 3, 14, 9, 7, 6, 13, 11, 12};
//generator poly of RS code
int8_t g[n-k+1] = {10, 3, 6, 13, 0};

//RS encoding: data[k] as input, codeword[n] as output. In each byte of codeword[] and data[], only the last m-bits are valid 
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

//used for encoding when m = 4
void encode_m_4(uint8_t codeword[n], uint8_t data[k]){

	uint8_t codeword1[n], codeword2[n];
	uint8_t data1[k], data2[k];

	uint8_t i;

	i = 0;
	while(i != k){
		data1[i] = (uint8_t)(data[i] & 0x0f);
		data2[i] = (uint8_t)((data[i] >> 4) & 0x0f);
		i++;
	}
	
	encode_rs(codeword1, data1);
	encode_rs(codeword2, data2);

	i = 0;
	while(i != n){
		codeword[i] = (uint8_t)((codeword2[i] << 4) | codeword1[i]);
		i++;
	}

}


// This is TX of a fuzzy commitment constructed by a (15,11) RS-code
// ps - 30 bytes, key - 22 bytes (we only take 128 bits from it as our encryption key)
// All the above codes are based on (15,11) RS-code. If the ECC changes, some slight changes are required
int main(){
 
	srand((unsigned)time(NULL));	//for random number generation

	uint8_t i, iteration;
	uint8_t commitment[n], key[k];

	//In order to share 128-bit key, there will be 'iterationnum' times of operations needed
	iteration = 0;
	while(iteration != iterationnum){

		uint8_t ps[n] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //assume we get n ps measurements (uniformly distributed random) and each measurement is one byte. Should be different at each iteration

		//randomly generate (part of) the 128-bit key
		i = 0;
		while(i != k){
			if(i < 16/iterationnum){
				key[i] = rand()%256;
			}
			i++;
		}

		//encode the key to be a codeword
		encode_m_4(commitment, key);

		//fuzzy commitment = codeword xor ps
		i = 0;
		while(i != n){
			commitment[i] ^= ps[i];
			i++;
		}

		printf("Key:\n");
	  	for(i = 0; i < 16/iterationnum; i++){
	    	printf("%d, ", key[i]);
	  	}
	  	printf("\n");
	  	printf("Commitment (Sent to Receiver):\n");
	  	for(i = 0; i < n; i++){
	    	printf("%d, ", commitment[i]);
	  	}
	  	printf("\n");

	  	//////The next thing is to send array 'commitment[]' to RX
  	
		iteration++;
	}

	//////There should be an ACK mechamism between TX and RX, such as TX sends a MAC to RX. 
  	
}

