/**
 * @file mpk.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <sys/stat.h>
#include <stdio.h>
#include <syslog.h>
#include <mpk/mpk.h>
#include "defines.h"
#include "config.h"
#include "pkginfo.h"
#include "manifest.h"
#include "package.h"
#include "filehandling.h"

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

    if (mpk_package_packmpk(&pkg, src_dir, dst_dir) != MPK_SUCCESS) {
        syslog(LOG_ERR, "mpk_package_packmpk() failed");
        return MPK_FAILURE;
    }

    mpk_pkginfo_delete(&pkg);

    return MPK_SUCCESS;
}

int mpk_install(const char *fpath)
{
//    struct mpk_pkginfo pkg;
//    const char *filename;
//    int filename_len;
//    const char *pkgname;
//    const char *unpack_dir;

//    /* get package name from fpath */
//    filename = basename(fpath);
//    filename_len = strlen(filename) + 1;
//    if (len < 5)
//        return MPK_FAILURE;
//    if (!(pkgname = malloc(filename_len)))
//        return MPK_FAILURE;
//    strcpy(pkgname, filename);
//    pkgname[filename_len - 5] = 0; /* cut off the extension '.mpk' */

//    /* package extract dir */
//    if (!(unpack_dir = malloc(strlen("/tmp/") + strlen(pkgname) + 1))) {
//        free(pkgname);
//        return MPK_FAILURE;
//    }
//    sprintf(unpack_dir, "/tmp/%s", pkgname);

//    if (mpk_package_unpackmpk(fpath, unpack_dir) != MPK_SUCCESS) {
//        syslog(LOG_ERR, "mpk_package_unpackmpk() failed");
//        free(unpack_dir);
//        free(pkgname);
//        return MPK_FAILURE;
//    }

//    if (mpk_manifest_read(&pkg, "...") != MPK_SUCCESS) {
//        syslog(LOG_ERR, "mpk_manifest_read() failed");
//        mpk_filehandling_deletedir(unpack_dir);
//        free(unpack_dir);
//        free(pkgname);
//        return MPK_FAILURE;
//    }

//    if (mpk_package_verify(&pkg, "publickey") != MPK_SUCCESS) {
//        syslog(LOG_ERR, "mpk_package_verifysignature() failed");
//        mpk_pkginfo_delete(&pkg);
//        mpk_filehandling_deletedir(unpack_dir);
//        free(unpack_dir);
//        free(pkgname);
//        return MPK_FAILURE;
//    }

//    /* run preinstall script */

//    /* copy files to destination */
//    for (struct mpk_file *f = pkg.files.lh_first; f; f = f->items.le_next) {

//    }
//    if (mpk_filehandling_copydir())

//    /* run install script */

//    /* run cleanup script */

//    /* save manifest and uninstall script */

//    /* add package to list of installed packages in database */

//    /* add package to the 'installed' list to report to backend */

//    /* delete the rest from /tmp */
//    mpk_filehandling_deletedir(unpack_dir);

//    mpk_pkginfo_delete(&pkg);
//    free(unpack_dir);
//    free(pkgname);

//    return MPK_SUCCESS;
}


mpk_ret_t mpk_uninstall(const char *name)
{
    /* TODO */

    /* get packageinfo from database/cache */

    /* run uninstaller */

    /* remove packageinfo from database and cache */

    /* add package to the 'removed' list to report to backend */

    /* delete package metedata from local cache */

    return MPK_SUCCESS;
}
