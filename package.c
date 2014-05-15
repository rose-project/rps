 /**
  * @file package.c
  * @author Josef Raschen <josef@raschen.org>
  */
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <libtar.h>
#include <bzlib.h>
#include <string.h>
#include <linux/limits.h>
#include "package.h"

#define CHUNKSIZE 512

/**
 * @brief mpk_package_binheader is the file header of a bmpk file
 */
struct mpk_package_binheader {
    uint8_t magic[4]; /* { 0x6d, 0x70, 0x6b, 0x2a } "mpk*" */
    uint8_t version; /* package format version */
    uint8_t style; /* raw, tar */
    uint8_t compression;
    uint8_t flags;
    uint64_t filesize;
    uint32_t data_offset; /* where data begins */
    uint32_t crc; /* crc32 of header */
    uint8_t hash[32]; /* sha256 hash of the package */
};

mpk_ret_t mpk_package_bundlempk(struct mpk_pkginfo *pkg, const char *srcdir,
    const char *outdir)
{
    TAR *tar;
    BZFILE *bz2;
    FILE *tbz2_file;
    int tar_fd;
    int bzerr;
    char src[PATH_MAX + 1];
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

    for (file = pkg->files.lh_first; file; file = file->items.le_next) {
        sprintf(src, "%s/%s", srcdir, file->name);
        if (tar_append_tree(tar, src, file->name) != 0)
            goto err2;
    }

    sprintf(src, "%s/manifest.txt", srcdir);
    if (tar_append_file(tar, src, "manifest.txt") != 0)
        goto err2;

    tar_close(tar);


    /* compress using bz2 */

    sprintf(tbz2_fpath, "%s/%s.mpk", outdir, pkg->name);
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
