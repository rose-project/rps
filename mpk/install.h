/**
 * @file install.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief Package install and uninstall handling.
 */
#ifndef _MPK_INSTALL_H
#define _MPK_INSTALL_H

#include "package.h"

/**
 * @brief Package installation
 * @param pkg Packackage info of the package to install.
 * @param pkg_path Location to which the package has been unpacked
 * @param prefix The root directory to use
 * @return
 */
int mpk_install_doinstall(struct mpk_pkginfo *pkg, const char *pkg_path,
    const char *prefix);

#endif /* _MPK_INSTALL_H */
