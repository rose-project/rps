/**
 * @file package.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief Creating and unpacking mpk package files.
 */
#ifndef _PACKAGE_H
#define _PACKAGE_H
#include "stdint.h"
#include "pkginfo.h"
#include "version.h"
#include "err.h"

#define MPK_PACKAGE_SIGNATURE_LEN 256 /* 2048 bit signature */
#define MPK_PACKAGE_CACHE_DIR "/tmp"

/**
 * @brief mpk_package_packmpk creates a mpk package from the packageinfo and
 * source data
 * @param pkg package info object of the package
 * @param srcdir directory of the source data
 * @return
 */
mpk_ret_t mpk_package_packmpk(struct mpk_pkginfo *pkg, const char *srcdir,
    const char *outdir);

mpk_ret_t mpk_package_unpackmpk(const char *package_file, const char *outdir);

/**
 * @brief mpk_package_verify checks if the signature of the package is correct
 * @param pkginf pkfinfo of the package to check
 * @param pkgdir directory of the extracted files
 * @param pubkey rsa public key
 * @return
 */
int mpk_package_verify(struct mpk_pkginfo *pkginf, const char *pkgdir,
    const char *pubkey);

#endif /* _PACKAGE_H */

