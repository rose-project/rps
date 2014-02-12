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
        if (item->pkgref) {
            if (item->pkgref->name)
                free(item->pkgref->name);
            free(item->pkgref);
            free(item);
        }
    }
}

mpk_ret_t mpk_pkgreflist_add(struct mpk_pkgreflist *list,
    struct mpk_pkgref *pkgref)
{
    struct mpk_pkgreflist_item *item
        = malloc(sizeof(struct mpk_pkgreflist_item));
    if (!item)
        return MPK_FAILURE;

    item->pkgref = pkgref;

    LIST_INSERT_HEAD(list, item, items);

    return MPK_SUCCESS;
}


mpk_ret_t mpk_pkgreflist_remove(struct mpk_pkgreflist *list,
    struct mpk_pkgreflist_item *item)
{
    LIST_REMOVE(item, items);
}
