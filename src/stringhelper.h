/**
 * @file stringhelper.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief some string handling functions
 */
#ifndef _STRINGHELPER_H
#define _STRINGHELPER_H

#include "err.h"

void byte2hex(char *dst, unsigned char byte);

int hex2byte(const char *str);

/**
 * @brief read_hexstr
 * @param barray bytearray to store the result into
 * @param blen size of the bytearray
 * @param hexstr the null terminated source string
 * @return MPK_SUCCESS or MPK_FAILURE
 */
mpk_ret_t read_hexstr(unsigned char barray[], int blen, char *hexstr);

void write_hexstr(char *hexstr, unsigned char barray[], int blen);

#endif /* _STRING_H */
