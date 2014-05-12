/**
 * @file manifest.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief API for parsing and writing of manifest files.
 */
#ifndef _MANIFEST_H
#define _MANIFEST_H

#include "err.h"
#include "pkginfo.h"

#define MANIFEST_VERSION "1.0"

/**
 * @brief mpk_manifest_read reads manifest file (or template) into pkginfo
 *        object
 *
 * @param pkginfo the object to write to; this has to be empty without any
 *      preallocated data
 * @param filename the file to read from
 * @return MPK_SUCCESS or MPK_FAILURE
 */
mpk_ret_t mpk_manifest_read(struct mpk_pkginfo *pkginfo, const char *filename);

/**
 * @brief mpk_manifest_write Writes the contents of the manifest object to a
 * file.
 *
 * @param filename
 * @param pkg
 * @return MPK_SUCCESS or MPK_FAILURE
 */
mpk_ret_t mpk_manifest_write(const char *filename, struct mpk_pkginfo *pkg);

#endif /* _MANIFEST_H */
