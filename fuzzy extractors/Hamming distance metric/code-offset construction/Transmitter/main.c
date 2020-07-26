#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//parameter settings of a (n,k) RS-code. n, k means num of symbols
#define m  4        // RS code over GF(2^m), each sysmbol contains m bits 
#define n  15       // n = 2^m-1, length of codeword (symbols) 
#define k  11       // k = n-2*t, length of data (symbols) 
// #define t  2     // number of symbol errors that can be corrected - error rate = t/n
#define iterationnum 128/(2*m*k) + 1

//GF(2^m)
//poly form
uint8_t alpha_to [n+1] = {1, 2, 4, 8, 3, 6, 12, 11, 5, 10, 7, 14, 15, 13, 9, 0};
//index form
int8_t index_of [n+1] = {-1, 0, 1, 4, 2, 8, 5, 10, 3, 14, 9, 7, 6, 13, 11, 12};
//generator poly of RS code
int8_t g[n-k+1] = {10, 3, 6, 13, 0};

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

//RS encoding: data[k] as input, codeword[n] as output. In each byte of codeword[] and data[], only the last m-bits are valid 
void encode_rs(uint8_t codeword[n], uint8_t data[k]){
    uint8_t i, j;
    int8_t temp;
    uint8_t b[n-k];

    i = 0;
    while(i != n-k){
        b[i] = 0;
        i++;
    }
     
    i = k;
    do{
        i--;
        temp = index_of[data[i]^b[n-k-1]];
        if(temp != -1){   
            for(j = n-k-1; j > 0; j--){
                if(g[j] != -1){
                    b[j] = b[j-1] ^ alpha_to[(g[j]+temp)%n];
                }
                else{
                    b[j] = b[j-1];
                }
            }
        b[0] = alpha_to[(g[0]+temp)%n];
        }else{
            for(j = n-k-1; j > 0; j--){
                b[j] = b[j-1];
            }
            b[0] = 0;
        }
    }while(i != 0);

    i = 0;
    while(i != n-k){
        codeword[i] = b[i];
        i++;
    }
    i = 0;
    while(i != k){
        codeword[i+n-k] = data[i];
        i++;
    }

} 

//used for encoding when m = 4
void encode_m_4(uint8_t codeword[n], uint8_t data[k]){

    uint8_t codeword1[n], codeword2[n];
    uint8_t data1[k], data2[k];

    uint8_t i;

    i = 0;
    while(i != k){
        data1[i] = (uint8_t)(data[i] & 0x0f);
        data2[i] = (uint8_t)((data[i] >> 4) & 0x0f);
        i++;
    }

    encode_rs(codeword1, data1);
    encode_rs(codeword2, data2);

    i = 0;
    while(i != n){
        codeword[i] = (uint8_t)((codeword2[i] << 4) | codeword1[i]);
        i++;
    }

}

void transform(const uint8_t *msg, uint32_t *h){

    uint32_t w[64];
    uint32_t a0, b1, c2, d3, e4, f5, g6, h7;
    uint32_t t1, t2;

    uint8_t i, j = 0;

    i = 0;
    while(i != 16){
        w[i] = (uint32_t)msg[j]<<24 | (uint32_t)msg[(uint8_t)(j+1)]<<16 | (uint32_t)msg[(uint8_t)(j+2)]<<8 | msg[(uint8_t)(j+3)];
        j += 4;
        i++;
    }

    i = 16;
    while(i != 64){
        w[i] = SSIG1(w[(uint8_t)(i-2)])+w[(uint8_t)(i-7)]+SSIG0(w[(uint8_t)(i-15)])+w[(uint8_t)(i-16)];
        i++;
    }

    a0 = h[0];
    b1 = h[1];
    c2 = h[2];
    d3 = h[3];
    e4 = h[4];
    f5 = h[5];
    g6 = h[6];
    h7 = h[7];

    i = 0;
    while(i != 64){
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
        i++;
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

    blocknum = len/SHA256_BLOCK_SIZE;
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

    i = 0;
    while(i != blocknum){
        transform(tmp, h);
        tmp += SHA256_BLOCK_SIZE;
        i++;
    }

    tmp = cover_data;
    blocknum = cover_size/SHA256_BLOCK_SIZE;

    i = 0;
    while(i != blocknum){
        transform(tmp, h);
        tmp += SHA256_BLOCK_SIZE;
        i++;
    }

    memcpy(sha, h, sizeof(uint32_t)*8);
}

// This is TX of a 'code-offset' fuzzy extractor constructed by a (15,11) RS-code
// ps - 15 bytes
// All the above codes are based on (15,11) RS-code. If the ECC changes, some slight changes are required
int main(){

    srand((unsigned)time(NULL));    //for random number generation

    uint8_t i;
    uint8_t nonce[k], randStr[n], ss[n];
    uint8_t ps[n] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15}; //assume we get n ps measurements (uniformly distributed random) and each measurement is one byte. Should be different at each iteration
    uint32_t key[8];               //the final key

    //randomly generate a nonce
    i = 0;
    while(i != k){
        nonce[i] = rand()%256;
        i++;
    }
    //Randomly generate a random String 
    i = 0;
    while(i != n){
        randStr[i] = rand()%256;
        i++;
    }

    //encode the nonce to be a codeword
    encode_m_4(ss, nonce);

    //Secure sketch = PS xor codeword(of the nonce)
    i = 0;
    while(i != n){
        ss[i] ^= ps[i];
        i++;
    }

    //////The next thing is to send arrays 'ss[]' and 'randStr[]' to RX

    //Generate the 128-bit key
    //ps values xor random string - to ensure the key is totally random
    i = 0;
    while(i != n){
        ps[i] ^= randStr[i];
        i++;
    }

    //generate the uniformly distributed key using sha-256
    sha256(ps, n, key);


    printf("secure sketch: (sent to Receiver)\n");
    for(i = 0; i < n; i++){
        printf("%d, ", ss[i]);
    }
    printf("\n");
    printf("The random String: (sent to Receiver)\n");
    for(i = 0; i < n; i++){
        printf("%d, ", randStr[i]);
    }
    printf("\n");
    //take the first 128 bits of sha-256 output as the key
    printf("The final 128-bit key:\n");
    for(i = 0; i < 4; i++){
        printf("%x", key[i]);
    }
    printf("\n");

}