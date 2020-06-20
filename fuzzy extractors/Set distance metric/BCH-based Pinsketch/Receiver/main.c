#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BCHdecode.h"

//the parameters could be changed: m, n, length, t from 'BCHencode.h'; numerr, recdCode[] from 'main.c'
int main()
{
	register int i, j, degree;
	////////////////For test only////////////////
	srand( (unsigned)time( NULL ) );
	// uint8_t copy[nummeasure];
	// int decerror = 0;
	// int numerr = 1; 	//num of bit errors
	// int errpos[numerr];
	////////////////For test only////////////////

	// Get p(x)
	read_p();               
	// generate the Galois Field GF(2**mm)
	generate_gf();
	
	// Received info from TX
	uint8_t PS[] = {204, 216, 92, 141, 59, 119, 171, 118, 185, 224, 22, 212, 147, 50, 30, 18}; //I changed the 2nd 
	uint8_t SSTX[] = {165, 108, 233, 54};
	uint8_t realPS[nummeasure];

    // // Make a copy -- for test only
    // for(i=0; i<nummeasure; i++){
    // 	copy[i] = PS[i];
    // }


	//Calculate the syndrome of RX
	for(i = 0; i < t; i++){
		SSRX[i] = 0;
		degree = 2*(i+1) - 1;	//degree is 1, 3, 5...
		for(j = 0; j < nummeasure; j++){
			SSRX[i] ^= alpha_to[(PS[j]*degree)%n];
		}
		SSRX[i] = index_of[SSRX[i]];	//SSRX is in index form
		// printf("%d, ", SSRX[i]);
	}         
	// printf("\n");

	//Calculate the syndrome of error = syn(TX) xor syn(RX)
	int flag = 0;
	for(i = 0; i < t; i++){
		SSRX[i] = index_of[alpha_to[SSRX[i]] ^ alpha_to[SSTX[i]]];
		// printf("%d, ", SSRX[i]);
		if(SSRX[i]!=n){
			flag = 1;
		}
	}
	// printf("\n");

	//if has errors 
	if(flag == 1){

		// calculate the whole syndrome 
		s[0] = 0;
		for(i = 1; i < 2*t+1; i++){
			s[i] = 0;
			if(i%2!=0){
				s[i] = SSRX[(i-1)/2];
			}else{
				s[i] = (s[i/2]*2)%n;
			}
			// printf("%d, ", s[i]);
		}
		// printf("\n");

		//Decode the syndrome	
		printf("The set difference:\n");
		decode_bch(); 

		//Correct the set
		//PS = set difference(PS[], loc[]) 

	}else{
		printf("There are no errors\n");
	}

}
