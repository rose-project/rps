/**
 * @file package.h
 * @brief Creating and unpacking mpk package files.
 */
#ifndef _PACKAGE_H
#define _PACKAGE_H
#include <string>
#include <boost/filesystem.hpp>
#include <rps/manifest.h>

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
    boost::filesystem::path mUnpackedDir;
    boost::filesystem::path mPackagePath;
    static const boost::filesystem::path mWorkDir;
};

}

#endif /* _PACKAGE_H */

