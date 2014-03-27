/**
 * @file file.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <syslog.h>
#include "err.h"
#include "file.h"


struct mpk_file *mpk_file_create()
{
    struct mpk_file *f = malloc(sizeof(struct mpk_file));
    if (!f) {
        return NULL;
    }

    f->name = NULL;
    memset(f->hash, 0, sizeof(f->hash));

    return f;
}

void mpk_file_delete(struct mpk_file **file)
{
    if (*file) {
        if ((*file)->name) {
            free((*file)->name);
        }
        free(*file);
        *file = NULL;
    }
}

mpk_ret_t mpk_file_calchash(struct mpk_file *file, const char *basedir)
{
    char *filename;
    int len;
    int fd;
    SHA256_CTX ctx;
    int size;
    unsigned char buffer[256];

    if (!file || !file->name) {
        return MPK_FAILURE;
    }

    if (basedir) {
        len = strlen(file->name) + 1 + strlen(basedir) + 1;
        if (!(filename = malloc(len))) {
            return MPK_FAILURE;
        }
        snprintf(filename, len, "%s/%s", basedir, file->name);
    } else {
        len = strlen(file->name) + 1;
        if (!(filename = malloc(strlen(file->name) + 1))) {
            return MPK_FAILURE;
        }
        strncpy(filename, file->name, len);
    }

    if ((fd = open(filename, O_RDONLY)) == -1) {
        free(filename);
        return MPK_FAILURE;
    }

    if (SHA256_Init(&ctx) == 0) {
        close(fd);
        free(filename);
        return MPK_FAILURE;
    }

    while ((size = read(fd, buffer, 256)) > 0) {
        if (SHA256_Update(&ctx, buffer, size) == 0) {
            close(fd);
            free(filename);
            return MPK_FAILURE;
        }
    }
    if (size < 0) {
        close(fd);
        free(filename);
        return MPK_FAILURE;
    }

    if (SHA256_Final(file->hash, &ctx) != 1) {
        close(fd);
        free(filename);
        return MPK_FAILURE;
    }

    close(fd);
    return MPK_SUCCESS;
}

mpk_ret_t mpk_filelist_init(struct mpk_filelist *list)
{
    LIST_INIT(list);

    return MPK_SUCCESS;
}

void mpk_filelist_delete(struct mpk_filelist *list)
{
    struct mpk_file *item;
    while (!LIST_EMPTY(list)) {
        item = LIST_FIRST(list);
        LIST_REMOVE(item, items);
        if (item->name)
            free(item->name);
        free(item);
    }
}

mpk_ret_t mpk_filelist_add(struct mpk_filelist *list, struct mpk_file *file)
{
    if (!list || !file) {
        syslog(LOG_ERR, "invalid argument(s)");
        return MPK_FAILURE;
    }

    LIST_INSERT_HEAD(list, file, items);

    return MPK_SUCCESS;
}
