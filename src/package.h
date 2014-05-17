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

#define MPK_PACKAGE_CACHE_DIR "/tmp"

#define MPK_PACKAGE_STYLE_RAW = 0
#define MPK_PACKAGE_STYLE_TAR = 1

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
 * @brief mpk_package_bundlebmpk write out a bmpk package
 * @param pkg package info object of the package
 * @param srcdir directory of the source data
 * @return
 */
mpk_ret_t mpk_package_bundlebmpk(struct mpk_pkginfo *pkg, const char *srcdir);

#endif /* _PACKAGE_H */

