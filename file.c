/**
 * @file file.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <stdlib.h>
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
