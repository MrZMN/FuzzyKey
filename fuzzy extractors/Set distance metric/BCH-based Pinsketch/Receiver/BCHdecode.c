#include "BCHdecode.h"
#include "stdint.h"
#include <stdio.h>

void read_p()
/*
 * Get precomputed coefficients p[] of p(x) from m. 
 */
{
	int	i;

	for (i=1; i<m; i++)
		p[i] = 0;
	p[0] = p[m] = 1;
	if (m == 2)			p[1] = 1;
	else if (m == 3)	p[1] = 1;
	else if (m == 4)	p[1] = 1;
	else if (m == 5)	p[2] = 1;
	else if (m == 6)	p[1] = 1;
	else if (m == 7)	p[1] = 1;
	else if (m == 8)	p[4] = p[5] = p[6] = 1;
	else if (m == 9)	p[4] = 1;
	else if (m == 10)	p[3] = 1;
	else if (m == 11)	p[2] = 1;
	else if (m == 12)	p[3] = p[4] = p[7] = 1;
	else if (m == 13)	p[1] = p[3] = p[4] = 1;
	else if (m == 14)	p[1] = p[11] = p[12] = 1;
	else if (m == 15)	p[1] = 1;
	else if (m == 16)	p[2] = p[3] = p[5] = 1;
	else if (m == 17)	p[3] = 1;
	else if (m == 18)	p[7] = 1;
	else if (m == 19)	p[1] = p[5] = p[6] = 1;
	else if (m == 20)	p[3] = 1;
	
}

void generate_gf()
/*
 * Generate field GF(2**m) from the irreducible polynomial p(X) with
 * coefficients in p[0]..p[m].
 *
 * Lookup tables:
 *   index->polynomial form: alpha_to[] contains j=alpha^i;
 *   polynomial form -> index form:	index_of[j=alpha^i] = i
 *
 * alpha=2 is the primitive element of GF(2**m) 
 */
{
	register int    i, mask;

	mask = 1;
	alpha_to[m] = 0;
	for (i = 0; i < m; i++) {
		alpha_to[i] = mask;
		index_of[alpha_to[i]] = i;
		if (p[i] != 0)
			alpha_to[m] ^= mask;
		mask <<= 1;
	}
	index_of[alpha_to[m]] = m;
	mask >>= 1;
	for (i = m + 1; i < n; i++) {
		if (alpha_to[i - 1] >= mask)
		  alpha_to[i] = alpha_to[m] ^ ((alpha_to[i - 1] ^ mask) << 1);
		else
		  alpha_to[i] = alpha_to[i - 1] << 1;
		index_of[alpha_to[i]] = i;
	}
	index_of[0] = -1;
}



void decode_bch()
/*
 * Simon Rockliff's implementation of Berlekamp's algorithm.
 *
 * Assume we have received bits in recd[i], i=0..(n-1).
 *
 * Compute the 2*t syndromes by substituting alpha^i into rec(X) and
 * evaluating, storing the syndromes in s[i], i=1..2t (leave s[0] zero) .
 * Then we use the Berlekamp algorithm to find the error location polynomial
 * elp[i].
 *
 * If the degree of the elp is >t, then we cannot correct all the errors, and
 * we have detected an uncorrectable error pattern. We output the information
 * bits uncorrected.
 *
 * If the degree of elp is <=t, we substitute alpha^i , i=1..n into the elp
 * to get the roots, hence the inverse roots, the error location numbers.
 * This step is usually called "Chien's search".
 *
 * If the number of errors located is not equal the degree of the elp, then
 * the decoder assumes that there are more than t errors and cannot correct
 * them, only detect them. We output the information bits uncorrected.
 */
{
	register int    i, j, u, q, syn_error = 0;
	int             elp[2*t+2][t+1], d[2*t], l[2*t], u_l[2*t],reg[2*t];	//elp is the error location poly; d, l, u_l comes from book 'Error control coding' page 140
	uint8_t         root[t]; 


	syn_error = 1; 

	//Step 2: if error, calculate the error location poly 
	if (syn_error) {	
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
		d[0] = 0;			//index form - a^0
		d[1] = s[1];		//index form 
		elp[0][0] = 0;		//index form - a^0
		elp[1][0] = 1;		//polynomial form - 1
		//?
		for(i = 1; i < 2*t; i++) {
			elp[0][i] = -1;	//index form 
			elp[1][i] = 0;	//polynomial form
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
					elp[u + 1][i] = 0;		//set all coeffs of elp 0
				}
				for(i = 0; i <= l[q]; i++){
					if (elp[q][i] != -1){
						elp[u + 1][i + u - q] = alpha_to[(d[u] + n - d[q] + elp[q][i]) % n];
					}
				}
				for (i = 0; i <= l[u]; i++) {
					elp[u + 1][i] ^= elp[u][i];		//next elp
					elp[u][i] = index_of[elp[u][i]];
				}
			}
			u_l[u + 1] = u - l[u + 1];	//update u_l
 
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
				elp[u][i] = index_of[elp[u][i]];	//convert each coeff of elp poly into index form
			}

			//Step 3: Chien search - find roots of the error location polynomial
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
				if(!q){	//store root and error location number indices
					root[count] = i;	//root number
					loc[count] = n - i;	//error location
					printf("%d, ", loc[count]);
					count++;
				}
			}
			
				
		}
	}
}
