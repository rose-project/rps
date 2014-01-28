/**
 * @file pkgref.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <stdlib.h>
#include "pkgref.h"

mpk_ret_t mpk_pkgreflist_init(struct mpk_pkgreflist *list)
{
    LIST_INIT(list);

    return MPK_SUCCESS;
}

void mpk_pkgreflist_delete(struct mpk_pkgreflist *list)
{
    struct mpk_pkgreflist_item *item;
    while (!LIST_EMPTY(list)) {
        item = LIST_FIRST(list);
        LIST_REMOVE(item, items);
        if (item->pkgref.name)
            free(item->pkgref.name);
        free(item);
    }
}
