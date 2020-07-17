#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//parameter settings of a (n,k) RS-code
#define m  5		// RS code over GF(2^m), each sysmbol contains m bits 
#define n  31		// n = 2^m-1, length of codeword (symbols) 
// #define t  2 	// number of symbol errors that can be corrected
#define k  27		// k = n-2*t, length of data (symbols) 
#define pslength m*n/8+1	//20
#define keylength m*k/8+1	//17

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

//GF(2^m) and the generator poly
int alpha_to [n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
int index_of [n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};
int g[n-k+1] = {10, 29, 19, 24, 0};

void encode_rs(int [], int []);
void transform(const uint8_t *, uint32_t *);
void sha256(const uint8_t *, uint32_t, uint32_t *);

// This is a 'code-offset' fuzzy extractor constructed by a (31,27) RS-code
// ps is 155 bit string (in 20 bytes)
// All the above codes are based on (31,27) RS-code. If the ECC changes, some slight changes are required
int main(){

	srand((unsigned)time(NULL));	//for random number generation

	register int i, j;
	int bytepos, bitpos;
	int idx = 0;

	uint8_t ps[pslength];			//stores the ps measurements
	int nonce[keylength];		//stores a random nonce
	int ss[pslength];			//stores the tansmitted info from TX->RX
	int data[k], codeword[n];	//For ECC coding usage
	uint8_t randStr[pslength];			//stores the ps measurements
	uint32_t key[8];			//the final key

	//Randomly generate the ps values - first legit 155 bits in 20 bytes
	for(i = 0; i < pslength; i++){
		ps[i] = rand()%256;
	}
	ps[pslength-1] &=  0xe0;	//leave the 156-160th bits 0
	//Randomly generate the random String - first legit 155 bits in 20 bytes
	for(i = 0; i < pslength; i++){
		randStr[i] = rand()%256;
	}
	//Randomly generate a nonce - first legit 135 bits in 17 bytes
	for(i = 0; i < keylength; i++){
		nonce[i] = rand()%256;
	} 
	nonce[keylength-1] &= 0xfe;	//leave the 136th bit 0

	//For ECC encoding only. Convert the nonce array from 17 bytes(8 bit each) -> 27 symbols(5 bits each)
	for(i = 0; i < k; i ++){
		data[i] = 0;
		for(j = 0; j < m; j++){
			idx = m*i + j;
			bytepos = idx / 8;
			bitpos = idx % 8;
			data[i] <<= 1;
			data[i] ^= ((nonce[bytepos] & (0x80 >> bitpos)) >> (7 - bitpos));
		}
	}

	//Encoding. data(27 symbols) -> codeword(31 symbols)
	encode_rs(codeword, data);

	//For reducing the trasmission load. Convert the 31 symbols(5 bits each) -> 20 bytes(8 bits each)
	for(i = 0; i < pslength; i++){
		ss[i] = 0;
		for(j = 0; j < 8; j++){
			idx = i*8 + j;
			bytepos = idx / m;
			bitpos = idx % m;
			ss[i] <<= 1;
			if(bytepos < n){
				ss[i] ^= (codeword[bytepos] & (0x10 >> bitpos)) >> (4 - bitpos); 
			}
		}
	}

	//PS xor Codeword(of the nonce). This is the info to be transmitted
	for( i = 0; i < pslength; i++){
		ss[i] ^= ps[i];
	}

	printf("The PS values: (copy it to Receiver.c)\n");
	for(i = 0; i < pslength; i++){
		printf("%d, ", ps[i]);
	}
	printf("\n");

	printf("The random String: (copy it to Receiver.c)\n");
	for(i = 0; i < pslength; i++){
		printf("%d, ", randStr[i]);
	}
	printf("\n");

	printf("secure sketch is: (copy it to Receiver.c)\n");
	for(i = 0; i < pslength; i++){
		printf("%d, ", ss[i]);
	}
	printf("\n");

	//////The next thing is to send arrays 'ss[]' (20 bytes) and 'randStr[]' (20 bytes) to RX
  	//////There should be an ACK mechamism between TX and RX, such as TX sends a MAC to RX. 

	//ps values xor random string
	for(i = 0; i < pslength; i++){
		ps[i] ^= randStr[i];
	}

	//generate the key
	sha256(ps, pslength, key);

	//take the first 128 bits of sha-256 output as the key
	for(i = 0; i < 4; i++){
		printf("%x", key[i]);
	}

}

//data[] as input, codeword[] as output
void encode_rs(int recd[n], int data[k]){
	register int i,j;
  	int feedback;
	int b[n-k];

  	for(i = 0; i < n-k; i++){
    	b[i] = 0;
  	}
     
  	for(i = k-1; i >= 0; i--){
    	feedback = index_of[data[i]^b[n-k-1]];
    	if(feedback != -1){   
      		for(j=n-k-1; j>0; j--){
        		if(g[j] != -1){
          			b[j] = b[j-1]^alpha_to[(g[j]+feedback)%n];
        		}
        		else{
          			b[j] = b[j-1];
        		}
      		}
      	b[0] = alpha_to[(g[0]+feedback)%n];
    	}
    	else{
      		for(j=n-k-1; j>0; j--){
        		b[j] = b[j-1] ;
      		}
      		b[0] = 0 ;
    	}
  	}

  	for(i=0; i<n-k; i++){
    	recd[i] = b[i];
  	}
  	for(i=0; i<k; i++){
    	recd[i+n-k] = data[i];
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

