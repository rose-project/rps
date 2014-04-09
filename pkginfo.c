/**
 * @file pkginfo.c
 * @author Josef Raschen <josef@raschen.org>
 */
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
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

mpk_ret_t mpk_pkginfo_calcfilehashes(struct mpk_pkginfo *pkginf,
    const char *pkgroot)
{
    struct mpk_file *file;

    for (file = pkginf->files.lh_first; file; file = file->items.le_next) {
            syslog(LOG_INFO, "file %s", file->name);
        if (mpk_file_calchash(file, pkgroot) != MPK_SUCCESS) {
            return MPK_FAILURE;
        }
    }

    return MPK_SUCCESS;
}

mpk_ret_t mpk_pkginfo_sign(struct mpk_pkginfo *pkginf, const char *pkey_file)
{
    EVP_PKEY *private_key;
    FILE *priv_key_file;
    RSA *rsa_private_key = NULL;
    EVP_MD_CTX ctx;
    struct mpk_pkgreflist_item *pkgref;
    struct mpk_file *file;
    unsigned int sig_len;

    /* load private key */

    if ((private_key = EVP_PKEY_new()) == NULL)
        goto err1;
    if ((priv_key_file = fopen(pkey_file, "r")) == NULL)
        goto err2;
    if (!PEM_read_RSAPrivateKey(priv_key_file, &rsa_private_key, NULL, NULL))
        goto err3;
    if (!EVP_PKEY_assign_RSA(private_key, rsa_private_key))
        goto err3;


    /* create signature */

    EVP_MD_CTX_init(&ctx);

    if (!EVP_SignInit(&ctx, EVP_sha512()))
        goto err3;

    if (!EVP_SignUpdate(&ctx, &pkginf->manifest, sizeof(pkginf->manifest)))
        goto err3;

    if (!EVP_SignUpdate(&ctx, pkginf->name, strlen(pkginf->name)))
        goto err3;

    if (!EVP_SignUpdate(&ctx, &pkginf->version, sizeof(pkginf->version)))
        goto err3;

    if (!EVP_SignUpdate(&ctx, &pkginf->arch, sizeof(pkginf->arch)))
        goto err3;

    if (!EVP_SignUpdate(&ctx, &pkginf->regions, sizeof(pkginf->regions)))
        goto err3;

    for (pkgref = pkginf->depends.lh_first; pkgref;
            pkgref = pkgref->items.le_next) {
        if (!EVP_SignUpdate(&ctx, pkgref->pkgref->name,
                strlen(pkgref->pkgref->name))) {
            goto err3;
        }
        if (!EVP_SignUpdate(&ctx, &pkgref->pkgref->ver,
                sizeof(pkgref->pkgref->ver))) {
            goto err3;
        }
        if (!EVP_SignUpdate(&ctx, &pkgref->pkgref->op,
                sizeof(pkgref->pkgref->op))) {
            goto err3;
        }
    }

    for (pkgref = pkginf->conflicts.lh_first; pkgref;
            pkgref = pkgref->items.le_next) {
        if (!EVP_SignUpdate(&ctx, pkgref->pkgref->name,
                strlen(pkgref->pkgref->name))) {
            goto err3;
        }
        if (!EVP_SignUpdate(&ctx, &pkgref->pkgref->ver,
                sizeof(pkgref->pkgref->ver))) {
            goto err3;
        }
        if (!EVP_SignUpdate(&ctx, &pkgref->pkgref->op,
                sizeof(pkgref->pkgref->op))) {
            goto err3;
        }
    }

    if (!EVP_SignUpdate(&ctx, &pkginf->priority, sizeof(pkginf->priority)))
        goto err3;

    if (!EVP_SignUpdate(&ctx, pkginf->source, strlen(pkginf->source)))
        goto err3;

    if (!EVP_SignUpdate(&ctx, pkginf->vendor, strlen(pkginf->vendor)))
        goto err3;

    if (!EVP_SignUpdate(&ctx, pkginf->description, strlen(pkginf->description)))
        goto err3;

    if (!EVP_SignUpdate(&ctx, pkginf->maintainer, strlen(pkginf->maintainer)))
        goto err3;

    if (!EVP_SignUpdate(&ctx, pkginf->license, strlen(pkginf->license)))
        goto err3;

    for (file = pkginf->files.lh_first; file; file = file->items.le_next) {
        if (!EVP_SignUpdate(&ctx, file->name, strlen(file->name)))
            goto err3;

        if (!EVP_SignUpdate(&ctx, file->hash, MPK_FILEHASH_SIZE))
            goto err3;
    }

    if (!EVP_SignFinal(&ctx, pkginf->signature, &sig_len, private_key))
        goto err3;

    fclose(priv_key_file);

    return MPK_SUCCESS;

    err3:
        fclose(priv_key_file);
    err2:
        EVP_PKEY_free(private_key);
    err1:
        return MPK_FAILURE;
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
