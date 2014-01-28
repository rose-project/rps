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

#endif /* _STRINGLIST_H */
