#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// #define m 8         //computations over GF(2^m)
#define n 255       	//n = 2^m -1 as primitive binary BCH code
#define t 4         	//error correcting capability - max set difference size between ps measurements on TX/RX side
#define nummeasure 20	//num of ps measurements on both sides

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
//poly form
int alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 0};
//index form
int index_of[n+1] = {-1, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};

void decode_bch(int [], int []);
void dosetdiff(uint8_t [], int, int [], int);
void bubble_sort(uint8_t [], int);
void transform(const uint8_t *, uint32_t *);
void sha256(const uint8_t *, uint32_t, uint32_t *);

//This is RX of a BCH-based Pinsketch
int main(){

    register int i, j, degree, flag = 0;

    uint8_t ps[nummeasure] = {243, 69, 179, 151, 137, 107, 187, 15, 26, 105, 145, 206, 162, 123, 13, 1, 82, 9, 30, 76};   	//stores the ps measurements at RX side, may have several mismatches with the ones at TX side
    uint8_t randStr[nummeasure] = {246, 221, 16, 246, 8, 23, 247, 47, 243, 17, 241, 5, 163, 47, 229, 76, 141, 58, 247, 47};
    int ssTX[t] = {52, 225, 104, 0};                                                        				//stores secure sketch received from TX
    int ssRX[t];																						//secure sketch of RX side
    int s[2*t+1];   																					//the whole syndrome, generated from ssRX[] xor ssTX[]
    uint32_t key[8];      //the final key

    //Calculate the secure sketch
    for(i = 0; i < t; i++){
        ssRX[i] = 0;
        degree = 2*i+1;     //degree is 1, 3, 5... we only need the odd-indexes because for binary BCH code, each even-index value could be generated from the odd-index values

        for(j = 0; j < nummeasure; j++){
        	if(ps[j] != 0){
				ssRX[i] ^= alpha_to[(index_of[ps[j]]*degree) % n];	//poly form
			}
        }
        
        //Calculate the syndrome of error = syn(TX) xor syn(RX)
        ssRX[i] = index_of[ssRX[i] ^ ssTX[i]];						//index form

		if(ssRX[i] != -1){	//when the syndrome of error isn't a zero vector, there's mismatch
            flag = 1;
        }
    }

    //if there're mismatches
    if(flag){

        // calculate the whole syndrome
        s[0] = 0;
        for(i = 1; i < 2*t+1; i++){
            s[i] = 0;
            if(i%2 != 0){
                s[i] = ssRX[(i-1)/2];
            }else{
            	if(s[i/2] == -1){
            		s[i] = -1;
            	}else{
            		s[i] = (s[i/2]*2)%n;
            	}
            }
        }

        //Decode the syndrome
        int setdiff[t];		//stores the set difference between ps measurements at TX/RX sides
        for(i = 0; i < t; i++){
        	setdiff[i] = -1;
        }
        decode_bch(s, setdiff);
        
        //Correct the set - only concern the mismatch situation, i.e. TX/RX have same num of measurements and some of them mismatch
        dosetdiff(ps, nummeasure, setdiff, t);

    }else{
           printf("There are no errors\n");
    }

    printf("The ps measurements are:\n");
    for(i = 0; i < nummeasure; i++){
        printf("%d, ", ps[i]);
    }
    printf("\n");

    bubble_sort(ps, nummeasure);

    //ps values xor random string
    for(i = 0; i < nummeasure; i++){
        ps[i] ^= randStr[i];
    }

    //generate the key
    sha256(ps, nummeasure, key);

    //take the first 128 bits of sha-256 output as the key
    for(i = 0; i < 4; i++){
        printf("%x", key[i]);
    }

}

//calculate arr1[] = setdifference (arr1[], arr2[])
void dosetdiff(uint8_t arr1[], int arr1size, int arr2[], int arr2size){
	unsigned int i, j;
    int temp1[arr1size], temp2[arr2size];
	for(i = 0; i < arr1size; i++){
		for(j = 0; j < arr2size; j++){
			if(arr1[i] == arr2[j]){
				temp1[i] = -1;
				temp2[j] = -1;
			}
		}
	}

	for(i = 0; i < arr2size; i++){
		if(temp2[i] != -1){
			for(j = 0; j < arr1size; j++){
				if(temp1[j] == -1){
					arr1[j] = arr2[i];
                    temp1[j] = 0;
					break;
				}
			}
		}
	}
}

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


//Simon Rockliff's implementation of Berlekamp's algorithm.
void decode_bch(int s[], int setdiff[]){

    register int    i, j, u, q, count;
    int             elp[2*t+2][t+1], d[2*t], l[2*t], u_l[2*t],reg[2*t]; //elp is the error location poly; d, l, u_l comes from book 'Error control coding' page 140

    //Step 1: calculate the error location poly
    /*
     * Compute the error location polynomial via the Berlekamp
     * iterative algorithm. Following the terminology of Lin and
     * Costello's book :   d[u] is the 'mu'th discrepancy, where
     * 'mu'= u-1 and 'mu' (the Greek letter!) is the step number
     * ranging from -1 to 2*t (see L&C),  l[u] is the degree of
     * the elp at that step(step mu), and u_l[u] is the difference between
     * the step number (mu) and the degree of the elp. elp[u][] actually corresponds to step mu
     */
    //initialise the iterative table - see P140 in the book
    d[0] = 0;           //index form - a^0
    d[1] = s[1];        //index form
    elp[0][0] = 0;      //index form - a^0
    elp[1][0] = 1;      //polynomial form - 1
    //?
    for(i = 1; i < 2*t; i++) {
        elp[0][i] = -1; //index form
        elp[1][i] = 0;  //polynomial form
    }
    //the reason to use mu is that there's an index of -1
    l[0] = 0;
    l[1] = 0;
    u_l[0] = -1;
    u_l[1] = 0;

    u = 0;

    //get the error location poly
    do {
        u++;
        if(d[u] == -1) {
            l[u + 1] = l[u];
            for (i = 0; i <= l[u]; i++) {
                elp[u + 1][i] = elp[u][i];
                elp[u][i] = index_of[elp[u][i]];
            }
        }else

        //search for words with greatest u_l[q] for which d[q]!=0
        {
            q = u - 1;
            while ((d[q] == -1) && (q > 0))
                q--;
            //have found first non-zero d[q]
            if(q > 0) {
                j = q;
                do{
                    j--;
                    if ((d[j] != -1) && (u_l[q] < u_l[j]))
                        q = j;
                }while (j > 0);
            }//have now found q such that d[u]!=0 and u_l[q] is maximum

            //get the degree of next elp poly
            if(l[u] > l[q] + u - q){
                l[u + 1] = l[u];
            }
            else{
                l[u + 1] = l[q] + u - q;
            }

            //form new elp poly
            for(i = 0; i < 2*t; i++){
                elp[u + 1][i] = 0;      //set all coeffs of elp 0
            }
            for(i = 0; i <= l[q]; i++){
                if (elp[q][i] != -1){
                    elp[u + 1][i + u - q] = alpha_to[(d[u] + n - d[q] + elp[q][i]) % n];
                }
            }
            for (i = 0; i <= l[u]; i++) {
                elp[u + 1][i] ^= elp[u][i];     //next elp
                elp[u][i] = index_of[elp[u][i]];
            }
        }
        u_l[u + 1] = u - l[u + 1];  //update u_l

        //form (u+1)th discrepancy d
        if (u < 2*t) {
        /* no discrepancy computed on last iteration */
            if (s[u + 1] != -1){
                d[u + 1] = alpha_to[s[u + 1]];
            }else{
                d[u + 1] = 0;
            }
            for (i = 1; i <= l[u + 1]; i++){
                if ((s[u + 1 - i] != -1) && (elp[u + 1][i] != 0)){
                    d[u + 1] ^= alpha_to[(s[u + 1 - i] + index_of[elp[u + 1][i]]) % n];
                }
            }
            //put d[u+1] into index form
            d[u + 1] = index_of[d[u + 1]];
        }
    }while ((u < 2*t) && (l[u + 1] <= t));

    u++;

    if(l[u] <= t){//Can correct errors?
        for (i = 0; i <= l[u]; i++){
            elp[u][i] = index_of[elp[u][i]];    //convert each coeff of elp poly into index form
        }


        //Step 2: Chien search - find roots of the error location polynomial
        for(i = 1; i <= l[u]; i++){
            reg[i] = elp[u][i];
        }
        count = 0;
        for(i = 1; i <= n; i++){
            q = 1;
            for(j = 1; j <= l[u]; j++){
                if (reg[j] != -1) {
                    reg[j] = (reg[j] + j) % n;
                    q ^= alpha_to[reg[j]];
                }
            }
            if(!q){ //store root and error location number indices
                // root[count] = i;    //root number
                setdiff[count] = n - i; //error location
                if(setdiff[count] != -1){
        			setdiff[count] = alpha_to[setdiff[count]];
        		}
                count++;
            }
        }
    }
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
        tmp += SHA256_BLOCK_SIZE;
    }
    
    memcpy(sha, h, sizeof(uint32_t)*8);

}


