/*
 * Danny (Jaekyun) Lee
 * jlee692@jhu.edu
 * Computer Systems Fundamentals
*/

// C implementation of hexdump main function

#include "hexfuncs.h"  // this is the only header file which may be included!

/*
 * Formats the offset of the hex dump using the hex_format_offset
 * function and inputs it into the hexDump array.
 * 
 * Parameters: 
 *      hexDump: main array that holds a line of hex dump
 *      index: index that keeps track of the position in hexDump array
 *      position: long value that represents the 'position' of input
 * Returns:
 *      int value that represents the new index on hexDump
 */
int offset(char *hexDump, long index, long position) {
    char sbuf[9] = {0};
    hex_format_offset(position, sbuf);
    for (int i = 0; i < 8; i++) {
        hexDump[index++] = sbuf[i];
    }
    hexDump[index++] = ':';
    return index;
}

/*
 * Formats the max 16 byte char array into 16 byte hexadecimal values
 * through hex_format_byte_as_hex function, which is then inputted 
 * into the hexDump array.
 * 
 * Parameters: 
 *      hexDump: main array that holds a line of hex dump
 *      index: index that keeps track of the position in hexDump array
 *      data_buf[]: char array that holds upto 16 chars that were read
 *                  from stdin
 * Returns:
 *      int value that represents the new index on hexDump
 */
int convertToHex(char *hexDump, long index, char data_buf[]) {
    char sbuf[2] = {0};
    int end = 0;

    for (int i = 0; i < 16; i++) {
        hexDump[index++] = ' ';
        if (data_buf[i] == '\0') {
            end = 1;
        }
        if (end == 0) {
            hex_format_byte_as_hex(data_buf[i], sbuf);
            hexDump[index++] = sbuf[0];
            hexDump[index++] = sbuf[1];
        } else if (end == 1) {
            hexDump[index++] = ' ';
            hexDump[index++] = ' ';
        }
    }

    return index;
}

/*
 * Converts non-printable characters into dots and leaves printable 
 * characters unmodified using hex_to_printable function. The result
 * is copied into the hexDump array.
 * 
 * Parameters: 
 *      hexDump: main array that holds a line of hex dump
 *      index: index that keeps track of the position in hexDump array
 *      data_buf[]: char array that holds upto 16 chars that were read
 *                  from stdin
 * Returns:
 *      int value that represents the new index on hexDump
 */
int convertToPrintable(char *hexDump, long index, char data_buf[]) {
    hexDump[index++] = ' ';
    hexDump[index++] = ' ';

    for (int i = 0; data_buf[i] != '\0'; i++) {
        hexDump[index++] = hex_to_printable(data_buf[i]);
    }

    hexDump[index++] = '\n';
    hexDump[index++] = '\0';
    return index;
}

int main(void) {
    char hexDump[78];
    char data_buf[17] = {0};
    long data_len = 0;
    long index = 0;
    long position = 0;

    data_len = hex_read(data_buf);

    while (data_len > 0) {
        data_buf[data_len] = '\0';
        index = offset(hexDump, index, position);
        position += 16;
        index = convertToHex(hexDump, index, data_buf);
        index = convertToPrintable(hexDump, index, data_buf);
        hex_write_string(hexDump);
        data_len = hex_read(data_buf);
        index = 0;
    }

}