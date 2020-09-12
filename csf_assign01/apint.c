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
	/* TODO: implement */
	assert(1);
	ApInt * ap = malloc(sizeof(ApInt));
	ap->bitString = (uint64_t *) malloc(sizeof(uint64_t));
	ap->length = 1;
	ap->bitString[0] = val; 
	return ap;
}

ApInt *apint_create_from_hex(const char *hex) {
	/* TODO: implement */
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
	/* TODO: implement */
	assert(ap != NULL);
	free(ap->bitString);
	free(ap);
}

uint64_t apint_get_bits(ApInt *ap, unsigned n) {
	/* TODO: implement */
	assert(1);
	if (n > ap->length - 1) {
		return 0;
	} else {
		return (ap->bitString)[n];
	}
}

int apint_highest_bit_set(ApInt *ap) {
	/* TODO: implement */
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

		int hbs = (ap->length - 1) * 64 + order;

		return hbs;
	}
}

ApInt *apint_lshift(ApInt *ap) {
	/* TODO: implement */
	assert(ap != NULL);

	// Check if additional index is needed
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
	/* TODO: implement */
	assert(ap != NULL);

	int indexShift = n / 64;
	int modShift = n % 64;

	size_t newLength;

	if (((ap->bitString)[ap->length - 1] >> (64 - modShift)) != 0) {
		newLength = ap->length + indexShift + 1;
	} else {
		newLength = ap->length + indexShift;
	}

	ApInt * newAp = malloc(sizeof(ApInt));
	newAp->bitString = (uint64_t *) malloc(sizeof(uint64_t) * newLength);
	newAp->length = newLength;

	// Fill LS array elements w/ 0 for >64 shifts
	for (int i = 0; i < indexShift; i++) {
		(newAp->bitString)[i] = 0;
	}

	uint64_t msb = (ap->bitString)[0] >> (64 - modShift);
	(newAp->bitString)[indexShift] = (ap->bitString)[0] << modShift;

	for (unsigned i = 1; i < ap->length; i++) {
		(newAp->bitString)[indexShift + i] = ((ap->bitString)[i] << modShift) | msb;
		msb = (ap->bitString[i] >> (64 - modShift));
	}

	if (newAp->length > ap->length + indexShift) {
		newAp->bitString[ap->length + indexShift] = msb;
	}

	return newAp;
}

char *apint_format_as_hex(ApInt *ap) {
	/* TODO: implement */
	assert(ap != NULL);

	char hexArr[17];

	// Find the length of hex chars at the most sig arr element
	int start = -1;
	sprintf(hexArr, "%016lx", ap->bitString[ap->length - 1]);
	for (unsigned i = 0; i < strlen(hexArr); i++) {
		if (hexArr[i] != '0') {
			start = i;
			break;
		}
	}

	int fLength;
	if (start != -1) {
		fLength = 16 - start;
	} else {	// val = 0
		fLength = 1;
	}

	int totalLength = 16 * (ap->length - 1) + fLength + 1;
	char * hexString = (char *) malloc(sizeof(char) * totalLength);
	
	// Concatenate the most sig arr element to hexString
	if (start != -1) {
		// Suppress the leading zeroes
		char newHexArr[17 - start];
		memcpy(newHexArr, &hexArr[start], 17 - start);
		strcat(hexString, newHexArr);
	} else {
		if (ap->length == 1) {	// val = 0
			hexString[0] = '0';
		}
	}

	// Concatenate the rest of the arr elements
	for (int i = ap->length - 2; i >= 0; i--) {
		sprintf(hexArr, "%016lx", ap->bitString[i]);
		strcat(hexString, hexArr);
	}

	hexString[totalLength] = '\0';

	return hexString;
}

// ApInt *apint_add(const ApInt *a, const ApInt *b) {
// 	/* TODO: implement */
// 	assert(0);
// 	return NULL;
// }


// ApInt *apint_sub(const ApInt *a, const ApInt *b) {
// 	/* TODO: implement */
// 	assert(0);
// 	return NULL;
// }

// int apint_compare(const ApInt *left, const ApInt *right) {
// 	/* TODO: implement */
// 	assert(0);
// 	return 0;
// }
