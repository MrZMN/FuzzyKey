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

	//RX's set generated from PS measurements. Each element should be different from each other. We use 0~s-1 to simulate it.
	uint8_t ps[s] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
	//points received from TX
	uint8_t points[s+chaff][2] = {{0, 255}, {1, 88}, {2, 175}, {3, 246}, {4, 94}, {5, 90}, {6, 236}, {7, 31}, {8, 187}, {9, 244}, {10, 137}, {11, 142}, {12, 54}, {13, 88}, {14, 163}, {15, 55}, {16, 151}, {17, 123}, {18, 160}, {19, 131}, {20, 193}, {21, 82}, {22, 117}, {23, 99}, {24, 89}, {25, 246}, {26, 84}, {27, 211}, {28, 254}, {29, 183}, {30, 202}, {31, 221}, {32, 69}, {33, 131}, {34, 75}, {35, 140}, {36, 129}, {37, 233}, {38, 81}, {39, 244}, {40, 16}, {41, 10}, {42, 126}, {43, 110}, {44, 159}, {45, 237}, {46, 90}, {47, 159}, {48, 111}, {49, 164}, {50, 139}, {51, 112}, {52, 147}, {53, 164}, {54, 127}, {55, 172}, {56, 122}, {57, 24}, {58, 17}, {59, 51}, {60, 220}, {61, 204}, {62, 51}, {63, 138}, {64, 148}, {65, 211}, {66, 206}, {67, 104}, {68, 152}, {69, 128}, {70, 220}, {71, 60}, {72, 118}, {73, 250}, {74, 245}, {75, 148}, {76, 189}, {77, 124}, {78, 19}, {79, 195}, {80, 23}, {81, 114}, {82, 246}, {83, 185}, {84, 84}, {85, 19}, {86, 211}, {87, 33}, {88, 227}, {89, 128}, {90, 125}, {91, 141}, {92, 89}, {93, 222}, {94, 27}, {95, 86}, {96, 220}, {97, 147}, {98, 31}, {99, 173}, {100, 219}, {101, 32}, {102, 22}, {103, 113}, {104, 157}, {105, 237}, {106, 173}, {107, 150}, {108, 47}, {109, 89}, {110, 238}, {111, 20}, {112, 165}, {113, 9}, {114, 82}, {115, 40}, {116, 157}, {117, 88}, {118, 131}, {119, 190}, {120, 190}, {121, 218}, {122, 56}, {123, 38}, {124, 114}, {125, 222}, {126, 147}, {127, 9}, {128, 63}, {129, 218}, {130, 165}, {131, 22}, {132, 23}, {133, 196}, {134, 229}, {135, 117}, {136, 61}, {137, 6}, {138, 230}, {139, 44}, {140, 196}, {141, 62}, {142, 247}, {143, 232}, {144, 87}, {145, 238}, {146, 114}, {147, 253}, {148, 34}, {149, 179}, {150, 250}, {151, 114}, {152, 126}, {153, 10}, {154, 188}, {155, 214}, {156, 42}, {157, 210}, {158, 43}, {159, 8}, {160, 214}, {161, 36}, {162, 218}, {163, 38}, {164, 82}, {165, 188}, {166, 20}, {167, 115}, {168, 211}, {169, 38}, {170, 119}, {171, 88}, {172, 237}, {173, 109}, {174, 176}, {175, 254}, {176, 16}, {177, 139}, {178, 148}, {179, 64}, {180, 227}, {181, 239}, {182, 210}, {183, 189}, {184, 31}, {185, 188}, {186, 101}, {187, 19}, {188, 147}, {189, 16}, {190, 34}, {191, 69}, {192, 96}, {193, 32}, {194, 10}, {195, 29}, {196, 201}, {197, 137}, {198, 219}, {199, 54}, {200, 245}, {201, 230}, {202, 139}, {203, 226}, {204, 251}, {205, 178}, {206, 147}, {207, 80}, {208, 249}, {209, 56}, {210, 179}, {211, 110}, {212, 141}, {213, 191}, {214, 32}, {215, 76}, {216, 149}, {217, 76}, {218, 19}, {219, 109}, {220, 83}, {221, 8}, {222, 216}, {223, 36}, {224, 106}, {225, 156}, {226, 70}, {227, 155}, {228, 37}, {229, 6}, {230, 247}, {231, 43}, {232, 149}, {233, 10}, {234, 174}, {235, 186}, {236, 161}, {237, 108}, {238, 243}, {239, 34}, {240, 135}, {241, 70}, {242, 150}, {243, 55}, {244, 178}, {245, 122}, {246, 201}, {247, 223}, {248, 140}, {249, 85}, {250, 119}, {251, 52}, {252, 253}, {253, 242}, {254, 87}};
	//matched points
	uint8_t matchedpoints[s][2];
	//the key
	uint8_t key[keypolydeg+1];

	//add some errors (for test only)
	ps[0] = 21;
	// ps[1] = 22;
	// ps[2] = 23;

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