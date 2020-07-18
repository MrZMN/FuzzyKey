#include <stdio.h>

#define polydegree 7      //the degree of the polynomial - corresponding to (polydegree+1) coefficients, which form a 128-bit key
#define numlegitpoint 10  //the num of points taken from the key polynomial
#define totalnum 110      //numlegitpoint + numchaffpoint
#define errtolerant 2     //the error tolerance, which is the max set difference between PS measurements at TX/RX sides
#define opernum 2         //the num of operations needed

//GF(2^8)
#define n 255           //n = 2^m-1
//poly form
int alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 1};
//index form
int index_of[n+1] = {-1, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};


//put the row (whose |flag-th value| is not 0) to the top
void exchange_row(int *matrix[], int flag, int row){
  int i;
  int *temp;

  for(i = flag+1; i < row; i++){  //check the rows from row flag+1 to the bottom
    if(*(matrix[i]+flag) != -1){
      temp = matrix[flag];
      matrix[flag] = matrix[i];
      matrix[i] = temp;
    }
  }
}

//do guass elimination on matrix[]
void guass_elimination(int *matrix[], int row, int col){
  int i, j, k;
  int div;

  for(i = 0; i < row-1; i++){   //do from first row to second last row 
    if(*(matrix[i]+i) == -1){
      exchange_row(matrix, i, row);    
    } 
    if(*(matrix[i]+i) == -1){   //after changing the rows, for row i, if the row-th value is still 0, skip this turn
      continue;
    }

    for(j = i+1; j < row; j++){ //do from row+1 to the bottom   
        if(*(matrix[j]+i) == -1){
          div = -1;
        }else{
          div = (*(matrix[j]+i) + (n - *(matrix[i]+i))) % n;  //div is in index form
        }
      if(div != -1){
        for(k = i; k < col; k++){ //do elementry row transformation on each column
          if(*(matrix[i]+k) != -1){
            if(*(matrix[j]+k) == -1){
              *(matrix[j]+k) = (div + *(matrix[i]+k)) % n;
            }else{
              *(matrix[j]+k) = index_of[alpha_to[*(matrix[j]+k)] ^ alpha_to[(div + *(matrix[i]+k)) % n]];   //make sure for each row behind, their row-th element is reduced to 0
            }
          }
        }
      }
    }
  }
}

//solve the solutions on a triangular matrix matrix[] (after guass elimination)
void solve_equation(int solutions[], int *matrix[], int row, int col){
  int i, j;
  for(i = row-1; i >= 0; i--){
    for(j = col-2; j > i; j--){
      if(*(matrix[i]+j) != -1 && solutions[j] != -1){
        if(*(matrix[i]+col-1) == -1){
          *(matrix[i]+col-1) = index_of[alpha_to[(*(matrix[i]+j) + solutions[j]) % n]];
        }else{
            *(matrix[i]+col-1) = index_of[alpha_to[*(matrix[i]+col-1)] ^ alpha_to[(*(matrix[i]+j) + solutions[j]) % n]]; 
        }
      }
    }
    if(*(matrix[i]+col-1) == -1){
      solutions[i] = -1;
    }else{
      solutions[i] = (*(matrix[i]+col-1) + (n - *(matrix[i]+i))) % n; 
    }
  }
}

//Division between polynomials. lenA is the length of polyA[], same for lenB. store the result poly in poly[]
void poly_division(int A[], int B[], int lenA, int lenB, int key[]){
  int hA = 0;              //h stands for head
  int hB = lenA - lenB;    
  int i, div, tem, tem1;

  while(hA <= hB){
    div = (A[hA] + (n - B[hB])) % n;
    key[hA] = div;
    // printf("%d\n", div);
    tem = hA;
    for(i = 0; i < lenB; i++){
      if(B[hB+i] != -1){
        if(A[hA+i] == -1){
          A[hA+i] = alpha_to[(B[hB+i] + div)%n];
        }else{
          A[hA+i] = alpha_to[A[hA+i]] ^ alpha_to[(B[hB+i] + div)%n];
        }
        A[hA+i] = index_of[A[hA+i]];
      }

      if(A[hA+i] == -1){
        tem++;
      }if(A[hA+i] != -1 || tem == lenA){
        tem1 = tem;
      }      
    }
    hA = tem1;
  }

}

//Berlekamp-Welch algorithm
void BerlekampWelch(int matchedx[], int matchedy[], int key[]){
  unsigned int i,j;
  int Ab_matrix[numlegitpoint][numlegitpoint+1];                                  //Ab_matrix[][] stores the augmented matrix in B-W algorithm
  int *Ab[numlegitpoint];                                                         //Ab is a pointer array - used to accelarate computation

//generate the augmented matrix - this requires modification for diff settings
    for(i = 0; i < numlegitpoint; i++){
        for(j = 0; j < numlegitpoint-errtolerant/2; j++){
            Ab_matrix[i][j] = (matchedx[i] * (numlegitpoint-errtolerant/2-1-j)) % n;
        }
        if(matchedy[i] == -1){
            Ab_matrix[i][9] = -1;
            Ab_matrix[i][10] = -1;
        }else{
            Ab_matrix[i][9] = matchedy[i];
            Ab_matrix[i][10] = (matchedx[i] + matchedy[i]) %n;
        }
    }
    //convert the 2D augmented array into 1D pointer array format, to accelerate computation
    for(i = 0; i < numlegitpoint; i++){
        Ab[i] = Ab_matrix[i];
    }

    //Elementary row transformation to solve the solutions of equations
    guass_elimination(Ab, numlegitpoint, numlegitpoint+1);
    int solution[numlegitpoint];
    solve_equation(solution, Ab, numlegitpoint, numlegitpoint+1);

    //The solutions of the equations consist of 2 polys: assume they are A[] and B[] 
    int A[numlegitpoint - errtolerant/2], B[numlegitpoint - errtolerant/2];
    for(i = 0; i < numlegitpoint - errtolerant/2; i++){
        A[i] = solution[i];
        if(i < numlegitpoint - errtolerant -1){
        B[i] = -1;
        }else if(i == numlegitpoint - errtolerant -1){
        B[i] = 0;
        }else{
        B[i] = solution[i + errtolerant/2];
        }
    }

    //The key poly is the division of A[]/B
    for(i = 0; i < numlegitpoint - errtolerant; i++){
        key[i] = -1;
    }
    poly_division(A, B, numlegitpoint - errtolerant/2, errtolerant/2 + 1, key);

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

//This is the RX of fuzzy vault
int main(){

    register int i, j, k;

    int wholeps[opernum*numlegitpoint] = {158, 98, 112, 169, 153, 66, 93, 130, 81, 144, 70, 5, 99, 228, 92, 177, 172, 93, 85, 234};   //the ps measurements at RX side in 'opernum' num of operations (divided into operations because of device resource limit).
    //add some errors. The error tolerance depends on t.
    wholeps[0] ^= 1;
    wholeps[10] ^= 1;
    // wholeps[11] ^= 1;
    //points received from TX. Most of them are chaff points
    int wholex[opernum*totalnum] = {237, 197, 22, 16, 182, 213, 158, 91, 52, 108, 29, 111, 56, 98, 109, 137, 205, 112, 187, 236, 228, 232, 59, 203, 76, 21, 80, 31, 253, 131, 95, 4, 37, 196, 123, 202, 227, 85, 161, 6, 191, 127, 220, 142, 240, 132, 233, 169, 153, 86, 221, 170, 66, 241, 94, 151, 120, 57, 115, 43, 185, 92, 93, 128, 200, 143, 168, 247, 133, 130, 122, 225, 234, 186, 249, 245, 209, 126, 26, 181, 223, 229, 100, 218, 255, 50, 17, 8, 184, 210, 231, 27, 226, 81, 175, 49, 157, 124, 183, 58, 14, 144, 244, 28, 250, 38, 239, 84, 60, 9, 45, 144, 117, 70, 97, 215, 51, 77, 125, 5, 130, 75, 99, 32, 162, 228, 102, 48, 139, 155, 188, 173, 16, 0, 159, 10, 189, 201, 52, 21, 92, 64, 113, 42, 4, 98, 55, 153, 216, 25, 178, 88, 71, 15, 19, 80, 169, 104, 248, 91, 129, 246, 167, 177, 137, 6, 136, 148, 211, 233, 62, 224, 135, 172, 202, 146, 182, 190, 150, 18, 119, 243, 57, 138, 134, 41, 230, 23, 207, 11, 29, 208, 86, 205, 108, 115, 203, 114, 106, 34, 93, 192, 69, 254, 2, 242, 53, 212, 161, 168, 49, 221, 128, 85, 116, 187, 234, 149, 58, 96};
    int wholey[opernum*totalnum] = {116, 115, 196, 212, 21, 80, 28, 158, 201, 152, 2, 75, 34, 179, 88, 116, 190, 194, 155, 104, 147, 126, 42, 50, 144, 185, 255, 163, 148, 15, 222, 174, 148, 248, 2, 126, 135, 92, 5, 94, 8, 21, 18, 49, 184, 163, 222, 137, 202, 252, 70, 192, 226, 5, 242, 108, 1, 236, 213, 103, 93, 83, 86, 70, 160, 154, 171, 56, 65, 139, 194, 176, 166, 179, 209, 51, 56, 246, 41, 161, 116, 107, 134, 251, 199, 178, 18, 180, 211, 72, 196, 215, 171, 31, 205, 0, 66, 5, 119, 228, 123, 36, 100, 115, 218, 167, 108, 215, 32, 196, 34, 230, 21, 22, 77, 207, 46, 224, 149, 184, 90, 190, 213, 195, 96, 205, 25, 63, 245, 180, 4, 225, 65, 163, 227, 205, 225, 26, 168, 218, 86, 139, 164, 72, 197, 65, 244, 226, 233, 76, 43, 100, 60, 212, 50, 63, 122, 97, 108, 225, 165, 58, 33, 87, 165, 250, 220, 231, 193, 16, 243, 176, 67, 116, 26, 251, 212, 33, 2, 212, 176, 26, 14, 47, 89, 90, 31, 5, 220, 78, 108, 231, 179, 132, 179, 66, 48, 10, 29, 252, 99, 15, 44, 157, 233, 33, 59, 224, 110, 63, 232, 180, 154, 4, 114, 109, 206, 41, 218, 68};
    int ps[numlegitpoint];          //ps measurements at RX side
    int pointsx[totalnum];          //all x-coordinates, received from TX
    int pointsy[totalnum];          //all y-coordinates, received from RX
    int matchedx[numlegitpoint];       //x-coordinates of the matched points
    int matchedy[numlegitpoint];       //y-coordinates of the matched points
    int wholekey[opernum*polydegree+opernum];     //may need to agree the whole 128-bit key for 'opernum' num of times, due to the hardware constraints
    

    //divide the computation into opernum pieces because of device limitation, otherwise MSP430 wont hold the correcting process
    for(k = 0; k < opernum; k++){

        for(i = 0; i < numlegitpoint; i++){
          ps[i] = wholeps[k*numlegitpoint+i];
        }
        for(i = 0; i < totalnum; i++){
          pointsx[i] = wholex[k*totalnum+i];
        }
        for(i = 0; i < totalnum; i++){
          pointsy[i] = wholey[k*totalnum+i];
        }

        //look for the legit points from all received points
        int matched = 0;
        for(i = 0; i < totalnum; i++){
            if(isinset(pointsx[i], ps, numlegitpoint)){
                matchedx[matched] = pointsx[i];
                matchedy[matched] = pointsy[i];
                matched ++;
            }           
        }

        //if there're less than 'numlegitpoint' points matched, use (0,0) to sub
        if(matched != numlegitpoint){
            for(i = 0; i < numlegitpoint-matched; i++){
                matchedx[numlegitpoint-i-1] = 0;
                matchedy[numlegitpoint-i-1] = 0;
            }
        }

        int key[polydegree+1];
        //Input: all the matched points. Output: key[] stores the key polynomial (whose coefficients form the key)
        BerlekampWelch(matchedx, matchedy, key);

        for(i = 0; i < polydegree+1; i++){
          wholekey[i+(polydegree+1)*k] = key[i];
        }
    }

    printf("the 128-bit key:\n");
    for(i = 0; i < opernum*polydegree+opernum; i++){
          printf("%x", wholekey[i]);
    }
    printf("\n");

  
}
