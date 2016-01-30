 /**
  * @file package.c
  */
#include <experimental/filesystem>
#include <iostream>
#include <memory>
#include <vector>
#include <rps/exception.h>

#define _GNU_SOURCE
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <fcntl.h>
#include <syslog.h>
#include <libgen.h>
#include <unistd.h>
#include <libtar.h>
#include <bzlib.h>
#include <linux/limits.h>
#include <rps/defines.h>
#include "rps/package.h"

/** The defaut size used for chunks of data used in for packing/unpacking and
 * signature generation generation
 */
#define CHUNKSIZE 512

#define PACKAGE_VERSION 1

#define PACKAGE_STYLE_RAW 0
#define PACKAGE_STYLE_TAR 1

#define PACKAGE_COMPRESSION_NONE 1
#define PACKAGE_COMPRESSION_BZ2 1

/**
 * @brief mpk_package_signature
 */
struct mpk_package_header {
    uint8_t magic[4]; /* { 0x6d, 0x70, 0x6b, 0x2a } "mpk*" */
    uint8_t fversion; /* package format version */
    uint8_t style; /* raw, tar */
    uint8_t compression;
    uint8_t flags;
    uint64_t package_size; /* size of the complete package including header */
    uint8_t package_signature[MPK_PACKAGE_SIGNATURE_LEN];
    uint8_t header_signature[MPK_PACKAGE_SIGNATURE_LEN];
};

int mpk_package_packmpk(struct mpk_pkginfo *pkg, const char *srcdir,
    const char *outdir)
{
    int version_str_len;
    TAR *tar;
    BZFILE *bz2;
    FILE *tbz2_file;
    int tar_fd;
    int bzerr;
    char src[PATH_MAX + 1];
    char dst[PATH_MAX + 1];
    char tar_fpath[PATH_MAX + 1];
    char tbz2_fpath[PATH_MAX + 1];
    unsigned char buffer[CHUNKSIZE];
    int size;
    struct mpk_file *file;

    /* create tar */

    sprintf(tar_fpath, "/tmp/%s_files.tar", pkg->name);
    if (access(tar_fpath, F_OK) == 0)
        if (unlink(tar_fpath) != 0)
            goto err0;

    if (tar_open(&tar, tar_fpath, NULL, O_WRONLY|O_CREAT, 0644, 0) != 0)
        goto err0;

    for (file = pkg->tool.lh_first; file; file = file->items.le_next) {
        sprintf(src, "%s/tool/%s", srcdir, file->name);
        sprintf(dst, "tool/%s", file->name);
        if (tar_append_tree(tar, src, dst) != 0)
            goto err2;
    }

    for (file = pkg->data.lh_first; file; file = file->items.le_next) {
        if (file->type == MPK_FILE_TYPE_R || file->type == MPK_FILE_TYPE_EXE
                || file->type == MPK_FILE_TYPE_W
                || file->type == MPK_FILE_TYPE_S) {
            sprintf(src, "%s/data/%s", srcdir, file->name);
            sprintf(dst, "data/%s", file->name);
            if (tar_append_tree(tar, src, dst) != 0)
                goto err2;
        }
    }

    sprintf(src, "%s/manifest.txt", srcdir);
    if (tar_append_file(tar, src, "manifest.txt") != 0)
        goto err2;

    tar_close(tar);


    /* compress using bz2 */

    version_str_len = mpk_version_serializedsize(&pkg->version);
    char *version_str;
    if (!(version_str = (char *)malloc(version_str_len + 1)))
        goto err2;
    if (mpk_version_serialize(version_str, NULL, version_str_len, &pkg->version)
            != MPK_SUCCESS) {
        free(version_str);
        goto err2;
    }
    version_str[version_str_len] = 0;

    sprintf(tbz2_fpath, "%s/%s-%s.mpk", outdir, pkg->name, version_str);
    free(version_str);
    printf("path:%s\n", tbz2_fpath);

    if ((tar_fd = open(tar_fpath, O_RDONLY)) == -1)
        goto err1;

    if ((tbz2_file = fopen(tbz2_fpath, "wb")) == NULL)
        goto err3;
    bz2 = BZ2_bzWriteOpen(&bzerr, tbz2_file, 9, 0, 30);
    if (bzerr != BZ_OK)
        goto err4;

    while ((size = read(tar_fd, buffer, CHUNKSIZE)) > 0)
        BZ2_bzWrite(&bzerr, bz2, buffer, size);
    BZ2_bzWriteClose(&bzerr, bz2, 0, NULL, NULL);
    fclose(tbz2_file);
    close(tar_fd);
    if (bzerr != BZ_OK || size < 0)
        goto err1;

    if (unlink(tar_fpath) != 0)
        goto err0;

    return MPK_SUCCESS;

err4:
    fclose(tbz2_file);
err3:
    close(tar_fd);
    goto err1;
err2:
    tar_close(tar);
err1:
    unlink(tar_fpath);
err0:
    return MPK_FAILURE;
}

int mpk_package_unpackmpk(const char *package_file, char *outdir)
{
    FILE *tbz2_file, *tar_file;
    const char *tar_fpath = "/tmp/mpk-temp.tar";
    BZFILE *bz2;
    int bzerr;
    unsigned char buf[CHUNKSIZE];
    size_t n;
    TAR *tar;

    if (!package_file || !outdir)
        return MPK_FAILURE;

    if (!(tar_file = fopen(tar_fpath, "w")))
        return MPK_FAILURE;

    if (!(tbz2_file = fopen(package_file, "r"))) {
        syslog(LOG_ERR, "could not open file: %s", package_file);
        fclose(tar_file);
        return MPK_FAILURE;
    }

    /* decompress bz2 */

    bz2 = BZ2_bzReadOpen(&bzerr, tbz2_file, 0, 0, NULL, 0);
    if (bzerr != BZ_OK) {
        fclose(tbz2_file);
        fclose(tar_file);
        return MPK_FAILURE;
    }
    while (1) {
        n = BZ2_bzRead(&bzerr, bz2, buf, CHUNKSIZE);
        if (bzerr != BZ_OK && bzerr != BZ_STREAM_END) {
            fclose(tar_file);
            unlink(tar_fpath);
            BZ2_bzReadClose(&bzerr, bz2);
            fclose(tbz2_file);
            return MPK_FAILURE;
        }

        if (fwrite(buf, 1, n, tar_file) != n) {
            fclose(tar_file);
            unlink(tar_fpath);
            BZ2_bzReadClose(&bzerr, bz2);
            fclose(tbz2_file);
            return MPK_FAILURE;
        }

        if (bzerr == BZ_STREAM_END)
            break;
    }

    BZ2_bzReadClose(&bzerr, bz2);
    fclose(tbz2_file);
    fclose(tar_file);

    /* create output directory */

    if (mkdir(outdir, 0700) != 0) {
        syslog(LOG_ERR, "mkdir failed: %s", strerror(errno));
        unlink(tar_fpath);
        return MPK_FAILURE;
    }

    /* unpack tar */

    if (tar_open(&tar, tar_fpath, NULL, O_RDONLY, 0644, 0) != 0) {
        rmdir(outdir);
        unlink(tar_fpath);
        return MPK_FAILURE;
    }

    if (tar_extract_all(tar, outdir) != 0) {
        syslog(LOG_ERR, "tar_extract_all() failed: %s", strerror(errno));
        tar_close(tar);
        rmdir(outdir);
        unlink(tar_fpath);
        return MPK_FAILURE;
    }
    tar_close(tar);
    if (unlink(tar_fpath) != 0)
        return MPK_FAILURE;

    return MPK_SUCCESS;
}

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
