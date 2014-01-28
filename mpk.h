/**
 * @file mpk.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief Basic high level user api of libmpk.
 */
#ifndef _MPK_H
#define _MPK_H

#include "err.h"

/**
 * @brief mpk_init initialized the library (e.g. logging). It has to be called
 *          before using any API of libmpk.
 * @return MPK_SUCCESS or MPK_FAILURE
 */
mpk_ret_t mpk_init();

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
mpk_ret_t mpk_create(const char *src_dir, const char *dst_dir);

/**
 * @brief run_install simply runs installation of a single package without
 *          considering any dependency.
 * @param filename the mpk to install
 * @return MPK_SUCCESS or MPK_FAILURE
 */
mpk_ret_t mpk_install(const char *filename);

/**
 * @brief mpk_uninstall removes a package without respecting dependencies
 * @param package
 * @return
 */
mpk_ret_t mpk_uninstall(const char *name);

#endif /* _MPK_H */
