/*
 * Danny (Jaekyun) Lee
 * jlee692@jhu.edu
 * Computer Systems Fundamentals
*/

// C implementation of hexdump main function

#include "hexfuncs.h"  // this is the only header file which may be included!

int main(void) {
    // Fix this so it doesn't use calloc
    char * hexDump = (char *) calloc(77, sizeof(char));
    
    char data_buf[17] = {0};
    long data_len = 0;
    long index = 0;
    long position = 0;

    data_len = hex_read(data_buf);

    while (data_len > 0) {
        data_buf[data_len] = '\0';

        // code goes here
        offset(hexDump, index, position);
        position += 16;


        data_len = hex_read(data_buf);
    }

}

void offset(char *hexDump, long index, long position) {
    char sbuf[9] = {0};
    hex_format_offset(position, sbuf);
    for (int i = 0; i < 8; i++) {
        hexDump[index++] = sbuf[i];
    }
    hexDump[index++] = ':';
}

void convertToHex(char *hexDump, long index, char data_buf[]) {
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
}

void convertToPrintable(char *hexDump, long index, char data_buf[]) {
    hexDump[index++] = ' ';
    hexDump[index++] = ' ';

    for (int i = 0; data_buf[i] != '\0'; i++) {
        hexDump[index++] = hex_to_printable(data_buf[i]);
    }

    hexDump[index++] = '\n';
}