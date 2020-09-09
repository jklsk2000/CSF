/*
 * CSF Assignment 1
 * Arbitrary-precision integer data type
 * Function implementations
 */

#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <inttypes.h>
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

	size_t tempLength = ceil(strlen(hex) / 16);
	int leftover = strlen(hex) % 16;

	ApInt * ap = malloc(sizeof(ApInt));
	ap->bitString = (uint64_t *) malloc(sizeof(uint64_t) * tempLength);
	ap->length = tempLength;


	for (int i = 0; i < tempLength - 1; i++) {
		int start = leftover + 16 * (tempLength - 2 - i);
		char hexCharArr[16]; 
		strncpy(hexCharArr, hex + start, 16);
		ap->bitString[i] = (uint64_t) strtoul(hexCharArr, NULL, 16);
	}

	char fHexCharArr[leftover];
	strncpy(fHexCharArr, hex, leftover);
	ap->bitString[tempLength] = (uint64_t) strtoul(fHexCharArr, NULL, 16);

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

//FIX
// int apint_highest_bit_set(ApInt *ap) {
// 	/* TODO: implement */
// 	assert(ap != NULL);
// 	if (ap->length == 1 && (ap->bitString)[0] == 0) {
// 		return -1;
// 	} else {
// 		return (ap->bitString)[ap->length - 1];
// 	}
// }

// ApInt *apint_lshift(ApInt *ap) {
// 	/* TODO: implement */
// 	assert(0);
// 	return NULL;
// }

// ApInt *apint_lshift_n(ApInt *ap, unsigned n) {
// 	/* TODO: implement */
// 	assert(0);
// 	return NULL;
// }

// char *apint_format_as_hex(ApInt *ap) {
// 	/* TODO: implement */
// 	assert(0);
// 	return NULL;
// }

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
