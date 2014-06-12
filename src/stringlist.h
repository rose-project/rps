/**
 * @file stringlist.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief simple API for strings
 */
#ifndef _STRINGLIST_H
#define _STRINGLIST_H

#include <sys/queue.h>
#include "err.h"

struct mpk_stringlist_item {
    char *str;
    LIST_ENTRY(mpk_stringlist_item) items;
};

LIST_HEAD(mpk_stringlist, mpk_stringlist_item);

mpk_ret_t mpk_stringlist_init(struct mpk_stringlist *list);

void mpk_stringlist_delete(struct mpk_stringlist *list);

mpk_ret_t mpk_stringlist_add(struct mpk_stringlist *list, const char *str);

mpk_ret_t mpk_stringlist_addend(struct mpk_stringlist *list, const char *str);

void mpk_stringlist_remove(struct mpk_stringlist *list,
    struct mpk_stringlist_item *item);

#endif /* _STRINGLIST_H */
