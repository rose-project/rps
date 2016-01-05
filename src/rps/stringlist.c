/**
 * @file stringlist.c
 */
#include <stdlib.h>
#include <string.h>
#include "rps/defines.h"
#include "rps/stringlist.h"

int mpk_stringlist_init(struct mpk_stringlist *list)
{
    LIST_INIT(list);

    return MPK_SUCCESS;
}

void mpk_stringlist_empty(struct mpk_stringlist *list)
{
    struct mpk_stringlist_item *item;
    while (!LIST_EMPTY(list)) {
        item = LIST_FIRST(list);
        LIST_REMOVE(item, items);
        free(item->str);
        free(item);
    }
}

int mpk_stringlist_add(struct mpk_stringlist *list, const char *str)
{
    struct mpk_stringlist_item *item
        = malloc(sizeof(struct mpk_stringlist_item));
    if (!item)
        return MPK_FAILURE;

    if (!(item->str = malloc(strlen(str) + 1))) {
        free(item);
        return MPK_FAILURE;
    }

    strcpy(item->str, str);

    LIST_INSERT_HEAD(list, item, items);

    return MPK_SUCCESS;
}

int mpk_stringlist_addend(struct mpk_stringlist *list, const char *str)
{
    struct mpk_stringlist_item *item
        = malloc(sizeof(struct mpk_stringlist_item));
    if (!item)
        return MPK_FAILURE;

    if (!(item->str = malloc(strlen(str) + 1))) {
        free(item);
        return MPK_FAILURE;
    }

    strcpy(item->str, str);

    struct mpk_stringlist_item *it, *last = NULL;
    for (it = list->lh_first; it; it = it->items.le_next) {
        last = it;
    }

    if (last)
        LIST_INSERT_AFTER(last, item, items);
    else
        LIST_INSERT_HEAD(list, item, items);

    return MPK_SUCCESS;
}

void mpk_stringlist_remove(struct mpk_stringlist_item *item)
{
    LIST_REMOVE(item, items);
}

