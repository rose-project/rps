/**
 * @file package.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief Creating and unpacking mpk package files.
 */
#ifndef _PACKAGE_H
#define _PACKAGE_H
#include "pkginfo.h"
#include "err.h"

mpk_ret_t mpk_package_create(struct mpk_pkginfo *pkg, const char *filename,
    const char *srcdir);

#endif /* _PACKAGE_H */

