/**
 * @file pkginfo.h
 * @author Josef Raschen <josef@raschen.org>
 * @brief API for handling with package info of manifest files.
 */
#ifndef _PKGINFO_H
#define _PKGINFO_H

#include <sys/queue.h>
#include "pkgref.h"
#include "file.h"
#include "version.h"
#include "stringlist.h"
#include "err.h"

#define MPK_PKGINFO_SIGNATURE_LEN 256

/*
 * has to be in sync with archname in pkginfo.c
 */
enum MPK_PKGINFO_ARCH {
    MPK_PKGINFO_ARCH_UNKNOWN = -1,
    MPK_PKGINFO_ARCH_GENERIC = 0,
    MPK_PKGINFO_ARCH_X86,
    MPK_PKGINFO_ARCH_X86_64,
    MPK_PKGINFO_ARCH_POWERPC,
    MPK_PKGINFO_ARCH_ARMV5,
    MPK_PKGINFO_ARCH_ARMV6,
    MPK_PKGINFO_ARCH_ARMV6HF,
    MPK_PKGINFO_ARCH_ARMV7,
    MPK_PKGINFO_ARCH_ARMV7HF,
    MPK_PKGINFO_ARCH_ARMV8,
    MPK_PKGINFO_ARCH_COUNT
};

/**
 * @brief Contains all information about a single package.
 */
struct mpk_pkginfo {
    struct mpk_version manifest;
    char *name;                             /**< Package name */
    struct mpk_version version;             /**< version of the package */
    enum MPK_PKGINFO_ARCH arch;             /**< the target architecure */
    struct mpk_stringlist regions;
    struct mpk_pkgreflist depends;
    struct mpk_pkgreflist conflicts;
    int priority;
    char *source;
    char *vendor;
    char *description;
    char *maintainer;
    char *license;
    struct mpk_filelist files;
    unsigned char signature[MPK_PKGINFO_SIGNATURE_LEN];
};

/**
 * @brief mpk_pkginfo_initinitializes an empty mpk_pkginfo object
 * @return MPK_SUCCESS or MPK_FAILURE
 */
mpk_ret_t mpk_pkginfo_init(struct mpk_pkginfo *pkg);

/**
 * @brief mpk_pkginfo_clean frees any dynamically allocated data and resets to
 *        default values
 * @param pkg the pkginfo object
 */
void mpk_pkginfo_delete(struct mpk_pkginfo *pkg);

/* TODO: should be moved to package.h */
/**
 * @brief mpk_pkginfo_calcfilehashes determines the SHA256 hashes for each file
 * in the pkginf->files
 * @param pkginf the package info containing the list of files
 * @param pkgroot base directory to which the filenames in the list of files
 * are relative to
 * @return MPK_SUCCESS or MPK_FAILURE
 */
mpk_ret_t mpk_pkginfo_calcfilehashes(struct mpk_pkginfo *pkginf,
    const char *pkgroot);

/* TODO: should be moved to package.h */
/**
 * @brief mpk_pkginfo_sign calculate the signature of the pkginfo data
 * @param pkginf the package info to sign
 * @return MPK_SUCCESS or MPK_FAILURE
 */
mpk_ret_t mpk_pkginfo_sign(struct mpk_pkginfo *pkginf, const char *pkey_file);

mpk_ret_t mpk_pkginfo_arch_deserialize(enum MPK_PKGINFO_ARCH *arch, char *str);

mpk_ret_t mpk_pkginfo_arch_serialize(char *dst, int *written, int len,
    enum MPK_PKGINFO_ARCH arch);

mpk_ret_t mpk_pkginfo_signature_deserialize(unsigned char signature[],
    char *src);

#endif
