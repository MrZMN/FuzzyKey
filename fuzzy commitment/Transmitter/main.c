#include <stdio.h>
#include <time.h>
#include <stdlib.h>

//parameter settings of a (n,k) RS-code
#define m  5		// RS code over GF(2^m), each sysmbol contains m bits 
#define n  31		// n = 2^m-1, length of codeword (symbols) 
// #define t  2 	// number of symbol errors that can be corrected
#define k  27		// k = n-2*t, length of data (symbols) 
#define pslength m*n/8+1	//20 bytes
#define keylength m*k/8+1	//17 bytes

void encode_rs(int [], int []);

//GF(2^m) and the generator poly
int alpha_to [n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
int index_of [n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};
int g[n-k+1] = {10, 29, 19, 24, 0};

// This is a fuzzy commitment constructed by a (31,27) RS-code
// ps is 155 bit string (in 20 bytes), key is 135 bit string (in 17 bytes, we only use the first 128 bit as our encryption key)
// All the above codes are based on (31,27) RS-code. If the ECC changes, some slight changes are required
int main(){

	srand((unsigned)time(NULL));	//for random number generation

	register int i, j;
	int bytepos, bitpos;
	int idx = 0;

	int ps[pslength];			//stores the ps measurements
	int key[keylength];			//stores the random encryption key
	int ecc[pslength];			//stores the tansmitted info from TX->RX
	int data[k], codeword[n];	//For ECC coding usage

	//Randomly generate the ps values - first legit 155 bits in 20 bytes
	for(i = 0; i < pslength; i++){
		ps[i] = rand()%256;
	}
	ps[pslength-1] &=  0xe0;	//leave the 156-160th bits 0
	//Randomly generate the key - first legit 135 bits in 17 bytes
	for(i = 0; i < keylength; i++){
		key[i] = rand()%256;
	} 
	key[keylength-1] &= 0xfe;	//leave the 136th bit 0

	//For ECC encoding only. Convert the key array from 17 bytes(8 bit each) -> 27 symbols(5 bits each)
	for(i = 0; i < k; i ++){
		data[i] = 0;
		for(j = 0; j < m; j++){
			idx = m*i + j;
			bytepos = idx / 8;
			bitpos = idx % 8;
			data[i] <<= 1;
			data[i] ^= ((key[bytepos] & (0x80 >> bitpos)) >> (7 - bitpos));
		}
	}

	//Encoding. data(27 symbols) -> codeword(31 symbols)
	encode_rs(codeword, data);

	//For reducing the trasmission load. Convert the 31 symbols(5 bits each) -> 20 bytes(8 bits each)
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

	//PS xor Codeword(of the key). This is the info to be transmitted
	for( i = 0; i < pslength; i++){
		ecc[i] ^= ps[i];
	}

	//We only take the first 128 bits of key[] as our key
	printf("The 128-bit key:\n");
  	for(i = 0; i < 16; i++){
  		if(key[i] <= 0xf){
  			printf("0%x", key[i]);
  		}else{
  			printf("%x", key[i]);
  		}
  	}
  	printf("\n");

	printf("The PS values: (copy it to Receiver.c) \n");
	for(i = 0; i < pslength; i++){
		printf("%d, ", ps[i]);
	}
	printf("\n");

	printf("TX should transmit: (copy it to Receiver.c)\n");
	for(i = 0; i < pslength; i++){
		printf("%d, ", ecc[i]);
	}
	printf("\n");
  	

  	//////The next thing is to send array 'ecc[]' (20 bytes) to RX
  	//////There should be an ACK mechamism between TX and RX, such as TX sends a MAC to RX. 
}

//data[] as input, codeword[] as output
void encode_rs(int recd[n], int data[k]){
	register int i,j;
  	int feedback;
	int b[n-k];

  	for(i = 0; i < n-k; i++){
    	b[i] = 0;
  	}
     
  	for(i = k-1; i >= 0; i--){
    	feedback = index_of[data[i]^b[n-k-1]];
    	if(feedback != -1){   
      		for(j=n-k-1; j>0; j--){
        		if(g[j] != -1){
          			b[j] = b[j-1]^alpha_to[(g[j]+feedback)%n];
        		}
        		else{
          			b[j] = b[j-1];
        		}
      		}
      	b[0] = alpha_to[(g[0]+feedback)%n];
    	}
    	else{
      		for(j=n-k-1; j>0; j--){
        		b[j] = b[j-1] ;
      		}
      		b[0] = 0 ;
    	}
  	}

  	for(i=0; i<n-k; i++){
    	recd[i] = b[i];
  	}
  	for(i=0; i<k; i++){
    	recd[i+n-k] = data[i];
  	} 
} 
