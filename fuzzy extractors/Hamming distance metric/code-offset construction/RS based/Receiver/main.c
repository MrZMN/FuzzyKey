#include <stdio.h>
#include <stdint.h>

// #define m  5		//number of bits per symbol
#define n  31		//n = 2^m-1, length of codeword (symbols) 
#define k  29		//k = n-2*t, length of data (symbols) 
#define t  1 		//number of symbol errors that can be corrected
//The max tolerable mismatch rate = t/n

//GF(2^5)
//polynomial form
uint8_t alpha_to[n+1] = {1, 2, 4, 8, 16, 5, 10, 20, 13, 26, 17, 7, 14, 28, 29, 31, 27, 19, 3, 6, 12, 24, 21, 15, 30, 25, 23, 11, 22, 9, 18, 0};
//index form
int8_t index_of[n+1] = {-1, 0, 1, 18, 2, 5, 19, 11, 3, 29, 6, 27, 20, 8, 12, 23, 4, 10, 30, 17, 7, 22, 28, 26, 21, 25, 9, 16, 13, 14, 24, 15};

//RS decoding (Simon Rockliff's implementation of Berlekamp's algorithm)
void decode_rs(int8_t codeword[n]){
    uint8_t i, j, u, q, count=0, syn_error=0;
    int8_t elp[n-k+2][n-k], d[n-k+2], l[n-k+2], u_lu[n-k+2], s[n-k+1];
    int8_t root[t], loc[t], z[t+1], err[n], reg[t+1];

    i = 0;
    while(i != n){
        codeword[i] = index_of[codeword[i]];
        i++;
    }

    i = 1;
    while(i != n-k+1){
        s[i] = 0;
        j = 0;
        while(j != n){
            if(codeword[j] != -1){
                s[i] ^= alpha_to[(codeword[j]+i*j)%n];     
            }else{
                s[i] ^= alpha_to[n];    //against timing attack
            }
            j++;
        }
        if(s[i] != 0){
            syn_error = 1;        
        }
        s[i] = index_of[s[i]];
        i++;
    }

    if(syn_error){
        d[0] = 0 ;           
        d[1] = s[1] ;        
        elp[0][0] = 0 ;      
        elp[1][0] = 1 ;      
        i = 1;
        while(i != n-k){
            elp[0][i] = -1 ;   
            elp[1][i] = 0 ;   
            i++;
        }
        l[0] = 0 ;
        l[1] = 0 ;
        u_lu[0] = -1 ;
        u_lu[1] = 0 ;
        u = 0 ;

        do{
            u++;
            if(d[u] == -1){
                l[(u+1)] = l[u];

                i = 0;
                while(i != l[u]+1){
                    elp[(u+1)][i] = elp[u][i];
                    elp[u][i] = index_of[elp[u][i]];
                    i++;
                }
            }else{
                q = u-1 ;
                while((d[q]==-1) && (q>0)){
                    q--;
                }
                if(q>0){ 
                    j = q;
                    do{ 
                        j--;
                        if((d[j] != -1) && (u_lu[q] < u_lu[j])){
                            q = j;
                        }
                    }while(j>0);
                }

                if(l[u] > l[q]+u-q){
                    l[(u+1)] = l[u];
                }else{
                    l[(u+1)] = l[q]+u-q;
                }

                i = 0;
                while(i != n-k){
                    elp[(u+1)][i] = 0;
                    i++;
                }

                i = 0;
                while(i != l[q]+1){
                    if(elp[q][i] != -1){
                        elp[(u+1)][(i+u-q)] = alpha_to[(d[u]+n-d[q]+elp[q][i])%n] ;
                    }
                    i++;
                }

                i = 0;
                while(i != l[u]+1){
                    elp[(u+1)][i] ^= elp[u][i];
                    elp[u][i] = index_of[elp[u][i]];  
                    i++;
                }
            }
              
            u_lu[(u+1)] = u-l[(u+1)];

            if(u<n-k){    
                if(s[(u+1)] != -1){
                    d[(u+1)] = alpha_to[s[(u+1)]];
                }else{
                    d[(u+1)] = 0;
                }

                i = 1;
                while(i != l[(u+1)]+1){
                    if((s[(u+1-i)]!=-1) && (elp[(u+1)][i]!=0)){
                        d[(u+1)] ^= alpha_to[(s[(u+1-i)]+index_of[elp[(u+1)][i]])%n];
                    }
                    i++;
                }
                d[(u+1)] = index_of[d[(u+1)]];    
            }

        }while((u<n-k) && (l[(u+1)] <= t));

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
                    if(reg[j]!=-1){
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

            if(count==l[u]){         
                i = 1;
                while(i != l[u]+1){
                    if((s[i]!=-1) && (elp[u][i]!=-1)){
                        z[i] = alpha_to[s[i]] ^ alpha_to[elp[u][i]] ;
                    }else if((s[i]!=-1) && (elp[u][i] == -1)){
                        z[i] = alpha_to[s[i]] ;
                    }else if((s[i]==-1) && (elp[u][i] != -1)){
                        z[i] = alpha_to[elp[u][i]] ;
                    }else{
                        z[i] = 0 ;
                    }

                    j = 1;
                    while(j != i){
                        if((s[j]!=-1) && (elp[u][(i-j)]!=-1)){
                            z[i] ^= alpha_to[(elp[u][(i-j)] + s[j])%n] ;
                        }
                        j++;
                    }
                    z[i] = index_of[z[i]];        
                    i++;
                }

                i = 0;
                while(i != n){
                    err[i] = 0;
                    if(codeword[i]!=-1){        
                        codeword[i] = alpha_to[codeword[i]];
                    }else{
                        codeword[i] = alpha_to[n];  
                    }
                    i++;
                }

                i = 0;
                while(i != l[u]){
                    err[loc[i]] = 1;       

                    j = 1;
                    while(j != l[u]+1){
                        if(z[j]!=-1){
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
                        q = q % n ;
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
                        codeword[i] = alpha_to[n];
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
                    codeword[i] = alpha_to[n];
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
                codeword[i] = alpha_to[n];
            }
            i++;
        }
    }
}

//RX of code-offset construction of fuzzy extractor based on RS code
int main(){

	uint8_t i;
	
	//Received secure sketch from TX
	uint8_t securesketch[n] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
	//Extracted PS bit string (uniformly distributed random). Assume it's composed of all 1s.
	int8_t ps[n] = {31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31, 31};

    //add some mismatches to the PS bit string (for test only)
    // ps[0] ^= 1;
    ps[10] ^= 1;

    for(i = 0; i < n; i++){
    	ps[i] ^= securesketch[i];
    }

    //Decode the RS codeword
    decode_rs(ps);

    for(i = 0; i < n; i++){
    	ps[i] ^= securesketch[i];
    }

    /*
    //Retrieve the corrected PS string
	printf("Corrected PS string:\n");
    for(i = 0; i < n; i++){
    	printf("%d, ", ps[i]);
    }
    printf("\n");
    */

//////////////////////////////////////////////////////////////////

    // //Strong extractor
}