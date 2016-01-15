/**
 * @file file.c
 */
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <syslog.h>
#include "stringhelper.h"
#include "rps/defines.h"
#include "rps/file.h"


/* has to be in sync with enum MPK_FILE_TYPE */
const char *file_type_str[] = {
    "r",
    "x",
    "w",
    "s",
    "d"
};

struct mpk_file *mpk_file_create()
{
    struct mpk_file *f = (mpk_file*)malloc(sizeof(struct mpk_file));
    if (!f) {
        return NULL;
    }

    f->name = NULL;
    memset(f->hash, 0, sizeof(f->hash));
    f->hash_is_set = false;
    f->type = MPK_FILE_TYPE_UNDEFINED;
    f->target = NULL;

    return f;
}

void mpk_file_delete(struct mpk_file **file)
{
    if (*file) {
        if ((*file)->name)
            free((*file)->name);
        if ((*file)->target)
            free((*file)->target);
        free(*file);
        *file = NULL;
    }
}

int mpk_file_hash_serialize(char *str, struct mpk_file *file)
{
    if (!str)
        return MPK_FAILURE;

    write_hexstr(str, file->hash, MPK_FILEHASH_SIZE);
    str[MPK_FILEHASH_SIZE * 2] = 0;

    return MPK_SUCCESS;
}

int mpk_file_calchash(struct mpk_file *file, const char *basedir)
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
        if (!(filename = (char*)malloc(len))) {
            return MPK_FAILURE;
        }
        snprintf(filename, len, "%s/%s", basedir, file->name);
    } else {
        len = strlen(file->name) + 1;
        if (!(filename = (char*)malloc(strlen(file->name) + 1))) {
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
    file->hash_is_set = true;
    return MPK_SUCCESS;
}

int mpk_filelist_init(struct mpk_filelist *list)
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
        if (item->target)
            free(item->target);
        free(item);
    }
}

int mpk_filelist_add(struct mpk_filelist *list, struct mpk_file *file)
{
    if (!list || !file) {
        syslog(LOG_ERR, "invalid argument(s)");
        return MPK_FAILURE;
    }

    LIST_INSERT_HEAD(list, file, items);

    return MPK_SUCCESS;
}

int mpk_filelist_addend(struct mpk_filelist *list, struct mpk_file *file)
{
    if (!list || !file) {
        syslog(LOG_ERR, "invalid argument(s)");
        return MPK_FAILURE;
    }

    struct mpk_file *it, *last = NULL;
    for (it = list->lh_first; it; it = it->items.le_next) {
        last = it;
    }

    if (last)
        LIST_INSERT_AFTER(last, file, items);
    else
        LIST_INSERT_HEAD(list, file, items);

    return MPK_SUCCESS;
}

const char *mpk_file_type_str(enum MPK_FILE_TYPE type)
{
    if (type == MPK_FILE_TYPE_UNDEFINED || type >= MPK_FILE_TYPE_CNT)
        return NULL;

    return file_type_str[type];
}


namespace RPS {

File::File()
{

}

File::~File()
{

}

}
