/**
 * @file mpk.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <sys/stat.h>
#include <stdio.h>
#include <syslog.h>
#include "defines.h"
#include "config.h"
#include "pkginfo.h"
#include "manifest.h"
#include "package.h"
#include "mpk.h"

#define MPK_CONFIG "/etc/libmpk.conf" /* default configuration file */

mpk_ret_t mpk_init()
{
    /* initialize logging */
    openlog("libmpk", LOG_PID|LOG_CONS, LOG_USER);

    /* read the config file */
    if (mpk_config_init(MPK_CONFIG) != MPK_SUCCESS) {
        return MPK_FAILURE;
    }

    syslog(LOG_INFO, "Initialized libmpk.");

    return MPK_SUCCESS;
}

void mpk_deinit()
{
    mpk_config_destroy();

    /* finish logging */
    closelog();
}

mpk_ret_t mpk_create(const char *src_dir, const char *dst_dir, const char *pkey)
{
    struct mpk_pkginfo pkg;
    char manifest_tmpl_fpath[MPK_PATH_MAX];
    char manifest_fpath[MPK_PATH_MAX];
    struct stat sb;

    sprintf(manifest_tmpl_fpath, "%s/manifest.txt.tmpl", src_dir);

    if (stat(manifest_tmpl_fpath, &sb) == -1) {
        syslog(LOG_ERR, "cannot find manifest template: %s",
            manifest_tmpl_fpath);
        return MPK_FAILURE;
    }

    if ((mpk_pkginfo_init(&pkg)) != MPK_SUCCESS) {
        syslog(LOG_ERR, "mpk_pkginfo_create() failed");
        return MPK_FAILURE;
    }

    if (mpk_manifest_read(&pkg, manifest_tmpl_fpath) != MPK_SUCCESS) {
        syslog(LOG_ERR, "mpk_manifest_read() failed");
        mpk_pkginfo_init(&pkg);
        return MPK_FAILURE;
    }

    if (mpk_pkginfo_calcfilehashes(&pkg, src_dir)
            != MPK_SUCCESS) {
        syslog(LOG_ERR, "mpk_pkginfo_calcfilehashes()) failed");
        return MPK_FAILURE;
    }

    if (mpk_pkginfo_sign(&pkg, pkey) != MPK_SUCCESS) {
        syslog(LOG_ERR, "mpk_pkginfo_sign() failed");
        return MPK_FAILURE;
    }

    sprintf(manifest_fpath, "%s/manifest.txt", src_dir);
    if (mpk_manifest_write(manifest_fpath, &pkg) != MPK_SUCCESS) {
        syslog(LOG_ERR, "mpk_manifest_write() failed");
        return MPK_FAILURE;
    }

    if (mpk_package_packmpk(&pkg, src_dir, dst_dir)) {
        syslog(LOG_ERR, "mpk_archive_bundlefiles() failed");
        return MPK_FAILURE;
    }

    mpk_pkginfo_delete(&pkg);

    return MPK_SUCCESS;
}


mpk_ret_t mpk_install(const char *filename)
{
    /* unpack */
    mpk_package_unpackmpk(filename, "/tmp");

    /* verify signature */

    /* run installation */

    return MPK_SUCCESS;
}


mpk_ret_t mpk_uninstall(const char *name)
{
    /* TODO */

    /* run uninstaller */

    /* delete package metedata from local cache */

    return MPK_SUCCESS;
}
