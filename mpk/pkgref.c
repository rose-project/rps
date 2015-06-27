/**
 * @file pkgref.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <stdlib.h>
#include <string.h>
#include "mpk/defines.h"
#include "mpk/pkgref.h"

void mpk_pkgref_initempty(struct mpk_pkgref *pkg)
{
    if (!pkg)
        return;

    pkg->name = NULL;
    pkg->ver = MPK_VERSION_DEFAULT;
    pkg->op = MPK_VERSION_OPERATOR_UNKNOWN;
}

int mpk_pkgref_create(struct mpk_pkgref **pkg, const char *name, struct mpk_version *v)
{
    struct mpk_pkgref *p;

    if (!name || !v)
        return MPK_FAILURE;

    if (!(p = malloc(sizeof(struct mpk_pkgref))))
        return MPK_FAILURE;

    if (!(p->name = malloc(strlen(name) + 1))) {
        free(p);
        return MPK_FAILURE;
    }

    strcpy(p->name, name);
    p->ver = *v;

    *pkg = p;

    return MPK_SUCCESS;
}

void mpk_pkgref_delete(struct mpk_pkgref *pkg)
{
    if (!pkg)
        return;

    if (!pkg->name) {
        free(pkg);
        return;
    }

    free(pkg->name);
    free(pkg);
}

int mpk_pkgreflist_init(struct mpk_pkgreflist *list)
{
    LIST_INIT(list);

    return MPK_SUCCESS;
}

void mpk_pkgreflist_empty(struct mpk_pkgreflist *list)
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

int mpk_pkgreflist_add(struct mpk_pkgreflist *list,
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

int mpk_pkgreflist_addend(struct mpk_pkgreflist *list,
    struct mpk_pkgref *pkgref)
{
    struct mpk_pkgreflist_item *item
        = malloc(sizeof(struct mpk_pkgreflist_item));
    if (!item)
        return MPK_FAILURE;

    item->pkgref = pkgref;

    struct mpk_pkgreflist_item *it, *last = NULL;
    for (it = list->lh_first; it; it = it->items.le_next) {
        last = it;
    }

    if (last)
        LIST_INSERT_AFTER(last, item, items);
    else
        LIST_INSERT_HEAD(list, item, items);

    return MPK_SUCCESS;
}

int mpk_pkgreflist_remove(struct mpk_pkgreflist_item *item)
{
    LIST_REMOVE(item, items);

    return MPK_SUCCESS;
}

int mpk_pkgreflist_print(FILE *f, struct mpk_pkgreflist *list)
{
    struct mpk_pkgreflist_item *pkg;

    if (!f || !list) {
        return MPK_FAILURE;
    }

    /* TODO: There may sometimes be no Version defined. We need to handle this.
     */
    for (pkg = list->lh_first; pkg; pkg = pkg->items.le_next) {
        fprintf(f, "  - {name: %s, version", pkg->pkgref->name);
        mpk_version_print(f, &pkg->pkgref->ver);
        fprintf(f, ", op: ");
        mpk_version_operator_print(f, pkg->pkgref->op);
        fprintf(f, "}\n");
    }

    return MPK_SUCCESS;

}
