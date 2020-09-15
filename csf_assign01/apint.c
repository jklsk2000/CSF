/*
 * CSF Assignment 1
 * Arbitrary-precision integer data type
 * Function implementations
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>
#include "apint.h"

ApInt *apint_create_from_u64(uint64_t val) {
	ApInt * ap = malloc(sizeof(ApInt));
	ap->bitString = (uint64_t *) malloc(sizeof(uint64_t));
	ap->length = 1;
	ap->bitString[0] = val; 
	return ap;
}

ApInt *apint_create_from_hex(const char *hex) {
	assert(hex != NULL);

	int hexLength = ceil((double) strlen(hex) / 16);
	int leftover = strlen(hex) % 16;

	ApInt * ap = malloc(sizeof(ApInt));
	ap->bitString = (uint64_t *) malloc(sizeof(uint64_t) * hexLength);
	ap->length = (size_t) hexLength;

	int start;
	int fArrLen;

	for (int i = 0; i < hexLength - 1; i++) {
		if (leftover == 0) {
			start = 16 * (hexLength - 1 - i);
		} else {
			start = leftover + 16 * (hexLength - 2 -i);
			if (i == hexLength - 1) {
				break;
			}
		}
		char hexArr[17];
		memcpy(hexArr, &hex[start], 16);
		hexArr[16] = '\0';
		ap->bitString[i] = (uint64_t) strtoul(hexArr, NULL, 16);
	}

	if (leftover == 0) {
		fArrLen = 17;
	} else {
		fArrLen = leftover + 1;
	}

	char fHexArr[fArrLen];
	memcpy(fHexArr, hex, fArrLen - 1);
	fHexArr[fArrLen - 1] = '\0';
	ap->bitString[hexLength - 1] = (uint64_t) strtoul(fHexArr, NULL, 16);

	return ap;
}

void apint_destroy(ApInt *ap) {
	assert(ap != NULL);
	free(ap->bitString);
	free(ap);
}

uint64_t apint_get_bits(ApInt *ap, unsigned n) {
	assert(1);
	if (n > ap->length - 1) {
		return 0;
	} else {
		return (ap->bitString)[n];
	}
}

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

ApInt *apint_add(const ApInt *a, const ApInt *b) {
	assert(a != NULL && b != NULL);

	size_t sumLength;
	size_t shortLength;
	uint64_t * longer;

	if (a->length >= b->length) {
		sumLength = a->length;
		longer = a->bitString;
		shortLength = b->length;
	} else {
		sumLength = b->length;
		longer = b->bitString;
		shortLength = a->length;
	}

	// Create an oversized array in case of overflow
	ApInt * sum = malloc(sizeof(ApInt));
	sum->bitString = malloc(sizeof(uint64_t) * (sumLength + 1));
	sum->length = sumLength + 1;

	int overflow = 0;

	// Both a and b have index elements
	for (unsigned i = 0; i < shortLength; i++) {
		uint64_t aValue = a->bitString[i];
		uint64_t bValue = b->bitString[i];
		uint64_t indexSum;

		if (overflow) {
			indexSum = aValue + bValue + 1;
		} else {
			indexSum = aValue + bValue;
		}

		if (indexSum < aValue || indexSum < bValue) {
			overflow = 1;
		} else {
			overflow = 0;
		}
		sum->bitString[i] = indexSum;
	}

	// Only one have index elements 
	// (either a or b is bigger than the other)
	for (unsigned i = shortLength; i < sumLength; i++) {
		uint64_t value = longer[i];
		uint64_t indexSum;
		
		if (overflow) {
			indexSum = value + 1;
		} else {
			indexSum = value;
		}

		if (indexSum < value) {
			overflow = 1;
		} else {
			overflow = 0;
		}
		sum->bitString[i] = indexSum;
	}

	// Add one more arr index if overflow, or else resize the arr
	if (overflow) {
		sum->bitString[sumLength] = 1UL;
	} else {
		ApInt * newSum = malloc(sizeof(ApInt));
		newSum->bitString = malloc(sizeof(uint64_t) * sumLength);
		newSum->length = sumLength;
		for (unsigned i = 0; i < sumLength; i++) {
			newSum->bitString[i] = sum->bitString[i];	// problem area!
		}
		apint_destroy(sum);
		return newSum;
	}

	return sum;
}

ApInt *apint_sub(const ApInt *a, const ApInt *b) {
	assert(a != NULL && b != NULL);

	// Check if subtraction is possible (give unsigned answer)
	if (apint_compare(a, b) == -1) {
		return NULL;
	}

	ApInt * ap = malloc(sizeof(ApInt));
	ap->bitString = malloc(sizeof(uint64_t) * a->length);
	ap->length = a->length;

	int underflow = 0;

	// Subtract each arr element and check for underflows
	for (unsigned i = 0; i < b->length; i++) {
		uint64_t aval = a->bitString[i];
		uint64_t bval = b->bitString[i];
		uint64_t res;

		if (underflow) {
			res = aval - bval - 1;
		} else {
			res = aval - bval;
		}

		if (aval < bval) {
			underflow = 1;
		} else {
			underflow = 0;
		}

		ap->bitString[i] = res;
	}

	// Check underflows for the rest of the arr elements
	for (unsigned i = b->length; i < a->length; i++) {
		uint64_t val = a->bitString[i];
		uint64_t res;

		if (underflow) {
			res = val - 1;
		} else {
			res = val;
		}

		if (val < res) {
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
