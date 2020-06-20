#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "BCHencode.h"

// the parameters could be changed: 
int main()
{
	register int i, j, degree;
	srand( (unsigned)time( NULL ) );

	//get p(x)
	read_p();               
	//generate the Galois Field GF(2**mm)
	generate_gf() ;

	for(i = 0; i < n; i ++){
		printf("%d, %d, %d \n", i, alpha_to[i], index_of[i]);
	}


	//Randomly generate the data - assume each measurement consists of 1 byte, 128 bites in total
	for(i = 0; i < nummeasure; i++){
		PS[i] = rand()%256;
	}

	//Calculate the syndrome of TX
	for(i = 0; i < t; i++){
		SS[i] = 0;
		degree = 2*(i+1) - 1;	//degree is 1, 3, 5...
		for(j = 0; j < nummeasure; j++){
			SS[i] ^= alpha_to[(PS[j]*degree)%n];
		}
		SS[i] = index_of[SS[i]];	//SS is in index form
	}         
	
	//print
	printf("The PS measurements are:\n");
	for (i = 0; i < nummeasure; i++) {
		printf("%d, ", PS[i]);
	}
	printf("\n");

	printf("The secure sketch is:\n");
	for (i = 0; i < t; i++) {
		printf("%d, ", SS[i]);
	}
	printf("\n");

}
