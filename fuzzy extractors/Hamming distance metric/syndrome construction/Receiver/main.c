
#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BCHdecode.h"
#include "sha1.h"

// This is a syndrome construction of fuzzy extractor constructed by a (255,131) bch code
// Receive 2 strings: pv, SS 
// the parameters could be changed: m, n, length, t from 'BCHencode.h'; numerr, PVstring[], SS[] from 'main.c'
int main()
{
	register int i;
	char oneByte[] ="aa";
	int bytepos;
	int bitpos;
	////////////////For test only////////////////
	srand( (unsigned)time( NULL ) );
	int numerr = 10; 	//num of bit errors
	int errpos[numerr];
	////////////////For test only////////////////

	char PVstring[]   = "fc143f4a24e15c3daf8c41f23b4e3f60c4c98576399d730ca80c613d7d0dacea";				// for test
	char SS[]  		  = "1275ebe37b1e1b5efdb7d825f134f413db7be009bddf17a4be2b20d4893ad47437da63b7";		// syn(pv) received from TX

	int pv[n/8+1];					//physiological signal - n bit
	int syn[2*t+1];					//syn(pv) received from TX
	int pvcopy[n/8+1];				//a copy of pv

	//convert string to int arrays
	for(i=0; i<n/8+1; i++){
		strncpy(oneByte, PVstring+2*i, 2);			
		pv[i] = strtol(oneByte, NULL, 16);	
		pvcopy[i] = pv[i];		// --- for test only	
	}
	syn[0] = 0;
	for(i=1; i<=2*t; i++){
		strncpy(oneByte, SS+2*(i-1), 2);			
		syn[i] = strtol(oneByte, NULL, 16);				
	}

	//inject some errors in pv to simulate pv'
	for (i = 0; i < numerr; i++){
		errpos[i] = rand()%(n/8+1);
	}
	for (i = 0; i < numerr; i++){
		pv[errpos[i]] ^= 0x80;			//just an example to change a bit
		printf(" %d ",errpos[i]*8);	//bit error locations
	}

	//calculate syn(pv')
	initBCH();
	printf("This is a (%d, %d) binary codeword code\n", length, k);

	//convert the 255 bits pv: from 32 bytes to 255 bytes 
	for(i = 0; i < length; i++){
		bytepos = i/8;	//from 0 - 31
		bitpos = i%8;	//from 0 - 7
		recd[i] = (pv[bytepos] & (0x80>>bitpos)) >> (7-bitpos); 
	}

	//get the syn(pv')
	getsyndrome();


	//print syn again for test
	// printf("The syn of original pv is: ");
	// for(i = 1; i<= 2*t; i++){
	// 	if(syn[i]<=0xf){
	// 		printf("0%x", syn[i]);
	// 	}else{
	// 		printf("%x", syn[i]);
	// 	}
	// }
	// printf("\n");

	// printf("The syn of modified pv is: ");
	// for(i = 1; i<= 2*t; i++){
	// 	if(s[i]<=0xf){
	// 		printf("0%x", s[i]);
	// 	}else{
	// 		printf("%x", s[i]);
	// 	}
	// }
	// printf("\n");

	//calculate syn(pv) XOR syn(pv')
	for(i = 1; i<= 2*t; i++){
		s[i] ^= syn[i]; 			// two syndromes xor
	}

	//calculate error locations
	geterrpos();
	printf("\n");

	//change the errors
	for(i = 0; i < errnum; i++){
		bytepos = loc[i]/8;	//from 0 - 31
		bitpos = loc[i]%8;	//from 0 - 7
		pv[bytepos] ^= (1 << (7-bitpos));
	}
	//check correction
	int flag = 1;
	for(i=0; i<n/8+1; i++){
		if(pv[i] != pvcopy[i]){
			flag = 0;
		}
	}
	if(flag == 1){
		printf("pv matched correctly\n");
	}else{
		printf("pv unmatched\n");
	}

}
