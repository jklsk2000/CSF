/*
 * CSF Assignment 1
 * Danny Lee
 * jlee692@jhu.edu
 * Arbitrary-precision integer data type
 * Function implementations
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include "apint.h"

/*
 *	Create an ApInt instance of length 1 using a uint64_t value
 * 	
 * 	Parameters:
 * 		val - uint64_t value to be inputted into ApInt
 * 	Return:
 * 		an instance of ApInt that represent val 
 */
ApInt *apint_create_from_u64(uint64_t val) {
	ApInt * ap = malloc(sizeof(ApInt));
	ap->bitString = (uint64_t *) malloc(sizeof(uint64_t));
	ap->length = 1;
	ap->bitString[0] = val; 
	return ap;
}

/*
 *	Helper function that copies the hexadecimal char array by breaking
 * 	it up and storing it into each array index of the ApInt instance
 * 	
 * 	Parameters:
 * 		*ap - pointer to an ApInt instance that we are storing the hexadecimal
 * 		*hexString - a pointer to a char array containing the hexadecimal
 * 		hexLength - total length of the array for the ApInt instance
 * 		leftover - number of leftover hex digits 
 * 	Return:
 * 		a pointer to an ApInt instance with updated bit string array 
 */
ApInt *apint_hex_helper(ApInt *ap, char *hexString, int hexLength, int leftover) {
	int start;
	char hexArr[17];
	for (int i = 0; i < hexLength - 1; i++) {
		if (leftover == 0) {
			start = 16 * (hexLength - 1 - i);
		} else {
			start = leftover + 16 * (hexLength - 2 -i);
		}
		memcpy(hexArr, &hexString[start], 16);
		hexArr[16] = '\0';
		ap->bitString[i] = (uint64_t) strtoul(hexArr, NULL, 16);
	}

	int fArrLen;
	if (leftover == 0) {
		fArrLen = 17;
	} else {
		fArrLen = leftover + 1;
	}

	char fHexArr[fArrLen];
	memcpy(fHexArr, hexString, fArrLen - 1);
	fHexArr[fArrLen - 1] = '\0';
	ap->bitString[hexLength - 1] = (uint64_t) strtoul(fHexArr, NULL, 16);
	return ap;
}

/*
 *	Create an arbitrarily long instance of ApInt given a hexadecimal
 *	from a char array. The function accepts the digits 0-9 and the
 * 	uppercase and lowercase letters a-f. 
 * 
 * 	Parameters:
 * 		*hex - a pointer to aa const char array that stores the 
 * 		hexadecimal
 * 	Return:
 * 		a pointer to an instance of ApInt representing the hexadecimal
 */
ApInt *apint_create_from_hex(const char *hex) {
	assert(hex != NULL);
	char * hexString;
	int counter = -1;

	// Trimming the leading zeros
	for (unsigned i = 0; i < strlen(hex); i++) {
		if (hex[i] != '0') {
			counter = i;
			break;
		}
		if (i == strlen(hex) - 1 && counter == -1) {
			counter = strlen(hex) - 1;
		}
	}

	hexString = (char *) malloc (sizeof(char) * (strlen(hex) - counter + 1));
	memcpy(hexString, &hex[counter], strlen(hex) - counter);
	hexString[strlen(hex) - counter] = '\0';
	int hexLength = ceil(strlen(hexString) / 16.0);
	int leftover = strlen(hexString) % 16;

	ApInt * ap = malloc(sizeof(ApInt));
	ap->bitString = (uint64_t *) malloc(sizeof(uint64_t) * hexLength);
	ap->length = (size_t) hexLength;
	ap = apint_hex_helper(ap, hexString, hexLength, leftover);
	free(hexString);
	return ap;
}

/*
 *	Free the allocated memory taken up by ApInt struct and 
 * 	the bitString uint64_t array.
 * 
 * 	Parameters:
 * 		*ap - a pointer to an ApInt instance to be deallocated
 * 	Return:
 * 		N/A
 */
void apint_destroy(ApInt *ap) {
	assert(ap != NULL);
	free(ap->bitString);
	free(ap);
}

/*
 *	Finds a 64 bit segment of ApInt's array and returns it. The 
 * 	segment depends on the unsigned int parameter passed through.
 * 	Returns 0 if out of bounds.
 * 	
 * 	Parameters:
 * 		*ap - a pointer to an ApInt instance 
 * 		n - the array index to be accessed 
 * 	Return:
 * 		a uint64_t of the desired segment from ap
 */
uint64_t apint_get_bits(ApInt *ap, unsigned n) {
	assert(1);
	if (n > ap->length - 1) {
		return 0;
	} else {
		return (ap->bitString)[n];
	}
}

/*
 *	Finds the most significant bit in the bit string array and  
 * 	returns an int of its position. Returns -1 if the ApInt is
 * 	equal to 0.
 * 
 * 	Parameters:
 * 		*ap - a pointer to an ApInt instance 
 * 	Return:
 * 		an integer that gives the position of the most significant
 * 		bit in terms of how many bits come before it
 */
int apint_highest_bit_set(ApInt *ap) {
	assert(ap != NULL);
	if (ap->length == 1 && (ap->bitString)[0] == 0) {
		return -1;
	} else {
		uint64_t lastArr = ap->bitString[ap->length - 1];
		int order = 0;
		for (int i = 0; i < 64; i++) {
			if ((lastArr >> i) & 1) {
				order = i;
			}
		}
		return (ap->length - 1) * 64 + order;
	}
}

/*
 *	Left shifts the bit string in ap by 1.
 * 
 * 	Parameters:
 * 		*ap - a pointer to an ApInt instance 
 * 	Return:
 * 		a pointer to a new ApInt instance that's equal to
 * 		ap shifted by 1 to the left.
 */
ApInt *apint_lshift(ApInt *ap) {
	assert(ap != NULL);

	size_t newLength;
	if ((ap->bitString[ap->length - 1] >> 63) & 1) {
		newLength = ap->length + 1;
	} else {
		newLength = ap->length;
	}

	ApInt * newAp = malloc(sizeof(ApInt));
	newAp->bitString = (uint64_t *) malloc(sizeof(uint64_t) * newLength);
	newAp->length = newLength;

	uint64_t msb = (ap->bitString[0] >> 63);
	(newAp->bitString)[0] = (ap->bitString[0] << 1);

	for (unsigned i = 1; i < ap->length; i++) {
		(newAp->bitString)[i] = (ap->bitString[i] << 1) | msb;
		msb = (ap->bitString[i] >> 63);
	}
	if (newAp->length > ap->length) {
		newAp->bitString[ap->length] = 1;
	}
	return newAp;
}

/*
 *	Left shifts the bit string of ap by n positions. 
 * 
 * 	Parameters:
 * 		*ap - a pointer to an ApInt instance 
 * 		n - an unsigned integer indicating how many positions
 * 		each bit should shift to the left
 * 	Return:
 * 		a pointer to a new ApInt instance with a bit string equal
 * 		to that of ap but shifted n times to the left
 */
ApInt *apint_lshift_n(ApInt *ap, unsigned n) {
	assert(ap != NULL);
	int arrShift = n / 64;
	int modShift = n % 64;
	size_t lsLength = 0;

	if (modShift == 0) {
		lsLength = ap->length + arrShift;
	} else if ((ap->bitString[ap->length - 1] >> (64 - modShift)) != 0) {
		lsLength = ap->length + arrShift + 1;
	} else {
		lsLength = ap->length + arrShift;
	}

	ApInt * ls = malloc(sizeof(ApInt));
	ls->bitString = malloc(sizeof(uint64_t) * lsLength);
	ls->length = lsLength;
	
	for(int i = 0; i < arrShift; i++) {
		ls->bitString[i] = 0UL;
	}

	uint64_t msb = 0;
	for (unsigned i = 0; i < ap->length; i++) {
		ls->bitString[arrShift + i] = (ap->bitString[i] << modShift) | msb;
		if (modShift) {
			msb = ap->bitString[i] >> (64 - modShift);
		}
	}
	if (ls->length > ap->length + arrShift) {
		ls->bitString[ap->length + arrShift] = msb;
	}
	return ls;
}

/*
 *	Takes in a pointer to an ApInt instance and transforms its
 * 	bit string to a pointer to a char array that represents
 * 	its hexadecimal.
 * 
 * 	Parameters:
 * 		*ap - a pointer to an ApInt instance 
 * 	Return:
 * 		a char array that is equivalent to the bit string of ap
 * 		char array should be all lowercase
 */
char *apint_format_as_hex(ApInt *ap) {
	assert(ap != NULL);

	int isZero = 0;
	int hbs = apint_highest_bit_set(ap);

	if (hbs == -1) {
		isZero = 1;
		hbs = 0;
	}

	int start = ceil(((hbs % 64) + 1.0) / 4);
	
	// Extra +1 for the null terminator
	int totalLength = 16 * (ap->length - 1) + start + 1;
	char * hexString = (char *) malloc(sizeof(char) * totalLength);

	for (int i = 0; i < totalLength; i++) {
		hexString[i] = '\0';
	}

	char hexArr[17] = {0};
	sprintf(hexArr, "%016lx", ap->bitString[ap->length - 1]);

	if (isZero) {
		hexString[0] = '0';
	} else {
		char fHexArr[start + 1];
		memcpy(fHexArr, &hexArr[16 - start], start + 1);
		strcat(hexString, fHexArr);
	}

	// Concatenate the rest of the arr elements
	for (int i = ap->length - 2; i >= 0; i--) {
		sprintf(hexArr, "%016lx", ap->bitString[i]);
		strcat(hexString, hexArr);
	}

	hexString[totalLength - 1] = '\0';

	return hexString;
}

/*
 *	Helper function that takes in a pointer of an ApInt instance of 
 *  sum, longer, and shorter. The l and s are added together
 * 	at each array index and gets put into the corresponding array index
 * 	in sum with overflow accounted for. The length of sum is truncated
 * 	if overflow in the most significant index doens't occur.
 * 
 * 	Parameters:
 * 		*sum - a pointer to a ApInt instace representing sum of two values
 * 		*l - a pointer to a const ApInt instance
 * 		*s - a pointer to a const ApInt instance
 * 	Return:
 * 		a pointer to a ApInt instance that represent a and b's sum.
 */
ApInt *apint_add_helper(ApInt *sum, const ApInt *l, const ApInt *s) {
	int overflow = 0;

	for (unsigned i = 0; i < l->length; i++) {
		uint64_t lval = l->bitString[i];
		uint64_t indexSum = lval;
		if (i < s->length) {
			uint64_t sval = s->bitString[i];
			indexSum += sval;
		}
		if (overflow) {
			indexSum++;
		}
		if (indexSum < lval) {
			overflow = 1;
		} else {
			overflow = 0;
		}
		sum->bitString[i] = indexSum;
	}

	// Add one more arr index if overflow, or else resize the arr
	if (overflow) {
		sum->bitString[l->length] = 1UL;
	} else {
		ApInt * newSum = malloc(sizeof(ApInt));
		newSum->bitString = malloc(sizeof(uint64_t) * l->length);
		newSum->length = l->length;
		for (unsigned i = 0; i < l->length; i++) {
			newSum->bitString[i] = sum->bitString[i];
		}
		apint_destroy(sum);
		return newSum;
	}
	return sum;
}

/*
 *	Adds two ApInt instances and returns a pointer to a new
 *	ApInt instance that is the sum of the two.	
 * 
 * 	Parameters:
 * 		*a - a pointer to an ApInt instance
 * 		*b - a pointer to an ApInt instance
 * 	Return:
 * 		a pointer to a new ApInt instance that represent a and b's
 * 		sum.
 */
ApInt *apint_add(const ApInt *a, const ApInt *b) {
	assert(a != NULL && b != NULL);

	const ApInt *l;
	const ApInt *s;
	if (a->length >= b->length) {
		l = a;
		s = b;
	} else {
		l = b;
		s = a;
	}

	// Create an oversized array in case of overflow
	ApInt * sum = malloc(sizeof(ApInt));
	sum->bitString = malloc(sizeof(uint64_t) * (l->length + 1));
	sum->length = l->length + 1;
	sum = apint_add_helper(sum, l, s);
	return sum;
}

/*
 *	Helper function that takes in a pointer of an ApInt instance of 
 *  ap, a, and b. b is subtracted from b at each array index and gets
 * 	put into the corresponding array index in ap with underflow
 * 	accounted for. ap is truncated if underflow occurs in the most
 * 	significant index.
 * 
 * 	Parameters:
 * 		*sum - a pointer to a ApInt instace representing sum of two values
 * 		*l - a pointer to a const ApInt instance
 * 		*s - a pointer to a const ApInt instance
 * 	Return:
 * 		a pointer to a ApInt instance that represent a and b's sum.
 */
ApInt *apint_sub_helper(ApInt *ap, const ApInt *a, const ApInt *b) {
	int underflow = 0;
	for (unsigned i = 0; i < a->length; i++) {
		uint64_t aval = a->bitString[i];
		uint64_t res = aval;
		if (i < b->length) {
			res -= b->bitString[i];
		}
		if (underflow) {
			res--;
		}
		if (res > aval) {
			underflow = 1;
		} else {
			underflow = 0;
		}
		ap->bitString[i] = res;
	}

	// Check if the leading arr elements are 0
	int start = ap->length - 1;
	for (int i = ap->length - 1; i >= 0; i--) {
		if (ap->bitString[i] != 0UL) {
			start = i;
			break;
		}
	}

	// Truncate (realloc) the bitString if needed
	if (start != (int) (ap->length - 1)) {
		ApInt * newAp = malloc(sizeof(ApInt));
		newAp->bitString = malloc(sizeof(uint64_t) * (start + 1));
		newAp->length = start + 1;
		for (unsigned i = 0; i < newAp->length; i++) {
			newAp->bitString[i] = ap->bitString[i];
		}
		apint_destroy(ap);
		return newAp;
	}
	return ap;
}

/*
 *	Subtracts two ApInt instances and returns a pointer to a new
 * 	ApInt instance that is the difference of the two. The right is
 * 	subtracted from the left ApInt parameters
 * 	
 * 	Parameters:
 * 		*a - a pointer to an ApInt instance (first term)
 * 		*b - a pointer to an ApInt instance (second term)
 * 	Return:
 * 		a pointer to a new ApInt instance that represents the
 * 		difference between a and b.
 */
ApInt *apint_sub(const ApInt *a, const ApInt *b) {
	assert(a != NULL && b != NULL);

	// Check if subtraction is possible (give unsigned answer)
	if (apint_compare(a, b) == -1) {
		return NULL;
	}

	ApInt * ap = malloc(sizeof(ApInt));
	ap->bitString = malloc(sizeof(uint64_t) * a->length);
	ap->length = a->length;
	ap = apint_sub_helper(ap, a, b);
	return ap;
}

/*
 *	Compares two ApInt instances and returns a number based
 * 	on their relationships	
 * 
 * 	Parameters:
 * 		*left - a pointer to an ApInt instance (first term)
 * 		*rogjt - a pointer to an ApInt instance (second term)
 * 	Return:
 * 		an integer -1, 0, or 1 depending on whether left is smaller
 * 		than right, left is equal to right, and left is larger
 * 		than right, respectively.
 */
int apint_compare(const ApInt *left, const ApInt *right) {
	assert(left != NULL && right != NULL);
	if (left->length > right->length) {
		return 1;
	} else if (left->length < right->length) {
		return -1;
	} else {
		if (left->bitString[left->length - 1] > 
		right->bitString[right->length - 1]) {
			return 1;
		} else if (left->bitString[left->length - 1] < 
		right->bitString[right->length - 1]) {
			return -1;
		} else {
			return 0;
		}
	}
	return 0;
}
