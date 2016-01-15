/**
 * @file package.h
 * @brief Creating and unpacking mpk package files.
 */
#ifndef _PACKAGE_H
#define _PACKAGE_H
#include <stdint.h>
#include <rps/pkginfo.h>
#include <rps/version.h>

#define MPK_PACKAGE_SIGNATURE_LEN 256 /* 2048 bit signature */
#define MPK_PACKAGE_CACHE_DIR "/tmp"

namespace RPS {

class Package {
    Package();
    virtual ~Package();
};

}

/**
 * @brief mpk_package_packmpk creates a mpk package from the packageinfo and
 * source data
 * @param pkg package info object of the package
 * @param srcdir directory of the source data
 * @return
 */
int mpk_package_packmpk(struct mpk_pkginfo *pkg, const char *srcdir,
    const char *outdir);

/**
 * @brief Unpacks a mpk.
 * @param unpacked_to Returns the path to which the package has been unpacked.
 * @param package_file The file to unpack.
 * @param outdir The destination directory.
 * @return
 */
int mpk_package_unpackmpk(const char *package_file, char *outdir);

/**
 * @brief mpk_package_verify checks if the signature of the package is correct
 * @param pkginf pkfinfo of the package to check
 * @param pkgdir directory of the extracted files
 * @param pubkey rsa public key
 * @return
 */
int mpk_package_verify(struct mpk_pkginfo *pkginf, const char *pkgdir,
    const char *pubkey);

/**
 * @brief Returns a allocated string of the package filename without any
 * extension.
 * @param fpath Path include mpk filename.
 * @return The result string located on the heap.
 *
 * Example: /tmp/testpackage-1.2.0.mpk -> testpackage-1.2.0
 */
char *mpk_package_name_from_fpath(const char *fpath);

#endif /* _PACKAGE_H */

