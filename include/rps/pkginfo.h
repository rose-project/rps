/**
 * @file pkginfo.h
 * @brief API for handling with package info of manifest files.
 */
#ifndef _PKGINFO_H
#define _PKGINFO_H

#include <sys/queue.h>
#include <stdbool.h>
#include <rps/pkgref.h>
#include <rps/file.h>
#include <rps/version.h>
#include <rps/stringlist.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MPK_PKGINFO_SIGNATURE_LEN 256

/**
 * @brief Contains all information about a single package.
 */
struct mpk_pkginfo {
    struct mpk_version manifest;
    char *name;                       /**< Package name */
    struct mpk_version version;             /**< version of the package */
    char *arch;                       /**< the target architecure */
    struct mpk_stringlist regions;
    struct mpk_pkgreflist depends;
    struct mpk_pkgreflist conflicts;
    int priority;
    char *source;
    char *vendor;
    char *description;
    char *maintainer;
    char *license;
    struct mpk_filelist tool;
    struct mpk_filelist data;
/*    uint8_t package_hash[MPK_PACKAGE_HASH_LEN]; */
    unsigned char signature[MPK_PKGINFO_SIGNATURE_LEN];
    bool is_signed;
};

/**
 * @brief mpk_pkginfo_initinitializes an empty mpk_pkginfo object
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_pkginfo_init(struct mpk_pkginfo *pkg);

/**
 * @brief mpk_pkginfo_clean frees any dynamically allocated data and resets to
 *        default values
 * @param pkg the pkginfo object
 */
void mpk_pkginfo_clean(struct mpk_pkginfo *pkg);

/* TODO: should be moved to package.h */
/**
 * @brief mpk_pkginfo_calcfilehashes determines the SHA256 hashes for each file
 * in the pkginf->files
 * @param pkginf the package info containing the list of files
 * @param pkgroot base directory to which the filenames in the list of files
 * are relative to
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_pkginfo_calcfilehashes(struct mpk_pkginfo *pkginf,
    const char *pkgroot);

/* TODO: should be moved to package.h */
/**
 * @brief mpk_pkginfo_sign calculate the signature of the pkginfo data
 * @param pkginf the package info to sign
 * @return MPK_SUCCESS or MPK_FAILURE
 */
int mpk_pkginfo_sign(struct mpk_pkginfo *pkginf, const char *pkey_file);

int mpk_pkginfo_signature_deserialize(unsigned char signature[],
    char *src);

#ifdef __cplusplus
}
#endif

#endif
