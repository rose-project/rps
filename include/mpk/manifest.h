/**
 * @file manifest.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief API for parsing and writing of manifest files.
 */
#ifndef _MANIFEST_H
#define _MANIFEST_H

#include <mpk/pkginfo.h>

#ifdef __cplusplus
extern "C" {
#endif

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
int mpk_manifest_read(struct mpk_pkginfo *pkginfo, const char *filename);

/**
 * @brief mpk_manifest_write Writes the contents of the manifest object to a
 * file.
 *
 * @param filename
 * @param pkg
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_manifest_write(const char *filename, struct mpk_pkginfo *pkg);

#ifdef __cplusplus
}
#endif

#endif /* _MANIFEST_H */
