/**
 * @file stringhelper.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <ctype.h>
#include <syslog.h>
#include "stringhelper.h"

void byte2hex(char *dst, unsigned char byte)
{
    static char digits[] = { '0', '1', '2', '3', '4', '5', '6' , '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f'};

    *dst = digits[byte >> 4];
    *(dst + 1) = digits[byte & 0xf];
}

int hex2byte(const char *str)
{
    char ch;
    unsigned char b;

    ch = str[0];
    if ((ch >= '0') && (ch <= '9')) {
        b = (ch - '0') << 4;
    } else if ((ch >= 'A') && (ch <= 'F')) {
        b = (ch - 'A' + 10) << 4;
    } else if ((ch >= 'a') && (ch <= 'f')) {
        b = (ch - 'a' + 10) << 4;
    } else {
        return -1;
    }

    ch = str[1];
    if ((ch >= '0') && (ch <= '9')) {
        b = b | (ch - '0');
    } else if ((ch >= 'A') && (ch <= 'F')) {
        b = b | (ch - 'A' + 10);
    } else if ((ch >= 'a') && (ch <= 'f')) {
        b = b | (ch - 'a' + 10);
    } else {
        return -1;
    }

    return b;
}


mpk_ret_t read_hexstr(unsigned char barray[], int blen, const char *hexstr)
{
    int i;

    while ((i < blen) && *hexstr && *(hexstr + 1)) {
        if (!isxdigit(*hexstr) || !isxdigit(*(hexstr + 1))) {
            syslog(LOG_ERR, "Invalid character.");
            return MPK_FAILURE;
        }
        barray[i] = hex2byte(hexstr);
        i++;
        hexstr += 2;
    }
    if (i != blen) {
        syslog(LOG_ERR, "Invalid file hash.");
        return MPK_FAILURE;
    }

    return MPK_SUCCESS;
}


void write_hexstr(char *hexstr, unsigned char barray[], int blen)
{
    int i;
    char *dst = hexstr;
    unsigned char *byte = barray;

    for (i = 0; i < blen; i++) {
        byte2hex(dst, *byte);
        dst += 2;
        byte++;
    }
}
