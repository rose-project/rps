/**
 * @file package.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief Creating, unpacking and signing and verification of mpk archive files.
 */
#ifndef _PACKAGE_H
#define _PACKAGE_H

#include "err.h"

mpk_ret_t mpk_package_create(const char *filename, const char *srcdir);

#endif /* _PACKAGE_H */

