#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BCHdecode.h"
#include "sha1.h"

// This is a syndrome construction of fuzzy extractor constructed by a (255,131) bch code
// pv is 255 bit string (in 32 bytes)
// the parameters could be changed: m, n, length, t from 'BCHencode.h'
int main()
{
	register int i;
	int bytepos;
	int bitpos;
	srand((unsigned)time(NULL));

	int pv[n/8+1];					//physiological signal - n bit

	// Randomly generate the PV - first legit 255 bits in 32 bytes
	for(i = 0; i < n/8+1; i++){
		pv[i] = rand()%256;
	}
	pv[n/8] = pv[n/8] & 0xfe;		//leave the 256th bit 0

	initBCH();
	printf("This is a (%d, %d) binary codeword code\n", length, k);

	//convert the 255 bits pv: from 32 bytes to 255 bytes 
	for(i = 0; i < length; i++){
		bytepos = i/8;	//from 0 - 31
		bitpos = i%8;	//from 0 - 7
		recd[i] = (pv[bytepos] & (0x80>>bitpos)) >> (7-bitpos); 
	}

	//Get the syndrome s[]
	getsyndrome();

	printf("The PV measured: \n");
	for(i = 0; i < n/8+1; i++){
		if(pv[i] <= 0xf){
			printf("0%x", pv[i]);
		}else{
			printf("%x", pv[i]);
		}
	}
	printf("\n");

	printf("\n");
	printf("secure sketch is:\n");
	for(i = 1; i <= 2*t; i++){
		if(s[i] <= 0xf){
			printf("0%x", s[i]);
		}else{
			printf("%x", s[i]);
		}
	}
	printf("\n");
  	
}
