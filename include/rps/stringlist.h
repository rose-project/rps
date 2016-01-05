/**
 * @file stringlist.h
 * @brief simple API for strings
 */
#ifndef _STRINGLIST_H
#define _STRINGLIST_H

#include <sys/queue.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief A single element of the stringlist.
 */
struct mpk_stringlist_item {
    char *str; /**< A null terminated string. */
    LIST_ENTRY(mpk_stringlist_item) items; /**< linked list metadata */
};

LIST_HEAD(mpk_stringlist, mpk_stringlist_item);
    /**< stringlist head structure */

/**
 * @brief initialize an empty stringlist
 * @param list Pointer to the head object of the list.
 * @return MPK_SUCCESS
 */
int mpk_stringlist_init(struct mpk_stringlist *list);

/**
 * @brief Delete all contents of the list an free the elements memory.
 * @param list The head object of the list to delete.
 */
void mpk_stringlist_empty(struct mpk_stringlist *list);

/**
 * @brief Add an new string to the front of a list.
 * @param list List to add to.
 * @param str A null terminated string that will be copied to the new list
 * element.
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_stringlist_add(struct mpk_stringlist *list, const char *str);

/**
 * @brief Add an new string to the front of a list.
 * @param list List to add to.
 * @param str A null terminated string that will be copied to the new list
 * element.
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_stringlist_addend(struct mpk_stringlist *list, const char *str);

/**
 * @brief Remove an element from a list.
 * @param item Item to remove.
 */
void mpk_stringlist_remove(struct mpk_stringlist_item *item);

#ifdef __cplusplus
}
#endif

#endif /* _STRINGLIST_H */
