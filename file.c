/**
 * @file file.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <stdlib.h>
#include "file.h"

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
