#include <stdint.h>

//Which one do you want to test?
#define test1TX		//TX of (31,29,1) RS syndrome extractor
#define test1RX		//RX of (31,29,1) RS syndrome extractor
#define test2TX		//TX of (31,27,2) RS syndrome extractor
#define test2RX		//RX of (31,27,2) RS syndrome extractor
#define test3TX		//TX of (63,49,7) RS syndrome extractor
#define test3RX 	//RX of (63,49,7) RS syndrome extractor
#define test4TX		//TX of (255,50,2) Pinsketch 	
#define test4RX		//RX of (255,50,2) Pinsketch 	
#define test5TX		//TX of (255,20,2) Pinsketch 
#define test5RX 	//RX of (255,20,2) Pinsketch 
#define test6TX		//TX of (255,10,2) Pinsketch 
#define test6RX 	//RX of (255,10,2) Pinsketch 

#define modified		//our constant-time version 
#define unmodified		//the unmodified version


////////////////////RS-based Syndrome extractor////////////////////RS-based Syndrome extractor
////////////////////RS-based Syndrome extractor////////////////////RS-based Syndrome extractor


//TX 

#if defined(test1TX) || defined(test2TX) || defined(test3TX)

#ifdef test1TX

//(31,29,1) RS
#define n  31       //n = 2^m-1, length of codeword (symbols)
#define k  29       //k = n-2*t, length of data (symbols)
#define t  1
//GF tables
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
int8_t index_of[n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};

#elif defined test2TX

//(31,27,2) RS
#define n  31       //n = 2^m-1, length of codeword (symbols)
#define k  27       //k = n-2*t, length of data (symbols)
#define t  2
//GF tables
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
int8_t index_of[n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};

#elif defined test3TX

//(63,49,7) RS
#define n  63   //n = 2^m-1, length of codeword (symbols)
#define k  49   //k = n-2*t, length of data (symbols)
#define t  7
//GF tables
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24, 48, 35, 5, 10, 20, 40, 19, 38, 15, 30, 60, 59, 53, 41, 17, 34, 7, 14, 28, 56, 51, 37, 9, 18, 36, 11, 22, 44, 27, 54, 47, 29, 58, 55, 45, 25, 50, 39, 13, 26, 52, 43, 21, 42, 23, 46, 31, 62, 63, 61, 57, 49, 33, 0};
int8_t index_of[n+1] = {-1, 0, 1, 6, 2, 12, 7, 26, 3, 32, 13, 35, 8, 48, 27, 18, 4, 24, 33, 16, 14, 52, 36, 54, 9, 45, 49, 38, 28, 41, 19, 56, 5, 62, 25, 11, 34, 31, 17, 47, 15, 23, 53, 51, 37, 44, 55, 40, 10, 61, 46, 30, 50, 22, 39, 43, 29, 60, 42, 21, 20, 59, 57, 58};

#endif

#if defined(test1TX) || defined(test2TX)

//Barrett Reduction for constant-time modular 31
uint8_t mod31(int16_t divisor){
	int16_t as = divisor >> 5;
	divisor -= as * 31;
	divisor -= 31 & (-(1 ^ ((uint16_t)(divisor-31)) >> 15));
	return divisor;
}

#else

//Barrett Reduction for constant-time modular 63
uint8_t mod63(int16_t divisor){
    int16_t as = divisor >> 6;
    divisor -= as * 63;
    divisor -= 63 & (-(1 ^ ((uint16_t)(divisor-63)) >> 15));
    return divisor;
}

#endif


//calculate the secure sketch
void getsyndrome(uint8_t syndrome[], uint8_t codeword[]){

    uint8_t i, j;
	int8_t temp[n];

 	i = 0;
    while(i != n){
		temp[i] = index_of[codeword[i]];
		i++;
	}

    syndrome[0] = 0;

    i = 1;
    while(i != n-k+1){
    	syndrome[i] = 0;
    	j = 0;

        while(j != n){

      		#ifdef modified

  		    //constant-time
            #if defined(test1TX) || defined(test2TX)
  		    syndrome[i] ^= alpha_to[mod31((temp[j]+i*j) & (-(1^(uint16_t)temp[j] >> 15)))] & (-(1 ^ ((uint16_t)temp[j]) >> 15));
            #else 
            syndrome[i] ^= alpha_to[mod63((temp[j]+i*j) & (-(1^(uint16_t)temp[j] >> 15)))] & (-(1 ^ ((uint16_t)temp[j]) >> 15));
            #endif

      		#elif defined unmodified

      		//not constant-time
      		if(temp[j] != -1){
                syndrome[i] ^= alpha_to[(temp[j]+i*j) % n];
      		}

      		#endif
      		j++;      
      	}
        i++;
    }
}

void testedcode(uint8_t ps[]){

	//public information sent in air
	uint8_t securesketch[2*t+1];

	//calculate the syndrome
	getsyndrome(securesketch, ps);

}

#endif

//RX

#if defined(test1RX) || defined(test2RX) || defined(test3RX)

#ifdef test1RX

//(31,29,1) RS
#define n  31       //n = 2^m-1, length of codeword (symbols)
#define k  29       //k = n-2*t, length of data (symbols)
#define t  1
//GF tables
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
int8_t index_of[n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};

#elif defined test2RX

//(31,27,2) RS
#define n  31       //n = 2^m-1, length of codeword (symbols)
#define k  27       //k = n-2*t, length of data (symbols)
#define t  2
//GF tables
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
int8_t index_of[n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};

#elif defined test3RX

//(63,49,7) RS
#define n  63   //n = 2^m-1, length of codeword (symbols)
#define k  49   //k = n-2*t, length of data (symbols)
#define t  7
//GF tables
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 3, 6, 12, 24, 48, 35, 5, 10, 20, 40, 19, 38, 15, 30, 60, 59, 53, 41, 17, 34, 7, 14, 28, 56, 51, 37, 9, 18, 36, 11, 22, 44, 27, 54, 47, 29, 58, 55, 45, 25, 50, 39, 13, 26, 52, 43, 21, 42, 23, 46, 31, 62, 63, 61, 57, 49, 33, 0};
int8_t index_of[n+1] = {-1, 0, 1, 6, 2, 12, 7, 26, 3, 32, 13, 35, 8, 48, 27, 18, 4, 24, 33, 16, 14, 52, 36, 54, 9, 45, 49, 38, 28, 41, 19, 56, 5, 62, 25, 11, 34, 31, 17, 47, 15, 23, 53, 51, 37, 44, 55, 40, 10, 61, 46, 30, 50, 22, 39, 43, 29, 60, 42, 21, 20, 59, 57, 58};

#endif

#if defined(test1RX) || defined(test2RX)

//Barrett Reduction for constant-time modular 31
uint8_t mod31(int16_t divisor){
    int16_t as = divisor >> 5;
    divisor -= as * 31;
    divisor -= 31 & (-(1 ^ ((uint16_t)(divisor-31)) >> 15));
    return divisor;
}

#else

//Barrett Reduction for constant-time modular 63
uint8_t mod63(int16_t divisor){
    int16_t as = divisor >> 6;
    divisor -= as * 63;
    divisor -= 63 & (-(1 ^ ((uint16_t)(divisor-63)) >> 15));
    return divisor;
}

#endif

//calculate the syndrome of a codeword	
void getsyndrome(int8_t syndrome[], int8_t codeword[]){

    uint8_t i, j;

    i = 0;
    while(i != n){
        codeword[i] = index_of[(uint8_t)(codeword[i])];
        i++;
    }

    syndrome[0] = 0;

    i = 1;
    while(i != n-k+1){
        syndrome[i] = 0;
        j = 0;
        while(j != n){

            #ifdef modified

            //constant-time
            #if defined(test1RX) || defined(test2RX)
            syndrome[i] ^= alpha_to[mod31((codeword[j]+i*j) & (-(1^(uint16_t)codeword[j] >> 15)))] & (-(1 ^ ((uint16_t)codeword[j]) >> 15));
            #else
            syndrome[i] ^= alpha_to[mod63((codeword[j]+i*j) & (-(1^(uint16_t)codeword[j] >> 15)))] & (-(1 ^ ((uint16_t)codeword[j]) >> 15));
            #endif

            #elif defined unmodified

            //not constant-time
            if(codeword[j] != -1){
            	syndrome[i] ^= alpha_to[(codeword[j]+i*j) % n];
            }

            #endif
            j++;
        }
        i++;
    }
}

//Correct the errors 		
void correcterrors(int8_t s[], int8_t codeword[]){

    uint16_t i,j,u,q, count=0;
    int8_t elp[n-k+2][n-k], d[n-k+2], l[n-k+2], u_lu[n-k+2];
    int8_t root[t], loc[t], z[t+1], err[n], reg[t+1];

    i = 1;
    while(i != n-k+1){
        s[i] = index_of[s[i]];
        i++;
    }

    d[0] = 0;           
    d[1] = s[1];        
    elp[0][0] = 0;      
    elp[1][0] = 1;      
    i = 1;
    while(i != n-k){
        elp[0][i] = -1;   
        elp[1][i] = 0;   
        i++;
    }
    l[0] = 0;
    l[1] = 0;
    u_lu[0] = -1;
    u_lu[1] = 0;
    u = 0;

    do{
        u++;
        if (d[u] == -1){
            l[u+1] = l[u];

            i = 0;
            while(i != l[u]+1){
                elp[u+1][i] = elp[u][i];
                elp[u][i] = index_of[elp[u][i]];
                i++;
            }
        }else{
            q = u-1;
            while ((d[q]==-1) && (q>0)){
                q--;
            }
            if (q>0){
                j = q;
                do{
                    j--;
                    if((d[j] != -1) && (u_lu[q] < u_lu[j])){
                        q = j;
                    }
                }while(j>0);
            }

            if (l[u] > l[q]+u-q){
                l[u+1] = l[u];
            }
            else{
                l[u+1] = l[q]+u-q;
            }

            i = 0;
            while(i != n-k){
                elp[u+1][i] = 0;
                i++;
            }

            i = 0;
            while(i != l[q]+1){
                if (elp[q][i] != -1){
                    elp[u+1][i+u-q] = alpha_to[(d[u]+n-d[q]+elp[q][i])%n];
                }
                i++;
            }

            i = 0;
            while(i != l[u]+1){
                elp[u+1][i] ^= elp[u][i];
                elp[u][i] = index_of[elp[u][i]];  
                i++;
            }
        }

        u_lu[u+1] = u-l[u+1];

        if(u<n-k){   
            if(s[u+1] != -1){
                d[u+1] = alpha_to[s[u+1]];
            }else{
                d[u+1] = 0;
            }

            i = 1;
            while(i != l[u+1]+1){
                if ((s[u+1-i]!=-1) && (elp[u+1][i]!=0)){
                    d[u+1] ^= alpha_to[(s[u+1-i]+index_of[elp[u+1][i]])%n];
                }
                i++;
            }
            d[u+1] = index_of[d[u+1]];    
        }

    }while ((u<n-k) && (l[u+1] <= t));

    u++;
    if(l[u]<=t){        
        i = 0;
        while(i != l[u]+1){
            elp[u][i] = index_of[elp[u][i]];
            i++;
        }

        i = 1;
        while(i != l[u]+1){
            reg[i] = elp[u][i];
            i++;
        }
        count = 0;

        i = 1;
        while(i != n+1){
            q = 1;
            j = 1;
            while(j != l[u]+1){
                if (reg[j]!=-1){
                    reg[j] = (reg[j]+j)%n ;
                    q ^= alpha_to[reg[j]];
                }
                j++;
            }
            if(!q){        
                root[count] = i;
                loc[count] = n-i;
                count++;
            }
            i++;
        }

    if (count==l[u]){    
        i = 1;
        while(i != l[u]+1){
            if ((s[i]!=-1) && (elp[u][i]!=-1)){
                z[i] = alpha_to[s[i]] ^ alpha_to[elp[u][i]];
            }else if ((s[i]!=-1) && (elp[u][i] == -1)){
                z[i] = alpha_to[s[i]];
            }else if ((s[i]==-1) && (elp[u][i] != -1)){
                z[i] = alpha_to[elp[u][i]];
            }else{
                z[i] = 0;
            }

            j = 1;
            while(j != i){
                if((s[j]!=-1) && (elp[u][i-j]!=-1)){
                    z[i] ^= alpha_to[(elp[u][i-j] + s[j])%n];
                }
                j++;
            }
            z[i] = index_of[z[i]];        
            i++;
        }
	
        i = 0;
        while(i != n){
            err[i] = 0;

    		#ifdef modified

    		//constant-time 
    		int8_t midval = codeword[i];
            codeword[i] = alpha_to[midval & (-(1 ^ ((uint16_t)midval) >> 15))] & (-(1 ^ ((uint16_t)midval) >> 15));

    		#elif defined unmodified

    		//not constant-time
    		if(codeword[i] == -1){
    			codeword[i] = 0;
    		}else{
    			codeword[i] = alpha_to[codeword[i]];
    		}

    		#endif

            i++;
        }

        i = 0;
        while(i != l[u]){
            err[loc[i]] = 1;      

            j = 1;
            while(j != l[u]+1){
                if (z[j]!=-1){
                    err[loc[i]] ^= alpha_to[(z[j]+j*root[i])%n];
                }
                j++;
            }
            if(err[loc[i]]!=0){
                err[loc[i]] = index_of[err[loc[i]]];
                q = 0;     

                j = 0;
                while(j != l[u]){
                    if(j!=i){
                        q += index_of[1^alpha_to[(loc[j]+root[i])%n]];
                    }
                    j++;
                }
                q = q % n;
                err[loc[i]] = alpha_to[(err[loc[i]]-q+n)%n];
                codeword[loc[i]] ^= err[loc[i]];  
            }
            i++;
        }
    }else{  
        i = 0;
        while(i != n){
            if(codeword[i]!=-1){
                codeword[i] = alpha_to[codeword[i]]; 
            }else{
                codeword[i] = 0; 
            }
            i++;
        }
    }
    }else{        
        i = 0;
        while(i != n){
            if(codeword[i]!=-1){
                codeword[i] = alpha_to[codeword[i]]; 
            }else{
                codeword[i] = 0; 
            }
            i++;
        }
    }
}

void testedcode(int8_t ss[], int8_t ps[], uint8_t errorloc[]){
	
    uint8_t i;
	
    //add some mismatches to the PS bit string (for test only)
	for(i = 0; i < t; i++){
		ps[errorloc[i]] ^= 1;
	}

    //secure sketch of RX
    int8_t syndrome[2*t+1];

    //calculate the secure sketch of RX
    getsyndrome(syndrome, ps);

    //calculate syn(ps) XOR syn(ps').
    i = 1;
    while(i != 2*t+1){
        syndrome[i] ^= ss[i];
        i++;
    }

    //correct the errors
    correcterrors(syndrome, ps);
	
}

#endif


////////////////////Pinsketch Extractor////////////////////Pinsketch Extractor
////////////////////Pinsketch Extractor////////////////////Pinsketch Extractor

//TX

#if defined(test4TX) || defined(test5TX) || defined(test6TX)

#if defined test4TX

//(255,50,2) 
//parameter settings of a (n,k) BCH-code.
#define s 50    //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
#define t 2     //error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number
#define n 255   //universe size
//The max tolerable mismatch rate = t/2s
//The initial entropy is log(n,s) with base 2, where (n,s) stands for the number of combinations of taking random s elements from n elements without putting back

#elif defined test5TX

//(255,20,2) 
//parameter settings of a (n,k) BCH-code.
#define s 20    //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
#define t 2     //error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number
#define n 255   //universe size
//The max tolerable mismatch rate = t/2s
//The initial entropy is log(n,s) with base 2, where (n,s) stands for the number of combinations of taking random s elements from n elements without putting back

#elif defined test6TX

//(255,10,2)
//parameter settings of a (n,k) BCH-code.
#define s 10    //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
#define t 2     //error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number
#define n 255   //universe size
//The max tolerable mismatch rate = t/2s
//The initial entropy is log(n,s) with base 2, where (n,s) stands for the number of combinations of taking random s elements from n elements without putting back

#endif

//GF(2^8) tables
//polynomial form
int16_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 0};
//index form
int16_t index_of[n+1] = {-1, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};

//Barrett Reduction for constant-time modular 255
uint8_t mod255(int16_t divisor){
	int16_t as = divisor >> 8;
	divisor -= as * 255;
	divisor -= 255 & (-(1 ^ ((uint16_t)(divisor-255)) >> 15));
	return divisor;
}

//Bubble Sort in ascending order
void bubble_sort(uint8_t array[], uint8_t len){
    uint8_t i, j;
    i = 0;
    while(i != len-1){
        j = 0;
        while(j != len-1-i){
			
			#ifdef modified

            //constant-time sorting
			int16_t diff = array[j] - array[(uint8_t)(j+1)];
			uint8_t big = (array[j] & (-(1 ^ ((uint16_t)diff >> 15)))) ^ (array[j+1] & (-(1 ^ ((uint16_t)-diff >> 15)))); 
			uint8_t small = (array[j] & (-(1 ^ ((uint16_t)-diff >> 15)))) ^ (array[j+1] & (-(1 ^ ((uint16_t)diff >> 15)))); 
			array[j] = small;
			array[j+1] = big;

			#elif defined unmodified

            //not constant-time
			if(array[j] > array[j+1]){
				uint8_t temp = array[j+1];
				array[j+1] = array[j];
				array[j] = temp;
			}

			#endif
			
            j++;
        }
        i++;
    }
}


//Calculate the secure sketch of TX
void securesketch(uint8_t ps[], int16_t ss[]){
    uint8_t i, j, degree;
    i = 0;

    while(i != t){
        ss[i] = 0;
        degree = 2*i+1;         //degree is 1, 3, 5... we only need the odd-indexes because for binary BCH code, each even-index value could be generated from the odd-index values
        j = 0;
        while(j != s){

			#ifdef modified

            //constant-time
			ss[i] ^= alpha_to[mod255(index_of[ps[j]]*degree)];

			#elif defined unmodified

            //not constant-time
			ss[i] ^= alpha_to[(index_of[ps[j]]*degree) % n];

			#endif

            j++;
        }
        i++;
    }
}


void testedcode(uint8_t ps[]){

    //public information sent to RX
    int16_t ssTX[t];
    securesketch(ps, ssTX);
	
    //sort the array (sort because RX could only recover all the PS elements, but doesn't ensure the PS values are in the same order)
    bubble_sort(ps, s);
}

#endif

//RX

#if defined(test4RX) || defined(test5RX) || defined(test6RX)

#if defined(test4RX)

//(255,50,2)
//parameter settings of a (n,k) BCH-code.
#define s 50    //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
#define t 2     //error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number
#define n 255   //universe size
//The max tolerable mismatch rate = t/2s
//The initial entropy is log(n,s) with base 2, where (n,s) stands for the number of combinations of taking random s elements from n elements without putting back

#elif defined(test5RX)

//(255,20,2)
//parameter settings of a (n,k) BCH-code.
#define s 20    //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
#define t 2     //error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number
#define n 255   //universe size
//The max tolerable mismatch rate = t/2s
//The initial entropy is log(n,s) with base 2, where (n,s) stands for the number of combinations of taking random s elements from n elements without putting back

#elif defined(test6RX)

//(255,10,2)
//parameter settings of a (n,k) BCH-code.
#define s 10    //set size. The bit length of each set element is log(n+1) with base 2. Each element should be different from each other
#define t 2     //error tolerance, the maximum symmetric difference size between TX/RX sets, must be even number
#define n 255   //universe size
//The max tolerable mismatch rate = t/2s
//The initial entropy is log(n,s) with base 2, where (n,s) stands for the number of combinations of taking random s elements from n elements without putting back

#endif

//GF(2^8) tables
//polynomial form
int16_t alpha_to[n+1] = {1, 2, 4, 8, 16, 32, 64, 128, 113, 226, 181, 27, 54, 108, 216, 193, 243, 151, 95, 190, 13, 26, 52, 104, 208, 209, 211, 215, 223, 207, 239, 175, 47, 94, 188, 9, 18, 36, 72, 144, 81, 162, 53, 106, 212, 217, 195, 247, 159, 79, 158, 77, 154, 69, 138, 101, 202, 229, 187, 7, 14, 28, 56, 112, 224, 177, 19, 38, 76, 152, 65, 130, 117, 234, 165, 59, 118, 236, 169, 35, 70, 140, 105, 210, 213, 219, 199, 255, 143, 111, 222, 205, 235, 167, 63, 126, 252, 137, 99, 198, 253, 139, 103, 206, 237, 171, 39, 78, 156, 73, 146, 85, 170, 37, 74, 148, 89, 178, 21, 42, 84, 168, 33, 66, 132, 121, 242, 149, 91, 182, 29, 58, 116, 232, 161, 51, 102, 204, 233, 163, 55, 110, 220, 201, 227, 183, 31, 62, 124, 248, 129, 115, 230, 189, 11, 22, 44, 88, 176, 17, 34, 68, 136, 97, 194, 245, 155, 71, 142, 109, 218, 197, 251, 135, 127, 254, 141, 107, 214, 221, 203, 231, 191, 15, 30, 60, 120, 240, 145, 83, 166, 61, 122, 244, 153, 67, 134, 125, 250, 133, 123, 246, 157, 75, 150, 93, 186, 5, 10, 20, 40, 80, 160, 49, 98, 196, 249, 131, 119, 238, 173, 43, 86, 172, 41, 82, 164, 57, 114, 228, 185, 3, 6, 12, 24, 48, 96, 192, 241, 147, 87, 174, 45, 90, 180, 25, 50, 100, 200, 225, 179, 23, 46, 92, 184, 0};
//index form
int16_t index_of[n+1] = {-1, 0, 1, 231, 2, 207, 232, 59, 3, 35, 208, 154, 233, 20, 60, 183, 4, 159, 36, 66, 209, 118, 155, 251, 234, 245, 21, 11, 61, 130, 184, 146, 5, 122, 160, 79, 37, 113, 67, 106, 210, 224, 119, 221, 156, 242, 252, 32, 235, 213, 246, 135, 22, 42, 12, 140, 62, 227, 131, 75, 185, 191, 147, 94, 6, 70, 123, 195, 161, 53, 80, 167, 38, 109, 114, 203, 68, 51, 107, 49, 211, 40, 225, 189, 120, 111, 222, 240, 157, 116, 243, 128, 253, 205, 33, 18, 236, 163, 214, 98, 247, 55, 136, 102, 23, 82, 43, 177, 13, 169, 141, 89, 63, 8, 228, 151, 132, 72, 76, 218, 186, 125, 192, 200, 148, 197, 95, 174, 7, 150, 71, 217, 124, 199, 196, 173, 162, 97, 54, 101, 81, 176, 168, 88, 39, 188, 110, 239, 115, 127, 204, 17, 69, 194, 52, 166, 108, 202, 50, 48, 212, 134, 41, 139, 226, 74, 190, 93, 121, 78, 112, 105, 223, 220, 241, 31, 158, 65, 117, 250, 244, 10, 129, 145, 254, 230, 206, 58, 34, 153, 19, 182, 237, 15, 164, 46, 215, 171, 99, 86, 248, 143, 56, 180, 137, 91, 103, 29, 24, 25, 83, 26, 44, 84, 178, 27, 14, 45, 170, 85, 142, 179, 90, 28, 64, 249, 9, 144, 229, 57, 152, 181, 133, 138, 73, 92, 77, 104, 219, 30, 187, 238, 126, 16, 193, 165, 201, 47, 149, 216, 198, 172, 96, 100, 175, 87};

//Barrett Reduction for constant-time modular 255
uint8_t mod255(int16_t divisor){
	int16_t as = divisor >> 8;
	divisor -= as * 255;
	divisor -= 255 & (-(1 ^ ((uint16_t)(divisor-255)) >> 15));
	return divisor;
}

//Bubble Sort in ascending order 
void bubble_sort(uint8_t array[], uint8_t len){
    uint8_t i, j;
    i = 0;
    while(i != len-1){
        j = 0;
        while(j != len-1-i){

			#ifdef modified

            //constant-time
			int16_t diff = array[j] - array[(uint8_t)(j+1)];
			uint8_t big = (array[j] & (-(1 ^ ((uint16_t)diff >> 15)))) ^ (array[j+1] & (-(1 ^ ((uint16_t)-diff >> 15)))); 
			uint8_t small = (array[j] & (-(1 ^ ((uint16_t)-diff >> 15)))) ^ (array[j+1] & (-(1 ^ ((uint16_t)diff >> 15)))); 
			array[j] = small;
			array[j+1] = big;

			#elif defined unmodified

            //not constant-time
			if(array[j] > array[j+1]){
				uint8_t temp = array[j+1];
				array[j+1] = array[j];
				array[j] = temp;
			}

			#endif
			
            j++;
        }
        i++;
    }
}

//Calculate the secure sketch of RX
void securesketch(uint8_t ps[], int16_t ss[]){
    uint8_t i, j, degree;

    i = 0;
    while(i != t){
        ss[i] = 0;
        degree = 2*i+1;         //degree is 1, 3, 5... we only need the odd-indexes because for binary BCH code, each even-index value could be generated from the odd-index values
        j = 0;
        while(j != s){

			#ifdef modified

            //constant-time
			ss[i] ^= alpha_to[mod255(index_of[ps[j]]*degree)];

			#elif defined unmodified

            //not constant-time
			ss[i] ^= alpha_to[(index_of[ps[j]]*degree) % n];

			#endif

            j++;
        }
        i++;
    }
}

//calculate the whole syndrome of binary BCH code
void recoversyn(int16_t syndrome[], int16_t ss[]){

    uint8_t i;

    syndrome[0] = 0;
    i = 1;
    while(i != 2*t+1){
        syndrome[i] = 0;
        if((i % 2) != 0){
            syndrome[i] = ss[(i-1)/2];
        }else{
			
            if(syndrome[(i/2)] == -1){
                syndrome[i] = -1;
            }else{

				#ifdef modified

                //constant-time
                syndrome[i] = mod255(syndrome[(i/2)]*2);

				#elif defined unmodified

                //not constant-time
            	syndrome[i] = (syndrome[(i/2)]*2) % n;		

				#endif

            }	
        }
        i++;
    }
}

//Simon Rockliff'ss implementation of Berlekamp'ss algorithm.
void getsetdiff(int16_t ss[], int16_t setdiff[]){

    uint16_t    i, j, u, q, count;
    int16_t     elp[2*t+2][t+1], d[2*t], l[2*t], u_l[2*t],reg[2*t];

    //Step 1: calculate the error location poly
    d[0] = 0;
    d[1] = ss[1];
    elp[0][0] = 0;
    elp[1][0] = 1;

    i = 1;
    while(i != 2*t){
        elp[0][i] = -1;
        elp[1][i] = 0;
        i++;
    }

    l[0] = 0;
    l[1] = 0;
    u_l[0] = -1;
    u_l[1] = 0;

    u = 0;

    do{
        u++;
        if(d[u] == -1){
            l[u + 1] = l[u];
            i = 0;
            while(i != l[u]+1){
                elp[u + 1][i] = elp[u][i];
                elp[u][i] = index_of[(uint8_t)(elp[u][i])];
                i++;
            }
        }else{
            q = u - 1;
            while ((d[q] == -1) && (q > 0))
                q--;

            if(q > 0) {
                j = q;
                do{
                    j--;
                    if ((d[j] != -1) && (u_l[q] < u_l[j]))
                        q = j;
                }while (j > 0);
            }

            if(l[u] > l[q] + u - q){
                l[u + 1] = l[u];
            }
            else{
                l[u + 1] = l[q] + u - q;
            }

            i = 0;
            while(i != 2*t){
                elp[u + 1][i] = 0;
                i++;
            }
            i = 0;
            while(i != l[q]+1){
                if (elp[q][i] != -1){
                    elp[u + 1][i + u - q] = alpha_to[(d[u] + n - d[q] + elp[q][i]) % n];
                }
                i++;
            }
            i = 0;
            while(i != l[u]+1){
                elp[u + 1][i] ^= elp[u][i];
                elp[u][i] = index_of[elp[u][i]];
                i++;
            }
        }
        u_l[u + 1] = u - l[u + 1];

		
        if (u < 2*t) {
            d[u + 1] = alpha_to[(uint8_t)(ss[u + 1])] & (-(1 ^ ((uint16_t)ss[u + 1]) >> 15));
            i = 1;
            while(i != l[u + 1]+1){
                if ((1 ^ ((uint16_t)ss[u + 1 - i]) >> 15) && (elp[u + 1][i] != 0)){            
                    d[u + 1] ^= alpha_to[(ss[u + 1 - i] + index_of[(uint8_t)(elp[u + 1][i])]) % n];
                }
                i++;
            }

            d[u + 1] = index_of[(uint8_t)(d[u + 1])];
        }
		
    }while ((u < 2*t) && (l[u + 1] <= t));

    u++;

    if(l[u] <= t){
        i = 0;
        while(i != l[u]+1){
            elp[u][i] = index_of[(uint8_t)(elp[u][i])];
            i++;
        }

        //Step 2: Chien search - find roots of the error location polynomial
        i = 1;
        while(i != l[u]+1){
            reg[i] = elp[u][i];
            i++;
        }
        count = 0;

        i = 1;
        while(i != n+1){
            q = 1;
            j = 1;
            while(j != l[u]+1){
                if (reg[j] != -1) {
                    reg[j] = (reg[j] + j) % n;
                    q ^= alpha_to[(uint8_t)(reg[j])];
                }
                j++;
            }
            if(!q){
                setdiff[count] = n - i; //error location
                if(setdiff[count] != -1){
                    setdiff[count] = alpha_to[(uint8_t)(setdiff[count])];
                }
                count++;
            }
            i++;
        }
    }
}

//calculate arr1[] = setdifference (arr1[], arr2[])
void dosetdiff(uint8_t arr1[], uint8_t arr1size, int16_t arr2[], uint8_t arr2size){

    uint8_t i, j, ctr;
    int8_t temp1[arr1size], temp2[arr2size];
    for(ctr = 0; ctr < arr1size; ctr++){
		temp1[ctr] = 0;
	}
	for(ctr = 0; ctr < arr2size; ctr++){
		temp2[ctr] = 0;
	}
    for(i = 0; i < arr1size; i++){
        for(j = 0; j < arr2size; j++){
            if(arr1[i] == arr2[j]){
                temp1[i] = -1;
                temp2[j] = -1;
            }
        }
    }

    for(i = 0; i < arr2size; i++){
        if(temp2[i] != -1 && arr2[i] != -1){
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

void calculHelper(int16_t ss[], uint8_t ps[]){
	securesketch(ps, ss);
}

void testedcode(int16_t ssTX[], uint8_t ps[], uint8_t errorloc){
	
		uint8_t i;

		ps[errorloc] = 254;

        int16_t ssRX[t];                
        securesketch(ps, ssRX);

        //Calculate syndrome of error = SSTX xor SSRX
        i = 0;
        while(i != t){
            ssRX[i] = index_of[(uint8_t)(ssRX[i] ^ ssTX[i])];
            i++;
        }

		int16_t syndrome[2*t+1];        
		//calculate the whole syndrome (ssTX[] and ssRX[] only contain the odd-indexes values)
		recoversyn(syndrome, ssRX);

		//Decode the syndrome
		int16_t setdiff[t];     //stores the set difference between ps measurements at TX/RX sides
		i = 0;
		while(i != t){
			setdiff[i] = -1;
			i++;
		}
		getsetdiff(syndrome, setdiff);

		//Correct the set - only concern the mismatch situation, i.e. TX/RX have same num of measurements and some of them mismatch
		dosetdiff(ps, s, setdiff, t);
        
        //sort the array (sort because RX could only recover all the PS elements, but doesn't ensure the PS values are in the same order)
        bubble_sort(ps, s);
}

#endif
