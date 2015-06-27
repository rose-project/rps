/**
 * @file pkgref.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief API providing a package referencing object.
 */
#ifndef _PKGREF_H
#define _PKGREF_H

#include <sys/queue.h>
#include <mpk/version.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The mpk_pkgref struct containts everything to uniquely idetify a
 * packge.
 */
struct mpk_pkgref {
    char *name;
    struct mpk_version ver;
    enum MPK_VERSION_OPERATOR op; /* TODO: remove operator */
};

/**
 * @brief Initializes an empty pkgref object.
 * @param pkg The object ti initialize.
 */
void mpk_pkgref_initempty(struct mpk_pkgref *pkg);

/**
 * @brief Allocated memory for a pkgref object and the contents of name and version arguments
 * @param pkg Used to return a pointer to the object.
 * @param name The package name.
 * @param v The package version.
 * @return MPK_SUCCESS or MPK_FAIL
 */
int mpk_pkgref_create(struct mpk_pkgref **pkg, const char *name, struct mpk_version *v);

/**
 * @brief Free all memory of the pkgref object.
 * @param pkg The object to delete.
 */
void mpk_pkgref_delete(struct mpk_pkgref *pkg);

/**
 * @brief A list item of the pkgref list.
 */
struct mpk_pkgreflist_item {
    struct mpk_pkgref *pkgref; /**< The payload. */
    LIST_ENTRY(mpk_pkgreflist_item) items; /**< Linked list metasata */
};

/**
 * @brief Definition of a pkgref list head element.
 */
LIST_HEAD(mpk_pkgreflist, mpk_pkgreflist_item);

int mpk_pkgreflist_init(struct mpk_pkgreflist *list);

void mpk_pkgreflist_empty(struct mpk_pkgreflist *list);

int mpk_pkgreflist_add(struct mpk_pkgreflist *list,
    struct mpk_pkgref *pkgref);

int mpk_pkgreflist_addend(struct mpk_pkgreflist *list,
    struct mpk_pkgref *pkgref);

int mpk_pkgreflist_remove(struct mpk_pkgreflist_item *item);

int mpk_pkgreflist_print(FILE *f, struct mpk_pkgreflist *list);

#ifdef __cplusplus
}
#endif

#endif /* _PKGREF_H */
