#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define polydegree 7
#define numlegitpoint 10
#define errtolerant 2
#define totalnum 110
//parameter settings for a perticular binary (length, k, d) BCH code
#define n 255           //n = 2^m-1
int tolerantflag = 1;

//GF(2^8)
int alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 1};
int index_of[n+1] = {-1, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};

void guass_elimination(int *[], int, int);
void exchange_row(int *[], int, int);
void solve_equation(int [], int *[], int, int);
void poly_division(int [], int [], int, int, int []);
int isinset(int, int [], int);
void BerlekampWelch(int [], int [], int []);

//This is the Receiver which uses fuzzy vault
int main(){

    srand((unsigned)time(NULL));    //For random number generation

    register int i, j, k, index = 0;

    int wholeps[2*numlegitpoint] = {27, 109, 2, 239, 67, 76, 178, 126, 105, 229, 18, 7, 136, 197, 28, 81, 20, 93, 92, 58};
    //errors
    wholeps[0] ^= 1;
    wholeps[10] ^= 1;
    // wholeps[11] ^= 1;

    int wholex[2*totalnum] = {136, 31, 47, 116, 89, 7, 59, 230, 45, 39, 213, 187, 218, 98, 48, 132, 86, 144, 113, 206, 64, 142, 135, 27, 38, 204, 109, 148, 210, 53, 2, 91, 239, 19, 238, 34, 199, 202, 164, 70, 133, 216, 22, 67, 69, 16, 146, 247, 217, 29, 186, 101, 97, 100, 163, 118, 152, 172, 119, 76, 121, 170, 55, 30, 94, 25, 224, 189, 10, 137, 245, 155, 41, 223, 32, 177, 12, 178, 165, 226, 190, 222, 127, 46, 181, 75, 107, 83, 126, 169, 143, 192, 52, 95, 105, 173, 80, 229, 56, 159, 102, 37, 18, 125, 122, 6, 176, 185, 36, 215, 171, 84, 21, 109, 18, 74, 232, 13, 85, 99, 7, 221, 76, 115, 183, 72, 103, 151, 45, 73, 167, 44, 233, 59, 203, 136, 129, 161, 175, 194, 197, 128, 89, 144, 236, 187, 4, 40, 204, 28, 63, 231, 90, 105, 81, 57, 62, 106, 147, 237, 5, 200, 205, 47, 234, 220, 19, 68, 241, 239, 70, 157, 251, 170, 60, 150, 163, 30, 247, 49, 139, 253, 135, 180, 188, 20, 227, 43, 29, 64, 15, 210, 153, 209, 130, 206, 191, 222, 142, 114, 46, 93, 244, 124, 229, 8, 195, 33, 158, 134, 92, 249, 196, 127, 53, 193, 162, 58, 2, 186};
    int wholey[2*totalnum] = {81, 59, 88, 56, 225, 36, 63, 61, 92, 218, 92, 113, 136, 47, 221, 178, 182, 250, 156, 21, 159, 70, 161, 78, 13, 75, 44, 4, 156, 234, 198, 142, 16, 188, 203, 177, 222, 170, 56, 140, 44, 63, 149, 215, 239, 181, 24, 192, 230, 46, 91, 157, 246, 94, 81, 76, 125, 90, 57, 102, 234, 124, 102, 4, 230, 244, 114, 55, 114, 73, 116, 54, 22, 183, 42, 122, 180, 245, 190, 120, 40, 102, 185, 4, 70, 190, 71, 249, 90, 154, 200, 57, 107, 146, 135, 232, 157, 69, 165, 154, 165, 215, 245, 162, 48, 183, 39, 233, 153, 41, 161, 242, 78, 37, 138, 146, 245, 226, 160, 156, 29, 176, 43, 183, 234, 124, 53, 40, 42, 233, 136, 25, 74, 20, 220, 180, 20, 52, 152, 180, 17, 198, 70, 58, 100, 29, 103, 215, 132, 217, 118, 88, 134, 66, 117, 130, 51, 32, 188, 9, 168, 223, 77, 28, 203, 89, 47, 216, 81, 93, 129, 223, 182, 3, 162, 151, 188, 249, 15, 88, 179, 180, 82, 97, 133, 113, 238, 103, 23, 250, 21, 229, 99, 12, 156, 97, 40, 60, 170, 220, 227, 235, 217, 232, 152, 240, 254, 69, 54, 133, 32, 168, 208, 0, 155, 212, 152, 4, 103, 104};
    int ps[numlegitpoint];         //ps measurements at RX side, in index form
    int pointsx[totalnum];   //received from TX
    int pointsy[totalnum];        //received from RX
    int setAx[numlegitpoint];                                                       //x values of the matched points
    int setAy[numlegitpoint];                                                       //y values of the matched points
    int wholekey[2*polydegree+2];
    
    for(k = 0; k < 2; k++){

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
              setAx[matched] = pointsx[i];
              setAy[matched] = pointsy[i];
              matched ++;
          }           
      }

      //if there're less than 'numlegitpoint' points matched, use (0,0) to sub
      if(matched != numlegitpoint){
          for(i = 0; i < numlegitpoint-matched; i++){
              setAx[numlegitpoint-i-1] = 0;
              setAy[numlegitpoint-i-1] = 0;
          }
      }

      for(i = 0; i < numlegitpoint; i++){
        printf("%d, ", setAx[i]);
      }
      printf("\n");
      for(i = 0; i < numlegitpoint; i++){
        printf("%d, ", setAy[i]);
      }
      printf("\n");

      int key[polydegree+1];
      BerlekampWelch(setAx, setAy, key);

      for(i = 0; i < polydegree+1; i++){
        wholekey[index] = key[i];
        index++;
      }

    }

    for(i = 0; i < 2*polydegree+2; i++){
      if(wholekey[i] <= 0xf){
          printf("0%x", wholekey[i]);
      }else{
          printf("%x", wholekey[i]);
      }
    }

  
}

void BerlekampWelch(int setAx[], int setAy[], int key[]){
  unsigned int i,j;
  int Ab_matrix[numlegitpoint][numlegitpoint+1];                                  //Ab_matrix[][] stores the augmented matrix in B-W algorithm
  int *Ab[numlegitpoint];                                                         //Ab is a pointer array - used to accelarate computation

//generate the augmented matrix - this requires modification for diff settings
    for(i = 0; i < numlegitpoint; i++){
        for(j = 0; j < numlegitpoint-errtolerant/2; j++){
            Ab_matrix[i][j] = (setAx[i] * (numlegitpoint-errtolerant/2-1-j)) % n;
        }
        if(setAy[i] == -1){
            Ab_matrix[i][9] = -1;
            Ab_matrix[i][10] = -1;
        }else{
            Ab_matrix[i][9] = setAy[i];
            Ab_matrix[i][10] = (setAx[i] + setAy[i]) %n;
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

    if(tolerantflag){
        // printf("key(x):\n");
        // for(i = 0; i < numlegitpoint-errtolerant; i++){
        //     if(key[i] <= 0xf){
        //         printf("0%x", key[i]);
        //     }else{
        //         printf("%x", key[i]);
        //     }
        // }
        // printf("\n");
    }else{
        // printf("Too many errors\n");
    }
}

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

//lenA is the length of polyA (degreeA+1), same for lenB
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

  for(i = 0; i < lenA; i++){
    if(A[i] != -1){
      tolerantflag = 0;
    }
  }
}

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
