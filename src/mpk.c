/**
 * @file mpk.c
 * @author Josef Raschen <josef@raschen.org>
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
#include <linux/limits.h>
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
    char *outdir_name, *package_name;

    if (!package_file || !dst_dir)
        return MPK_FAILURE;

    if (!(package_name = mpk_package_name_from_fpath(package_file)))
        return MPK_FAILURE;

    if (!(outdir_name = malloc(strlen(dst_dir) + 1 + strlen(package_name) + 1))) {
        free(package_name);
        return MPK_FAILURE;
    }

    sprintf(outdir_name, "%s/%s", dst_dir, package_name);
    free(package_name);
    package_name = NULL;

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
    char *package_name, *outdir_name, *manifest_fname;
    struct mpk_pkginfo pkg;
    char tmp_path1[PATH_MAX + 1];
    char tmp_path2[PATH_MAX + 1];
//    const char *filename;
//    int filename_len;
//    const char *pkgname;

    if (!fpath || *prefix || strnlen(fpath, PATH_MAX) == PATH_MAX
            || strnlen(prefix, PATH_MAX) == PATH_MAX) {
        syslog(LOG_ERR, "%s: illegal arguments", __func__);
        return MPK_FAILURE;
    }

    if (!(package_name = mpk_package_name_from_fpath(fpath)))
        return MPK_FAILURE;

    if (!(outdir_name = malloc(strlen(prefix) + 1 + strlen(MPK_PACKAGE_STORE)
            + 1 + strlen(package_name) + 1))) {
        free(package_name);
        return MPK_FAILURE;
    }
    sprintf(outdir_name, "%s/%s/%s", prefix, MPK_PACKAGE_STORE, package_name);

    if (mpk_package_unpackmpk(fpath, outdir_name)
            != MPK_SUCCESS) {
        syslog(LOG_ERR, "mpk_package_unpackmpk() failed");
        free(package_name);
        free(outdir_name);
        return MPK_FAILURE;
    }

    free(package_name);

    int len = strlen(outdir_name) * strlen("/manifest.txt") + 1;
    if (!(manifest_fname = malloc(len))) {
        mpk_filehandling_deletedir(outdir_name);
        free(outdir_name);
        return MPK_FAILURE;
    }
    snprintf(manifest_fname, len, "%s/manifest.txt", outdir_name);

    if (mpk_manifest_read(&pkg, manifest_fname) != MPK_SUCCESS) {
        syslog(LOG_ERR, "mpk_manifest_read() failed");
        mpk_filehandling_deletedir(outdir_name);
        free(outdir_name);
        free(manifest_fname);
        return MPK_FAILURE;
    }

    sprintf(tmp_path1, "%s/%s", MPK_PACKAGE_STORE, pkg.name);
    if (symlink(tmp_path1, outdir_name) != 0) {
        syslog(LOG_ERR, "could not create symlink %s -> %s", tmp_path1,
            outdir_name);
        mpk_filehandling_deletedir(outdir_name);
        free(outdir_name);
        free(manifest_fname);
        return MPK_FAILURE;
    }

    struct mpk_file *f;
    for (f = pkg.files.lh_first; f; f = f->items.le_next) {
        struct stat st;
        if (access(f->name, R_OK|W_OK) == 0) {
            /* file already exists */
            continue;
        }

        sprintf(tmp_path1, "%s/%s", prefix, f->name);
        sprintf(tmp_path2, "%s/%s/%s", MPK_PACKAGE_STORE, pkg.name, f->name);
        if (symlink(tmp_path1, tmp_path2) != 0) {
            mpk_filehandling_deletedir(outdir_name);
            free(outdir_name);
            free(manifest_fname);
            return MPK_FAILURE;
        }
    }

    return MPK_SUCCESS;


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
