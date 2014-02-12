/**
 * @file pkgref.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief API providing a package referencing object.
 */
#ifndef _PKGREF_H
#define _PKGREF_H

#include <sys/queue.h>
#include "err.h"
#include "version.h"

/**
 * @brief The mpk_pkgref struct containts everything to uniquely idetify a
 * packge.
 */
struct mpk_pkgref {
    char *name;
    struct mpk_version ver;
    enum MPK_VERSION_OPERATOR op;
};

struct mpk_pkgreflist_item {
    struct mpk_pkgref *pkgref;
    LIST_ENTRY(mpk_pkgreflist_item) items;
};

LIST_HEAD(mpk_pkgreflist, mpk_pkgreflist_item);

mpk_ret_t mpk_pkgreflist_init(struct mpk_pkgreflist *list);

void mpk_pkgreflist_delete(struct mpk_pkgreflist *list);

mpk_ret_t mpk_pkgreflist_add(struct mpk_pkgreflist *list,
    struct mpk_pkgref *pkgref);

mpk_ret_t mpk_pkgreflist_remove(struct mpk_pkgreflist *list,
    struct mpk_pkgreflist_item *item);

#endif /* _PKGREF_H */
