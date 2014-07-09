 /**
  * @file package.c
  * @author Josef Raschen <josef@raschen.org>
  */
#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <syslog.h>
#include <libgen.h>
#include <unistd.h>
#include <libtar.h>
#include <bzlib.h>
#include <linux/limits.h>
#include <mpk/defines.h>
#include "package.h"

/** The defaut size used for chunks of data used in for packing/unpacking and
 * signature generation generation
 */
#define CHUNKSIZE 512

#define PACKAGE_VERSION 1

#define PACKAGE_STYLE_RAW 0
#define PACKAGE_STYLE_TAR 1

#define PACKAGE_COMPRESSION_NONE 1
#define PACKAGE_COMPRESSION_BZ2 1

/**
 * @brief mpk_package_signature
 */
struct mpk_package_header {
    uint8_t magic[4]; /* { 0x6d, 0x70, 0x6b, 0x2a } "mpk*" */
    uint8_t fversion; /* package format version */
    uint8_t style; /* raw, tar */
    uint8_t compression;
    uint8_t flags;
    uint64_t package_size; /* size of the complete package including header */
    uint8_t package_signature[MPK_PACKAGE_SIGNATURE_LEN];
    uint8_t header_signature[MPK_PACKAGE_SIGNATURE_LEN];
};

int mpk_package_packmpk(struct mpk_pkginfo *pkg, const char *srcdir,
    const char *outdir)
{
    TAR *tar;
    BZFILE *bz2;
    FILE *tbz2_file;
    int tar_fd;
    int bzerr;
    char src[PATH_MAX + 1];
    char dst[PATH_MAX + 1];
    char tar_fpath[PATH_MAX + 1];
    char tbz2_fpath[PATH_MAX + 1];
    unsigned char buffer[CHUNKSIZE];
    int size;
    struct mpk_file *file;

    /* create tar */

    sprintf(tar_fpath, "/tmp/%s_files.tar", pkg->name);
    if (access(tar_fpath, F_OK) == 0)
        if (unlink(tar_fpath) != 0)
            goto err0;

    if (tar_open(&tar, tar_fpath, NULL, O_WRONLY|O_CREAT, 0644, 0) != 0)
        goto err0;

    for (file = pkg->tools.lh_first; file; file = file->items.le_next) {
        sprintf(src, "%s/tools/%s", srcdir, file->name);
        sprintf(dst, "tools/%s", file->name);
        if (tar_append_tree(tar, src, dst) != 0)
            goto err2;
    }

    for (file = pkg->files.lh_first; file; file = file->items.le_next) {
        sprintf(src, "%s/data/%s", srcdir, file->name);
        sprintf(dst, "data/%s", file->name);
        if (tar_append_tree(tar, src, dst) != 0)
            goto err2;
    }

    sprintf(src, "%s/manifest.txt", srcdir);
    if (tar_append_file(tar, src, "manifest.txt") != 0)
        goto err2;

    tar_close(tar);


    /* compress using bz2 */

    int version_str_len = mpk_version_serializedsize(&pkg->version);
    char *version_str;
    if (!(version_str = malloc(version_str_len + 1)))
        goto err2;
    if (mpk_version_serialize(version_str, NULL, version_str_len, &pkg->version)
            != MPK_SUCCESS) {
        free(version_str);
        goto err2;
    }
    version_str[version_str_len] = 0;

    sprintf(tbz2_fpath, "%s/%s-%s.mpk", outdir, pkg->name, version_str);
    free(version_str);
    printf("path:%s\n", tbz2_fpath);

    if ((tar_fd = open(tar_fpath, O_RDONLY)) == -1)
        goto err1;

    if ((tbz2_file = fopen(tbz2_fpath, "wb")) == NULL)
        goto err3;
    bz2 = BZ2_bzWriteOpen(&bzerr, tbz2_file, 9, 0, 30);
    if (bzerr != BZ_OK)
        goto err4;

    while ((size = read(tar_fd, buffer, CHUNKSIZE)) > 0)
        BZ2_bzWrite(&bzerr, bz2, buffer, size);
    BZ2_bzWriteClose(&bzerr, bz2, 0, NULL, NULL);
    fclose(tbz2_file);
    close(tar_fd);
    if (bzerr != BZ_OK || size < 0)
        goto err1;

    if (unlink(tar_fpath) != 0)
        goto err0;

    return MPK_SUCCESS;

err4:
    fclose(tbz2_file);
err3:
    close(tar_fd);
    goto err1;
err2:
    tar_close(tar);
err1:
    unlink(tar_fpath);
err0:
    return MPK_FAILURE;
}

int mpk_package_unpackmpk(const char *package_file, char *outdir)
{
    FILE *tbz2_file, *tar_file;
    const char *tar_fpath = "/tmp/mpk-temp.tar";
    BZFILE *bz2;
    int bzerr;
    unsigned char buf[CHUNKSIZE];
    size_t n;
    TAR *tar;

    if (!package_file || !outdir)
        return MPK_FAILURE;

    if (!(tar_file = fopen(tar_fpath, "w")))
        return MPK_FAILURE;

    if (!(tbz2_file = fopen(package_file, "r"))) {
        syslog(LOG_ERR, "could not open file: %s", package_file);
        fclose(tar_file);
        return MPK_FAILURE;
    }

    /* decompress bz2 */

    bz2 = BZ2_bzReadOpen(&bzerr, tbz2_file, 0, 0, NULL, 0);
    if (bzerr != BZ_OK) {
        fclose(tbz2_file);
        fclose(tar_file);
        return MPK_FAILURE;
    }
    while (1) {
        n = BZ2_bzRead(&bzerr, bz2, buf, CHUNKSIZE);
        if (bzerr != BZ_OK && bzerr != BZ_STREAM_END) {
            fclose(tar_file);
            unlink(tar_fpath);
            BZ2_bzReadClose(&bzerr, bz2);
            fclose(tbz2_file);
            return MPK_FAILURE;
        }

        if (fwrite(buf, 1, n, tar_file) != n) {
            fclose(tar_file);
            unlink(tar_fpath);
            BZ2_bzReadClose(&bzerr, bz2);
            fclose(tbz2_file);
            return MPK_FAILURE;
        }

        if (bzerr == BZ_STREAM_END)
            break;
    }

    BZ2_bzReadClose(&bzerr, bz2);
    fclose(tbz2_file);
    fclose(tar_file);

    /* create output directory */

    if (mkdir(outdir, 0700) != 0) {
        syslog(LOG_ERR, "mkdir failed: %s", strerror(errno));
        unlink(tar_fpath);
        return MPK_FAILURE;
    }

    /* unpack tar */

    if (tar_open(&tar, tar_fpath, NULL, O_RDONLY, 0644, 0) != 0) {
        rmdir(outdir);
        unlink(tar_fpath);
        return MPK_FAILURE;
    }

    if (tar_extract_all(tar, outdir) != 0) {
        syslog(LOG_ERR, "tar_extract_all() failed: %s", strerror(errno));
        tar_close(tar);
        rmdir(outdir);
        unlink(tar_fpath);
        return MPK_FAILURE;
    }
    tar_close(tar);
    if (unlink(tar_fpath) != 0)
        return MPK_FAILURE;

    return MPK_SUCCESS;
}

/* TODO: not very elegant, there should be a smarter solution */
char *mpk_package_name_from_fpath(const char *fpath)
{
     /* copy so we can hand over the string to basename() */
    char *fpath_c = strdup(fpath);

    char *tmp_str = basename(fpath_c);

    int len;
    if ((len = strlen(tmp_str)) < 4) {
        free(fpath_c);
        return NULL;
    }

    if (tmp_str[len - 4] != '.') {
        free(fpath_c);
        return NULL;
    }

    tmp_str[len - 4] = 0;

    char *package_name;
    if ((package_name = malloc(strlen(tmp_str) + 1))) {
        strcpy(package_name, tmp_str);
        free(fpath_c);
        return package_name;
    }

    free(fpath_c);
    return NULL;
}
