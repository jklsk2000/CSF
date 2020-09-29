/*
 * Danny (Jaekyun) Lee
 * jlee692@jhu.edu
 * Computer Systems Fundamentals
*/

// Unit tests for hex functions
// These tests should work for both your C implementations and your
// assembly language implementations

#include <stdio.h>
#include <stdlib.h>
#include "tctest.h"
#include "hexfuncs.h"

// test fixture object
typedef struct {
  char test_data_1[16];
  char test_data_2[16];
} TestObjs;

// setup function (to create the test fixture)
TestObjs *setup(void) {
  TestObjs *objs = malloc(sizeof(TestObjs));
  strcpy(objs->test_data_1, "Hello, world!\n");
  strcpy(objs->test_data_2, "hello\nthere\n .\0€");
  return objs;
}

// cleanup function (to destroy the test fixture)
void cleanup(TestObjs *objs) {
  free(objs);
}

// Prototypes for test functions
void testFormatOffset(TestObjs *objs);
void testFormatByteAsHex(TestObjs *objs);
void testHexToPrintable(TestObjs *objs);

int main(int argc, char **argv) {
  if (argc > 1) {
    tctest_testname_to_execute = argv[1];
  }

  TEST_INIT();

  TEST(testFormatOffset);
  TEST(testFormatByteAsHex);
  TEST(testHexToPrintable);

  TEST_FINI();

  return 0;
}

void testFormatOffset(TestObjs *objs) {
  (void) objs; // suppress warning about unused parameter
  char buf[16];
  hex_format_offset(1L, buf);
  ASSERT(0 == strcmp(buf, "00000001"));

  // Zero Case
  hex_format_offset(0xFFFFFFFFL, buf);
  ASSERT(0 == strcmp(buf, "ffffffff"));

  // Max Case
  hex_format_offset(1L, buf);
  ASSERT(0 == strcmp(buf, "00000001"));

  hex_format_offset(15L, buf);
  ASSERT(0 == strcmp(buf, "0000000f"));

  hex_format_offset(255L, buf);
  ASSERT(0 == strcmp(buf, "000000ff"));
  
  hex_format_offset(0xd3e9L, buf);
  ASSERT(0 == strcmp(buf, "0000d3e9"));

  hex_format_offset(0xccbe7dccL, buf);
  ASSERT(0 == strcmp(buf, "ccbe7dcc"));
}

void testFormatByteAsHex(TestObjs *objs) {
  char buf[16];
  // H
  hex_format_byte_as_hex(objs->test_data_1[0], buf);
  ASSERT(0 == strcmp(buf, "48"));

  // e
  hex_format_byte_as_hex(objs->test_data_1[1], buf);
  ASSERT(0 == strcmp(buf, "65"));

  // o
  hex_format_byte_as_hex(objs->test_data_1[4], buf);
  ASSERT(0 == strcmp(buf, "6f"));

  // ,
  hex_format_byte_as_hex(objs->test_data_1[5], buf);
  ASSERT(0 == strcmp(buf, "2c"));

  // ' '
  hex_format_byte_as_hex(objs->test_data_1[6], buf);
  ASSERT(0 == strcmp(buf, "20"));

  // !
  hex_format_byte_as_hex(objs->test_data_1[12], buf);
  ASSERT(0 == strcmp(buf, "21"));

  // \n
  hex_format_byte_as_hex(objs->test_data_1[13], buf);
  ASSERT(0 == strcmp(buf, "0a"));

  // \n
  hex_format_byte_as_hex(objs->test_data_2[5], buf);
  ASSERT(0 == strcmp(buf, "0a"));

  // .
  hex_format_byte_as_hex(objs->test_data_2[13], buf);
  ASSERT(0 == strcmp(buf, "2e"));

  // \0
  hex_format_byte_as_hex(objs->test_data_2[14], buf);
  ASSERT(0 == strcmp(buf, "00"));
}

void testHexToPrintable(TestObjs *objs) {
  ASSERT('H' == hex_to_printable(objs->test_data_1[0]));
  ASSERT('.' == hex_to_printable(objs->test_data_1[13]));

  ASSERT('e' == hex_to_printable(objs->test_data_1[1]));
  ASSERT(',' == hex_to_printable(objs->test_data_1[5]));
  ASSERT(' ' == hex_to_printable(objs->test_data_1[6]));
  ASSERT('!' == hex_to_printable(objs->test_data_1[12]));

  ASSERT('.' == hex_to_printable(objs->test_data_2[13]));
  ASSERT('.' == hex_to_printable(objs->test_data_2[14]));
  ASSERT('.' == hex_to_printable(objs->test_data_2[15]));
}
