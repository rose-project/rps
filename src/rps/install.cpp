/**
 * @file install.c
 */
#define _GNU_SOURCE
#include <linux/limits.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "rps/defines.h"
#include "rps/pkginfo.h"
#include "filehandling.h"
#include "install.h"


int mpk_install_doinstall(struct mpk_pkginfo *pkg, const char *pkg_path,
    const char *prefix)
{
    if (!pkg || !pkg_path)
        return MPK_FAILURE;

    /* create directories */
    struct mpk_file *f;
    char *path = NULL, *path_n = NULL;
    for (f = pkg->data.lh_first; f; f = f->items.le_next) {
        if (f->type == MPK_FILE_TYPE_DIR) {
            if (!(path_n = (char *)realloc(path,
                    strlen(prefix) + 1 + strlen(f->name) + 1))) {
                if (path)
                    free(path);
                /* TODO: delete ceated paths */
                return MPK_FAILURE;
            }
            path = path_n;
            sprintf(path, "%s/%s", prefix, f->name);
            if (mpk_filehandling_createdir(path, 0770) != MPK_SUCCESS) {
                if (path)
                    free(path);
                /* TODO: delete ceated paths */
                return MPK_FAILURE;
            }
        }
    }
    if (path)
        free(path);
    path = NULL;
    path_n = NULL;


    /* create symlinks for content from data */
    /* TODO: clean up symlinks in case of failure */

    char *link_dest = NULL, *link_name = NULL;
    for (f = pkg->data.lh_first; f; f = f->items.le_next) {
        if (f->type == MPK_FILE_TYPE_DIR)
            continue;

        int len = strlen("/usr/packages/") + strlen(pkg->name)
            + strlen("/data/") + strlen(f->name) + 1;
        if (len > PATH_MAX)
            return MPK_FAILURE;
        char *p;
        if (!(p = (char *)realloc(link_dest, len))) {
            if (link_dest) {
                free(link_dest);
                return MPK_FAILURE;
            }
        }
        link_dest = p;
        snprintf(link_dest, len, "/usr/packages/%s/data/%s", pkg->name,
            f->name);

        len = strlen(prefix) + strlen("/") + strlen(f->name) + 1;
        if (len > PATH_MAX) {
            free(link_dest);
            return MPK_FAILURE;
        }
        if (!(p = (char *)realloc(link_name, len))) {
            if (link_name) {
                free(link_name);
                free(link_dest);
                return MPK_FAILURE;
            }
        }
        link_name = p;
        snprintf(link_name, len, "%s/%s", prefix, f->name);

        if (symlink(link_dest, link_name) != 0) {
                free(link_name);
                free(link_dest);
                return MPK_FAILURE;
        }
    }
    if (link_name)
        free(link_name);
    link_name = NULL;
    if (link_dest)
        free(link_dest);
    link_dest = NULL;


    /* create package symlink */
    /* TODO: symlink target does not need to be absolute */

    int len = strlen(prefix) + strlen("/usr/packages/") + strlen(pkg->name) + 1;
    char *package_link_name;
    if (!(package_link_name = (char *)malloc(len)))
        return MPK_FAILURE;
    snprintf(package_link_name, len, "%s/usr/packages/%s", prefix, pkg->name);

    len = strlen("/usr/packages/") + strlen(pkg->name) + 1;
    char *package_link_dest;
    if (!(package_link_dest = (char *)malloc(len))) {
        free(package_link_name);
        return MPK_FAILURE;
    }
    snprintf(package_link_dest, len, "/usr/packages/%s", pkg->name);

    if (symlink(package_link_dest, package_link_name) != 0) {
        free(package_link_dest);
        free(package_link_name);
        return MPK_FAILURE;
    }
    free(package_link_dest);
    free(package_link_name);

    return MPK_SUCCESS;
}
