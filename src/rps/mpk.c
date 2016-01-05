/**
 * @file mpk.c
 */
#define _POSIX_C_SOURCE 200809L
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <syslog.h>
#include <dirent.h>
#include <linux/limits.h>
#include "rps/defines.h"
#include "rps/pkginfo.h"
#include "rps/manifest.h"
#include "filehandling.h"
#include "package.h"
#include "install.h"
#include "rps/mpk.h"

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







    char *outdir_name, *package_fname;

    if (!package_file || !dst_dir)
        return MPK_FAILURE;

    if (!(package_fname = mpk_filehandling_basename(package_file)))
        return MPK_FAILURE;

    if (!(outdir_name = malloc(strlen(dst_dir) + 1 + strlen(package_fname) + 1))) {
        free(package_fname);
        return MPK_FAILURE;
    }

    sprintf(outdir_name, "%s/%s", dst_dir, package_fname);
    free(package_fname);
    package_fname = NULL;

    if (mpk_package_unpackmpk(package_file, outdir_name) != MPK_SUCCESS) {
        syslog(LOG_ERR, "mpk_package_unpackmpk() failed");
        free(outdir_name);
        return MPK_FAILURE;
    }

    free(outdir_name);

    return MPK_SUCCESS;
}

int mpk_install(const char *fpath, const char *prefix)
{
    int len;

    if (!fpath)
        return MPK_FAILURE;

    if (!prefix)
        prefix = "";

    /* unpack */

    char *package_fname;
    if (!(package_fname = mpk_filehandling_basename(fpath)))
        return MPK_FAILURE;

    len = strlen(prefix) + strlen("/usr/packages/") + strlen(package_fname) + 1;
    char *outdir;
    if (!(outdir = malloc(len))) {
        free(package_fname);
        return MPK_FAILURE;
    }
    snprintf(outdir, len, "%s/usr/packages/%s", prefix, package_fname);
    free(package_fname);
    package_fname = NULL;

    if (mpk_package_unpackmpk(fpath, outdir) != MPK_SUCCESS) {
        free(outdir);
        return MPK_FAILURE;
    }

    /* read manifest */

    len = strlen(outdir) + strlen("/manifest.txt") + 1;
    char *manifest_fpath;
    if (!(manifest_fpath = malloc(len))) {
        mpk_filehandling_deletedir(outdir);
        free(outdir);
        return MPK_FAILURE;
    }
    snprintf(manifest_fpath, len, "%s/manifest.txt", outdir);

    struct mpk_pkginfo pkg;
    if ((mpk_pkginfo_init(&pkg)) != MPK_SUCCESS) {
        free(manifest_fpath);
        mpk_filehandling_deletedir(outdir);
        free(outdir);
        return MPK_FAILURE;
    }
    if ((mpk_manifest_read(&pkg, manifest_fpath)) != MPK_SUCCESS) {
        free(manifest_fpath);
        mpk_pkginfo_clean(&pkg);
        mpk_filehandling_deletedir(outdir);
        free(outdir);
        return MPK_FAILURE;
    }
    free(manifest_fpath);
    manifest_fpath = NULL;

    /* install */

    if (mpk_install_doinstall(&pkg, outdir, prefix) != MPK_SUCCESS) {
        mpk_filehandling_deletedir(outdir);
        mpk_pkginfo_clean(&pkg);
        mpk_filehandling_deletedir(outdir);
        free(outdir);
        return MPK_FAILURE;
    }

    mpk_pkginfo_clean(&pkg);
    free(outdir);

    return MPK_SUCCESS;
}

int mpk_uninstall(const char *name)
{
    /* TODO */

    /* get packageinfo from database/cache */

    /* run uninstaller */

    /* remove packageinfo from database and cache */

    /* add package to the 'removed' list to report to backend */

    /* delete package metedata from local cache */

    return MPK_SUCCESS;
}

int mpk_upgrade(const char *fpath, const char *prefix)
{

}
