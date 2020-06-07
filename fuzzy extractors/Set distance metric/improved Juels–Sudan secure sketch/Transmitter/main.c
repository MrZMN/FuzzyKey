#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define setsize 10		//PS value set's size (s in the fuzzy extractor paper)
#define errtolerant 4 	//error tolerance (t in the fuzzy extractor paper), which stands for symetric difference size (2 * num of sets mismatches), must be even number 

void combandproduct(int* arr, int start, int* result, int count, const int NUM, const int arr_len, long double* res, long double* realarr);
long double getcoeff(int size, int num, long double* set);

int main(){

	srand((unsigned)time(NULL));	//For random number generation -- test only --
	register int i;

	long double setA[setsize];			//setA[] stores the PS values measured by the TX
	long double ss[errtolerant];		//ss[] stores the secure sketch to be transmitted to RX

	//generate the random PS values
	for(i = 0; i < setsize; i++){
		setA[i] = (rand()%10000)/1000.0;	//assume each PS value corresponds to 5 digits info(Universal size: 2^16)
	}

	//generate the secure sketch 
	for(i = 0; i < errtolerant; i++){
		ss[i] = 0.0;
		ss[i] = getcoeff(setsize, i+1, setA);
		if(i%2==0){
			ss[i] = -ss[i];
		}
	}
	
	//print
	printf("The PS values at TX:\n");
	for(i = 0; i < setsize; i++){
		printf("%Lf, ", setA[i]);
	}
	printf("\n");
	printf("The Secure sketch:\n");
	for(i = 0; i < errtolerant; i++){
		printf("%Lf, ", ss[i]);
	}
	printf("\n");

	//Calculate the coeffs of plow(x)
	for(i = 0; i < setsize-errtolerant; i++){
		printf("%d, ", (int)(getcoeff(setsize, i+1+errtolerant, setA)+0.5));
	}

////The principle is: TX constructs a poly based on all PS measurements. TX devides poly into two parts phigh(x) and plow(x), and transmits coeffs of phigh(x) to RX
////Then RX reconstructs plow(x) based on its PS measurements, only when (s-t/2) PS values are the same as TX's (using B-W algorithm). 

	//The next step is to send ss[] to RX
}

//this fuction mainly finds all combinations of size 'NUM' from set 'realarr'  
void combandproduct(int* arr, int start, int* result, int count, const int NUM, const int arr_len, long double* res, long double* realarr)
{
    int i = 0;
    for (i = start; i < arr_len + 1 - count; i++)
    {
        result[count - 1] = i;
        if (count - 1 == 0)
        {
            int j;
            long double product = 1.0;
            for (j = NUM - 1; j >= 0; j--){
                product *= realarr[arr[result[j]]];
            }
            *res += product;
        }else
            combandproduct(arr, i + 1, result, count - 1, NUM, arr_len, res, realarr);
    }
}

//this function calculates the secure sketch values
long double getcoeff(int size, int num, long double* set){

    int i;
    int arr[size];
    for(i = 0; i < size; i++){
        arr[i] = i;
    }
    int result[num];
    long double val = 0.0;
    long double* valpointer = &val;

    combandproduct(arr, 0, result, num, num, sizeof(arr)/sizeof(int), valpointer, set);
    
    return val;
}