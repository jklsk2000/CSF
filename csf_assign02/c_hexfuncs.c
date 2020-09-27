/*
 * Danny (Jaekyun) Lee
 * jlee692@jhu.edu
 * Computer Systems Fundamentals
*/

// C implementation of hexdump functions

#include <unistd.h>  // this is the only system header file you may include!
#include "hexfuncs.h"

long hex_read(char data_buf[]) {
    return read(0, data_buf, 16);
}

void hex_write_string(const char s[]) {
    size_t i = 0;
    for (size_t i = 0; s[i] != '\0'; ++i);
    ssize_t write_size = write(1, s, i);
    if (write_size != -1) {
        return;
    }
}

void hex_format_offset(long offset, char sbuf[]) {
    for (int i = 7; i >= 0; i--) {
        int hex = offset & 0xf;
        if (hex >= 0 && hex <= 9) {
            sbuf[i] = hex + '0';
        } else if (hex >= 10 && hex <= 15) {
            sbuf[i] = (char) (hex + 87);
        }
        offset >>= 4;
    }
    sbuf[8] = '\0';
}

void hex_format_byte_as_hex(long byteval, char sbuf[]) {
    for (int i = 1; i >= 0; i--) {
        int hex = byteval & 0xf;
        if (hex >= 0 && hex <= 9) {
            sbuf[i] = hex + '0';
        } else if (hex >= 10 && hex <= 15) {
            sbuf[i] = (char) (hex + 87);
        }
        byteval >>= 4;
    }
    sbuf[2] = '\0';
}

long hex_to_printable(long byteval) {
    if ((byteval >= 0 && byteval <= 31) || (byteval >= 127 && byteval <= 255)) {
        byteval = 46;
    }
    return byteval;
}