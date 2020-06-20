#include "BCHencode.h"

void read_p()
//Get precomputed coefficients p[] of p(x) from m. 
{
	uint8_t	i;

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
	// else if (m == 9)	p[4] = 1;
	// else if (m == 10)	p[3] = 1;
	// else if (m == 11)	p[2] = 1;
	// else if (m == 12)	p[3] = p[4] = p[7] = 1;
	// else if (m == 13)	p[1] = p[3] = p[4] = 1;
	// else if (m == 14)	p[1] = p[11] = p[12] = 1;
	// else if (m == 15)	p[1] = 1;
	// else if (m == 16)	p[2] = p[3] = p[5] = 1;
	// else if (m == 17)	p[3] = 1;
	// else if (m == 18)	p[7] = 1;
	// else if (m == 19)	p[1] = p[5] = p[6] = 1;
	// else if (m == 20)	p[3] = 1;
	
}

void generate_gf()
/*
 * Generate field GF(2**m) from the irreducible polynomial p(X) 
 *
 * Lookup tables:
 *   index form ->polynomial form: alpha_to[] contains j=alpha^i;
 *   polynomial form -> index form:	index_of[j=alpha^i] = i
 *
 * alpha=2 is the primitive element of GF(2**m) 
 */
{
	register int i, mask;

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

