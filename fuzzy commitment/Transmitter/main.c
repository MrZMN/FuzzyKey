#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BCHencode.h"
#include "sha1.h"

// This is a fuzzy commitment constructed by a (255,131) bch code and SHA-1
// pv is 255 bit string (in 32 bytes), key is 131 bit string (in 17 bytes, only first 128 bit is the key)
// the parameteecc could be changed: m, n, length, t from 'eccencode.h'
int main()
{
	register int i;
	int bytepos;
	int bitpos;
	srand((unsigned)time(NULL));

	int pv[n/8+1];					//physiological signal - n bit
	int key[k/8+1];					//secret key - k bit (128 bit random + 0 paddings) 
	int ecc[n/8+1];					//encoded key - n bit
	int xor[n/8+1];					//the second part of the fuzzy commitment - pv ^ ecc
	char eccstring[2*(n/8+1)+1]; 	//string version of ecc
	char sha1char[21];				//string version of SHA-1 hash of the encoded key - 160 bit/20 bytes
	int sha1hex[21];				//the first part of the fuzzy commitment

	// Randomly generate the PV - first legit 255 bits in 32 bytes
	for(i = 0; i < n/8+1; i++){
		pv[i] = rand()%256;
	}
	pv[n/8] = pv[n/8] & 0xfe;	//leave the 256th bit 0
	// Randomly generate the key - first legit 131 bits in 17 bytes
	for(i = 0; i < k/8+1; i++){
		key[i] = rand()%256;
	} 
	key[k/8] = key[k/8] & 0xe0;	//leave the 132nd-136th (5 bits) bits 0

	initBCH();
	printf("This is a (%d, %d) binary ecc code\n", length, k);

	// Convert the 131 bits key: from 17 bytes to 131 bytes
	for(i = 0; i < k; i++){
		bytepos = i/8;	//from 0 - 16
		bitpos = i%8;	//from 0 - 7
		data[i] = (key[bytepos] & (0x80>>bitpos)) >> (7-bitpos); 
	}

	// Encode the ecc code to be recd - n-bit size
	encode_bch(recd, data, bb);   

	// Convert the 255 bits ecc code: from 255 bytes to 32 bytes
	for(i = 0; i < n/8+1; i++){
		ecc[i] = 0;
	}
	for(i = 0; i < length; i++){
		bytepos = i/8;	//from 0 - 31
		bitpos = i%8;	//from 0 - 7
		ecc[bytepos] |= recd[i] << (7-bitpos);   
	}      

	// Compute x XOR c
	for(i = 0; i < n/8+1; i++){
		xor[i] = ecc[i] ^ pv[i];			//second part of fuzzy commitment
	}

	printf("The key is:\n");
  	for(i = 0; i < 16; i++){
  		printf("%x", key[i]);
  	}
  	printf("\n");
  	printf("ECC of the key is:\n");
  	for(i = 0; i < n/8+1; i++){
		if(ecc[i] <= 0xf){
			printf("0%x", ecc[i]);
		}else{
			printf("%x", ecc[i]);
		}
	}
	printf("\n");
	printf("The PV measured: \n");
	for(i = 0; i < n/8+1; i++){
		if(pv[i] <= 0xf){
			printf("0%x", pv[i]);
		}else{
			printf("%x", pv[i]);
		}
	}
	printf("\n");
	printf("c - x is:\n");
	for(i = 0; i < n/8+1; i++){
		if(xor[i] <= 0xf){
			printf("0%x", xor[i]);
		}else{
			printf("%x", xor[i]);
		}
	}
	printf("\n");

	// Convert the ecc code: from int array to char array
	for(i = 0; i < 2*(n/8+1); i++){
    	sprintf((eccstring + (2*i)), "%02x", ecc[i]);
  	}
  	eccstring[2*(n/8+1)] = '\0';

	//calculate hash
	SHA1(sha1char, eccstring, strlen(eccstring));
	printf("h(c) is:\n");
	for(i = 0; i < 20; i++){
    	sha1hex[i] = sha1char[i]&0xff;  //the array sha1hex[] should be 20 bytes SHA-1 code, fiecct part of fuzzy commitment
    	if(sha1hex[i] <= 0xf){
    		printf("0%x", sha1hex[i]);
    	}else{
    		printf("%x", sha1hex[i]);
    	}
  	}
	printf("\n");
  	

}
