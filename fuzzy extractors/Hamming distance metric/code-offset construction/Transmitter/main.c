#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BCHencode.h"
#include "sha1.h"

// This is a code-offset construction of fuzzy extractor constructed by a (255,131) bch code
// pv and codeword are both 255 bit string (in 32 bytes)
// the parameters could be changed: m, n, length, t from 'BCHencode.h'
int main()
{
	register int i;
	int bytepos;
	int bitpos;
	srand((unsigned)time(NULL));

	int pv[n/8+1];					//physiological signal - n bit
	int nonce[k/8+1];				//a nonce - k bit
	int codeword[n/8+1];			//encoded nonce - n bit
	int ss[n/8+1];					//the secure sketch - pv ^ codeword

	// Randomly generate the PV - first legit 255 bits in 32 bytes
	for(i = 0; i < n/8+1; i++){
		pv[i] = rand()%256;
	}
	pv[n/8] = pv[n/8] & 0xfe;	//leave the 256th bit 0
	// Randomly generate the nonce - first legit 131 bits in 17 bytes
	for(i = 0; i < k/8+1; i++){
		nonce[i] = rand()%256;
	} 
	nonce[k/8] = nonce[k/8] & 0xe0;	//leave the 132nd-136th (5 bits) bits 0

	initBCH();
	printf("This is a (%d, %d) binary codeword code\n", length, k);

	// Convert the 131 bits nonce: from 17 bytes to 131 bytes
	for(i = 0; i < k; i++){
		bytepos = i/8;	//from 0 - 16
		bitpos = i%8;	//from 0 - 7
		data[i] = (nonce[bytepos] & (0x80>>bitpos)) >> (7-bitpos); 
	}

	// Encode the nonce to be recd - n-bit size
	encode_bch(recd, data, bb);   

	// Convert the 255 bits ecc code: from 255 bytes to 32 bytes
	for(i = 0; i < n/8+1; i++){
		codeword[i] = 0;
	}
	for(i = 0; i < length; i++){
		bytepos = i/8;	//from 0 - 31
		bitpos = i%8;	//from 0 - 7
		codeword[bytepos] |= recd[i] << (7-bitpos);   
	}      

	// Compute ss = pv XOR codeword
	for(i = 0; i < n/8+1; i++){
		ss[i] = codeword[i] ^ pv[i];			//secure sketch - sent to RX
	}

	printf("The PV measured: \n");
	for(i = 0; i < n/8+1; i++){
		if(pv[i] <= 0xf){
			printf("0%x", pv[i]);
		}else{
			printf("%x", pv[i]);
		}
	}
	printf("\n");

	printf("secure sketch is:\n");
	for(i = 0; i < n/8+1; i++){
		if(ss[i] <= 0xf){
			printf("0%x", ss[i]);
		}else{
			printf("%x", ss[i]);
		}
	}
	printf("\n");
  	
}
