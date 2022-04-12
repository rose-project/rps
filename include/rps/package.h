/**
 * @file package.h
 * @brief Creating and unpacking mpk package files.
 */
#ifndef _PACKAGE_H
#define _PACKAGE_H
#include <string>
#include <filesystem>
#include <rps/manifest.h>

namespace rose {

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
    void writePackge(std::filesystem::path dest_dir);

    /**
     * @brief baseFilename
     * @return the base name of the package file
     */
    std::string baseFilename() const;

    /**
     * @brief filename
     * @return the file name including extension
     */
    std::string filename() const;
private:

    /**
     * @brief Creates an *.rps file.
     */
    void pack();

    void unpack();

private:
    Manifest mManifest;
    std::filesystem::path mUnpackedDir;
    std::filesystem::path mPackagePath;
    constexpr static std::string_view FileExtension{"rps"};
    std::filesystem::path mWorkDir{"/tmp/rps"};
};

}

#endif /* _PACKAGE_H */

