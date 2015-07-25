/**
 * @file mpk.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief Basic high level user api of libmpk.
 */
#ifndef _MPK_H
#define _MPK_H

#include <mpk/defines.h>
#include <mpk/manifest.h>
#include <mpk/pkginfo.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief mpk_init initializes the library (e.g. logging). It has to be called
 *          before using any API of libmpk.
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_init();

/**
 * @brief mpk_deinit should be called after using any API of libmpk
 */
void mpk_deinit();

/**
 * @brief mpk_create creates and signs a new package.
 * @param src_dir the source directory containing the manifest template
 * @param dst_dir the directory to put the rulting mpk into
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_create(const char *src_dir, const char *dst_dir,
    const char *pkey);

/**
 * @brief mpk_unpack unpacks an mpk file
 * @param package_file the file to unpack
 * @param dst_dir the directory to put the contents to
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_unpack(const char *package_file, const char *dst_dir);

/**
 * @brief run_install simply runs installation of a single package without
 * considering any dependency or rights check.
 * @param fpath the mpk to install
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_install(const char *fpath, const char *prefix);

/**
 * @brief mpk_uninstall removes a package without respecting dependencies
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_uninstall(const char *name);

/**
 * @brief Installs a package and removes an older version installed before.
 * @param fpath The mpk file
 * @param prefix The root directory to use
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_upgrade(const char *fpath, const char *prefix);

#ifdef __cplusplus
}
#endif

#endif /* _MPK_H */
