#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define polydegree 7
#define numlegitpoint 10
#define numchaffpoint 100
#define totalnum 110
//parameter settings for a perticular binary (length, k, d) BCH code
#define n 255          	//n = 2^m-1

//GF(2^8)
int alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 1};
int index_of[n+1] = {-1, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};

int projectpoints(int, int, int []);
int isinset(int, int [], int);
int isonpoly(int, int, int [], int);

//This is the Transmitter which uses fuzzy vault
int main(){

	srand((unsigned)time(NULL));	//For random number generation

	register int i, j, randval, index;

	int key[polydegree + 1];			//(part of) the 128-bit key, each element in this array is from GF(2^8), in index form
	int legitps[numlegitpoint];			//legit ps measurements at TX side, in index form
	int legity[numlegitpoint];			//y values when projecting legit ps values on the poly key[] 
	int chaffps[numchaffpoint];			//random fake ps measurements, in index form (should be different from the ones in legitps[])
	int chaffy[numchaffpoint];			//random fake y values (the points can not be on poly key[])  
	int pointsx[totalnum];				//stores the mixed x values (mixed for security)
	int pointsy[totalnum];				//stores the mixed y values
	int legitpointpos[numlegitpoint];	//stores the random positions of the legit points
	int wholekey[2*polydegree+2];

	for(j = 0; j < 2; j++){

		//randomly generate the key and ps measurements
		for(i = 0; i < polydegree+1; i++){
			key[i] = rand()%256;
			wholekey[index] = key[i];
			index++;
		}
		//the ps measurements cant be the same
		for(i = 0; i < numlegitpoint; i++){
			randval = rand()%256;
			while(isinset(randval, legitps, numlegitpoint)){
				randval = rand()%256;
			}
			legitps[i] = randval; 
		}

		//generate the legit points on the poly
		for(i = 0; i < numlegitpoint; i++){
	        legity[i] = projectpoints(polydegree, legitps[i], key);
	    }

	    //generate the chaffy points that are not on the poly
	    for(i = 0; i < numchaffpoint; i++){
	    	//x - not the same as the legit ps values
	        randval = rand()%256;
	        while(isinset(randval, legitps, numlegitpoint) || isinset(randval, chaffps, numchaffpoint)){
	        	randval = rand()%256;
	        }
	        chaffps[i] = randval;
	        //y - ensure the points not on the poly
	        randval = rand()%256;
	        while(isonpoly(chaffps[i], randval, key, polydegree)){
	        	randval = rand()%256;
	        }
	        chaffy[i] = randval;
	    }

		//mix the points together for security
		for(i = 0; i < numlegitpoint; i++){
			randval = rand()%totalnum;
			while(isinset(randval, legitpointpos, numlegitpoint)){
				randval = rand()%totalnum;
			}
			legitpointpos[i] = randval;
		}
		//insert the legit points inside the chaffy points
		int idx1 = 0, idx2 = 0;
		for(i = 0; i < totalnum; i++){
			if(isinset(i, legitpointpos, numlegitpoint)){
				pointsx[i] = legitps[idx1];
				pointsy[i] = legity[idx1];
				idx1 ++;
			}else{
				pointsx[i] = chaffps[idx2];
				pointsy[i] = chaffy[idx2];
				idx2 ++;
			}
		}

	    printf("\n");
	    printf("ps measurements at TX side:\n");
	    for(i = 0; i < numlegitpoint; i++){
	    	printf("%d, ", legitps[i]);
	    }
	    printf("\n");
	    printf("All the x values:\n");
	    for(i = 0; i < totalnum; i++){
	    	printf("%d, ", pointsx[i]);
	    }
	    printf("\n");
	    printf("All the y values:\n");
	    for(i = 0; i < totalnum; i++){
	    	printf("%d, ", pointsy[i]);
	    }
	    printf("\n");

	//////The next step is to send all the mixed points to RX
    //////This version is on PC only. Modification required on embedded devices: generate and send the points in batches because of the MSP430 memory limit
	}


	 printf("the key poly:\n");
	    for(i = 0; i < 2*polydegree+2; i++){
	    	if(wholekey[i] <= 0xf){
	    		printf("0%x", wholekey[i]);
	    	}else{
	    		printf("%x", wholekey[i]);
	    	}
	    }


    
}


//project x -> y on the poly[] (its degree = size)
int projectpoints(int size, int x, int poly[]){
    unsigned int i;
    int y = -1;

    if(x != -1){
        for(i = 0; i < size + 1; i++){
            if(y == -1){
                y = (poly[i] + (x * (size-i))) % n;
            }else{
                y = index_of[alpha_to[y] ^ alpha_to[(poly[i] + (x * (size-i))) % n]];
            }
        }
    }
    return y;
}

//check if 'value' is in set[] 
int isinset(int value, int set[], int setsize){
	unsigned int i;
	int flag = 0;
	for(i = 0; i < setsize; i++){
		if(value == set[i]){
			flag = 1;
		}
	}
	return flag;
}

//check if point (x,y) is on the poly 
int isonpoly(int x, int y, int set[], int setsize){
	int i;
	int flag = 0;
	int px = projectpoints(setsize, x, set);

	if(y == px){
		flag = 1;
	}
	return flag;
}