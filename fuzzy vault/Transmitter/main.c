#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#define polydegree 7
#define numlegitpoint 15
#define numchaffpoint 2000
#define totalnum (numlegitpoint + numchaffpoint)

double mulfortimes(double num, int times);
int isinset(int val, int set[], int setsize);
int isonpoly(int chaffvalx, double chaffvaly, uint16_t key[]);

//This is the Transmitter which uses fuzzy vault
int main(){

	srand((unsigned)time(NULL));	//For random number generation
	register int i, j;
	double x;
	uint16_t key[polydegree + 1];
//////////////////////////
// For all the generated points (x,y): x is an integer from [0, 99999] - 4 bytes; y is a double - 8 bytes. This means each point is 12-byte long :(
// Therefore, for MSP430 (only 2KB memory), it's impossible to generate all the points then send them in only one time
// It's feasible to generate + send points in batches.
//////////////////////////
	int setAx[numlegitpoint];			//setAx stores the PS values measured by the TX
	double setAy[numlegitpoint];		//setAy stores y = p(x) where x in setAx[]
	int chaffx[numchaffpoint];			//chaffx stores random PS values which are different from legit PS measurements in set A
	double chaffy[numchaffpoint];		//chaffy stores random y values which don't lie on p(x) where x in chaffx[]
	int pointsx[totalnum];				//pointsx stores the mixed PS values
	double pointsy[totalnum];			//pointsy stores the mixed y values
	int legitpointpos[numlegitpoint];	//stores the random positions of the legit points in pointsx,y[] for concealing them

	//Generate 128-bit key - array storing 8 polynomial coeff (each 16 bit) from high degree x^7 -> low degree x^0
	for(i = 0; i < polydegree+1; i++){
		key[i] = rand()%65536;		///for test only///
	}

	//generate the legit points on the poly
	for(i = 0; i < numlegitpoint; i++){
		setAx[i] = rand()%100000;	//Here assume each PS value corresponds to 5 digits info (Universe size: 10^5), this could be adjusted
		x = setAx[i]/10000.0;		//devide by 10000 to avoid p(x) too large
		setAy[i] = 0.0;
		//project PS value x on the polynomial to obtain y = p(x)
		for(j = 0; j < polydegree+1; j++){
			setAy[i] += key[j] * mulfortimes(x, (polydegree-j)); 
		}
	}

	//generate the random chaff points
	for(i = 0; i < numchaffpoint; i++){
		chaffx[i] = rand()%100000;
		//check if random PS value occurs
		while(isinset(chaffx[i], setAx, numlegitpoint)){
			chaffx[i] = rand()%100000;
		}
		chaffy[i] = 1.0*rand()/RAND_MAX *552394181327;	//generate a random y from [0,552394181327]. The second num corresponds to p(9.9999)
		//check if random y == p(x)
		while(isonpoly(chaffx[i], chaffy[i], key)){
			chaffy[i] = 1.0*rand()/RAND_MAX *552394181327;
		}
	}

	//mix the points together
	for(i = 0; i < numlegitpoint; i++){
		legitpointpos[i] = -1;
	}
	for(i = 0; i < numlegitpoint; i++){
		int pos = rand()%totalnum;
		while(isinset(pos, legitpointpos, numlegitpoint)){
			pos = rand()%totalnum;
		}
		legitpointpos[i] = pos;
	}
	//insert the legit points inside the chaff points
	int idx1 = 0;
	int idx2 = 0;
	for(i = 0; i < totalnum; i++){
		if(isinset(i, legitpointpos, numlegitpoint)){
			pointsx[i] = setAx[idx1];
			pointsy[i] = setAy[idx1];
			idx1 ++;
		}else{
			pointsx[i] = chaffx[idx2];
			pointsy[i] = chaffy[idx2];
			idx2 ++;
		}
	}

	//print all
	printf("The key is: \n");
	for(i = 0; i < polydegree+1; i++){
		printf("%x", key[i]);
	}
	printf("\n\n");

	printf("setAx (PS measurements): \n");
	for(i = 0; i < numlegitpoint; i++){
		printf("%d, ", setAx[i]);
	}
	printf("\n\n");

	printf("pointsx (all the PS values): \n");
	for(i = 0; i < totalnum; i++){
		printf("%d, ", pointsx[i]);
	}
	printf("\n\n");

	printf("pointsx (all the y values): \n");
	for(i = 0; i < totalnum; i++){
		printf("%f, ", pointsy[i]);
	}
	printf("\n\n");

//////////////////////////
// The next step is to send all the points to RX. In detail, send two arrays pointsx[], pointsy[] to RX
// This version is on PC only. Modification required: generate and send the points in batches because of the MSP430 memory limit
//////////////////////////
}

//calculates the product of num*num*... for 'times' time
double mulfortimes(double num, int times){
	if(times == 0){
		return 1.0;
	}else{
		int i;
		double result = num;
		for(i = 1; i<times; i++){
			result = result * num;
		}
		return result;
	}
}

//check if 'val' is in 'set' whose size is 'setsize'
int isinset(int val, int set[], int setsize){
	int i;
	int flag = 0;
	for(i = 0; i < setsize; i++){
		if(val == set[i]){
			flag = 1;
		}
	}
	return flag;
}

//check if point (chaffvalx, chaffvaly) is on poly generated from key[]
int isonpoly(int chaffvalx, double chaffvaly, uint16_t key[]){
	int i;
	double polyval;
	double x = chaffvalx/10000.0;
	int flag = 0;
	for(i = 0; i < polydegree+1; i++){
		polyval += key[i] * mulfortimes(x, (polydegree-i)); 
	}
	if(chaffvaly == polyval){
		flag = 1;
	}
	return flag;
}