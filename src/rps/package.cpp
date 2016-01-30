 /**
  * @file package.cpp
  */
#include <experimental/filesystem>
#include <iostream>
#include <memory>
#include <vector>
#include <cstdlib>
#include <cerrno>
#include <fcntl.h>
#include <unistd.h>
#include <libtar.h>
#include <bzlib.h>
#include <rps/exception.h>
#include "rps/package.h"

/** The defaut size used for chunks of data used in for packing/unpacking and
 * signature generation generation
 */
#define CHUNKSIZE 512

namespace RPS {

const std::experimental::filesystem::path Package::mWorkDir = "/tmp/rps";

Package::Package()
{
    setupWorkdir();
}

Package::Package(std::string package_file)
    : mPackagePath(package_file)
{
    setupWorkdir();
    readPackageFile(package_file);
}

Package::~Package()
{

}

void Package::readPackageFile(const std::string &package_path)
{
    if (!package_path.empty())
        mPackagePath = package_path;


    // unpack bz2

    FILE *tbz2_file;

    if (!(tbz2_file = fopen(package_path.c_str(), "r"))) {
        throw Exception(std::string("could not open package file:") + package_path);
    }

    BZFILE *bz2;
    int bzerr;
    bz2 = BZ2_bzReadOpen(&bzerr, tbz2_file, 0, 0, NULL, 0);
    if (bzerr != BZ_OK) {
        fclose(tbz2_file);
        throw Exception("BZ2_bzReadOpen() failed");
    }

    FILE *tar_file;
    std::string tar_fpath = std::string(mWorkDir) + "/unpacked.tar";
    if (!(tar_file = fopen(tar_fpath.c_str(), "w"))) {
        BZ2_bzReadClose(&bzerr, bz2);
        fclose(tbz2_file);
        throw Exception(std::string("fopen() failed: ") + tar_fpath);
    }

    unsigned char buf[CHUNKSIZE];
    while (1) {
        size_t n = BZ2_bzRead(&bzerr, bz2, buf, CHUNKSIZE);
        if (bzerr != BZ_OK && bzerr != BZ_STREAM_END) {
            fclose(tar_file);
            std::experimental::filesystem::remove(tar_fpath);
            BZ2_bzReadClose(&bzerr, bz2);
            fclose(tbz2_file);
            throw Exception(std::string("BZ2_bzRead failed"));
        }

        if (fwrite(buf, 1, n, tar_file) != n) {
            fclose(tar_file);
            std::experimental::filesystem::remove(tar_fpath);
            BZ2_bzReadClose(&bzerr, bz2);
            fclose(tbz2_file);
            throw Exception(std::string("fwrite() failed"));
        }

        if (bzerr == BZ_STREAM_END)
            break;
    }

    fclose(tar_file);

    // unpack tar

     TAR *tar;
    if (tar_open(&tar, tar_fpath.c_str(), NULL, O_RDONLY, 0644, 0) != 0) {
        std::experimental::filesystem::remove(tar_fpath);
        throw Exception("tasr_open() failed");
    }

    mUnpackedDir = mWorkDir;
    mUnpackedDir /= mPackagePath.stem();
    std::cout << "unpack to: " << mUnpackedDir << std::endl;
    if (std::experimental::filesystem::exists(mUnpackedDir))
        std::experimental::filesystem::remove_all(mUnpackedDir);
    std::experimental::filesystem::create_directory(mUnpackedDir);

    std::string unpacked_dirname = mUnpackedDir.string();
    std::vector<char> unpacked_dirname_v(unpacked_dirname.c_str(),
        unpacked_dirname.c_str() + (unpacked_dirname.length() + 1));
    if (tar_extract_all(tar, &unpacked_dirname_v[0]) != 0) {
        std::experimental::filesystem::remove_all(mUnpackedDir);
        tar_close(tar);
        throw Exception("cannot extract tar");
    }
    tar_close(tar);
    std::experimental::filesystem::remove(tar_fpath);

}

void Package::readPackageDir(std::string package_dir)
{
    std::experimental::filesystem::path pkgdir = package_dir;
    if (!std::experimental::filesystem::exists(pkgdir))
        throw Exception("package directory '" + package_dir + "'does not exist");

    mManifest.readFromFile(package_dir + "/manifest.json");

    mUnpackedDir = package_dir;
}

void Package::signPackage(std::__cxx11::string priv_key)
{

}

void Package::verify(std::__cxx11::string pub_key)
{

}

void Package::writePackge(std::__cxx11::string dest_dir)
{
    if (mManifest.packageName().empty())
        throw Exception("package metatdata is not set");

    // create package workdir or delete old data
    std::experimental::filesystem::path package_tmp_dir =
            std::string("/tmp/rps/") + mManifest.packageName();
    if (std::experimental::filesystem::exists(package_tmp_dir))
        std::experimental::filesystem::remove_all(package_tmp_dir);

    std::experimental::filesystem::create_directory(package_tmp_dir);

    mManifest.writeManifestFile(package_tmp_dir.string() + "/manifest.json");

    // TODO: copy files

    // TODO: pack + compress
    pack();

    // TODO: copy to destination directory


}

void Package::setupWorkdir()
{
    // create rps working dir if not present
    std::experimental::filesystem::path rps_tmp_dir = "/tmp/rps";
    if (!std::experimental::filesystem::exists(rps_tmp_dir))
        std::experimental::filesystem::create_directory(rps_tmp_dir);
}

void Package::pack()
{
    TAR *tar;

    std::string package_base_filename(mManifest.packageName() + "-"
        + std::to_string(mManifest.packageVersion()) + "-"
        + mManifest.targetArch());

    // create tar

    std::experimental::filesystem::path tar_path = std::string("/tmp/rps/")
        + mManifest.packageName() + "-"
        + std::to_string(mManifest.packageVersion()) + "-"
        + mManifest.targetArch() + ".tar";

    if (tar_open(&tar, tar_path.c_str(), NULL, O_WRONLY|O_CREAT, 0644, 0) != 0)
        throw Exception("tar_open() failed");

    std::string mfst_path = mUnpackedDir.string() + "/manifest.json";
    if (tar_append_file(tar, (mUnpackedDir.string() + "/manifest.json").c_str(),
            "manifest.json") != 0) {
        tar_close(tar);
        throw Exception("tar_append_file() of 'manifest.json'' failed");
    }

    // TODO add files


    tar_close(tar);


    // compress

    std::experimental::filesystem::path tbz2_path = mWorkDir.string() + "/"
        + package_base_filename + ".rps";

    int tar_fd = open(tar_path.c_str(), O_RDONLY);
    if (tar_fd  == -1) {
        std::experimental::filesystem::remove(tar_path);
        throw Exception("open(tar_path) failed");
    }

    FILE *tbz2_file = fopen(tbz2_path.c_str(), "wb");
    if (tbz2_file == NULL) {
        close(tar_fd);
        std::experimental::filesystem::remove(tar_path);
        throw Exception("fopen(tbz2_path) failed");
    }

    int bzerr;
    BZFILE *bz2 = BZ2_bzWriteOpen(&bzerr, tbz2_file, 9, 0, 30);
    if (bzerr != BZ_OK) {
        fclose(tbz2_file);
        close(tar_fd);
        std::experimental::filesystem::remove(tar_path);
        throw Exception("BZ2_bzWriteOpen failed");
    }

    size_t size;
    unsigned char buffer[CHUNKSIZE];
    while ((size = read(tar_fd, buffer, CHUNKSIZE)) > 0)
        BZ2_bzWrite(&bzerr, bz2, buffer, size);

    BZ2_bzWriteClose(&bzerr, bz2, 0, NULL, NULL);
    fclose(tbz2_file);
    close(tar_fd);

    std::experimental::filesystem::remove(tar_path);

    if (bzerr != BZ_OK || size < 0) {
        throw Exception("write BZ2 failed");
    }
}

void Package::unpack()
{

}

} // namespace RPS
