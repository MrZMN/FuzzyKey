#include <stdio.h>
#include <stdint.h>

#define keypolydeg 7	//the degree of key polynomial. key length = (keypolydeg+1)*log(n+1) (bit)
#define s 10    		//set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
#define chaff 245		//number of chaff points added for security
#define t 2     		//error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number 
#define n 255   		
//The max tolerable mismatch rate = t/2s
//t = s-keypolydeg-1

//GF(2^8) tables
//polynomial form
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 0};
//index form
uint8_t index_of[n+1] = {255, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};

//check if 'value' is in set[] 
uint8_t isinset(uint8_t value, uint8_t set[], uint8_t setsize){
    uint8_t i;
    uint8_t flag = 0;
    for(i = 0; i < setsize; i++){
        if(value == set[i]){
            flag = 1;
        }
    }
    return flag;
}

//swap the row of a matrix
void exchange_row(uint8_t *matrix[], uint8_t flag, uint8_t row){
  uint8_t i;
  uint8_t *temp;

  for(i = flag+1; i < row; i++){ 
    if(*(matrix[i]+flag) != n){
      temp = matrix[flag];
      matrix[flag] = matrix[i];
      matrix[i] = temp;
    }
  }
}

//solve the solutions of an equation set 
void solve_equationset(uint8_t solutions[], uint8_t *matrix[], uint8_t row, uint8_t col){
  //input is a (row*col) size matrix 'matrix[]'

  int8_t i, j, k;
  uint8_t div;

  //elementary row transformation
  for(i = 0; i < row-1; i++){  

    if(*(matrix[i] + i) == n){   
      exchange_row(matrix, i, row);    
    } 
    if(*(matrix[i]+i) == n){   
      continue;
    }

    for(j = i+1; j < row; j++){   

        if(*(matrix[j]+i) == n){
          div = n;
        }else{
          div = (*(matrix[j]+i) + (n - *(matrix[i]+i))) % n;  
        }

      if(div != n){
        for(k = i; k < col; k++){   
          if(*(matrix[i]+k) != n){
            if(*(matrix[j]+k) == n){
              *(matrix[j]+k) = (div + *(matrix[i]+k)) % n;
            }else{
              *(matrix[j]+k) = index_of[alpha_to[*(matrix[j]+k)] ^ alpha_to[(div + *(matrix[i]+k)) % n]];  
            }
          }
        }
      }
    }
  }
  //the resulted matrix should be a upper triangular matrix 

  //solve the solutions of the equation set based on the upper triangular matrix 
  for(i = row-1; i >= 0; i--){

    for(j = col-2; j > i; j--){

      if(*(matrix[i]+j) != n && solutions[j] != n){     
        if(*(matrix[i]+col-1) == n){
          *(matrix[i]+col-1) = index_of[alpha_to[(*(matrix[i]+j) + solutions[j]) % n]];
        }else{
            *(matrix[i]+col-1) = index_of[alpha_to[*(matrix[i]+col-1)] ^ alpha_to[(*(matrix[i]+j) + solutions[j]) % n]]; 
        }
      }
    }

    if(*(matrix[i]+col-1) == n){
      solutions[i] = n;
    }else{
      solutions[i] = (*(matrix[i]+col-1) + (n - *(matrix[i]+i))) % n; 
    }
  }

  //array solutions[] stores the solutions of the equation set
}

//Division between polynomials. lenA, lenB are the lengthes of polyA[] and polyB[] respectively. store the quotient polynomial in poly[]
void poly_division(uint8_t A[], uint8_t B[], uint8_t lenA, uint8_t lenB, uint8_t plow[]){
  uint8_t hA = 0;              
  uint8_t hB = lenA - lenB;   
  uint8_t i, div, tem = 0, tem1 = 0;

  while(hA <= hB){
    
    div = (A[hA] + (n - B[hB])) % n;  

    plow[hA] = div;
    tem = hA;

    for(i = 0; i < lenB; i++){

      if(B[hB+i] != n){
        if(A[hA+i] == n){
          A[hA+i] = alpha_to[(B[hB+i] + div)%n];
        }else{
          A[hA+i] = alpha_to[A[hA+i]] ^ alpha_to[(B[hB+i] + div)%n];
        }
        A[hA+i] = index_of[A[hA+i]];
      }

      if(A[hA+i] == n){
        tem++;
      }
      if(A[hA+i] != n || tem == lenA-1 || tem == tem1+lenB){
        tem1 = tem;
      }
    }

    hA = tem1;
  }
  //the array plow[] stores the quotient polynoial
}

//Berlekamp-Welch algorithm to correct the mismatches on set elements
void BerlekampWelch(uint8_t ps[], uint8_t points[][2], uint8_t key[]){
  uint8_t i, j;
  uint8_t augmentMat[s][s+1];       //the augmented matrix used in B-W algorithm
  uint8_t *aMat[s];                 //a pointer array - used to accelarate computation
  uint8_t solution[s];              //the solutions of augmentMat[][]
  uint8_t A[s - t/2], B[s - t/2];   //Divide solutions[] into two parts as A() and B()

  ////Use Reed-Soloman Decoding (Berlekamp-Welch algorithm) to generate a poly from 's' num of points, succeed if at least (s-t/2) points are legit
  //generate the augmented matrix
  for(i = 0; i < s; i++){ 
    for(j = 0; j < s-t/2; j++){
      augmentMat[i][j] = (points[i][0] * (s-t/2-1-j)) % n;
    }
    if(points[i][1] == n){
      augmentMat[i][9] = n;
      augmentMat[i][10] = n;
    }else{
      augmentMat[i][9] = points[i][1];                        
      augmentMat[i][10] = (points[i][0] + points[i][1]) % n;
    }
  }
  //convert the 2D augmented array into 1D pointer array format, to accelerate computation
  for(i=0;i<s;i++){
      aMat[i] = augmentMat[i];
  }

  //Use Gussian Elimination to solve the solutions of the augmented matrix
  solve_equationset(solution, aMat, s, s+1);

  //The solutions of the equations consist of 2 polys as A[] and B[]
  for(i = 0; i < s - t/2; i++){
    A[i] = solution[i];
    if(i == s - t -1){
      B[i] = 0;
    }else{
      B[i] = solution[i + t/2];
    }
  }

  //The key polynomial is A()/B(), if the number of errors is within the error threshold
  for(i = 0; i < s - t; i++){
    key[i] = n;
  }
  poly_division(A, B, s - t/2, t/2 + 1, key);
}

void matchpoint(uint8_t ps[], uint8_t points[][2], uint8_t matchedpoints[][2]){
	uint8_t i;
	uint8_t matchidx = 0;
	for(i = 0; i < s+chaff; i++){
		if(isinset(points[i][0], ps, s)){
			matchedpoints[matchidx][0] = points[i][0];
			matchedpoints[matchidx][1] = points[i][1];
			matchidx++;
		}
	}

	if(matchidx != s){
		for(i = 0; i < s-matchidx; i++){
            matchedpoints[s-i-1][0] = 0;
			matchedpoints[s-i-1][1] = 0;
        }
	}
}

//RX of Fuzzy Vault
int main(){

	uint8_t i;

	//RX's set generated from PS measurements. Each element should be different from each other. We use 1~s to simulate it.
	uint8_t ps[s] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
	//points received from TX
	uint8_t points[s+chaff][2] = {{0, 130}, {1, 88}, {2, 175}, {3, 246}, {4, 94}, {5, 90}, {6, 236}, {7, 31}, {8, 187}, {9, 244}, {10, 179}, {11, 29}, {12, 217}, {13, 181}, {14, 231}, {15, 12}, {16, 68}, {17, 181}, {18, 33}, {19, 164}, {20, 118}, {21, 203}, {22, 202}, {23, 250}, {24, 207}, {25, 173}, {26, 129}, {27, 214}, {28, 111}, {29, 141}, {30, 100}, {31, 94}, {32, 82}, {33, 138}, {34, 73}, {35, 234}, {36, 168}, {37, 197}, {38, 30}, {39, 238}, {40, 242}, {41, 96}, {42, 210}, {43, 87}, {44, 62}, {45, 157}, {46, 197}, {47, 211}, {48, 14}, {49, 47}, {50, 108}, {51, 71}, {52, 176}, {53, 195}, {54, 38}, {55, 123}, {56, 98}, {57, 96}, {58, 242}, {59, 219}, {60, 148}, {61, 236}, {62, 55}, {63, 99}, {64, 202}, {65, 254}, {66, 153}, {67, 51}, {68, 78}, {69, 51}, {70, 132}, {71, 200}, {72, 84}, {73, 121}, {74, 78}, {75, 95}, {76, 153}, {77, 67}, {78, 188}, {79, 114}, {80, 238}, {81, 161}, {82, 27}, {83, 207}, {84, 127}, {85, 75}, {86, 43}, {87, 60}, {88, 175}, {89, 181}, {90, 32}, {91, 212}, {92, 136}, {93, 193}, {94, 124}, {95, 164}, {96, 105}, {97, 226}, {98, 121}, {99, 37}, {100, 240}, {101, 66}, {102, 235}, {103, 217}, {104, 19}, {105, 123}, {106, 48}, {107, 117}, {108, 160}, {109, 125}, {110, 236}, {111, 73}, {112, 237}, {113, 197}, {114, 173}, {115, 58}, {116, 16}, {117, 182}, {118, 130}, {119, 22}, {120, 11}, {121, 145}, {122, 216}, {123, 122}, {124, 217}, {125, 176}, {126, 214}, {127, 37}, {128, 234}, {129, 189}, {130, 165}, {131, 169}, {132, 180}, {133, 234}, {134, 71}, {135, 249}, {136, 207}, {137, 136}, {138, 130}, {139, 117}, {140, 110}, {141, 128}, {142, 183}, {143, 59}, {144, 149}, {145, 147}, {146, 196}, {147, 101}, {148, 171}, {149, 32}, {150, 46}, {151, 65}, {152, 171}, {153, 78}, {154, 48}, {155, 89}, {156, 39}, {157, 70}, {158, 61}, {159, 7}, {160, 79}, {161, 133}, {162, 159}, {163, 142}, {164, 185}, {165, 238}, {166, 93}, {167, 26}, {168, 101}, {169, 12}, {170, 138}, {171, 235}, {172, 99}, {173, 23}, {174, 75}, {175, 5}, {176, 198}, {177, 221}, {178, 57}, {179, 24}, {180, 205}, {181, 121}, {182, 19}, {183, 19}, {184, 7}, {185, 52}, {186, 113}, {187, 153}, {188, 161}, {189, 251}, {190, 249}, {191, 239}, {192, 207}, {193, 223}, {194, 169}, {195, 51}, {196, 138}, {197, 187}, {198, 142}, {199, 22}, {200, 201}, {201, 142}, {202, 219}, {203, 227}, {204, 108}, {205, 67}, {206, 177}, {207, 157}, {208, 192}, {209, 214}, {210, 185}, {211, 11}, {212, 199}, {213, 49}, {214, 183}, {215, 109}, {216, 90}, {217, 147}, {218, 153}, {219, 210}, {220, 135}, {221, 60}, {222, 253}, {223, 141}, {224, 233}, {225, 223}, {226, 211}, {227, 63}, {228, 9}, {229, 79}, {230, 148}, {231, 233}, {232, 71}, {233, 132}, {234, 178}, {235, 147}, {236, 198}, {237, 236}, {238, 119}, {239, 67}, {240, 43}, {241, 79}, {242, 158}, {243, 216}, {244, 199}, {245, 77}, {246, 7}, {247, 188}, {248, 87}, {249, 186}, {250, 177}, {251, 250}, {252, 103}, {253, 9}, {254, 215}};
	//matched points
	uint8_t matchedpoints[s][2];
	//the key
	uint8_t key[keypolydeg+1];

	//add some errors (for test only)
	// ps[0] = 21;
	// ps[1] = 22;
	ps[2] = 23;

  	//match the legit points from all received points
	matchpoint(ps, points, matchedpoints);

	//correct the miamatches on sets
	BerlekampWelch(ps, matchedpoints, key);

	//Print the key
	printf("Key:\n");
	for(i = 0; i < s - t; i++){
		printf("%d, ", key[i]);
	}
}