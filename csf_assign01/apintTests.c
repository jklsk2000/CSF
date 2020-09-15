/*
 * Unit tests for arbitrary-precision integer data type
 *
 * These tests are by no means comprehensive.  You will need to
 * add more tests of your own!  In particular, make sure that
 * you have tests for more challenging situations, such as
 *
 * - large values
 * - adding/subtracting/comparing values with different lengths
 * - special cases (carries when adding, borrows when subtracting, etc.)
 * - etc.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "apint.h"
#include "tctest.h"

typedef struct {
	ApInt *ap0;
	ApInt *ap1;
	ApInt *ap110660361;
	ApInt *max1;

	ApInt *hex97;
	ApInt *hex3500;
	ApInt *hexLower3500;
	ApInt *hexMixed3500;
	ApInt *hex68Bit;
	ApInt *hex128Bit;
	ApInt *hexMax128Bit;
	ApInt *hex3Long;
	ApInt *hex3Max;
	ApInt *hex4Long;
	ApInt *hexPadded;

	ApInt *compare1;
	ApInt *compare2;
	ApInt *compare3;
	ApInt *compare4;
	ApInt *compare5;

} TestObjs;

TestObjs *setup(void);
void cleanup(TestObjs *objs);

void testCreateFromU64(TestObjs *objs);
void testHighestBitSet(TestObjs *objs);
void testLshiftN(TestObjs *objs);
void testCompare(TestObjs *objs);
void testFormatAsHex(TestObjs *objs);
void testAdd(TestObjs *objs);
void testSub(TestObjs *objs);
void testGetBitsEdgeCase(TestObjs *objs);
void testCreateFromHex(TestObjs *objs);
void testLshift(TestObjs *objs);

int main(int argc, char **argv) {
	TEST_INIT();

	if (argc > 1) {
		/*
		 * name of specific test case to execute was provided
		 * as a command line argument
		 */
		tctest_testname_to_execute = argv[1];
	}

	TEST(testCreateFromU64);
	TEST(testGetBitsEdgeCase);
	TEST(testHighestBitSet);
	TEST(testLshift);
	TEST(testLshiftN);
	TEST(testCompare);
	TEST(testCreateFromHex);
	TEST(testFormatAsHex);
	TEST(testAdd);
	TEST(testSub);

	TEST_FINI();
}

TestObjs *setup(void) {
	TestObjs *objs = malloc(sizeof(TestObjs));
	objs->ap0 = apint_create_from_u64(0UL);
	objs->ap1 = apint_create_from_u64(1UL);
	objs->ap110660361 = apint_create_from_u64(110660361UL);
	objs->max1 = apint_create_from_u64(0xFFFFFFFFFFFFFFFFUL);

	objs->hex97 = apint_create_from_hex("61");
	objs->hex3500 = apint_create_from_hex("DAC");
	objs->hexLower3500 = apint_create_from_hex("dac");
	objs->hexMixed3500 = apint_create_from_hex("DaC");
	objs->hex68Bit = apint_create_from_hex("8A0293CBB55226D7C");
	objs->hex128Bit = apint_create_from_hex("11027b978d0975748a43c2abd45fcd57");
	objs->hexMax128Bit = apint_create_from_hex("ffffffffffffffffffffffffffffffff");
	objs->hex3Long = apint_create_from_hex("869f41946e390db6ee8f252c01fd72e94a52a4ad86cd608f");
	objs->hex3Max = apint_create_from_hex("ffffffffffffffffffffffffffffffffffffffffffffffff");
	objs->hex4Long = apint_create_from_hex("6938ab6214a1991c3954a0c75128cca33bd8abcb8f483ce810ecafa0a740dbbc");
	objs->hexPadded = apint_create_from_hex("00000000000000000000000000000000000cafe");

	objs->compare1 = apint_create_from_hex("30becde045ba9613ca2edc7b2d3247add56bfa160ff359b8bbb192441");
	objs->compare2 = apint_create_from_hex("6773cb839cdcda49b6b22546ece507ad877571e607dbd385dd2af7");
	objs->compare3 = apint_create_from_hex("db4cefc1ec5fbe0a03ec11c014663");
	objs->compare4 = apint_create_from_hex("8ffca6bbf283cb179dbf8e9c5f48ee2");
	objs->compare5 = apint_create_from_hex("8ffca6bbf283cb079dbf8e9c5f48ee2");

	return objs;
}

void cleanup(TestObjs *objs) {
	apint_destroy(objs->ap0);
	apint_destroy(objs->ap1);
	apint_destroy(objs->ap110660361);
	apint_destroy(objs->max1);

	apint_destroy(objs->hex97);
	apint_destroy(objs->hex3500);
	apint_destroy(objs->hexLower3500);
	apint_destroy(objs->hexMixed3500);
	apint_destroy(objs->hex68Bit);
	apint_destroy(objs->hex128Bit);
	apint_destroy(objs->hexMax128Bit);
	apint_destroy(objs->hex3Long);
	apint_destroy(objs->hex3Max);
	apint_destroy(objs->hex4Long);
	apint_destroy(objs->hexPadded);

	apint_destroy(objs->compare1);
	apint_destroy(objs->compare2);
	apint_destroy(objs->compare3);
	apint_destroy(objs->compare4);
	apint_destroy(objs->compare5);

	free(objs);
}

void testCreateFromU64(TestObjs *objs) {
	ASSERT(0UL == apint_get_bits(objs->ap0, 0));
	ASSERT(1UL == apint_get_bits(objs->ap1, 0));
	ASSERT(110660361UL == apint_get_bits(objs->ap110660361, 0));
	ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(objs->max1, 0));
}

void testGetBitsEdgeCase(TestObjs *objs) {
	// If index is out of bounds, get_bits returns 0
	ASSERT(1UL == apint_get_bits(objs->ap1, 0));
	ASSERT(0 == apint_get_bits(objs->ap1, 1));
	ASSERT(0 == apint_get_bits(objs->ap1, 2));
}

void testHighestBitSet(TestObjs *objs) {
	ASSERT(-1 == apint_highest_bit_set(objs->ap0));
	ASSERT(0 == apint_highest_bit_set(objs->ap1));
	ASSERT(26 == apint_highest_bit_set(objs->ap110660361));
	ASSERT(63 == apint_highest_bit_set(objs->max1));

	// Test for ApInts bigger than length 1
	ASSERT(67 == apint_highest_bit_set(objs->hex68Bit));
	ASSERT(124 == apint_highest_bit_set(objs->hex128Bit));
	ASSERT(127 == apint_highest_bit_set(objs->hexMax128Bit));
	ASSERT(191 == apint_highest_bit_set(objs->hex3Long));
	ASSERT(191 == apint_highest_bit_set(objs->hex3Max));
	ASSERT(254 == apint_highest_bit_set(objs->hex4Long));
}

void testLshift(TestObjs *objs) {
	ApInt * result;
	
	// Lshift without creating new arr element
	result = apint_lshift(objs->ap0);
	ASSERT(0UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);

	result = apint_lshift(objs->ap1);
	ASSERT(2UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);

	result = apint_lshift(objs->ap110660361);
	ASSERT(221320722UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);

	// Lshift creates new arr element
	result = apint_lshift(objs->max1);
	ASSERT(0xFFFFFFFFFFFFFFFEUL == apint_get_bits(result, 0));
	ASSERT(1UL == apint_get_bits(result, 1));
	ASSERT(0UL == apint_get_bits(result, 2));
	apint_destroy(result);

	result = apint_lshift(objs->hex68Bit);
	ASSERT(0x40527976AA44DAF8UL == apint_get_bits(result, 0));
	ASSERT(17UL == apint_get_bits(result, 1));
	ASSERT(0UL == apint_get_bits(result, 2));
	apint_destroy(result);

	result = apint_lshift(objs->hex128Bit);
	ASSERT(0x14878557A8BF9AAEUL == apint_get_bits(result, 0));
	ASSERT(0x2204F72F1A12EAE9UL == apint_get_bits(result, 1));
	ASSERT(0UL == apint_get_bits(result, 2));
	apint_destroy(result);

	result = apint_lshift(objs->hexMax128Bit);
	ASSERT(0xFFFFFFFFFFFFFFFEUL == apint_get_bits(result, 0));
	ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(result, 1));
	ASSERT(1UL == apint_get_bits(result, 2));
	ASSERT(0UL == apint_get_bits(result, 3));
	apint_destroy(result);
}

void testLshiftN(TestObjs *objs) {
	ApInt *result;

	// Testing LshiftN for N = 0
	result = apint_lshift_n(objs->ap1, 0);
	ASSERT(1UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);

	// Testing LshiftN for N < 64
	result = apint_lshift_n(objs->ap0, 17);
	ASSERT(0UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);

	result = apint_lshift_n(objs->ap1, 17);
	ASSERT(0x20000UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);

	result = apint_lshift_n(objs->ap110660361, 17);
	ASSERT(0xD3116120000UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	apint_destroy(result);

	result = apint_lshift_n(objs->max1, 17);
	ASSERT(0xFFFFFFFFFFFE0000UL == apint_get_bits(result, 0));
	ASSERT(0x1FFFFUL == apint_get_bits(result, 1));
	ASSERT(0UL == apint_get_bits(result, 2));
	apint_destroy(result);

	// Testing LshiftN for N = 64
	result = apint_lshift_n(objs->ap1, 64);
	ASSERT(0UL == apint_get_bits(result, 0));
	ASSERT(1UL == apint_get_bits(result, 1));
	ASSERT(0UL == apint_get_bits(result , 2));
	apint_destroy(result);

	// Testing LshiftN for N > 64
	result = apint_lshift_n(objs->ap1, 65);
	ASSERT(0UL == apint_get_bits(result, 0));
	ASSERT(2UL == apint_get_bits(result, 1));
	ASSERT(0UL == apint_get_bits(result , 2));
	apint_destroy(result);

	result = apint_lshift_n(objs->max1, 133);
	ASSERT(0UL == apint_get_bits(result, 0));
	ASSERT(0UL == apint_get_bits(result, 1));
	ASSERT(18446744073709551584UL == apint_get_bits(result, 2));
	ASSERT(31UL == apint_get_bits(result, 3));
	ASSERT(0UL == apint_get_bits(result , 4));
	apint_destroy(result);
}

void testCompare(TestObjs *objs) {
	/* 1 > 0 */
	ASSERT(apint_compare(objs->ap1, objs->ap0) > 0);
	/* 0 < 1 */
	ASSERT(apint_compare(objs->ap0, objs->ap1) < 0);
	/* 110660361 > 0 */
	ASSERT(apint_compare(objs->ap110660361, objs->ap0) > 0);
	/* 110660361 > 1 */
	ASSERT(apint_compare(objs->ap110660361, objs->ap1) > 0);
	/* 0 < 110660361 */
	ASSERT(apint_compare(objs->ap0, objs->ap110660361) < 0);
	/* 1 < 110660361 */
	ASSERT(apint_compare(objs->ap1, objs->ap110660361) < 0);

	// Equals Case
	/* 1 = 1 */
	ASSERT(apint_compare(objs->ap1, objs->ap1) == 0);

	// With big numbers
	ASSERT(apint_compare(objs->compare1, objs->compare2) == 1);
	ASSERT(apint_compare(objs->compare3, objs->compare4) == -1);
	ASSERT(apint_compare(objs->compare4, objs->compare5) == 1);
	ASSERT(apint_compare(objs->compare5, objs->compare5) == 0);
}

void testCreateFromHex(TestObjs *objs) {
	// Less than 16 chars
	ASSERT(97UL == apint_get_bits(objs->hex97, 0));

	// Accepts lowercase, uppercase, and mixed
	ASSERT(3500UL == apint_get_bits(objs->hex3500, 0));
	ASSERT(3500UL == apint_get_bits(objs->hexLower3500, 0));
	ASSERT(3500UL == apint_get_bits(objs->hexMixed3500, 0));

	// More than 16 chars
	ASSERT(11540822295398477180UL == apint_get_bits(objs->hex68Bit, 0));
	ASSERT(8UL == apint_get_bits(objs->hex68Bit, 1));
	ASSERT(9963020843931913559UL == apint_get_bits(objs->hex128Bit, 0));
	ASSERT(1225677939434681716UL == apint_get_bits(objs->hex128Bit, 1));
	ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(objs->hexMax128Bit, 0));
	ASSERT(0xFFFFFFFFFFFFFFFFUL == apint_get_bits(objs->hexMax128Bit, 1));
}

void testFormatAsHex(TestObjs *objs) {
	char *s;

	// Testing less than or equal to 16 chars
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(objs->ap0))));
	free(s);
	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(objs->ap1))));
	free(s);
	ASSERT(0 == strcmp("6988b09", (s = apint_format_as_hex(objs->ap110660361))));
	free(s);
	ASSERT(0 == strcmp("ffffffffffffffff", (s = apint_format_as_hex(objs->max1))));
	free(s);

	// Testing longer than 16 chars
	ASSERT(0 == strcmp("8a0293cbb55226d7c", 
		(s = apint_format_as_hex(objs->hex68Bit))));
	free(s);
	ASSERT(0 == strcmp("11027b978d0975748a43c2abd45fcd57", 
		(s = apint_format_as_hex(objs->hex128Bit))));
	free(s);
	ASSERT(0 == strcmp("ffffffffffffffffffffffffffffffff", 
		(s = apint_format_as_hex(objs->hexMax128Bit))));
	free(s);
	ASSERT(0 == strcmp("869f41946e390db6ee8f252c01fd72e94a52a4ad86cd608f", 
		(s = apint_format_as_hex(objs->hex3Long))));
	free(s);

	// Testing that there are no leading zeroes
	ASSERT(0 == strcmp("cafe", (s = apint_format_as_hex(objs->hexPadded))));
	free(s);
}

void testAdd(TestObjs *objs) {
	ApInt *a, *b, *sum;
	char *s;

	/* 0 + 0 = 0 */
	sum = apint_add(objs->ap0, objs->ap0);
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 1 + 0 = 1 */
	sum = apint_add(objs->ap1, objs->ap0);
	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 1 + 1 = 2 */
	sum = apint_add(objs->ap1, objs->ap1);
	ASSERT(0 == strcmp("2", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* 110660361 + 1 = 110660362 */
	sum = apint_add(objs->ap110660361, objs->ap1);
	ASSERT(0 == strcmp("6988b0a", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	/* FFFFFFFFFFFFFFFF + 1 = 10000000000000000 */
	sum = apint_add(objs->max1, objs->ap1);
	ASSERT(0 == strcmp("10000000000000000", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	// Stress tests using larger values
	sum = apint_add(objs->hexMax128Bit, objs->ap1);
	ASSERT(0 == strcmp("100000000000000000000000000000000", (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	free(s);

	a = apint_create_from_hex("28e91df7bd5fafdea9cdbc664bac86cf4518cc900fe984111074b4eb142aab23f8f11abac1f54");
	b = apint_create_from_hex("c22e0d4b88c45ca1f0f082a7d86a5dbd7fd5132a4beec7bd3eb671ed7");
	sum = apint_add(a, b);
	ASSERT(0 == strcmp("28e91df7bd5fafdea9ce7e9458f80f93a1babd8092915c7b6e3234c02754f712c0ae597133e2b",
       (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	a = apint_create_from_hex("1cebd807006a9e8a3432d8a60ee83f26");
	b = apint_create_from_hex("575fccde7318adcbfd7cf8f884dc79526e93c3e9de03432360e68e5bb95336bdecbc47d925381767bd9451302e134");
	sum = apint_add(a, b);
	ASSERT(0 == strcmp("575fccde7318adcbfd7cf8f884dc79526e93c3e9de03432360e68e5bb953388caa3cb7dfcf20baaaeb1eb21eb205a",
       (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	a = apint_create_from_hex("c3db35fbac8311afbfa0a77f62e2ad045b5dc9f3607b8e9a09dec060e4d4b8fbf41912e3");
	b = apint_create_from_hex("63972f02930eb5e7df946f8aaae1b85c7ae5a8171d21958bf3be32ff22ea4da7df390901dcb91ff77de24cd5f09e04f9");
	sum = apint_add(a, b);
	ASSERT(0 == strcmp("63972f02930eb5e7df946f8b6ebcee582768b9c6dcc23d0b56a0e0037e48179b3fb4979be697e05862b705d1e4b717dc",
       (s = apint_format_as_hex(sum))));
	apint_destroy(sum);
	apint_destroy(b);
	apint_destroy(a);
	free(s);
}

void testSub(TestObjs *objs) {
	ApInt *a, *b, *diff;
	char *s;

	/* subtracting 1 from ffffffffffffffff is fffffffffffffffe */
	diff = apint_sub(objs->max1, objs->ap1);
	ASSERT(0 == strcmp("fffffffffffffffe", (s = apint_format_as_hex(diff))));
	apint_destroy(diff);
	free(s);

	/* subtracting 0 from 1 is 1 */
	diff = apint_sub(objs->ap1, objs->ap0);
	ASSERT(0 == strcmp("1", (s = apint_format_as_hex(diff))));
	ASSERT(0 == apint_compare(diff, objs->ap1));
	apint_destroy(diff);
	free(s);

	/* subtracting 1 from 1 is 0 */
	diff = apint_sub(objs->ap1, objs->ap1);
	ASSERT(0 == strcmp("0", (s = apint_format_as_hex(diff))));
	ASSERT(0 == apint_compare(diff, objs->ap0));
	apint_destroy(diff);
	free(s);

	/* subtracting 1 from 0 can't be represented because it is negative */
	ASSERT(NULL == apint_sub(objs->ap0, objs->ap1));

	/* Stress test involving larger values */
	a = apint_create_from_hex("7e35207519b6b06429378631ca460905c19537644f31dc50114e9dc90bb4e4ebc43cfebe6b86d");
	b = apint_create_from_hex("9fa0fb165441ade7cb8b17c3ab3653465e09e8078e09631ec8f6fe3a5b301dc");
	diff = apint_sub(a, b);
	ASSERT(0 == strcmp("7e35207519b6afc4883c6fdd8898213a367d73b918de95f20766963b0251c622cd3ec4633b691",
		(s = apint_format_as_hex(diff))));
	apint_destroy(diff);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	a = apint_create_from_hex("7c99fe717cbd2fc4b3b4720dad3f4a498ee09db");
	b = apint_create_from_hex("3a11d61f7dd129dc1adeca7e9b79affd7fbb");
	diff = apint_sub(a, b);
	ASSERT(0 == strcmp("7c965d541ac552b215f2c421055592ae8f08a20",
		(s = apint_format_as_hex(diff))));
	apint_destroy(diff);
	apint_destroy(b);
	apint_destroy(a);
	free(s);

	a = apint_create_from_hex("57b219977dd0e0ebba2b1c19e3265e5d40a8357857fdd743b4dcffb5b92959dc2e6cd6eb0");
	b = apint_create_from_hex("742fbd162d508adc69751ff02f45e05d2dffc804061ea014de0f");
	diff = apint_sub(a, b);
	ASSERT(0 == strcmp("57b219977dd0e0ebba2b14d6e754fb8837fa6ee105fed44f56d72cd5bca9197a446b890a1",
		(s = apint_format_as_hex(diff))));
	apint_destroy(diff);
	apint_destroy(b);
	apint_destroy(a);
	free(s);
}