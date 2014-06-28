/**
 * @file mpk.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <sys/stat.h>
#include <stdio.h>
#include <syslog.h>
#include "mpk/defines.h"
#include "mpk/pkginfo.h"
#include "mpk/manifest.h"
#include "filehandling.h"
#include "package.h"
#include "mpk/mpk.h"

#define MPK_CONFIG "/etc/libmpk.conf" /* default configuration file */

int mpk_init()
{
    /* initialize logging */
    openlog("libmpk", LOG_PID|LOG_CONS, LOG_USER);

    syslog(LOG_INFO, "Initialized libmpk.");

    return MPK_SUCCESS;
}

void mpk_deinit()
{
    /* finish logging */
    closelog();
}

int mpk_create(const char *src_dir, const char *dst_dir, const char *pkey)
{
    struct mpk_pkginfo pkg;
    char manifest_tmpl_fpath[MPK_PATH_MAX];
    char manifest_fpath[MPK_PATH_MAX];
    struct stat sb;

    sprintf(manifest_tmpl_fpath, "%s/manifest-tmpl.json", src_dir);

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

    if (mpk_package_packmpk(&pkg, src_dir, dst_dir) != MPK_SUCCESS) {
        syslog(LOG_ERR, "mpk_package_packmpk() failed");
        return MPK_FAILURE;
    }

    mpk_pkginfo_clean(&pkg);

    return MPK_SUCCESS;
}

int mpk_unpack(const char *package_file, const char *dst_dir)
{
    if (mpk_package_unpackmpk(package_file, dst_dir) != MPK_SUCCESS) {
        syslog(LOG_ERR, "mpk_package_unpackmpk() failed");
        return MPK_FAILURE;
    }

    return MPK_SUCCESS;
}
