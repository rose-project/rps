/**
 * @file package.h
 * @brief Creating and unpacking mpk package files.
 */
#ifndef _PACKAGE_H
#define _PACKAGE_H
#include <string>
#include <experimental/filesystem>
#include <stdint.h>
#include <rps/pkginfo.h>
#include <rps/version.h>
#include <rps/manifest.h>

#define MPK_PACKAGE_SIGNATURE_LEN 256 /* 2048 bit signature */
#define MPK_PACKAGE_CACHE_DIR "/tmp"

namespace RPS {

class Package {
public:
    Package();
    Package(const std::string package_file);

    virtual ~Package();

    /**
     * @brief Read a package file.
     * @param package_path Path of the *.rpk.
     */
    void readPackageFile(const std::string &package_path = std::string());

    /**
     * @brief Read a prackage from a source dir.
     * @param package_dir The directory with the package files.
     */
    void readPackageDir(std::string package_dir);

    /**
     * @brief Sign a package.
     * @param The private key to use.
     */
    void signPackage(std::string priv_key);

    /**
     * @brief Verify the signature of a package.
     * @param pub_key The public key file.
     */
    void verify(std::string pub_key);

    /**
     * @brief Write a package file to the file system.
     * @param dest_dir The directory to write the *.rpk file to.
     */
    void writePackge(std::string dest_dir);

private:

    void setupWorkdir();

    /**
     * @brief Creates an *.rps file.
     */
    void pack();

    void unpack();

private:
    Manifest mManifest;
    std::experimental::filesystem::path mUnpackedDir;
    std::experimental::filesystem::path mPackagePath;
    static const std::experimental::filesystem::path mWorkDir;
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

