/**
 * @file filehandling.c
 * @author Josef Raschen <josef@raschen.org>
 * @copyright 2014 Josef Raschen <josef@raschen.org>
 */
/** set feature set for nftw */
#define _GNU_SOURCE
#include <ftw.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <libgen.h>
#include <syslog.h>
#include <string.h>
#include <limits.h>
#include <errno.h>
#include "mpk/defines.h"
#include "filehandling.h"

/* TODO make thread safe */

static const char *src_fpath, *dst_fpath;
static int len_src_fpath, len_dst_fpath;

static int nftw_directory_delete(const char *fpath,
    const struct stat __attribute__((__unused__)) *sb,
    int tflag, struct FTW __attribute__((__unused__)) *ftwbuf)
{
    if (tflag == FTW_F) {
        printf("delete %s\n", fpath);
        if (unlink(fpath) != 0)
            return -1;
    } else if (tflag == FTW_D || tflag == FTW_DP) {
        printf("delete %s\n", fpath);
        if (rmdir(fpath) != 0)
            return -1;
    } else {
        printf("%s somesthing unexpected\n", fpath);
        return -1;
    }

    return 0;
}

int mpk_filehandling_deletedir(const char *name)
{
    /* delete directory and its contents using nftw */

    if (nftw(name, nftw_directory_delete, 20, FTW_DEPTH|FTW_PHYS) == -1) {
        return -1;
    }

    return 0;
}

static int nftw_directory_copy(const char *fpath,
    const struct stat __attribute__((__unused__)) *sb,
    int tflag, struct FTW __attribute__((__unused__)) *ftwbuf)
{
    char rel_fpath[PATH_MAX + 1];
    char abs_dst_fpath[PATH_MAX + 1];

    strncpy(rel_fpath, fpath + len_src_fpath, strlen(fpath) - len_src_fpath);
    sprintf(abs_dst_fpath, "%s/%s", dst_fpath, rel_fpath);
    printf("path: %s\n", abs_dst_fpath);

    if (tflag == FTW_F) {
        printf("copy %s to %s\n", fpath, abs_dst_fpath);
        if (mpk_filehandling_copyfile(abs_dst_fpath, fpath) != 0)
            return -1;
    } else if (tflag == FTW_D || tflag == FTW_DP) {
        printf("create direcory %s\n", abs_dst_fpath);
        if (mkdir(abs_dst_fpath, 0644) != 0)
            return -1;
    } else {
        printf("%s somesthing unexpected\n", fpath);
        return -1;
    }

    return 0;
}

int mpk_filehandling_copydir(const char *dst, const char *src)
{
    src_fpath = src;
    len_src_fpath = strlen(src);
    dst_fpath = dst;
    len_dst_fpath = strlen(dst);

    if (nftw(src, nftw_directory_copy, 20, FTW_PHYS) == -1) {
        return -1;
    }

    return 0;
}

#define FILE_COPY_CHUNKSIZE 512
int mpk_filehandling_copyfile(const char *dst, const char *src)
{
    int fd_src, fd_dst;
    unsigned char buf[FILE_COPY_CHUNKSIZE];
    int n_r;

    if ((fd_src = open(src, O_RDONLY)) < 0)
        return -1;
    if ((fd_dst = open(dst, O_WRONLY|O_CREAT, 0644)) < 0) {
        close(fd_src);
        return -1;
    }

    while ((n_r = read(fd_src, buf, FILE_COPY_CHUNKSIZE)) > 0) {
        int n_w;
        unsigned char *buf_ptr = buf;

        while (n_r > 0) {
            if ((n_w = write(fd_dst, buf, n_r)) < 0) {
                if (errno == EINTR)
                    continue; /* try again */
                close(fd_dst);
                unlink(dst);
                return -1;
            }
            n_r -= n_w;
            buf_ptr += n_w;
        }
    }
    if (n_r < 0) {
        close(fd_dst);
        unlink(dst);
        return -1;
    }

    close(fd_src);
    close(fd_dst);

    return 0;
}


int mpk_filehandling_createdir(const char *path)
{
    if (!path)
        return MPK_FAILURE;

    /* TODO */

    return MPK_FAILURE;
}


char *mpk_filehandling_basename(const char *fpath)
{
    /* we need to copy fpath as basename might try to change it */
    char *fpath_c = strdup(fpath);
    char *bname = basename(fpath_c);

    int len;
    if ((len = strlen(bname)) < 4) {
        free(fpath_c);
        return NULL;
    }

    /* cut the suffix */
    if (bname[len - 4] != '.') {
        free(fpath_c);
        return NULL;
    }
    bname[len - 4] = 0;

    /* copy the result */
    char *package_name;
    if (!(package_name = malloc(strlen(bname) + 1))) {
        free(fpath_c);
        return NULL;
    }
    strcpy(package_name, bname);
    free(fpath_c);
    return package_name;
}
