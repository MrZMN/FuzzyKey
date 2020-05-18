
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BCHdecode.h"
#include "sha1.h"

// This is a code-offset construction of fuzzy extractor constructed by a (255,131) bch code
// Receive 2 strings: pv, SS 
// the parameters could be changed: m, n, length, t from 'BCHencode.h'; numerr, PVstring[], SS[] from 'main.c'
int main()
{
	register int i;
	char oneByte[] ="aa";
	////////////////For test only////////////////
	srand( (unsigned)time( NULL ) );
	int numerr = 10; 	//num of bit errors
	int errpos[numerr];
	////////////////For test only////////////////

	char PVstring[]   = "1e108c14bd2e18d905dc2c8730ef441493e272d7bb6956e221914e5a05bfc7a0";		// for test
	char SS[]  		  = "22e897b4e186fdbfabefdde4aa24d5622cff22cbdc1da9b5cff34c99b63daf30";		// secure sketch received from TX

	int pv[n/8+1];					//physiological signal - n bit
	int ss[n/8+1];					//secure sketch received from TX
	int codeword[n/8+1];			//pv' XOR ss & decode(pv' XOR ss)
	int pvcopy[n/8+1];				//a copy of original pv -- for test only

	//convert string to int arrays
	for(i=0; i<n/8+1; i++){
		strncpy(oneByte, PVstring+2*i, 2);			
		pv[i] = strtol(oneByte, NULL, 16);	
		pvcopy[i] = pv[i];		// --- for test only		
	}
	for(i=0; i<n/8+1; i++){
		strncpy(oneByte, SS+2*i, 2);			
		ss[i] = strtol(oneByte, NULL, 16);			
	}

	//inject some errors in pv to simulate pv'
	for (i = 0; i < numerr; i++){
		errpos[i] = rand()%(n/8+1);
	}
	for (i = 0; i < numerr; i++){
		pv[errpos[i]] ^= 0x80;			//just an example to change a bit
	}

	//calculate c' = pv' XOR ss
	for(i = 0; i < n/8+1; i++){
		codeword[i] = pv[i] ^ ss[i];
	}

	initBCH();
	printf("This is a (%d, %d) binary codeword code\n", length, k);

	//convert the 255 bits codeword: from 32 bytes to 255 bytes 
	int bytepos;
	int bitpos;
	for(i = 0; i < length; i++){
		bytepos = i/8;	//from 0 - 31
		bitpos = i%8;	//from 0 - 7
		recd[i] = (codeword[bytepos] & (0x80>>bitpos)) >> (7-bitpos); 
	}

	//Decode the BCH code
	decode_bch(recd);

	//Convert the 255 bits corrected codeword: from 255 bytes to 32 bytes
	for(i = 0; i < n/8+1; i++){
		codeword[i] = 0;
	}
	for(i = 0; i < length; i++){
		bytepos = i/8;	//from 0 - 31
		bitpos = i%8;	//from 0 - 7
		codeword[bytepos] |= recd[i] << (7-bitpos);   //c = decode (c')
	}

	//recover pv = ss XOR codeword
	for(i = 0; i < n/8+1; i++){
		pv[i] = codeword[i] ^ ss[i];
	}

	printf("The PV is:\n");
	for(i = 0; i < n/8+1; i++){
		if(pv[i] <= 0xf){
			printf("0%x", pv[i]);
		}else{
			printf("%x", pv[i]);
		}
	} 

	//check correct
	int flag = 1;
	for(i = 0; i < n/8+1; i++){
		if(pvcopy[i] != pv[i]){
			flag = 0;
		}
	}
	if(flag == 1){
		printf("   --matched, pv agreement completes\n");
	}else{
		printf("   --not match, pv agreement fails\n");
	}

}
