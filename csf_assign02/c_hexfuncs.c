/*
 * Danny (Jaekyun) Lee
 * jlee692@jhu.edu
 * Computer Systems Fundamentals
*/

// C implementation of hexdump functions

#include <unistd.h>  // this is the only system header file you may include!
#include "hexfuncs.h"

/*
 * Read up to 16 characters from stdin and copy it over to data_buf
 * Returns the number of characters read.
 * 
 * Parameters: 
 *      data_buf[]: char array to hold the characters read from stdin
 * Returns:
 *      a long value that represents the numbers of characters read
 */
long hex_read(char data_buf[]) {
    return read(0, data_buf, 16);
}

/*
 * Writes the nul-terminated char array to stdout.
 * 
 * Parameters: 
 *      s[]: const char array that has the string to be written to stdout
 * Returns:
 *      N/A
 */
void hex_write_string(const char s[]) {
    size_t i = 0;
    for (i = 0; s[i] != '\0'; ++i);
    ssize_t write_size = write(1, s, i);
    if (write_size != -1) {
        return;
    }
}

/*
 * Formats a long value in hex digits and put into a char array
 * 
 * Parameters: 
 *      offset: long value that will be formatted
 *       * ASSUMPTIONS: offset will be between 0 to positive max 8-bit value
 *      sbuf[]: char array with the end result (offset converted to hex)
 * Returns:
 *      N/A
 */
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

/*
 * Formats each long value to the appropriate 2 digit hexadecimal value
 * and inputs it into a char array.
 * 
 * Parameters: 
 *      byteval: long value that correspond to a specific char (ASCII value)
 *      sbuf[]: char array that will hold the hex equivalent of byteval
 * Returns:
 *      N/A
 */
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

/*
 * Converts non-printable characters of the ASCII table to a dot. Keeps
 * other characters unmodified.
 * 
 * Parameters: 
 *      byteval: long value that correspond to a specific char (ASCII value)
 * Returns:
 *      long value that is either unmodified, or in the case of non-printable
 *      characters, a dot.
 */
long hex_to_printable(long byteval) {
    if ((byteval >= 0 && byteval <= 31) || (byteval >= 127 && byteval <= 255)) {
        byteval = 46;
    }
    return byteval;
}