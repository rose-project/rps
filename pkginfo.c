/**
 * @file pkginfo.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "err.h"
#include "stringhelper.h"
#include "pkginfo.h"

#define CHECK_AND_FREE(ptr) if (ptr) {  \
    free(ptr);                          \
    ptr = NULL;                         \
}                                       \

static const char *archname[] = {
    "generic",
    "x86",
    "x86_64",
    "PowerPC",
    "armv5",
    "armv6",
    "armv6hf",
    "armv7",
    "armv7hf",
    "armv8"
};

mpk_ret_t mpk_pkginfo_init(struct mpk_pkginfo *pkg)
{
    if (!pkg) {
        syslog(LOG_ERR, "parameter pkg is invalid");
        return MPK_FAILURE;
    }

    pkg->manifest = MPK_VERSION_DEFAULT;
    pkg->name = NULL;
    pkg->version = MPK_VERSION_DEFAULT;
    pkg->arch = MPK_PKGINFO_ARCH_UNKNOWN;
    mpk_stringlist_init(&pkg->regions);
    mpk_pkgreflist_init(&pkg->depends);
    mpk_pkgreflist_init(&pkg->conflicts);
    pkg->priority = 0;
    pkg->source = NULL;
    pkg->vendor = NULL;
    pkg->description = NULL;
    pkg->maintainer = NULL;
    pkg->license = NULL;
    mpk_filelist_init(&pkg->files);
    memset(pkg->signature, 0, MPK_PKGINFO_SIGNATURE_LEN);

    return MPK_SUCCESS;
}

void mpk_pkginfo_delete(struct mpk_pkginfo *pkg)
{
    if (!pkg) {
        syslog(LOG_WARNING, "mpk_pkginfo_deinit() called on NULL pointer");
        return;
    }

    CHECK_AND_FREE(pkg->name);
    mpk_stringlist_delete(&pkg->regions);
    mpk_pkgreflist_delete(&pkg->depends);
    mpk_pkgreflist_delete(&pkg->conflicts);
    CHECK_AND_FREE(pkg->source);
    CHECK_AND_FREE(pkg->vendor);
    CHECK_AND_FREE(pkg->description);
    CHECK_AND_FREE(pkg->maintainer);
    CHECK_AND_FREE(pkg->license);
    mpk_filelist_delete(&pkg->files);
}


mpk_ret_t mpk_pkginfo_arch_deserialize(enum MPK_PKGINFO_ARCH *arch, char *str)
{
    int i;

    for (i = 0; i < MPK_PKGINFO_ARCH_COUNT; i++) {
        if (!strcmp(str, archname[i])) {
            *arch = i;
            return MPK_SUCCESS;
        }
    }

    return MPK_FAILURE;
}


mpk_ret_t mpk_pkginfo_signature_deserialize(unsigned char signature[],
    char *src)
{
    return read_hexstr(signature,MPK_PKGINFO_SIGNATURE_LEN, src);
}
