#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define pslength 10     //num of ps measurements on both sides ('s' in the fuzzy extractor paper) in each operation, assume each ps value is one byte 
#define opernum 2       //the num of operations needed
#define t 2             //error tolerance (t in the fuzzy extractor paper), which stands for max set difference size between TX/RX, must be even number 

//define the operations for sha-256  
#define SHFR(x, times) (((x) >> (times)))
#define ROTR(x, times) (((x) >> (times)) | ((x) << ((sizeof(x) << 3) - (times))))
#define ROTL(x, times) (((x) << (times)) | ((x) >> ((sizeof(x) << 3) - (times))))
#define CHX(x, y, z) (((x) &  (y)) ^ (~(x) & (z)))
#define MAJ(x, y, z) (((x) &  (y)) ^ ( (x) & (z)) ^ ((y) & (z)))
#define BSIG0(x) (ROTR(x,  2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define BSIG1(x) (ROTR(x,  6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define SSIG0(x) (ROTR(x,  7) ^ ROTR(x, 18) ^ SHFR(x,  3))
#define SSIG1(x) (ROTR(x, 17) ^ ROTR(x, 19) ^ SHFR(x, 10))
#define SHA256_BLOCK_SIZE (512/8)
#define SHA256_COVER_SIZE (SHA256_BLOCK_SIZE*2)
//the IV of sha-256
static uint32_t inisett[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
};

//GF(2^8)
#define n 255           //n = 2^m-1
//poly form
int alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 0};
//index form
int index_of[n+1] = {-1, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};
int tolerantflag = 1;   //show if the mismatches are within the error tolerance interval


//project x -> y on the poly[] 
int projectpoints(int size, int numcoeff, int x, int poly[]){
    unsigned int i;
    int y = -1;

    if(x != -1){
        y = (x * size) % n;
        for(i = 0; i < numcoeff; i++){
            if(y == -1){
                y = (poly[i] + (x * (size-i-1))) % n;
            }else{
                y = index_of[alpha_to[y] ^ alpha_to[(poly[i] + (x * (size-i-1))) % n]];
            }
        }
    }
    return y;
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
void poly_division(int A[], int B[], int lenA, int lenB, int plow[]){
  int hA = 0;              //h stands for head
  int hB = lenA - lenB;    
  int i, div, tem, tem1;

  while(hA <= hB){
    div = (A[hA] + (n - B[hB])) % n;
    plow[hA] = div;
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

//sort arr[] in ascending form
void bubble_sort(uint8_t arr[], int len) {
    int i, j, temp;
    for (i = 0; i < len - 1; i++){
        for (j = 0; j < len - 1 - i; j++){
            if (arr[j] > arr[j + 1]){
                temp = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = temp;
            }
        }
    }
}

//Berlekamp-Welch algorithm
void BerlekampWelch(uint8_t ps[], int ss[]){
  register int i, j;
  int points[pslength][2];                                           //points[][2] stores the points generated by projecting ps values on phigh(x)
  int Ab_matrix[pslength][pslength+1];                                //Ab_matrix[][] stores the augmented matrix used in B-W algorithm
  int *Ab[pslength];                                                 //Ab is a pointer array - used to accelarate computation

  //evaluate the points by projecting ps values on phigh(x). The points are stored in points[][]
  for(i = 0; i < pslength; i++){
      points[i][0] = ps[i];
      points[i][1] = projectpoints(pslength, t, points[i][0], ss);
  }

  //Use Reed-Soloman Decoding (Berlekamp-Welch algorithm) to generate a poly from 'pslength' num of points, succeed if at least (s-t/2) points are legit
  //generate the augmented matrix - this requires modification for diff settings
  for(i = 0; i < pslength; i++){ 
    for(j = 0; j < pslength-t/2; j++){
      Ab_matrix[i][j] = (points[i][0] * (pslength-t/2-1-j)) % n;
    }
    if(points[i][1] == -1){
      Ab_matrix[i][9] = -1;     //e1
      Ab_matrix[i][10] = -1;
    }else{
      Ab_matrix[i][9] = points[i][1];                           //e1
      Ab_matrix[i][10] = (points[i][0] + points[i][1]) % n;
    }
  }
  //convert the 2D augmented array into 1D pointer array format, to accelerate computation
  for(i=0;i<pslength;i++){
      Ab[i] = Ab_matrix[i];
  }
  
  //Elementary row transformation to solve the solutions of equations
  guass_elimination(Ab, pslength, pslength+1);
  int solution[pslength];
  solve_equation(solution, Ab, pslength, pslength+1);

  //The solutions of the equations consist of 2 polys: assume they are A[] and B[]
  int A[pslength - t/2], B[pslength - t/2];
  for(i = 0; i < pslength - t/2; i++){
    A[i] = solution[i];
    if(i < pslength - t -1){
      B[i] = -1;
    }else if(i == pslength - t -1){
      B[i] = 0;
    }else{
      B[i] = solution[i + t/2];
    }
  }

  //The poly plow(x) (in fuzzy extractor paper) is the division of A[]/B
  int plow[pslength - t]; 
  for(i = 0; i < pslength - t; i++){
    plow[i] = -1;
  }
  poly_division(A, B, pslength - t/2, t/2 + 1, plow);

  if(tolerantflag){
    //Find the roots of p(x) = phigh(x) + plow(x), which are the PS values at the TX side
    int px[pslength];
    uint8_t roots[pslength]; 
 
    for(i = 0; i < t; i++){
      px[i] = ss[i]; 
    }
    for(i = t; i < pslength; i++){
      px[i] = plow[i - t];
    }

    int idx = 0, result = 0;
    for(i = 0; i < n; i++){   //test all posibilities on GF(2^m)
      result = projectpoints(pslength+1, pslength, i, px);
      if(result == -1){
        roots[idx] = i;
        idx++;
      }
    }

    for(i = 0; i < pslength; i++){
      ps[i] = roots[i];
    }

  }//otherwise it means there're too many errors

}

void transform(const uint8_t *msg, uint32_t *h){

    uint32_t w[64];
    uint32_t a0, b1, c2, d3, e4, f5, g6, h7;
    uint32_t t1, t2;
 
    int i, j = 0;
 
    for (i=0; i<16; i++) {
        w[i] = (uint32_t)msg[j]<<24 | (uint32_t)msg[j+1]<<16 | (uint32_t)msg[j+2]<<8 | msg[j+3];
        j += 4;
    }
 
    for(i=16; i<64; i++){
        w[i] = SSIG1(w[i-2])+w[i-7]+SSIG0(w[i-15])+w[i-16];
    }
 
    a0 = h[0];
    b1 = h[1];
    c2 = h[2];
    d3 = h[3];
    e4 = h[4];
    f5 = h[5];
    g6 = h[6];
    h7 = h[7];
 
    for (i= 0; i<64; i++) {
        t1 = h7 + BSIG1(e4) + CHX(e4, f5, g6) + inisett[i] + w[i];
        t2 = BSIG0(a0) + MAJ(a0, b1, c2);
        h7 = g6;
        g6 = f5;
        f5 = e4;
        e4 = d3 + t1;
        d3 = c2;
        c2 = b1;
        b1 = a0;
        a0 = t1 + t2;
    }
 
    h[0] += a0;
    h[1] += b1;
    h[2] += c2;
    h[3] += d3;
    h[4] += e4;
    h[5] += f5;
    h[6] += g6;
    h[7] += h7;
}
 
void sha256(const uint8_t *message, uint32_t len, uint32_t *sha)
{
    uint8_t *tmp = (uint8_t*)message;
    uint8_t  cover_data[SHA256_COVER_SIZE];
    uint32_t cover_size = 0;
    
    uint32_t i = 0;
    uint32_t blocknum = 0;
    uint32_t padlen = 0;
    uint32_t h[8];
 
    h[0] = 0x6a09e667;
    h[1] = 0xbb67ae85;
    h[2] = 0x3c6ef372;
    h[3] = 0xa54ff53a;
    h[4] = 0x510e527f;
    h[5] = 0x9b05688c;
    h[6] = 0x1f83d9ab;
    h[7] = 0x5be0cd19;
 
    memset(cover_data, 0x00, sizeof(uint8_t)*SHA256_COVER_SIZE);
 
    blocknum = len / SHA256_BLOCK_SIZE;
    padlen = len % SHA256_BLOCK_SIZE;
 
    if (padlen < 56 ) {
        cover_size = SHA256_BLOCK_SIZE;
    }else {
        cover_size = SHA256_BLOCK_SIZE*2;
    }
 
    if (padlen != 0) {
        memcpy(cover_data, tmp + (blocknum * SHA256_BLOCK_SIZE), padlen);
    }
    cover_data[padlen] = 0x80;
    cover_data[cover_size-4]  = ((len*8)&0xff000000) >> 24;
    cover_data[cover_size-3]  = ((len*8)&0x00ff0000) >> 16;
    cover_data[cover_size-2]  = ((len*8)&0x0000ff00) >> 8;
    cover_data[cover_size-1]  = ((len*8)&0x000000ff);
 
    for (i=0; i<blocknum; i++) {
        transform(tmp, h);
        tmp += SHA256_BLOCK_SIZE;
    }

    tmp = cover_data;
    blocknum = cover_size / SHA256_BLOCK_SIZE;
    for (i=0; i<blocknum; i++) {
        transform(tmp, h);
        int j;
        tmp += SHA256_BLOCK_SIZE;
    }


    
    memcpy(sha, h, sizeof(uint32_t)*8);

}

//The RX of Improved Juels-Sudan fuzzy vault
//Received two info from TX, which are randStr[] and wholess[] below 
int main(){

    register int i, j, k;
    
    uint8_t wholeps[opernum*pslength] = {146, 209, 212, 73, 96, 79, 23, 235, 237, 141, 226, 204, 177, 70, 50, 243, 25, 21, 96, 100}; //All the PS values (in 'opernum' num of operations) received from TX
    //add some errors. The error tolerance depends on t.
    wholeps[0] ^= 1;
    wholeps[10] ^= 1;
    // wholeps[11] ^= 1;
    uint8_t wholess[opernum*t] = {23, 218, 155, 220};   //All the secure sketches (in 'opernum' num of operations) received from TX
    uint8_t randStr[opernum*pslength] = {73, 123, 159, 178, 135, 200, 148, 202, 166, 145, 76, 91, 102, 199, 231, 124, 14, 186, 46, 49}; //Received from TX
    uint8_t ps[pslength];       //ps[] stores the PS values measured by the RX - poly form. In one operation only
    int ss[t];                  //ss[] stores the secure sketch received from TX. In one operation only
    uint32_t key[8];            //the final key

    //divide the computation into opernum pieces because of device limitation, otherwise MSP430 wont hold the correcting process
    for(k = 0; k < opernum; k++){

      //Reload the PS measurements and secure sketch for each operation
      for(i = 0; i < pslength; i++){
        ps[i] = wholeps[k*pslength+i];
      }
      for(i = 0; i < t; i++){
        ss[i] = wholess[k*t+i];
      }

      //Correct the mismatches of PS values
      BerlekampWelch(ps, ss);

      for(i = 0; i < pslength; i++){
        wholeps[i+k*pslength] = ps[i];
      }
    }

    //Generate the 128-bit key
    //sort the array (sort because RX could only recover all the PS elements, but doesn't ensure the PS values are in the same order)
    bubble_sort(wholeps, opernum*pslength);
  
    //ps values xor random string
    for(i = 0; i < opernum*pslength; i++){
        wholeps[i] ^= randStr[i];
    }

    //generate the uniformly distributed key using sha-256
    sha256(wholeps, opernum*pslength, key);

    //take the first 128 bits of sha-256 output as the key
    printf("The final key\n");
    for(i = 0; i < 4; i++){
        printf("%x", key[i]);
    }
    printf("\n");

}
