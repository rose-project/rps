/**
 * @file pkginfo.c
 * @author Josef Raschen <josef@raschen.org>
 */
#define _GNU_SOURCE
#include <syslog.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <ctype.h>
#include <openssl/evp.h>
#include <openssl/pem.h>
#include "stringhelper.h"
#include "mpk/defines.h"
#include "mpk/pkginfo.h"

#define CHECK_AND_FREE(ptr) if (ptr) {  \
    free(ptr);                          \
    ptr = NULL;                         \
}                                       \

int mpk_pkginfo_init(struct mpk_pkginfo *pkg)
{
    if (!pkg) {
        syslog(LOG_ERR, "parameter pkg is invalid");
        return MPK_FAILURE;
    }

    pkg->manifest = MPK_VERSION_DEFAULT;
    pkg->name = NULL;
    pkg->version = MPK_VERSION_DEFAULT;
    pkg->arch = NULL;
    mpk_stringlist_init(&pkg->regions);
    mpk_pkgreflist_init(&pkg->depends);
    mpk_pkgreflist_init(&pkg->conflicts);
    pkg->priority = 0;
    pkg->source = NULL;
    pkg->vendor = NULL;
    pkg->description = NULL;
    pkg->maintainer = NULL;
    pkg->license = NULL;
    mpk_filelist_init(&pkg->tool);
    mpk_filelist_init(&pkg->data);
    memset(pkg->signature, 0, MPK_PKGINFO_SIGNATURE_LEN);
    pkg->is_signed = false;

    return MPK_SUCCESS;
}

void mpk_pkginfo_clean(struct mpk_pkginfo *pkg)
{
    if (!pkg) {
        syslog(LOG_WARNING, "mpk_pkginfo_deinit() called on NULL pointer");
        return;
    }

    CHECK_AND_FREE(pkg->name);
    CHECK_AND_FREE(pkg->arch);
    mpk_stringlist_empty(&pkg->regions);
    mpk_pkgreflist_empty(&pkg->depends);
    mpk_pkgreflist_empty(&pkg->conflicts);
    CHECK_AND_FREE(pkg->source);
    CHECK_AND_FREE(pkg->vendor);
    CHECK_AND_FREE(pkg->description);
    CHECK_AND_FREE(pkg->maintainer);
    CHECK_AND_FREE(pkg->license);
    mpk_filelist_delete(&pkg->tool);
    mpk_filelist_delete(&pkg->data);
    memset(pkg->signature, 0, MPK_PKGINFO_SIGNATURE_LEN);
    pkg->is_signed = false;
}

int mpk_pkginfo_calcfilehashes(struct mpk_pkginfo *pkginf,
    const char *pkgroot)
{
    struct mpk_file *file;
    char basedir[PATH_MAX + 1];

    snprintf(basedir, PATH_MAX, "%s/tool", pkgroot);
    for (file = pkginf->tool.lh_first; file; file = file->items.le_next) {
        syslog(LOG_INFO, "tool %s", file->name);
        if (mpk_file_calchash(file, basedir) != MPK_SUCCESS) {
            return MPK_FAILURE;
        }
    }

    snprintf(basedir, PATH_MAX, "%s/data", pkgroot);
    for (file = pkginf->data.lh_first; file; file = file->items.le_next) {
        syslog(LOG_INFO, "file %s", file->name);
        if (file->type == MPK_FILE_TYPE_R
                || file->type == MPK_FILE_TYPE_EXE
                || file->type == MPK_FILE_TYPE_W) {
            if (mpk_file_calchash(file, basedir) != MPK_SUCCESS) {
                return MPK_FAILURE;
            }
        }
    }

    return MPK_SUCCESS;
}

/* TODO: move signature calculation to package.c */
int mpk_pkginfo_sign(struct mpk_pkginfo *pkginf, const char *pkey_file)
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

    if (!EVP_SignUpdate(&ctx, &pkginf->arch, strlen(pkginf->arch)))
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

    for (file = pkginf->tool.lh_first; file; file = file->items.le_next) {
        if (!EVP_SignUpdate(&ctx, file->name, strlen(file->name)))
            goto err3;

        if (!EVP_SignUpdate(&ctx, file->hash, MPK_FILEHASH_SIZE))
            goto err3;
    }

    for (file = pkginf->data.lh_first; file; file = file->items.le_next) {
        if (!EVP_SignUpdate(&ctx, file->name, strlen(file->name)))
            goto err3;

        if (!EVP_SignUpdate(&ctx, &file->type, sizeof(enum MPK_FILE_TYPE)))
            goto err3;

        if (!EVP_SignUpdate(&ctx, file->hash, MPK_FILEHASH_SIZE))
            goto err3;

        if (file->type == MPK_FILE_TYPE_S)
            if (!EVP_SignUpdate(&ctx, file->target, strlen(file->target)))
                goto err3;
    }

    if (!EVP_SignFinal(&ctx, pkginf->signature, &sig_len, private_key))
        goto err3;

    fclose(priv_key_file);

    pkginf->is_signed = true;
    return MPK_SUCCESS;

    err3:
        fclose(priv_key_file);
    err2:
        EVP_PKEY_free(private_key);
    err1:
        return MPK_FAILURE;
}

int mpk_pkginfo_signature_deserialize(unsigned char signature[],
    char *src)
{
    return read_hexstr(signature,MPK_PKGINFO_SIGNATURE_LEN, src);
}
