
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BCHdecode.h"
#include "sha1.h"

// This is a fuzzy commitment constructed by a (255,131) bch code and SHA-1
// Receive 3 strings: x, c XOR x, h(c) 
// the parameters could be changed: m, n, length, t from 'BCHencode.h'; numerr, PVstring[], XORstring[], hashstring[] from 'main.c'
int main()
{
	register int i;
	char oneByte[] ="aa";
	////////////////For test only////////////////
	srand( (unsigned)time( NULL ) );
	int copy[length];
	int numerr = 10; 	//num of bit errors
	int errpos[numerr];
	////////////////For test only////////////////

	char PVstring[]   = "196a4dd11dfcf0bf013a65a18a81b2798bb9b1d9a35f49421e2b5b4557765222";		// for test
	char XORstring[]  = "fcdf05d9acef2b2462158c516c8e3d13b42162a756436cdac123d17d088fdcde";		// received from TX
	char hashstring[] = "566d09985c4c6f7998db9391b804e574ec62d016";								// received from TX

	int pv[n/8+1];					//physiological signal - n bit
	int key[k/8+1];					//secret key - k bit (128 bit random + 0 paddings) 
	int xor[n/8+1];					//the second part of fuzzy commitment received from TX
	int sha1[21];					//the first part of fuzzy commitment received from TX
	int ecc[n/8+1];					//x' XOR (x XOR c)
	char eccstring[2*(n/8+1)+1]; 	//string version of ecc[]
	char sha1char[21];				//string version of SHA-1 hash of the encoded key - 160 bit
	unsigned int sha1hex[21];		//the first part of the fuzzy commitment

	for(i=0; i<n/8+1; i++){
		strncpy(oneByte, PVstring+2*i, 2);			
		pv[i] = strtol(oneByte, NULL, 16);					
	}
	for(i=0; i<n/8+1; i++){
		strncpy(oneByte, XORstring+2*i, 2);			
		xor[i] = strtol(oneByte, NULL, 16);			
	}
	for(i=0; i<20; i++){
		strncpy(oneByte, hashstring+2*i, 2);
		sha1[i] = strtol(oneByte, NULL, 16);					
	}

	initBCH();
	printf("This is a (%d, %d) binary ecc code\n", length, k);

	//inject some errors in x to simulate x'
	for (i = 0; i < numerr; i++){
		errpos[i] = rand()%(n/8+1);
	}
	for (i = 0; i < numerr; i++){
		pv[errpos[i]] ^= 0x80;			//just an example to change a bit
	}

	//calculate x' XOR (x XOR c)
	for(i = 0; i < n/8+1; i++){
		xor[i] ^= pv[i];
	}

	//convert the 255 bits xor: from 32 bytes to 255 bytes 
	int bytepos;
	int bitpos;
	for(i = 0; i < length; i++){
		bytepos = i/8;	//from 0 - 31
		bitpos = i%8;	//from 0 - 7
		recd[i] = (xor[bytepos] & (0x80>>bitpos)) >> (7-bitpos); 
	}

	//Decode the BCH code
	decode_bch(recd);

	// Convert the 255 bits ecc code: from 255 bytes to 32 bytes
	for(i = 0; i < n/8+1; i++){
		ecc[i] = 0;
	}
	for(i = 0; i < length; i++){
		bytepos = i/8;	//from 0 - 31
		bitpos = i%8;	//from 0 - 7
		ecc[bytepos] |= recd[i] << (7-bitpos);   
	}

	printf("The key is:\n");
	for(i = 0; i < 16; i++){
		key[i] = ((ecc[(length-k)/8 + i] & 0xf) << 4) | ((ecc[(length-k)/8 + 1 + i] & 0xf0) >> 4);
		if(key[i] <= 0xf){
			printf("0%x", key[i]);
		}else{
			printf("%x", key[i]);
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

	//check correct
	int flag = 1;
	for(i = 0; i < 20; i++){
		if(sha1[i] != sha1hex[i]){
			flag = 0;
		}
	}
	if(flag == 1){
		printf("   --matched, key agreement completes\n");
	}else{
		printf("   --not match, key agreement fails\n");
	}

}
