/**
 * @file stringlist.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <stdlib.h>
#include "stringlist.h"

mpk_ret_t mpk_stringlist_init(struct mpk_stringlist *list)
{
    LIST_INIT(list);

    return MPK_SUCCESS;
}

void mpk_stringlist_delete(struct mpk_stringlist *list)
{
    struct mpk_stringlist_item *item;
    while (!LIST_EMPTY(list)) {
        item = LIST_FIRST(list);
        LIST_REMOVE(item, items);
        free(item->str);
        free(item);
    }
}
